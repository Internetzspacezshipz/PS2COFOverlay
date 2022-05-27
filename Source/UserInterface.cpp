//Project
#include "UserInterface.h"
#include "UserSettings.h"

//imgui
#include "../Ext/imgui/imgui.h"
#include "../Ext/imgui/imgui_stdlib.h"

void UserInterface::DrawLoadout()
{
	if (ImGui::TreeNode("Edit Loadout"))
	{
		LoadoutConfigState& CurrentLoadoutState = GetActiveEditedLoadout();
		auto& Array1 = CurrentLoadoutState.WeaponConfigStates_Primaries;
		auto& Array2 = CurrentLoadoutState.WeaponConfigStates_Secondaries;

		ImGui::InputText("Loadout Name", &CurrentLoadoutState.LoadoutName);

		ImGui::Separator();

		if (ImGui::Button("Add weapon"))
		{
			Array1.emplace_back(WeaponConfigState());
		}

		if (Array1.size())
		{
			ImGui::SameLine();
			if (ImGui::Button("Remove weapon"))
			{
				if (Array2.size() >= Array1.size())
				{
					Array2.pop_back();
				}
				Array1.pop_back();
			}
		}

		if (Array2.size() < Array1.size())
		{
			ImGui::SameLine();
			if (ImGui::Button("Add underbarrel weapon"))
			{
				Array2.emplace_back(WeaponConfigState());
			}
		}

		if (Array2.size())
		{
			ImGui::SameLine();
			if (ImGui::Button("Remove underbarrel weapon"))
			{
				Array2.pop_back();
			}
		}

		for (int i = 0; i < Array1.size(); i++)
		{
			ImGui::PushID(i);

			ImGui::Text("%i", i+1);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(300.f);
			ImGui::InputText("Weapon Type", &Array1[i].WeaponName);

			if (i < Array2.size())
			{
				ImGui::PushID(INT_MAX - i);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(300.f);
				ImGui::InputText("Underbarrel Type", &Array2[i].WeaponName);

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		if (ImGui::Button("Save Loadout"))
		{
			nlohmann::json SaveJson = nlohmann::json();
			CurrentLoadoutState.Serialize(true, SaveJson);
			DataLoader::Get().SaveUserLoadoutConfig(CurrentLoadoutState.LoadoutName, SaveJson);
			
			//Reverse load
			nlohmann::json LoadJson = DataLoader::Get().LoadUserLoadoutConfig(CurrentLoadoutState.LoadoutName);
			CurrentLoadoutState.Serialize(false, LoadJson);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Loadout"))
		{
			nlohmann::json LoadJson = DataLoader::Get().LoadUserLoadoutConfig(CurrentLoadoutState.LoadoutName);
			CurrentLoadoutState.Serialize(false, LoadJson);
		}

		ImGui::TreePop();
	}
}


void UserInterface::DrawWeapon()
{
	if (ImGui::TreeNode("Edit Weapon"))
	{
		std::string Description("Unfortunately weapons are laid out in a really weird fashion in game, so this might be confusing.\nRecommend checking out https://liquidwarp.honu.pw/stats/weapons/infantry-list.html for stats in this format.");
		ImGui::InputTextMultiline("", &Description, ImVec2(1000, 50), ImGuiInputTextFlags_ReadOnly);

		WeaponConfig& EditedWeaponConfig = GetActiveEditedWeapon(EditedWeaponName);

		ImGui::InputText("Weapon Name", &EditedWeaponName);

		if (ImGui::Button("Save weapon"))
		{
			nlohmann::json LoadedWeaponJson = nlohmann::json();
			EditedWeaponConfig.Serialize(true, LoadedWeaponJson);
			EditedWeaponConfig.Name = EditedWeaponName;
			DataLoader::Get().SaveWeaponJson(EditedWeaponName, LoadedWeaponJson);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load weapon"))
		{
			nlohmann::json LoadedWeaponJson = DataLoader::Get().LoadWeaponJson(EditedWeaponName);
			EditedWeaponConfig.Serialize(false, LoadedWeaponJson);
			EditedWeaponConfig.Name = EditedWeaponName;
		}

		ImGui::Separator();

		ImGui::InputInt("Mag size", &EditedWeaponConfig.MagSize);
		ImGui::InputFloat("Reload time short", &EditedWeaponConfig.ReloadTimeShort);
		ImGui::InputFloat("Reload time long", &EditedWeaponConfig.ReloadTimeLong);

		ImGui::InputFloat("Equip time", &EditedWeaponConfig.TransitionToTime);
		ImGui::InputFloat("Unequip time", &EditedWeaponConfig.TransitionFromTime);

		if (ImGui::Button("Add group"))
		{
			EditedWeaponConfig.FireGroups.push_back(WeaponFireGroupConfig());
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove group"))
		{
			EditedWeaponConfig.FireGroups.pop_back();
		}

		for (int i = 0; i < EditedWeaponConfig.FireGroups.size(); i++)
		{
			DrawWeaponFireGroup(EditedWeaponConfig, i);
		}

		ImGui::TreePop();
	}
}

void UserInterface::DrawWeaponFireGroup(WeaponConfig& EditedWeaponConfig, int GroupIndex)
{
	ImGui::PushID(GroupIndex);

	ImGui::Text("Fire Group %i", GroupIndex + 1);
	ImGui::SameLine();

	if (ImGui::TreeNode("Edit"))
	{
		WeaponFireGroupConfig& Group = EditedWeaponConfig.FireGroups[GroupIndex];
		ImGui::InputFloat("Fire group switch time", &Group.FireModeSwitchTime);

		if (ImGui::Button("Add fire mode"))
		{
			Group.FireModeConfigs.push_back(WeaponFireModeConfig());
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove fire mode"))
		{
			Group.FireModeConfigs.pop_back();
		}

		std::vector<int> FireModesToDupe{};

		for (int i = 0; i < Group.FireModeConfigs.size(); i++)
		{
			ImGui::PushID(INT_MAX - i);
			ImGui::Text("Fire Mode %i", i + 1);
			ImGui::SameLine();
			if (ImGui::TreeNode("Edit"))
			{
				if (i == 0)
				{
					ImGui::Text("NOTE: This is usually hipfire stats. ");
				}
				else if (i == 1)
				{
					ImGui::Text("NOTE: This is usually ADS stats. Will be active on ADS button press.");
				}

				if (ImGui::Button("Duplicate Fire Mode"))
				{
					FireModesToDupe.push_back(i);
				}

				WeaponFireModeConfig& FireMode = Group.FireModeConfigs[i];

				ImGui::InputFloat("Zoom", &FireMode.Zoom);
				ImGui::InputInt("Rate of fire", &FireMode.ROF);
				ImGui::InputInt("Shots per burst (0 = full auto)", &FireMode.ShotsPerBurst);
				ImGui::InputFloat("Fire Mode Delay Before Fire", &FireMode.DelayBeforeFire);
				ImGui::Checkbox("Allows Fire Cancel", &FireMode.bAllowsFireCancel);

				DrawWeaponConeOfFire(FireMode.Standing, 0);
				DrawWeaponConeOfFire(FireMode.Crouching, 1);
				DrawWeaponConeOfFire(FireMode.StandingMove, 2);
				DrawWeaponConeOfFire(FireMode.CrouchingMove, 3);

				ImGui::TreePop();
			}
			ImGui::PopID();
		}

		for (auto& DupeFireMode : FireModesToDupe)
		{
			//Copy firemode
			Group.FireModeConfigs.push_back(Group.FireModeConfigs[DupeFireMode]);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void UserInterface::DrawWeaponConeOfFire(WeaponConeConfig& Cone, int GroupIndex)
{
	ImGui::PushID(GroupIndex + (INT_MAX/2));

	const char* Name;

	switch (GroupIndex)
	{
	case 0: Name = "Standing:"; break;
	case 1: Name = "Crouching:"; break;
	case 2: Name = "Standing Move:"; break;
	case 3: Name = "Crouching Move:"; break;
	default: Name = "";
	}

	if (ImGui::TreeNode(Name))
	{
		ImGui::InputFloat("Maximum", &Cone.Maximum);
		ImGui::InputFloat("Minimum", &Cone.Minimum);

		ImGui::InputFloat("Bloom per shot", &Cone.Bloom);
		ImGui::InputFloat("Recovery rate", &Cone.RecoveryRate);
		ImGui::InputFloat("Recovery delay", &Cone.RecoveryDelay);
		ImGui::InputFloat("Grow rate", &Cone.GrowRate);

		ImGui::InputFloat("Pellet spread (unused currently)", &Cone.PelletSpread);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void UserInterface::DrawUserSettings()
{
	if (ImGui::TreeNode("Edit settings"))
	{
		auto& US = UserSettings::Get();

		ImGui::InputFloat("FOV", &US.FOV);

		ImGui::InputFloat("Font Size", &US.FontSize);
		
		ImGui::ColorPicker4("Crosshair Color", &US.CrosshairColor[0]);

		ImGui::InputFloat2("Crosshair shape", &US.CrosshairShape[0]);

		ImGui::Checkbox("InfiniteAmmo", &US.bInfiniteAmmo);

		ImGui::TreePop();
	}
}

WeaponConfigState* DebugState = nullptr;

void UserInterface::DrawUI(LoadoutConfigState& CurrentLoadoutState)
{
	if (bMenuOpen)
	{
		_ActiveEditedLoadoutPtr = &CurrentLoadoutState;

		ImGui::Begin("Test", &bMenuOpen);

		DrawLoadout();

		ImGui::Separator();

		DrawWeapon();

		ImGui::Separator();

		DrawUserSettings();

		if (DebugState)
		{
			ImGui::Separator();

			ImGui::Text("Mag %i", DebugState->CurrentBullets);
			ImGui::Text("Reload time %f", DebugState->TimeToReload.TimeUntilCompleted);
			ImGui::Text("Time to fire %f", DebugState->TimeToNextFire.TimeUntilCompleted);
		}

		ImGui::End();
	}
}

void UserInterface::DrawStateDebug(WeaponConfigState& State)
{
	if (!DebugState)
	{
		DebugState = &State;
	}
	else
	{
		DebugState = nullptr;
	}
}

UserInterface::UserInterface()
{
	ImGui::GetIO().FontGlobalScale = UserSettings::Get().FontSize;
}

