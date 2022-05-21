#pragma once

//JSON INCL
#include "..\Ext\Json\json.hpp"

//https://liquidwarp.honu.pw/stats/weapons/infantry/nsx-naginata-804249.html
struct WeaponConeConfig
{
	float Minimum;
	float Maximum;

	//Others
	//TODO: use this.
	float PelletSpread;

	float Bloom;
	float RecoveryRate;
	float GrowRate;

	//ms after firing that the cone recovery begins. Usually 0.
	float RecoveryDelay;

	WeaponConeConfig() :
		Minimum(0.f),
		Maximum(0.f),
		Bloom(0.f),
		GrowRate(50.f),
		RecoveryRate(20.f),
		PelletSpread(0.f),
		RecoveryDelay(0.f)
	{}
};

struct WeaponFireModeConfig
{
	//Default ADS seems to be 1.35
	//Default hip is 1
	float Zoom;

	//ROF in rounds per minute
	int ROF;

	// 0 == automatic
	// 1 == single fire
	// 2,3,4,5 etc == burst
	int ShotsPerBurst = -1;

	//the amount of time to wait before firing (Kuwa/Yumi/others). Defaults to -1.f (off).
	float DelayBeforeFire = -1.f;

	//Applicable with weapons with fire delays.
	//This determines whether or not letting go of LMB will cancel the firing (Kuwa)
	//Set to true for allowing it to cancel, and false for disallowing cancels (Yumi)
	bool bAllowsFireCancel = true;

	const WeaponConeConfig& GetConeConfig(bool bMoving, bool bCrouching) const
	{ 
		return bCrouching ? 
			bMoving ? CrouchingMove : Crouching : 
			bMoving ? StandingMove : Standing ;
	}

	WeaponConeConfig& GetConeConfig(bool bMoving, bool bCrouching)
	{ 
		return bCrouching ?
			bMoving ? CrouchingMove : Crouching :
			bMoving ? StandingMove : Standing ;
	}

	WeaponConeConfig Standing;
	WeaponConeConfig Crouching;
	WeaponConeConfig StandingMove; //Known as 'running' cone of fire in the https://liquidwarp.honu.pw/ website.
	WeaponConeConfig CrouchingMove;
};

struct WeaponConfig
{
	//Hipfire fire configuration
	WeaponFireModeConfig HipConfig;

	//Aim down sights fire configuration
	WeaponFireModeConfig ADSConfig;

	//Number of bullets the magazine holds.
	int MagSize;

	//Short reload (tactical reload)
	float ReloadTimeShort;

	//Long reload (empty magazine reload)
	float ReloadTimeLong;

	//Gets the inner configuration of the current fire mode based on whether we're adsing or not.
	const WeaponFireModeConfig& GetFireModeConfig(bool bADS) const { return bADS ? ADSConfig : HipConfig; }
	WeaponFireModeConfig& GetFireModeConfig(bool bADS) { return bADS ? ADSConfig : HipConfig; }

	WeaponConfig() :
		HipConfig(),
		ADSConfig(),
		MagSize(0),
		ReloadTimeLong(0.f),
		ReloadTimeShort(0.f)
	{};
};

//Countdown state system
template<typename ValueType>
struct Countdown
{
	ValueType TimeUntilCompleted = 0;

	ValueType Val() const { return TimeUntilCompleted; }
	bool Completed() const { return TimeUntilCompleted <= 0; }

	void Set(ValueType Time)
	{
		TimeUntilCompleted = Time;
	}

	void Tick(ValueType ReduceBy)
	{
		if (TimeUntilCompleted > 0)
		{
			TimeUntilCompleted -= ReduceBy;
		}
	}
};

//For storing weapon state based off of the weapon config.
struct WeaponConfigState
{
	//The static configuration of the weapon.
	WeaponConfig* Config = nullptr;

	//This determines whether the user has let go of LMB after pressing it,
	//allowing the next burst to occur (for burst weapons)
	bool bHasUnlatched = true;
	//Whether the player pressed the reload button.
	bool bIsReloading = false;

	int CurrentBullets = 0;
	int ShotsLeftInBurst = 0; // 0 means it is not going to fire yet.

	float CurrentCOF = 0.f;

	Countdown<float> TimeToNextFire;//Amount of time before the next fire is available.
	Countdown<float> TimeToRecovery;//The amount of time until recovery starts. Usually zero, but is reset whenever lmb is held.
	Countdown<float> DelayBeforeFiring;//The amount of time until we can fire for weapons with delays before firing.
	Countdown<float> TimeToReload;//Amount of time until reload completed...

	//Outputs the current zoom level for adjustment of AngleToPixel calc
	float Tick(float DeltaTime, 
		bool bTryingToFiring, 
		bool bMoving, 
		bool bIsCrouched, 
		bool bIsADSed, 
		bool bReloadPressed);

	//Time in miliseconds between shots.
	//float GetRefireTime(bool bIsADS) const { return 1000. / (Config.GetInnerConfig(bIsADS).ROF / 60.); }
};

