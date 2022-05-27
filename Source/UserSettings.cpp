#include "UserSettings.h"

void UserSettings::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, FOV);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, FontSize);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bToggleCrouch);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bToggleADS);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bInfiniteAmmo);

	JSON_SERIALIZE_CARRAY(TargetJson, bSerializing, CrosshairColor, float);
	JSON_SERIALIZE_CARRAY(TargetJson, bSerializing, CrosshairShape, float);
}
