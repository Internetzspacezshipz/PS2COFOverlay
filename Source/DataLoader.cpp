//Main
#include "DataLoader.h"

//Boost
#include "boost/filesystem/fstream.hpp"



DataLoader::DataLoader() :
	SettingsFolderPath(boost::filesystem::current_path() / SettingsSubFolder),
	WeaponsFolderPath(boost::filesystem::current_path() / WeaponsSubFolder),
	LoadoutsFolderPath(boost::filesystem::current_path() / LoadoutsSubFolder)
{
}

nlohmann::json DataLoader::LoadFileToJson(boost::filesystem::path Path) const
{
	boost::filesystem::ifstream File;
	File.open(Path, std::ios_base::in);
	if (!File.is_open())
	{
		return nlohmann::json();
	}
	std::string Str = std::string();
	File >> Str;
	return nlohmann::json(Str);
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
