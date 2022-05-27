#pragma once

//Boost
#include "boost/filesystem.hpp"
#include "../Ext/Json/json.hpp"

//Serialization helper macros.

#define JSON_SERIALIZE_VARIABLE(JsonVariable, bIsSerializing, VariableName)	   \
if (bIsSerializing)															   \
{																			   \
	JsonVariable[""#VariableName""] = VariableName;							   \
}																			   \
else if (JsonVariable.contains(""#VariableName""))							   \
{																			   \
	VariableName = JsonVariable[""#VariableName""];							   \
}

#define JSON_SERIALIZE_OBJECT(JsonVariable, bIsSerializing, VariableName)	   \
if (bIsSerializing)															   \
{																			   \
	nlohmann::json NewObject;												   \
	VariableName.Serialize(bIsSerializing, NewObject);						   \
	JsonVariable[""#VariableName""] = NewObject;							   \
}																			   \
else  if (JsonVariable.contains(""#VariableName""))							   \
{																			   \
	nlohmann::json InnerObject = JsonVariable[""#VariableName""];			   \
	VariableName.Serialize(bIsSerializing, InnerObject);					   \
}

#define JSON_SERIALIZE_OBJECT_ARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType)	\
if (bIsSerializing)																			\
{																							\
	nlohmann::json NewArray = nlohmann::json::array({});								\
	for (auto& Elem : ArrayName)															\
	{																						\
		nlohmann::json NewObject;															\
		Elem.Serialize(bSerializing, NewObject);											\
		NewArray.push_back(NewObject);														\
	}																						\
	JsonVariable[""#ArrayName""] = NewArray;												\
}																							\
else if (JsonVariable.contains(""#ArrayName""))												\
{																							\
	ArrayName.clear();																		\
	auto& InnerArray = JsonVariable[""#ArrayName""];										\
	for (auto& Elem : InnerArray)															\
	{																						\
		ObjectType NewSubObject;															\
		NewSubObject.Serialize(bSerializing, Elem);											\
		ArrayName.push_back(NewSubObject);													\
	}																						\
}


#define JSON_SERIALIZE_ARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType)	\
if (bIsSerializing)																	\
{																					\
	nlohmann::json NewArray = nlohmann::json::array({});						\
	for (auto& Elem : ArrayName)													\
	{																				\
		NewArray.push_back(Elem);													\
	}																				\
	JsonVariable[""#ArrayName""] = NewArray;										\
}																					\
else if (JsonVariable.contains(""#ArrayName""))										\
{																					\
	ArrayName.clear(); 																\
	auto& InnerArray = JsonVariable[""#ArrayName""];								\
	for (auto& Elem : InnerArray)													\
	{																				\
		ArrayName.push_back(Elem);													\
	}																				\
}

#define JSON_SERIALIZE_CARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType)	\
if (bIsSerializing)																	\
{																					\
	nlohmann::json NewArray = nlohmann::json::array({});						\
	for (auto& Elem : ArrayName)													\
	{																				\
		NewArray.push_back(Elem);													\
	}																				\
	JsonVariable[""#ArrayName""] = NewArray;										\
}																					\
else if (JsonVariable.contains(""#ArrayName""))										\
{																					\
	auto& InnerArray = JsonVariable[""#ArrayName""];								\
	for (int i = 0; i < ArrayName.size(); i++)										\
	{																				\
		ArrayName[i] = InnerArray[i];												\
	}																				\
}

struct SerializerInterface
{
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) = 0;
};


//CONST FOLDER LOCATIONS
const boost::filesystem::path UseSettingsFileName = "Settings";

const boost::filesystem::path SettingsSubFolder = "Settings";
const boost::filesystem::path WeaponsSubFolder = "Settings/Weapons";
const boost::filesystem::path LoadoutsSubFolder = "Settings/Loadouts";

const std::string FileExtension = ".json";

//You're never gonna believe what this class does.
class DataLoader
{
	//Total path to given folders for simplicity of use.
	boost::filesystem::path SettingsFolderPath;
	boost::filesystem::path WeaponsFolderPath;
	boost::filesystem::path LoadoutsFolderPath;

	nlohmann::json LoadFileToJson(boost::filesystem::path Path) const;
	bool SaveJsonToFile(boost::filesystem::path Path, const nlohmann::json& Json) const;

	DataLoader();
public:

	static DataLoader& Get()
	{
		static DataLoader DL = DataLoader();
		return DL;
	}

	nlohmann::json LoadUserSettings() const;
	nlohmann::json LoadWeaponJson(const std::string& WeaponName) const;
	nlohmann::json LoadUserLoadoutConfig(const std::string& SpecifiedLoadout) const;

	bool SaveUserSettings(nlohmann::json& InJson) const;
	bool SaveWeaponJson(const std::string& WeaponName, nlohmann::json& InJson) const;
	bool SaveUserLoadoutConfig(const std::string& SpecifiedLoadout, nlohmann::json& InJson) const;
};
