#pragma once

//General incl
#include <windows.h>

//Proj incl
#include "WeaponConfig.h"
#include "MiscUtils.inl"

//Boost incl
#include "boost/shared_ptr.hpp"

//Output of Ticking functions
struct TickOutput
{
	float CurrentZoom;
	float CurrentConeOfFire;
};

//For storing weapon state based off of the weapon config.
struct WeaponConfigState
	: public SerializerInterface
{
	//The static configuration of the weapon.
	boost::shared_ptr<WeaponConfig> Config;

	std::string WeaponName;

	//This determines whether the user has let go of LMB after pressing it,
	//allowing the next burst to occur (for burst weapons)
	bool bHasUnlatched = true;
	//Whether the player pressed the reload button.
	bool bIsReloading = false;

	int CurrentBullets = 0;
	int ShotsLeftInBurst = 0; // 0 means it is not going to fire yet.

	float CurrentCOF = 0.f;

	//The current fire group in the FireGroup array. Usually 0.
	int CurrentFireGroup = 0;

	MiscUtils::Countdown<float> TimeToNextFire;//Amount of time before the next fire is available.
	MiscUtils::Countdown<float> TimeToRecovery;//The amount of time until recovery starts. Usually zero, but is reset whenever lmb is held.
	MiscUtils::Countdown<float> DelayBeforeFiring;//The amount of time until we can fire for weapons with delays before firing.
	MiscUtils::Countdown<float> TimeToReload;//Amount of time until reload completed...

	void SetFullAmmo() { CurrentBullets = Config->MagSize; };

	//Outputs the current zoom level for adjustment of AngleToPixel calc
	TickOutput Tick(
		float DeltaTime,
		bool bTryingToFiring,
		bool bMoving,
		bool bIsCrouched,
		bool bIsADSed,
		bool bReloadPressed);

	//Call when the weapon was switched off of, which will cause this weapon to reset certain countdowns (reload)
	void WeaponSwitchedOff();

	//Creates and loads Config according to the WeaponName
	bool LoadWeaponConfig();

	//Time in miliseconds between shots.
	//float GetRefireTime(bool bIsADS) const { return 1000. / (Config.GetInnerConfig(bIsADS).ROF / 60.); }
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
};

struct LoadoutConfigState
	: public SerializerInterface
{
	std::string LoadoutName;

	//Somewhat complex, this is a 2d array of weapon config states.
	//the first dimension is for when you switch to another actual weapon,
	//the second is for a secondary type of weapon in the same slot (eg underbarrel grenade launchers or shotguns)
	std::vector<WeaponConfigState> WeaponConfigStates_Primaries;
	std::vector<WeaponConfigState> WeaponConfigStates_Secondaries;

	//The index of the above array in which our current weapon is.
	int CurEq = -1;
	bool bIsSecondary = false;

	MiscUtils::Countdown<float> TimeBeforeFrom;//Amount of time until weapon has been switched off of
	MiscUtils::Countdown<float> TimeBeforeTo;//Amount of time until weapon has been switched to

	bool IsSwitching() { return !TimeBeforeTo.Completed(); }
	void SetSwitching(float FromTime, float ToTime);

	//Handles weapon inputs.
	//Outputs the current zoom level.
	TickOutput Tick(float DeltaTime);

	void MaybeSwitchWeapon();

	bool WeaponSwitchHelper(int NewNumber, int& Previous_Item, bool& Previous_bIsSecondary);

	WeaponConfigState& GetWeaponConfigState(int Index, bool bIsSecondary)
	{
		return bIsSecondary ? WeaponConfigStates_Secondaries[Index] : WeaponConfigStates_Primaries[Index];
	}

	WeaponConfigState& GetCurrentEquipped()
	{
		return GetWeaponConfigState(CurEq, bIsSecondary);
	}

	template<typename Type>
	void VisitWeaponConfigs(Type Visitor)
	{
		for (auto& Elem : WeaponConfigStates_Primaries)
		{
			if (!Visitor(Elem))
			{
				return;
			}
		}

		for (auto& Elem : WeaponConfigStates_Secondaries)
		{
			if (!Visitor(Elem))
			{
				return;
			}
		}
	}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
};

