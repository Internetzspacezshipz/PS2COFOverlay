#include "UserSettings.h"

void UserSettings::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, FOV);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, FontSize);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bToggleCrouch);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bToggleADS);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bInfiniteAmmo);

	//Menu keys
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Keycode_Open);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Keycode_Quit);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Keycode_Deactivate);
	
	//Play keys
	JSON_SERIALIZE_ARRAY(TargetJson, bSerializing, Playcode_Crouch, int);
	JSON_SERIALIZE_ARRAY(TargetJson, bSerializing, Playcode_Move, int);
	JSON_SERIALIZE_ARRAY(TargetJson, bSerializing, Playcode_Fire, int);
	JSON_SERIALIZE_ARRAY(TargetJson, bSerializing, Playcode_Aim, int);
	JSON_SERIALIZE_ARRAY(TargetJson, bSerializing, Playcode_Reload, int);

	JSON_SERIALIZE_CARRAY(TargetJson, bSerializing, CrosshairColor, float);
	JSON_SERIALIZE_CARRAY(TargetJson, bSerializing, CrosshairShape, float);
}
