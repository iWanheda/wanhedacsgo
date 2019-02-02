#include "night_mode.h"

void cnight_mode::do_nightmode() {

	static bool nightmode_performed = false, nightmode_lastsetting;

	if (!g_csgo::Engine->IsConnected() || !g_csgo::Engine->IsInGame()) {

		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (!local_player) {

		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	if (nightmode_lastsetting != settings::options.night_bool) {

		nightmode_lastsetting = settings::options.night_bool;
		nightmode_performed = false;
	}

	if (!nightmode_performed) {

		static SDK::ConVar* r_DrawSpecificStaticProp = g_csgo::cvar->FindVar("r_DrawSpecificStaticProp");
		r_DrawSpecificStaticProp->nFlags &= ~FCVAR_CHEAT;

		static SDK::ConVar* sv_skyname = g_csgo::cvar->FindVar("sv_skyname");
		sv_skyname->nFlags &= ~FCVAR_CHEAT;

		for (SDK::MaterialHandle_t i = g_csgo::MaterialSystem->FirstMaterial(); i != g_csgo::MaterialSystem->InvalidMaterial(); i = g_csgo::MaterialSystem->NextMaterial(i)) {

			SDK::IMaterial *pMaterial = g_csgo::MaterialSystem->GetMaterial(i);

			if (!pMaterial)
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), "World")) {

				if (settings::options.night_bool)
					pMaterial->ColorModulate(0.08, 0.08, 0.05);
				else
					pMaterial->ColorModulate(1, 1, 1);

				if (settings::options.night_bool) {

					sv_skyname->SetValue("sky_csgo_night02");
					pMaterial->SetMaterialVarFlag(SDK::MATERIAL_VAR_TRANSLUCENT, false);
					pMaterial->ColorModulate(0.05, 0.05, 0.05);
				} else {

					sv_skyname->SetValue("vertigoblue_hdr");
					pMaterial->ColorModulate(1.00, 1.00, 1.00);
				}
			} else if (strstr(pMaterial->GetTextureGroupName(), "StaticProp")) {

				if (settings::options.night_bool)
					pMaterial->ColorModulate(0.28, 0.28, 0.28);
				else
					pMaterial->ColorModulate(1, 1, 1);
			}
		} nightmode_performed = true;
	}
}

cnight_mode* nightmode = new cnight_mode();