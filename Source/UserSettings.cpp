#include "UserSettings.h"

void UserSettings::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, FOV);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bToggleCrouch);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bToggleADS);
}
