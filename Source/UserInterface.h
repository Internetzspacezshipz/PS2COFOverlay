#pragma once
//Proj includes
#include "LoadoutConfigState.h"

struct UserInterface
{

	bool bMenuOpen = false;

	//returns the current state of bMenuOpen
	bool ToggleMenuOpen()
	{
		bMenuOpen = !bMenuOpen;
		return bMenuOpen;
	}

	static UserInterface& Get()
	{
		static UserInterface UI = UserInterface();
		return UI;
	}

	void DrawLoadout();
	void DrawWeapon();
	void DrawWeaponFireGroup(WeaponConfig& EditedWeaponConfig, int GroupIndex);
	void DrawWeaponConeOfFire(WeaponConeConfig& Cone, int GroupIndex);
	void DrawStateDebug(WeaponConfigState& State);
	void DrawUserSettings();

	//Do overall draw
	void DrawUI(LoadoutConfigState& CurrentLoadoutState);

private:
	//UI private parts ;)

	UserInterface();

	LoadoutConfigState _UnloadedEditedLoadout;
	LoadoutConfigState* _ActiveEditedLoadoutPtr;

	LoadoutConfigState& GetActiveEditedLoadout()
	{
		if (_ActiveEditedLoadoutPtr)
		{
			return *_ActiveEditedLoadoutPtr;
		}
		else
		{
			return _UnloadedEditedLoadout;
		}
	}

	std::string EditedWeaponName;
	boost::shared_ptr<WeaponConfig> _ActiveEditedWeaponPtr;

	WeaponConfig& GetActiveEditedWeapon(std::string Name)
	{
		if (_ActiveEditedLoadoutPtr)
		{
			_ActiveEditedLoadoutPtr->VisitWeaponConfigs(
			[&](WeaponConfigState& Config)->bool
			{
				if (Config.WeaponName == Name)
				{
					_ActiveEditedWeaponPtr = Config.Config;
					return false;
				}
				return true;
			});
		}

		if (_ActiveEditedWeaponPtr)
		{
			return *_ActiveEditedWeaponPtr;
		}
		else
		{
			_ActiveEditedWeaponPtr = boost::shared_ptr<WeaponConfig>(new WeaponConfig());
			return *_ActiveEditedWeaponPtr;
		}
	}
};
