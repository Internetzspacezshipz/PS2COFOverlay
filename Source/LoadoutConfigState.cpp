#include "LoadoutConfigState.h"
#include "DataLoader.h"
#include "UserSettings.h"


TickOutput WeaponConfigState::Tick(
	float DeltaTime,
	bool bTryingToFire,
	bool bMoving,
	bool bIsCrouched,
	bool bIsADSed,
	bool bReloadPressed)
{
	if (!Config)
	{
		return TickOutput{ 1.f, 0.f };
	}

	//Get configuration
	const WeaponFireGroupConfig& CurrentFireGroupConfig = Config->GetFireGroupConfig(CurrentFireGroup);
	const WeaponFireModeConfig& CurrentFireModeConfig = CurrentFireGroupConfig.GetFireModeConfig(bIsADSed);
	const WeaponConeConfig& CurrentConeConfig = CurrentFireModeConfig.GetConeConfig(bMoving, bIsCrouched);

	//Timers
	TimeToNextFire.Tick(DeltaTime);
	DelayBeforeFiring.Tick(DeltaTime);
	TimeToReload.Tick(DeltaTime);
	TimeToRecovery.Tick(DeltaTime);

	if (UserSettings::Get().bInfiniteAmmo) { CurrentBullets = Config->MagSize; }

	//Initiate reload.
	if (bReloadPressed
		&& CurrentBullets < Config->MagSize
		&& ShotsLeftInBurst == 0 //Can only reload when not firing.
		&& TimeToReload.Completed())//Cannot reload when already reloading.
	{
		TimeToReload.Set(CurrentBullets ? Config->ReloadTimeShort : Config->ReloadTimeLong);
		bIsReloading = true;
	}

	//If reload completed
	if (bIsReloading && TimeToReload.Completed())
	{
		//Currently don't really care about setting up cutting by ammo reserve. 
		//Not really worth coding atm. TODO later.
		CurrentBullets = Config->MagSize;
		bIsReloading = false;
	}

	//If not reloading, do all normal logic.
	if (!bIsReloading)
	{
		//Handle input wanting to fire/set up next burst/shot.
		if (bTryingToFire 
			&& DelayBeforeFiring.Completed() 
			&& (TimeToRecovery.Completed())
			&& ShotsLeftInBurst == 0
			&& CurrentBullets > 0)//If has bullets
		{
			//IF USING FULL AUTO
			if (CurrentFireModeConfig.ShotsPerBurst == 0)
			{
				ShotsLeftInBurst = 1;
			}
			//IF USING BURST FIRE
			else if (CurrentFireModeConfig.ShotsPerBurst > 0 && bHasUnlatched) //Was previously letting go of LMB
			{
				//Latch so the player has to release MB1
				bHasUnlatched = false;

				//Yumi/Kuwa/other delay fire weapons
				//If should delay.
				if (CurrentFireModeConfig.DelayBeforeFire > 0.f)
				{
					DelayBeforeFiring.Set(CurrentFireModeConfig.DelayBeforeFire);
				}

				ShotsLeftInBurst = CurrentFireModeConfig.ShotsPerBurst;
			}
		}
		else if (!bTryingToFire)
		{
			bHasUnlatched = true;
			//If we can allow an unlatch before firing, and we are no longer trying to fire.
			if (CurrentFireModeConfig.bAllowsFireCancel)
			{
				//Cancelling burst...
				ShotsLeftInBurst = 0;
			}
		}

		//If still in burst	and not out of bullets.
		if (ShotsLeftInBurst > 0
			&& CurrentBullets > 0)
		{
			//If we can fire
			if (TimeToNextFire.Completed()
				&& DelayBeforeFiring.Completed())
			{
				//Reset recovery timer if in process of firing.
				TimeToRecovery.Set(CurrentConeConfig.RecoveryDelay);

				//FIRE!!!!!!
				CurrentCOF += CurrentConeConfig.Bloom;

				ShotsLeftInBurst--;
				CurrentBullets--;

				//Reset timer for time to next fire.
				TimeToNextFire.Set(1.f/(CurrentFireModeConfig.ROF / 60.f));
			}
		}
	}

	//if haven't fired in a while...
	//Not trying to fire, or out of ammo.
	if (((TimeToRecovery.Completed()
		&& (ShotsLeftInBurst == 0 || DelayBeforeFiring.Completed()))
		|| CurrentBullets == 0)
		&& TimeToNextFire.Completed())
	{
		CurrentCOF -= CurrentConeConfig.RecoveryRate * DeltaTime;
		//Minimum cone of fire
		CurrentCOF = fmaxf(CurrentCOF, CurrentConeConfig.Minimum);
	}
	//Maximum cone of fire
	CurrentCOF = fminf(CurrentCOF, CurrentConeConfig.Maximum);

	return TickOutput{ CurrentFireModeConfig.Zoom, CurrentCOF };
}

