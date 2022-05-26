//Main
#include "DataLoader.h"

//Boost
#include "boost/filesystem/fstream.hpp"




nlohmann::json DataLoader::LoadFileToJson(boost::filesystem::path Path) const
{
	boost::filesystem::ifstream File;
	File.open(Path, std::ios_base::in);
	if (!File.is_open() || File.fail())
	{
		return nlohmann::json();
	}
	nlohmann::json OutJson;
	File >> OutJson;

	if (OutJson.size() == 0 || File.fail())
	{
		return nlohmann::json();
	}
	return OutJson;
}

bool DataLoader::SaveJsonToFile(boost::filesystem::path Path, const nlohmann::json& InJson) const
{
	boost::filesystem::ofstream File(Path, std::ios_base::out);
	if (!File.is_open() || File.fail())
	{
		return false;
	}
	File << InJson;
	File.close();
	return File.fail();
}

DataLoader::DataLoader() :
	SettingsFolderPath(boost::filesystem::current_path() / SettingsSubFolder),
	WeaponsFolderPath(boost::filesystem::current_path() / WeaponsSubFolder),
	LoadoutsFolderPath(boost::filesystem::current_path() / LoadoutsSubFolder)
{
	boost::filesystem::create_directories(SettingsFolderPath);
	boost::filesystem::create_directories(WeaponsFolderPath);
	boost::filesystem::create_directories(LoadoutsFolderPath);
}

nlohmann::json DataLoader::LoadUserSettings() const
{
	return LoadFileToJson(SettingsFolderPath / UseSettingsFileName);
}

nlohmann::json DataLoader::LoadWeaponJson(const std::string& WeaponName) const
{
	return LoadFileToJson(WeaponsFolderPath / WeaponName);
}

nlohmann::json DataLoader::LoadUserLoadoutConfig(const std::string& SpecifiedLoadout) const
{
	return LoadFileToJson(LoadoutsFolderPath / SpecifiedLoadout);
}

bool DataLoader::SaveUserSettings(nlohmann::json& InJson) const
{
	return SaveJsonToFile(SettingsFolderPath / UseSettingsFileName, InJson);
}

bool DataLoader::SaveWeaponJson(const std::string& WeaponName, nlohmann::json& InJson) const
{
	return SaveJsonToFile(WeaponsFolderPath / WeaponName, InJson);
}

bool DataLoader::SaveUserLoadoutConfig(const std::string& SpecifiedLoadout, nlohmann::json& InJson) const
{
	return SaveJsonToFile(LoadoutsFolderPath / SpecifiedLoadout, InJson);
}
