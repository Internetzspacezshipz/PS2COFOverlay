#pragma once

//JSON INCL
#include "../Ext/Json/json.hpp"
#include "DataLoader.h"
#include "../Ext/boost_1_79_0/boost/scoped_array.hpp"

//https://liquidwarp.honu.pw/stats/weapons/infantry/nsx-naginata-804249.html
struct WeaponConeConfig 
	: public SerializerInterface
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

	virtual void Serialize(bool bSerializing, nlohmann::json& Json) override;

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

struct RangeGradiationConfig
	: public SerializerInterface
{
	//Whether we should use this gradiation in the UI
	bool bUseGradiation;

	//The scale of the gradiation marks
	float Scale;

	//The linearity of the gradiation marks
	float Linearity;

	//The number of marks on the gradiation scale
	int NumMarks;
	//Number of marks that we should not show (eg. for short distances)
	int NumInvisibleMarks;

	//The distance displayed on each of the gradiation marks.
	int MarkDistanceInterval;

	virtual void Serialize(bool bSerializing, nlohmann::json& Json) override;

	RangeGradiationConfig() :
		bUseGradiation(false),
		Scale(0.f),
		Linearity(0.f),
		NumMarks(0),
		NumInvisibleMarks(0),
		MarkDistanceInterval(0)
	{
	}
};

struct WeaponFireModeConfig 
	: public SerializerInterface
{
	RangeGradiationConfig RangeGradiation;

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

	virtual void Serialize(bool bSerializing, nlohmann::json& Json) override;
};

//TODO: allow toggleable fire modes. (eg. TRAP-M1's 2 burst fire modes)
//This should also transmit through the \

struct WeaponFireGroupConfig
	: public SerializerInterface
{
	std::vector<WeaponFireModeConfig> FireModeConfigs;

	//Gets the inner configuration of the current fire group based on whether we're adsing or not.
	const WeaponFireModeConfig& GetFireModeConfig(int Type) const { return FireModeConfigs[Type]; }
	WeaponFireModeConfig& GetFireModeConfig(int Type) { return FireModeConfigs[Type]; }

	virtual void Serialize(bool bSerializing, nlohmann::json& Json) override;

	WeaponFireGroupConfig() 
	{
		//add at least one.
		FireModeConfigs.push_back(WeaponFireModeConfig());
	}
};

struct WeaponConfig
	: public SerializerInterface
{
	//Usually only one of these. Sometimes there can be more,
	//if a weapon has a different fire mode (eg full auto vs burst)
	std::vector<WeaponFireGroupConfig> FireGroups;

	//Number of bullets the magazine holds.
	int MagSize;

	//Short reload (tactical reload) in seconds
	float ReloadTimeShort;

	//Long reload (empty magazine reload) in seconds
	float ReloadTimeLong;

	//Ensure you only use these after making the FireGroups array.
	const WeaponFireGroupConfig& GetFireGroupConfig(int Index) const { return FireGroups[Index]; }
	WeaponFireGroupConfig& GetFireGroupConfig(int Index) { return FireGroups[Index]; }
	
	virtual void Serialize(bool bSerializing, nlohmann::json& Json) override;

	WeaponConfig() :
		FireGroups(),
		MagSize(0),
		ReloadTimeLong(0.f),
		ReloadTimeShort(0.f)
	{
		//add at least one.
		FireGroups.push_back(WeaponFireGroupConfig());
	};
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
