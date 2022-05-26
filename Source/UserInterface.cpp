//Project
#include "UserInterface.h"

//imgui
#include "../Ext/imgui/imgui.h"
#include "../Ext/imgui/imgui_stdlib.h"

void DrawLoadout(LoadoutConfigState& CurrentLoadoutState)
{
	auto& Array1 = CurrentLoadoutState.WeaponConfigStates_Primaries;
	auto& Array2 = CurrentLoadoutState.WeaponConfigStates_Secondaries;

	ImGui::InputText("Loadout Name", &CurrentLoadoutState.LoadoutName);

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

	ImGui::SameLine();
	if (Array2.size() < Array1.size() && ImGui::Button("Add underbarrel weapon"))
	{
		Array2.emplace_back(WeaponConfigState());
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
		//Buggy rn fix pls UI instances are not separate in a for loop. Need some way of resolving that.
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Weapon Type", &Array1[i].WeaponName);

		if (i < Array2.size())
		{
			ImGui::SameLine();
			ImGui::SetNextItemWidth(300.f);
			ImGui::InputText("Underbarrel Type", &Array2[i].WeaponName);
		}
	}
}

void WeaponPanel::DrawUI(WeaponConfig& Weapon)
{

}

void UserInterface::DrawUI(LoadoutConfigState& CurrentLoadoutState)
{
	if (bMenuOpen)
	{
		//ImGui::ColorPicker4("Colors", &ImGui::GetStyle().Colors[ImGuiCol_WindowBg].x);
		ImGui::Begin("Test", &bMenuOpen);

		DrawLoadout(CurrentLoadoutState);
		//TEST SHIT.
		if (ImGui::Button("Save Loadout", ImVec2(150, 20)))
		{
			ImGui::Text("SSSSSSSSSSSSSSSSSSSSSSSSSSS");
		}
		ImGui::Text("WHATEVER TEST SHIT THIS IS");

		ImGui::End();
	}
}

