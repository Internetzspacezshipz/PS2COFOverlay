#pragma once

#include "DataLoader.h"
#include "boost/array.hpp"

struct UserSettings
	: public SerializerInterface
{
	float FOV = 74.f;
	float FontSize = 0.9f;

	bool bToggleCrouch = false;
	bool bToggleADS = false;
	bool bInfiniteAmmo = false;

	boost::array<float, 4> CrosshairColor { 1.f, 1.f, 0.f, 1.f };
	boost::array<float, 2> CrosshairShape { 15, 2 };

	//TODO EVENTUALLY: rebindable keys

	static UserSettings& Get()
	{
		static UserSettings US = UserSettings();
		return US;
	}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
}; 
