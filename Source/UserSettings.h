#pragma once

#include "DataLoader.h"

struct UserSettings
	: public SerializerInterface
{
	float FOV;
	bool bToggleCrouch;
	bool bToggleADS;

	//TODO EVENTUALLY: rebindable keys

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
}; 
