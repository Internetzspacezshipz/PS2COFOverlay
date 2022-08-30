#pragma once
//General incl
#include <windows.h>

//Proj incl
#include "DataLoader.h"

//Boost incl
#include "boost/array.hpp"

static bool AnyKeyPressed(const std::vector<int>& Keys)
{
	for (short i = 0; i < Keys.size(); i++)
	{
		if (GetAsyncKeyState(Keys[i]))
		{
			return true;
		}
	}
	return false;
}

struct UserSettings
	: public SerializerInterface
{
	float FOV = 74.f;
	float FontSize = 0.9f;

	bool bToggleCrouch = false;
	bool bToggleADS = false;
	bool bInfiniteAmmo = true;

	boost::array<float, 4> CrosshairColor { 1.f, 1.f, 0.f, 1.f };
	boost::array<float, 2> CrosshairShape { 15, 2 };

	//Menu Keys
	int Keycode_Open = 0x72; //VK_F3;
	int Keycode_Quit = 0x71; //VK_F2;
	int Keycode_Deactivate = 0x75; //VK_F6;

	//Play keys
	std::vector<int> Playcode_Crouch = 
	{
		0x43,//C
		0x63,//c
	};

	std::vector<int> Playcode_Move = 
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

	std::vector<int> Playcode_Fire = 
	{
		0x01,//LMB
	};

	std::vector<int> Playcode_Aim = 
	{
		0x02,//RMB
	};

	std::vector<int> Playcode_Reload = 
	{
		0x52,//R
		0x72,//r
	};

	static UserSettings& Get()
	{
		static UserSettings US = UserSettings();
		return US;
	}

	void Reset()
	{
		*this = UserSettings();
	};

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
}; 
