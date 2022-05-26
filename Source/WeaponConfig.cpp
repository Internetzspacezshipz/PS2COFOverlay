#include <cmath>
#include "WeaponConfig.h"

//Yucky copy pasta
void WeaponConeConfig::Serialize(bool bSerializing, nlohmann::json& Json)
{
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, Minimum);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, Maximum);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, PelletSpread);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, Bloom);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, RecoveryRate);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, GrowRate);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, RecoveryDelay);
}

void RangeGradiationConfig::Serialize(bool bSerializing, nlohmann::json& Json)
{
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, bUseGradiation);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, Scale);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, Linearity);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, NumMarks);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, NumInvisibleMarks);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, MarkDistanceInterval);
}

void WeaponFireModeConfig::Serialize(bool bSerializing, nlohmann::json& Json)
{
	JSON_SERIALIZE_OBJECT(Json, bSerializing, RangeGradiation);

	JSON_SERIALIZE_VARIABLE(Json, bSerializing, Zoom);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, ROF);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, ShotsPerBurst);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, DelayBeforeFire);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, bAllowsFireCancel);
}

void WeaponFireGroupConfig::Serialize(bool bSerializing, nlohmann::json& Json)
{
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, FireGroupName);
	JSON_SERIALIZE_OBJECT_ARRAY(Json, bSerializing, FireModeConfigs, WeaponFireModeConfig);
}

void WeaponConfig::Serialize(bool bSerializing, nlohmann::json& Json)
{
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, Name);
	JSON_SERIALIZE_OBJECT_ARRAY(Json, bSerializing, FireGroups, WeaponFireGroupConfig);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, MagSize);

	JSON_SERIALIZE_VARIABLE(Json, bSerializing, ReloadTimeShort);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, ReloadTimeLong);

	JSON_SERIALIZE_VARIABLE(Json, bSerializing, TransitionToTime);
	JSON_SERIALIZE_VARIABLE(Json, bSerializing, TransitionFromTime);
}