void WeaponConfigState::WeaponSwitchedOff()
{
	TimeToRecovery.Set(0.f);
	//This might be a point where bugs could occur in weapon switching due to frame timings.
	if (bIsReloading && !TimeToReload.Completed())//If was actively reloading
	{
		TimeToReload.Set(CurrentBullets ? 0.f : Config->ReloadTimeLong);
	}
}

bool WeaponConfigState::LoadWeaponConfig()
{
	if (!WeaponName.size())
	{
		return false;
	}

	DataLoader DL = DataLoader::Get();

	//Init if not existing.
	if (!Config)
	{
		Config = boost::shared_ptr<WeaponConfig>(new WeaponConfig());
	}

	nlohmann::json WeaponJson = DL.LoadWeaponJson(WeaponName);

	if (WeaponJson.empty())
	{
		Config->Serialize(true, WeaponJson);
		DL.SaveWeaponJson(WeaponName, WeaponJson);// no error handling for now. pls fix later.
	}
	Config->Serialize(false, WeaponJson);

	//Start with full ammo
	CurrentBullets = Config->MagSize;

	return true;
}

void WeaponConfigState::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, WeaponName);
	if (!bSerializing && WeaponName.size())
	{
		LoadWeaponConfig();
	}
}

/* from WinUser.h L#532
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x3A - 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

bool IsMoving()
{
	short MovingKeys[8]
	{
		0x41,//A
		0x61,//a

		0x44,//D
		0x64,//d

		0x53,//S
		0x73,//s

		0x57,//W
		0x77,//w
	};

	for (short i = 0; i < 8; i++)
	{
		if (GetAsyncKeyState(MovingKeys[i]))
		{
			return true;
		}
	}
	return false;
}

bool bCurrentlyCrouched = false;
bool bLastCrouchPressed = false;

//@todo: make this work using user settings.
bool IsCrouching()
{
	//C = 0x43
	//c = 0x63

	bool bCPressed = GetAsyncKeyState(0x43) || GetAsyncKeyState(0x63);
	//if (bToggleCrouch)
	{
		if (bLastCrouchPressed != bCPressed)
		{
			bCurrentlyCrouched = !bCurrentlyCrouched;
			bLastCrouchPressed = bCPressed;
		}
	}
	return bCPressed;
}

TickOutput LoadoutConfigState::Tick(float DeltaTime)
{
	MaybeSwitchWeapon();

	if (WeaponConfigStates_Primaries.size() == 0 || CurEq == -1)// if -1, that means no weapon is equipped.
	{
		//Not yet initialized.
		return TickOutput{ 1.f, 0.f };
	}

	TimeBeforeFrom.Tick(DeltaTime);
	if (TimeBeforeFrom.Completed())
	{
		TimeBeforeTo.Tick(DeltaTime);
	}

	std::vector<WeaponConfigState>& CurrentArray = bIsSecondary ? WeaponConfigStates_Secondaries : WeaponConfigStates_Primaries ;

	WeaponConfigState& CurWeaponState = CurrentArray[CurEq];

	const bool bMoving = IsMoving();
	const bool bIsCrouched = IsCrouching();//crouch
	//const bool bIsSprinting = GetAsyncKeyState(VK_SHIFT);//sprint doesn't really matter rn

	//Process inputs.
	if (IsSwitching())
	{
		return CurWeaponState.Tick(DeltaTime, false, bMoving, bIsCrouched, false, false);
	}
	else
	{
		//These actions are only allowed while not switching
		const bool bFiring = GetAsyncKeyState(0x01);//LMB
		const bool bIsADSed = GetAsyncKeyState(0x02);//RMB
		const bool bIsReloadPressed = GetAsyncKeyState(0x52) || GetAsyncKeyState(0x72); //R/r

		return CurWeaponState.Tick(DeltaTime, bFiring, bMoving, bIsCrouched, bIsADSed, bIsReloadPressed);
	}
}

void LoadoutConfigState::SetSwitching(float FromTime, float ToTime)
{
	if (IsSwitching())
	{
		//Only allow switching while 
		TimeBeforeTo.Set(ToTime);
	}
	else
	{
		TimeBeforeFrom.Set(FromTime);
		TimeBeforeTo.Set(ToTime);
	}
}

//idk if want to use this code.
void LoadoutConfigState::MaybeSwitchWeapon()
{
	int Previous_Item = -1;
	bool Previous_bIsSecondary = false;

	bool bDidSwitch = false;

	//Weapon switching.
	if (GetAsyncKeyState(0x31))//1 key
	{
		bDidSwitch = WeaponSwitchHelper(0, Previous_Item, Previous_bIsSecondary);
	}
	else if (GetAsyncKeyState(0x32))//2 key
	{
		bDidSwitch = WeaponSwitchHelper(1, Previous_Item, Previous_bIsSecondary);
	}
	else if (GetAsyncKeyState(0x33))//3 key
	{
		bDidSwitch = WeaponSwitchHelper(2, Previous_Item, Previous_bIsSecondary);
	}
	else
	{
		bDidSwitch = WeaponSwitchHelper(-1, Previous_Item, Previous_bIsSecondary);
	}

	if (bDidSwitch)
	{
		//For secondary (grenade launchers) swaps, only take into account the to time.
		const bool bWasSecondarySwap = Previous_bIsSecondary != bIsSecondary && Previous_Item == CurEq;

		WeaponConfigState& Prev = GetWeaponConfigState(Previous_Item, Previous_bIsSecondary);
		WeaponConfigState& New = GetWeaponConfigState(CurEq, bIsSecondary);

		const float& FromTime = Prev.Config->TransitionFromTime;
		const float& ToTime = New.Config->TransitionToTime;

		SetSwitching(bWasSecondarySwap ? FromTime : 0.f, ToTime);

		Prev.WeaponSwitchedOff();
	}
}

bool LoadoutConfigState::WeaponSwitchHelper(int NewNumber, int& Previous_Item, bool& Previous_bIsSecondary)
{
	if (CurEq == NewNumber)
	{
		if (WeaponConfigStates_Secondaries.size() > NewNumber)
		{
			//Only allow equip if it's capable
			Previous_Item = CurEq;
			Previous_bIsSecondary = bIsSecondary;
			bIsSecondary = !bIsSecondary;
			return true;
		}
	}
	else if (WeaponConfigStates_Primaries.size() > NewNumber)
	{
		Previous_Item = CurEq;
		Previous_bIsSecondary = false;
		CurEq = NewNumber;
		bIsSecondary = false;
		return true;
	}
	return false;
}

void LoadoutConfigState::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_OBJECT_ARRAY(TargetJson, bSerializing, WeaponConfigStates_Primaries, WeaponConfigState);
	JSON_SERIALIZE_OBJECT_ARRAY(TargetJson, bSerializing, WeaponConfigStates_Secondaries, WeaponConfigState);
}