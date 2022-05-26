#pragma once

#include "DataLoader.h"

struct UserSettings
	: public SerializerInterface
{
	float FOV = 74.f;
	bool bToggleCrouch = false;
	bool bToggleADS = false;

	//TODO EVENTUALLY: rebindable keys

	static UserSettings& Get()
	{
		static UserSettings US = UserSettings();
		return US;
	}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
}; 
