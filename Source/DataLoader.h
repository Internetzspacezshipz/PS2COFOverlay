#pragma once

//Boost
#include "boost/filesystem.hpp"
#include "boost/array.hpp"
#include "boost/shared_array.hpp"
#include "../Ext/Json/json.hpp"

//#include "boost/scoped_array.hpp"

#define JSON_SERIALIZE_VARIABLE(JsonVariable, bIsSerializing, VariableName)	   \
if (bIsSerializing)															   \
{																			   \
	JsonVariable.push_back({ ""#VariableName"", VariableName});				   \
}																			   \
else																		   \
{																			   \
	VariableName = JsonVariable.find(""#VariableName"").value();			   \
}

#define JSON_SERIALIZE_OBJECT(JsonVariable, bIsSerializing, VariableName)	   \
if (bIsSerializing)															   \
{																			   \
	nlohmann::json NewObject;												   \
	VariableName.Serialize(bIsSerializing, NewObject);						   \
	JsonVariable.push_back({ ""#VariableName"", NewObject});				   \
}																			   \
else																		   \
{																			   \
	nlohmann::json InnerObject = JsonVariable[""#VariableName""];			   \
	VariableName.Serialize(bIsSerializing, InnerObject);					   \
}

#if  0
if (bSerializing)
{
	nlohmann::json NewArray = nlohmann::json::array({});
	for (auto& Elem : FireGroups)
	{
		nlohmann::json NewObject;
		Elem.Serialize(bSerializing, NewObject);
		NewArray.push_back(NewObject);
	}
	Json.emplace_back(json{ "ArrayName", NewArray });
}
else
{
	auto& InnerArray = Json.find("ArrayName").value();
	for (auto& Elem : InnerArray)
	{
		WeaponFireGroupConfig NewSubObject;
		NewSubObject.Serialize(bSerializing, Elem);
		FireGroups.push_back(NewSubObject);
	}
}
#endif

//above ^^^^

#define JSON_SERIALIZE_OBJECT_ARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType)\
if (bIsSerializing)																  \
{																				  \
	nlohmann::json NewArray = nlohmann::json::array({});					\
	for (auto& Elem : ArrayName)												  \
	{																			  \
		nlohmann::json NewObject;												  \
		Elem.Serialize(bSerializing, NewObject);								  \
		NewArray.push_back(NewObject);											  \
	}																			  \
	JsonVariable.emplace_back(nlohmann::json{ ""#ArrayName"", NewArray });		 \
}																				  \
else																			  \
{																				  \
	auto& InnerArray = JsonVariable.find(""#ArrayName"").value();				\
	for (auto& Elem : InnerArray)												  \
	{																			  \
		ObjectType NewSubObject;												  \
		NewSubObject.Serialize(bSerializing, Elem);								  \
		ArrayName.push_back(NewSubObject);										  \
	}																			  \
}


#define JSON_SERIALIZE_ARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType) \
if (bIsSerializing)																  \
{																				  \
	nlohmann::json NewArray = nlohmann::json::array({});					  \
	for (auto& Elem : ArrayName)												  \
	{																			  \
		NewArray.push_back(Elem);												  \
	}																			  \
	JsonVariable.emplace_back(nlohmann::json{ ""#ArrayName"", NewArray });				  \
}																				  \
else																			  \
{																				  \
	auto& InnerArray = JsonVariable.find(""#ArrayName"").value();						  \
	for (auto& Elem : InnerArray)												  \
	{																			  \
		ArrayName.push_back(Elem);												  \
	}																			  \
}


struct SerializerInterface
{
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) = 0;
};


//CONST FOLDER LOCATIONS
const boost::filesystem::path UseSettingsFileName = "Settings.json";

const boost::filesystem::path SettingsSubFolder = "Settings";
const boost::filesystem::path WeaponsSubFolder = "Weapons";
const boost::filesystem::path LoadoutsSubFolder = "Loadouts";

const std::string BaseLoadoutName = "Loadout_";
const std::string BaseWeaponName = "Weapon_";

//You're never gonna believe what this class does.
class DataLoader
{
	//Total path to given folders for simplicity of use.
	boost::filesystem::path SettingsFolderPath;
	boost::filesystem::path WeaponsFolderPath;
	boost::filesystem::path LoadoutsFolderPath;

public:
	DataLoader();

	nlohmann::json LoadFileToJson(boost::filesystem::path Path) const;

	nlohmann::json LoadUserSettings() const;
	nlohmann::json LoadWeaponJson(const std::string& WeaponName) const;
	nlohmann::json LoadUserLoadoutConfig(const std::string& SpecifiedLoadout) const;
};
