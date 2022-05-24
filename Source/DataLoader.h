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
	VariableName = JsonVariable[""#VariableName""];							   \
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

#define JSON_SERIALIZE_ARRAY(JsonVariable, bIsSerializing, ArrayName, ObjectType)\
if (bIsSerializing)																  \
{																				  \
	nlohmann::json NewArray = nlohmann::json::array({});						  \
	for (auto& Elem : ArrayName)												  \
	{																			  \
		nlohmann::json NewObject;												  \
		Elem.Serialize(bSerializing, NewObject);								  \
		NewArray.push_back(NewObject);											  \
	}																			  \
	JsonVariable.emplace_back(json{ ""#ArrayName"", NewArray });				  \
}																				  \
else																			  \
{																				  \
	auto& InnerArray = Json.find(""#ArrayName"").value();						  \
	for (auto& Elem : InnerArray)												  \
	{																			  \
		ObjectType NewSubObject;												  \
		NewSubObject.Serialize(bSerializing, Elem);								  \
		ArrayName.push_back(NewSubObject);										  \
	}																			  \
}


struct SerializerInterface
{
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) = 0;
};

//You're never gonna believe what this class does.
class DataLoader
{
	//Gets set to data folder path
	boost::filesystem::path DataFolderPath;

public:
	DataLoader();

	void GetWeaponConfigNames(boost::shared_array<std::string>& OutArray);
};
