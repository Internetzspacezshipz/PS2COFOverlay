#include <cmath>

#include "WeaponConfig.h"

float WeaponConfigState::Tick(float DeltaTime, 
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

	const WeaponFireModeConfig& CurrentFireModeConfig = Config->GetFireModeConfig(bIsADSed);
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
