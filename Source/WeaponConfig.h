#pragma once

//#include "..\Ext\SimpleJson\json.hpp"

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

	WeaponConeConfig() :
		Minimum(0.f),
		Maximum(0.f),
		Bloom(0.f),
		GrowRate(50.f),
		RecoveryRate(20.f),
		PelletSpread(0.f)
	{}
};

struct WeaponFireModeConfig
{
	//Default ADS seems to be 1.35
	//Default hip is 1
	float Zoom;

	//ROF in rounds per minute
	int ROF;

	// 0 == auto
	// 1 == single fire
	// 2,3,4,5 etc == burst
	int ShotsPerFire = -1;

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
	//HIPFIRE
	WeaponFireModeConfig HipConfig;

	//ADS
	WeaponFireModeConfig ADSConfig;

	int MagSize;

	const WeaponFireModeConfig& GetInnerConfig(bool bADS) const { return bADS ? ADSConfig : HipConfig; }
	WeaponFireModeConfig& GetInnerConfig(bool bADS) { return bADS ? ADSConfig : HipConfig; }

	WeaponConfig();
};

//For storing weapon state based off of the weapon config.
struct WeaponConfigState
{
	WeaponConfig Config;

	float CurrentCOF = 0.f;
	int CurrentBullets = 0;

	bool bIsADS;


	void Fire(bool bMoving, bool bIsCrouched, bool bIsADSed);

	//Sets the state of ADS.
	void SetADS(bool bNewADS) { bIsADS = bNewADS; }

	//Time in miliseconds between shots.
	double GetRefireTime() const { return 1000. / (Config.GetInnerConfig(bIsADS).ROF / 60.); }
};