#include "LoadoutConfigState.h"
#include "DataLoader.h"


float WeaponConfigState::Tick(
	float DeltaTime,
	bool bTryingToFire,
	bool bMoving,
	bool bIsCrouched,
	bool bIsADSed,
	bool bReloadPressed)
{
	if (!Config)
	{
		return 1.f;
	}

	//Get configuration
	const WeaponFireGroupConfig& CurrentFireGroupConfig = Config->GetFireGroupConfig(CurrentFireGroup);
	const WeaponFireModeConfig& CurrentFireModeConfig = CurrentFireGroupConfig.GetFireModeConfig(bIsADSed);
	const WeaponConeConfig& CurrentConeConfig = CurrentFireModeConfig.GetConeConfig(bMoving, bIsCrouched);

	//Timers
	TimeToNextFire.Tick(DeltaTime);
	DelayBeforeFiring.Tick(DeltaTime);
	TimeToReload.Tick(DeltaTime);

	//Initiate reload.
	if (bReloadPressed
		&& CurrentBullets < Config->MagSize
		&& ShotsLeftInBurst == 0)//Can only reload when not firing.
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
		if ((bTryingToFire || DelayBeforeFiring.Completed())
			&& CurrentBullets > 0)//If has bullets
		{
			//Reset recovery time.
			TimeToRecovery.Set(CurrentConeConfig.RecoveryDelay);

			//IF USING FULL AUTO
			if (CurrentFireModeConfig.ShotsPerBurst == 0)
			{
				//Only add another shot if we don't have any queued
				if (ShotsLeftInBurst == 0)
				{
					ShotsLeftInBurst = 1;
				}
			}
			//IF USING BURST FIRE
			else if (CurrentFireModeConfig.ShotsPerBurst > 0 && bHasUnlatched) //Was previously letting go of LMB
			{
				//if we're not currently firing a burst
				if (ShotsLeftInBurst == 0)
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
		}
		else if (!bTryingToFire)
		{
			bHasUnlatched = true;
			//If we can allow an unlatch before firing, and we are no longer trying to fire.
			if (CurrentFireModeConfig.bAllowsFireCancel)
			{
				DelayBeforeFiring.Set(0.f);
				//Cancelling burst...
				ShotsLeftInBurst = 0;
			}
		}

		//If still in burst	and not out of bullets.
		if (ShotsLeftInBurst > 0
			&& CurrentBullets > 0)
		{
			//Reset recovery timer if in process of firing.
			TimeToRecovery.Set(CurrentConeConfig.RecoveryDelay);

			//If we can fire
			if (TimeToNextFire.Completed()
				&& DelayBeforeFiring.Completed())
			{
				//FIRE!!!!!!
				CurrentCOF += CurrentConeConfig.Bloom;

				--ShotsLeftInBurst;
				--CurrentBullets;

				//Reset timer for time to next fire.
				TimeToNextFire.Set(1000. / CurrentFireModeConfig.ROF / 60.);
			}
		}
	}

	//if haven't fired in a while...
	if (TimeToRecovery.Completed()
		&& (!bTryingToFire || CurrentBullets == 0)//Not trying to fire, or out of ammo.
		&& 0 == ShotsLeftInBurst)
	{
		CurrentCOF -= CurrentConeConfig.RecoveryRate * DeltaTime;
		//Minimum cone of fire
		CurrentCOF = fmaxf(CurrentCOF, CurrentConeConfig.Minimum);
	}
	//Maximum cone of fire
	CurrentCOF = fminf(CurrentCOF, CurrentConeConfig.Maximum);

	return CurrentFireModeConfig.Zoom;
}

void WeaponConfigState::WeaponSwitchedOff()
{
	TimeToRecovery.Set(0.f);
	//This might be a point where bugs could occur in weapon switching due to frame timings.
	if (bIsReloading && !TimeToReload.Completed())//If was actively reloading
	{
		TimeToReload.Set(CurrentBullets ? Config->ReloadTimeShort : Config->ReloadTimeLong);
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

void LoadoutConfig::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_ARRAY(TargetJson, bSerializing, LoadoutSlot_Primaries, std::string);
	JSON_SERIALIZE_ARRAY(TargetJson, bSerializing, LoadoutSlot_Secondaries, std::string);
}

XDynamicArray<WeaponConfigState> LoadoutConfig::CreateWeaponConfigStates(bool bSecondaries) const
{
	XDynamicArray<WeaponConfigState> Out;

	//Out.Add()

	return Out;
}

float LoadoutConfigState::Tick(float DeltaTime)
{
	if (WeaponConfigStates_Primaries.Num()
		|| WeaponConfigStates_Secondaries.Num()
		|| CurEq == -1)// if -1, that means no weapon is equipped.
	{
		//Not yet initialized.
		return 1.f;
	}

	MaybeSwitchWeapon();

	XDynamicArray<WeaponConfigState>& CurrentArray = bIsSecondary ? WeaponConfigStates_Primaries : WeaponConfigStates_Secondaries;

	WeaponConfigState& CurWeaponPtr = CurrentArray[CurEq];

	//Process inputs.

	const bool bMoving = IsMoving();
	const bool bFiring = GetAsyncKeyState(0x01);//LMB
	const bool bIsADSed = GetAsyncKeyState(0x02);//RMB
	const bool bIsReloadPressed = GetAsyncKeyState(0x52) || GetAsyncKeyState(0x72); //R/r
	const bool bIsCrouched = IsCrouching();//crouch
	const bool bIsSprinting = GetAsyncKeyState(VK_SHIFT);//sprint doesn't really matter rn

	CurWeaponPtr.Tick(DeltaTime, bFiring, bMoving, bIsCrouched, bIsADSed, bIsReloadPressed);
}

//When switching from secondary immediately to primary, always set bIsSecondary to complete false.
#define LOADOUT_SWITCH_HELPER(Number) 	 

//idk if want to use this code.
void LoadoutConfigState::MaybeSwitchWeapon()
{
	int Previous_Item = -1;
	bool Previous_bIsSecondary = false;

	//Weapon switching.
	if (GetAsyncKeyState(0x31))//1 key
	{
		WeaponSwitchHelper(1, Previous_Item, Previous_bIsSecondary);
	}
	else if (GetAsyncKeyState(0x32))//2 key
	{
		WeaponSwitchHelper(2, Previous_Item, Previous_bIsSecondary);
	}
	else if (GetAsyncKeyState(0x33))//3 key
	{
		WeaponSwitchHelper(3, Previous_Item, Previous_bIsSecondary);
	}
	else
	{
		WeaponSwitchHelper(-1, Previous_Item, Previous_bIsSecondary);
	}
}

bool LoadoutConfigState::WeaponSwitchHelper(int NewNumber, int& Previous_Item, bool& Previous_bIsSecondary)
{
	if (CurEq == NewNumber)
	{
		if (WeaponConfigStates_Secondaries.IsValidIndex(NewNumber))
		{
			//Only allow equip if it's capable
			Previous_Item = CurEq;
			Previous_bIsSecondary = bIsSecondary;
			bIsSecondary = !bIsSecondary;
		}
	}
	else
	{
		//if (WeaponConfigStates_Primaries[]
		Previous_Item = CurEq;
		Previous_bIsSecondary = false;
		CurEq = NewNumber;
		bIsSecondary = false;
	}
}
