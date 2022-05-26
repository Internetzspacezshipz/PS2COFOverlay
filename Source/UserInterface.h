#pragma once
//Proj includes
#include "LoadoutConfigState.h"

//Displays and allows loadout editing.
struct LoadoutPanel
{
	//void DrawUI(LoadoutConfigState& CurrentLoadoutState);
};

//Displays and allows editing of weapons.
struct WeaponPanel
{
	void DrawUI(WeaponConfig& Weapon);
};

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

	//Do overall draw
	void DrawUI(LoadoutConfigState& CurrentLoadoutState);

private:
	//UI private parts ;)


};
