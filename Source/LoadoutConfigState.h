#pragma once

//Proj incl
#include "WeaponConfig.h"
#include "XDynamicArray.h"

//G incl
#include <windows.h>

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

	//The current fire group in the FireGroup array. Usually 0.
	int CurrentFireGroup = 0;

	Countdown<float> TimeToNextFire;//Amount of time before the next fire is available.
	Countdown<float> TimeToRecovery;//The amount of time until recovery starts. Usually zero, but is reset whenever lmb is held.
	Countdown<float> DelayBeforeFiring;//The amount of time until we can fire for weapons with delays before firing.
	Countdown<float> TimeToReload;//Amount of time until reload completed...

	//Outputs the current zoom level for adjustment of AngleToPixel calc
	float Tick(
		float DeltaTime,
		bool bTryingToFiring,
		bool bMoving,
		bool bIsCrouched,
		bool bIsADSed,
		bool bReloadPressed);

	//Call when the weapon was switched off of, which will cause this weapon to reset certain countdowns (reload)
	void WeaponSwitchedOff();

	//Time in miliseconds between shots.
	//float GetRefireTime(bool bIsADS) const { return 1000. / (Config.GetInnerConfig(bIsADS).ROF / 60.); }
};

//Loadout intermediate serialization object.
struct LoadoutConfig
	: public SerializerInterface
{
	//Too lazy to set this up in a multidimensional array.
	//This is the whole loadout essentially.
	//These strings should point to weapon configs in the weapons folder.
	std::vector<std::string> LoadoutSlot_Primaries;
	std::vector<std::string> LoadoutSlot_Secondaries;

	//Creates config states for LoadoutConfigState. This is dumb.
	XDynamicArray<WeaponConfigState> CreateWeaponConfigStates(bool bSecondaries) const;

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
};

struct LoadoutConfigState
{
	//Somewhat complex, this is a 2d array of weapon config states.
	//the first dimension is for when you switch to another actual weapon,
	//the second is for a secondary type of weapon in the same slot (eg underbarrel grenade launchers or shotguns)
	XDynamicArray<WeaponConfigState> WeaponConfigStates_Primaries;
	XDynamicArray<WeaponConfigState> WeaponConfigStates_Secondaries;

	//The index of the above array in which our current weapon is.
	int CurEq = 0;
	bool bIsSecondary = false;

	//Handles weapon inputs.
	//Outputs the current zoom level.
	float Tick(float DeltaTime);

	void MaybeSwitchWeapon();

	bool WeaponSwitchHelper(int NewNumber, int& Previous_Item, bool& Previous_bIsSecondary);

	LoadoutConfigState(const LoadoutConfig& InLoadoutConfig)
	{
		WeaponConfigStates_Primaries = InLoadoutConfig.CreateWeaponConfigStates(false);
		WeaponConfigStates_Secondaries = InLoadoutConfig.CreateWeaponConfigStates(true);
	}
};

