#include "nightmode.h"
#include "SDK/ConVar.h"
#include "includes.h"
#include "UTILS/interfaces.h"
/*
bool executed = false;

SDK::ConVar * sv_skyname = nullptr;
std::string fallback_skybox = "";

std::vector<MaterialBackup> materials;
std::vector<MaterialBackup> skyboxes;

void nightmode::clear_stored_materials() {
	fallback_skybox = "";
	sv_skyname = nullptr;
	materials.clear();
	skyboxes.clear();
}

void nightmode::modulate(SDK::MaterialHandle_t i, SDK::IMaterial *material, bool backup = false) {
	if (strstr(material->GetTextureGroupName(), "World")) {
		if (backup) materials.push_back(MaterialBackup(i, material));

		material->ColorModulate(0.15f, 0.15f, 0.15f);
	}
	else if (strstr(material->GetTextureGroupName(), "StaticProp")) {
		if (backup)
			materials.push_back(MaterialBackup(i, material));

		material->ColorModulate(0.4f, 0.4f, 0.4f);
	}
}

void nightmode::apply() {

	if (executed) {
		return;
	}

	executed = true;

	if (!sv_skyname) {
		sv_skyname = g_csgo::cvar->FindVar("sv_skyname");
		sv_skyname->nFlags &= ~FCVAR_CHEAT;
	}

	fallback_skybox = sv_skyname->GetString();
	sv_skyname->SetValue("sky_csgo_night02");

	SDK::ConVar* specific_prop = g_csgo::cvar->FindVar("r_drawspecificstaticprop");
	specific_prop->SetValue(false);

	if (materials.size()) {
		for (int i = 0; i < (int)materials.size(); i++)
			modulate(materials[i].handle, materials[i].material);

		return;
	}

	materials.clear();

	for (SDK::MaterialHandle_t i = g_csgo::MaterialSystem->FirstMaterial(); i != g_csgo::MaterialSystem->InvalidMaterial(); i = g_csgo::MaterialSystem->NextMaterial(i)) {
		SDK::IMaterial* material = g_csgo::MaterialSystem->GetMaterial(i);

		if (!material) {
			continue;
		}

		modulate(i, material, true);
	}
}

void nightmode::remove() {
	if (!executed) {
		return;
	}

	executed = false;
	settings::options.night_bool = false;

	if (sv_skyname) {
		sv_skyname->SetValue(fallback_skybox.c_str());
	}

	SDK::ConVar* specific_prop = g_csgo::cvar->FindVar("r_drawspecificstaticprop");
	specific_prop->SetValue(true);

	for (int i = 0; i < materials.size(); i++) {
		if (materials[i].material->GetReferenceCount() <= 0) continue;

		materials[i].restore();
		materials[i].material->Refresh();
	}

	materials.clear();

	sv_skyname = nullptr;
	fallback_skybox = "";
}*/