/*#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/IViewRenderBeams.h"
#include "../FEATURES/Backtracking.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseWeapon.h"
#include "../FEATURES/AutoWall.h"
#include "../SDK/CTrace.h"	
#include "../FEATURES/Resolver.h"
#include "../SDK/CGlobalVars.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Visuals.h"
#include "../UTILS/render.h"
#include "../SDK/IVDebugOverlay.h"
#include <string.h>

//--- Misc Variable Initalization ---//
float flPlayerAlpha[255];
CColor breaking;
CColor backtrack;
static bool bPerformed = false, bLastSetting;
float fade_alpha[65];
float dormant_time[65];
CColor main_color;
CColor ammo;
SDK::CBaseEntity *BombCarrier;

void CVisuals::set_hitmarker_time(float time)
{
	global::flHurtTime = time;
}
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
void CVisuals::Draw()
{
	if (!g_csgo::Engine->IsInGame()) {
		global::flHurtTime = 0.f;
		return;
	}
	DrawCrosshair();
	for (int i = 1; i <= 65; i++)
	{
		auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!entity) continue;
		if (!local_player) continue;

		bool is_local_player = entity == local_player;
		bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

		if (is_local_player) continue;
		if (entity->GetHealth() <= 0) continue;
		if (entity->GetVecOrigin() == Vector(0, 0, 0)) continue;

		//--- Colors ---//
		if (entity->GetIsDormant() && flPlayerAlpha[i] > 0) flPlayerAlpha[i] -= .3;
		else if (!entity->GetIsDormant() && flPlayerAlpha[i] < 255) flPlayerAlpha[i] = 255;

		float playerAlpha = flPlayerAlpha[i];
		int enemy_hp = entity->GetHealth();
		int hp_red = 255 - (enemy_hp * 2.55);
		int hp_green = enemy_hp * 2.55;
		CColor health_color = CColor(hp_red, hp_green, 1, playerAlpha);
		CColor dormant_color = CColor(100, 100, 100, playerAlpha);
		CColor box_color, still_health, alt_color, zoom_color, name_color, weapon_color, distance_color, arrow_col;

		static auto alpha = 0.f; static auto plus_or_minus = false;
		if (alpha <= 0.f || alpha >= 255.f) plus_or_minus = !plus_or_minus;
		alpha += plus_or_minus ? (255.f / 7 * 0.015) : -(255.f / 7 * 0.015); alpha = clamp(alpha, 0.f, 255.f);
		float arrow_colour[3] = { settings::options.fov_col.RGBA[0], settings::options.fov_col.RGBA[1], settings::options.fov_col.RGBA[2] };
		float arrowteam_colour[3] = { settings::options.arrowteam_col.RGBA[0], settings::options.arrowteam_col.RGBA[1], settings::options.arrowteam_col.RGBA[2] };

		if (entity->GetIsDormant())
		{
			main_color = dormant_color;
			still_health = health_color;
			alt_color = CColor(20, 20, 20, playerAlpha);
			zoom_color = dormant_color;
			breaking = dormant_color;
			backtrack = dormant_color;
			box_color = dormant_color;
			ammo = dormant_color;
			name_color = dormant_color;
			weapon_color = dormant_color;
			distance_color = dormant_color;
			arrow_col = dormant_color;
		}
		else if (!entity->GetIsDormant())
		{
			main_color = CColor(255, 255, 255, playerAlpha);
			still_health = health_color;
			alt_color = CColor(0, 0, 0, 165);
			zoom_color = CColor(150, 150, 220, 165);
			breaking = CColor(220, 150, 150, 165);
			backtrack = CColor(155, 220, 150, 165);
			box_color = settings::options.box_col;
			ammo = CColor(61, 135, 255, 165);
			name_color = settings::options.name_col;
			weapon_color = settings::options.weapon_col;
			distance_color = settings::options.distance_col;
			arrow_col = settings::options.fov_col;
		}
		Vector min, max, pos, pos3D, top, top3D; entity->GetRenderBounds(min, max);
		pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10); top3D = pos3D + Vector(0, 0, max.z + 10);

		if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
		{
			if (!is_teammate)
			{
				if (settings::options.box_bool) DrawBox(entity, box_color, pos, top);
				if (settings::options.name_bool) DrawName(entity, name_color, i, pos, top);
				if (settings::options.weap_bool) DrawWeapon(entity, weapon_color, i, pos, top);
				if (settings::options.health_bool) DrawHealth(entity, still_health, alt_color, pos, top);
				if (settings::options.ammo_bool) DrawAmmo(entity, ammo, alt_color, pos, top);
			}
			else if (is_teammate)
			{
				if (settings::options.boxteam) DrawBox(entity, settings::options.boxteam_col, pos, top);
				if (settings::options.nameteam) DrawName(entity, settings::options.nameteam_col, i, pos, top);
				if (settings::options.weaponteam) DrawWeapon(entity, settings::options.weaponteam_col, i, pos, top);
				if (settings::options.healthteam) DrawHealth(entity, still_health, alt_color, pos, top);
				if (settings::options.ammoteam) DrawAmmo(entity, ammo, alt_color, pos, top);
			}
			DrawInfo(entity, main_color, zoom_color, pos, top);
		}
	}
}

void CVisuals::ClientDraw()
{
	if (settings::options.spread_bool == 1) DrawInaccuracy();
	if (settings::options.spread_bool == 2) DrawInaccuracy1();
	if (settings::options.scope_bool) DrawBorderLines();
	penetration_reticle();
	DrawIndicator();
	DrawHitmarker();
	misc_visuals();
	//Clantag();
	cbullet_tracer tracer;
	
	tracer.render();
}

std::string str_to_upper(std::string strToConvert)
{
	std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

	return strToConvert;
}

void CVisuals::DrawBox(SDK::CBaseEntity* entity, CColor color, Vector pos, Vector top)
{
	float alpha = flPlayerAlpha[entity->GetIndex()];
	int height = (pos.y - top.y), width = height / 2;

	RENDER::DrawEmptyRect(pos.x - width / 2, top.y, (pos.x - width / 2) + width, top.y + height, color);
	RENDER::DrawEmptyRect((pos.x - width / 2) + 1, top.y + 1, (pos.x - width / 2) + width - 1, top.y + height - 1, CColor(20, 20, 20, alpha));
	RENDER::DrawEmptyRect((pos.x - width / 2) - 1, top.y - 1, (pos.x - width / 2) + width + 1, top.y + height + 1, CColor(20, 20, 20, alpha));
}

void CVisuals::DrawName(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top)
{
	SDK::player_info_t ent_info; g_csgo::Engine->GetPlayerInfo(index, &ent_info);

	int height = (pos.y - top.y), width = height / 2;
	RENDER::DrawF(pos.x, top.y - 7, FONTS::visuals_esp_font, true, true, color, ent_info.szName);
}

float CVisuals::resolve_distance(Vector src, Vector dest)
{
	Vector delta = src - dest;
	float fl_dist = ::sqrtf((delta.Length()));
	if (fl_dist < 1.0f) return 1.0f;
	return fl_dist;
}

void CVisuals::DrawDistance(SDK::CBaseEntity* entity, CColor color, Vector pos, Vector top)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	SDK::player_info_t ent_info;
	Vector vecOrigin = entity->GetVecOrigin(), vecOriginLocal = local_player->GetVecOrigin();

	char dist_to[32]; int height = (pos.y - top.y), width = height / 2;

	sprintf_s(dist_to, "%.0f FT", resolve_distance(vecOriginLocal, vecOrigin));
	RENDER::DrawF(pos.x, settings::options.ammo_bool ? pos.y + 12 : pos.y + 8, FONTS::visuals_esp_font, true, true, color, dist_to);
}

std::string fix_item_name(std::string name)
{
	if (name[0] == 'C')
		name.erase(name.begin());

	auto startOfWeap = name.find("Weapon");
	if (startOfWeap != std::string::npos)
		name.erase(name.begin() + startOfWeap, name.begin() + startOfWeap + 6);

	return name;
}

void CVisuals::DrawWeapon(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top)
{
	SDK::player_info_t ent_info; g_csgo::Engine->GetPlayerInfo(index, &ent_info);

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	if (!weapon) return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	if (!c_baseweapon) return;

	bool is_teammate = local_player->GetTeam() == entity->GetTeam(), distanceThing, distanceThing2;
	if (settings::options.ammo_bool) distanceThing = true; else distanceThing = false; if (settings::options.ammoteam) distanceThing2 = true; else distanceThing2 = false;
	int height = (pos.y - top.y), width = height / 2, distanceOn = distanceThing ? pos.y + 12 : pos.y + 8, distanceOn2 = distanceThing2 ? pos.y + 12 : pos.y + 8;

	if (c_baseweapon->is_revolver())
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "R8 REVOLVER");

	else if (c_baseweapon->GetItemDefenitionIndex() == SDK::WEAPON_USP_SILENCER)
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "USP-S");

	else if (c_baseweapon->GetItemDefenitionIndex() == SDK::WEAPON_M4A1_SILENCER)
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "M4A1-S");
	else
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, fix_item_name(weapon->GetClientClass()->m_pNetworkName));
}

void CVisuals::DrawHealth(SDK::CBaseEntity* entity, CColor color, CColor dormant, Vector pos, Vector top)
{
	int enemy_hp = entity->GetHealth(),
		hp_red = 255 - (enemy_hp * 2.55),
		hp_green = enemy_hp * 2.55,
		height = (pos.y - top.y),
		width = height / 2;

	float offset = (height / 4.f) + 5;
	UINT hp = height - (UINT)((height * enemy_hp) / 100);

	RENDER::DrawEmptyRect((pos.x - width / 2) - 6, top.y, (pos.x - width / 2) - 3, top.y + height, dormant);
	RENDER::DrawLine((pos.x - width / 2) - 4, top.y + hp, (pos.x - width / 2) - 4, top.y + height, color);
	RENDER::DrawLine((pos.x - width / 2) - 5, top.y + hp, (pos.x - width / 2) - 5, top.y + height, color);

	if (entity->GetHealth() < 100)
		RENDER::DrawF((pos.x - width / 2) - 4, top.y + hp, FONTS::visuals_esp_font, true, true, main_color, std::to_string(enemy_hp));
}

void CVisuals::penetration_reticle() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()); if (!local_player) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex())); if (!weapon) return;

	Vector aimPoint = local_player->GetEyePosition();

	int screen_x, screen_y;
	g_csgo::Engine->GetScreenSize(screen_x, screen_y);

	if (AutoWall->GetDamagez(aimPoint) >= 1.f)//settings::options.damage_val)
		RENDER::DrawFilledRect(screen_x / 2, screen_y / 2, 2, 2, CColor(0, 255, 0));
	else
		RENDER::DrawFilledRect(screen_x / 2, screen_y / 2, 2, 2, CColor(255, 0, 0));
}

void CVisuals::BombPlanted(SDK::CBaseEntity* entity)
{
	BombCarrier = nullptr;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	Vector vOrig; Vector vScreen;
	vOrig = entity->GetVecOrigin();
	SDK::CCSBomb* Bomb = (SDK::CCSBomb*)entity;

	float flBlow = Bomb->GetC4BlowTime();
	float TimeRemaining = flBlow;// -(g_csgo::Globals->interval_per_tick * local_player->GetTickBase());
	char buffer[64];
	sprintf_s(buffer, "B - %.1fs", TimeRemaining);
	RENDER::DrawF(10, 10, FONTS::visuals_lby_font, false, false, CColor(124, 195, 13, 255), buffer);
}

void CVisuals::DrawDropped(SDK::CBaseEntity* entity)
{
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);

	SDK::CBaseWeapon* weapon_cast = (SDK::CBaseWeapon*)entity;

	if (!weapon_cast)
		return;

	auto weapon = g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));

	if (!c_baseweapon)
		return;

	if (!weapon)
		return;

	SDK::CBaseEntity* plr = g_csgo::ClientEntityList->GetClientEntityFromHandle((HANDLE)weapon_cast->GetOwnerHandle());
	if (!plr && RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		std::string ItemName = fix_item_name(weapon->GetClientClass()->m_pNetworkName);
		int height = (pos.y - top.y);
		int width = height / 2;
		RENDER::DrawF(pos.x, pos.y, FONTS::visuals_esp_font, true, true, WHITE, ItemName.c_str()); //numpad_menu_font
	}
}

void CVisuals::DrawAmmo(SDK::CBaseEntity* entity, CColor color, CColor dormant, Vector pos, Vector top)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	if (!c_baseweapon) return;

	int height = (pos.y - top.y);

	float offset = (height / 4.f) + 5;
	UINT hp = height - (UINT)((height * 3) / 100);

	auto animLayer = entity->GetAnimOverlay(1);
	if (!animLayer.m_pOwner)
		return;

	auto activity = entity->GetSequenceActivity(animLayer.m_nSequence);

	int iClip = c_baseweapon->GetLoadedAmmo();
	int iClipMax = c_baseweapon->get_full_info()->max_clip;

	float box_w = (float)fabs(height / 2);
	float width;
	if (activity == 967 && animLayer.m_flWeight != 0.f)
	{
		float cycle = animLayer.m_flCycle;
		width = (((box_w * cycle) / 1.f));
	}
	else
		width = (((box_w * iClip) / iClipMax));

	RENDER::DrawFilledRect((pos.x - box_w / 2), top.y + height + 3, (pos.x - box_w / 2) + box_w + 2, top.y + height + 7, dormant); //outline
	RENDER::DrawFilledRect((pos.x - box_w / 2) + 1, top.y + height + 4, (pos.x - box_w / 2) + width + 1, top.y + height + 6, color); //ammo
}

void CVisuals::DrawInfo(SDK::CBaseEntity* entity, CColor color, CColor alt, Vector pos, Vector top)
{
	std::vector<std::pair<std::string, CColor>> stored_info;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	bool is_local_player = entity == local_player;
	bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

	if (settings::options.money_bool && !is_teammate)
		stored_info.push_back(std::pair<std::string, CColor>("$" + std::to_string(entity->GetMoney()), backtrack));
	else if (settings::options.moneyteam && is_teammate)
		stored_info.push_back(std::pair<std::string, CColor>("$" + std::to_string(entity->GetMoney()), backtrack));

	if (settings::options.info_bool && !is_teammate)
	{
		if (entity->GetArmor() > 0)
			stored_info.push_back(std::pair<std::string, CColor>(entity->GetArmorName(), color));

		if (entity->GetIsScoped())
			stored_info.push_back(std::pair<std::string, CColor>("zoom", alt));
	}
	else if (settings::options.flagsteam && is_teammate)
	{
		if (entity->GetArmor() > 0)
			stored_info.push_back(std::pair<std::string, CColor>(entity->GetArmorName(), color));

		if (entity->GetIsScoped())
			stored_info.push_back(std::pair<std::string, CColor>("zoom", alt));
	}

	int height = (pos.y - top.y), width = height / 2, i = 0;
	for (auto Text : stored_info)
	{
		RENDER::DrawF((pos.x + width / 2) + 5, top.y + i, FONTS::visuals_esp_font, false, false, Text.second, Text.first);
		i += 8;
	}
}

void CVisuals::DrawInaccuracy()
{

}
/*
void CVisuals::DrawFootSteps() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!g_csgo::Engine->IsInGame() || !local_player) { footsteps.clear(); return; }
	if (footsteps.size() > 5) 
		footsteps.pop_back();

	for (int i = 0; i < footsteps.size(); i++)
	{
		auto current = footsteps.at(i);
		if (!current.pPlayer) 
			continue;
		if (current.pPlayer->GetIsDormant()) 
			continue;

		SDK::BeamInfo_t beamInfo;
		beamInfo.m_nType = SDK::TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = settings::options.bulletlife;
		beamInfo.m_flWidth = settings::options.bulletsize;
		beamInfo.m_flEndWidth = 2.0f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current.color.RGBA[0];
		beamInfo.m_flGreen = current.color.RGBA[1];
		beamInfo.m_flBlue = current.color.RGBA[2];
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = SDK::FBEAM_ONLYNOISEONCE | SDK::FBEAM_NOTILE | SDK::FBEAM_HALOBEAM;

		beamInfo.m_vecStart = current.pPlayer->GetVecOrigin() + current.pPlayer->GetViewOffset();
		beamInfo.m_vecEnd = current.vecImpactPos;

		auto beam = g_csgo::ViewRenderBeams->CreateBeamPoints(beamInfo);
		if (beam) g_csgo::ViewRenderBeams->DrawBeam(beam);

		Impacts.erase(Impacts.begin() + i);
	}
}

void CVisuals::DrawBulletBeams()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!g_csgo::Engine->IsInGame() || !local_player) { Impacts.clear(); return; }
	if (Impacts.size() > 30) Impacts.pop_back();

	for (int i = 0; i < Impacts.size(); i++)
	{
		auto current = Impacts.at(i);
		if (!current.pPlayer) continue;
		if (current.pPlayer->GetIsDormant()) continue;

		bool is_local_player = current.pPlayer == local_player;
		bool is_teammate = local_player->GetTeam() == current.pPlayer->GetTeam() && !is_local_player;

		if (current.pPlayer == local_player)
			current.color = CColor(settings::options.bulletlocal_col[0] * 255, settings::options.bulletlocal_col[1] * 255, settings::options.bulletlocal_col[2] * 255, settings::options.bulletlocal_col[3] * 255);
		else if (current.pPlayer != local_player && !is_teammate)
			current.color = CColor(settings::options.bulletenemy_col[0] * 255, settings::options.bulletenemy_col[1] * 255, settings::options.bulletenemy_col[2] * 255, settings::options.bulletenemy_col[3] * 255);
		else if (current.pPlayer != local_player && is_teammate)
			current.color = CColor(settings::options.bulletteam_col[0] * 255, settings::options.bulletteam_col[1] * 255, settings::options.bulletteam_col[2] * 255, settings::options.bulletteam_col[3] * 255);

		SDK::BeamInfo_t beamInfo;
		beamInfo.m_nType = SDK::TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = 3;
		beamInfo.m_flWidth = 1;
		beamInfo.m_flEndWidth = 2.0f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current.color.RGBA[0];
		beamInfo.m_flGreen = current.color.RGBA[1];
		beamInfo.m_flBlue = current.color.RGBA[2];
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = SDK::FBEAM_ONLYNOISEONCE | SDK::FBEAM_NOTILE | SDK::FBEAM_HALOBEAM;

		beamInfo.m_vecStart = current.pPlayer->GetVecOrigin() + current.pPlayer->GetViewOffset();
		beamInfo.m_vecEnd = current.vecImpactPos;

		auto beam = g_csgo::ViewRenderBeams->CreateBeamPoints(beamInfo);
		if (beam) g_csgo::ViewRenderBeams->DrawBeam(beam);

		Impacts.erase(Impacts.begin() + i);
	}
}

void CVisuals::DrawCrosshair()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto crosshair = g_csgo::cvar->FindVar("crosshair");
	if (settings::options.xhair_type == 0)
	{
		crosshair->SetValue("1");
		return;
	}
	else
		crosshair->SetValue("0");

	int W, H, cW, cH;
	g_csgo::Engine->GetScreenSize(W, H);

	cW = W / 2; cH = H / 2;

	int dX = W / 120.f, dY = H / 120.f;
	int drX, drY;

	if (settings::options.xhair_type == 2)
	{
		drX = cW - (int)(dX * (((local_player->GetPunchAngles().y * 2.f) * 0.45f) + local_player->GetPunchAngles().y));
		drY = cH + (int)(dY * (((local_player->GetPunchAngles().x * 2.f) * 0.45f) + local_player->GetPunchAngles().x));
	}
	else
	{
		drX = cW;
		drY = cH;
	}

	g_csgo::Surface->DrawSetColor(BLACK);
	g_csgo::Surface->DrawFilledRect(drX - 4, drY - 2, drX - 4 + 8, drY - 2 + 4);
	g_csgo::Surface->DrawFilledRect(drX - 2, drY - 4, drX - 2 + 4, drY - 4 + 8);

	g_csgo::Surface->DrawSetColor(WHITE);
	g_csgo::Surface->DrawFilledRect(drX - 3, drY - 1, drX - 3 + 6, drY - 1 + 2);
	g_csgo::Surface->DrawFilledRect(drX - 1, drY - 3, drX - 1 + 2, drY - 3 + 6);
}


void CVisuals::DrawInaccuracy1()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon) return;

	int spread_Col[3] = { settings::options.spreadcirclecol[0], settings::options.spreadcirclecol[1], settings::options.spreadcirclecol[2] };


	int W, H, cW, cH;
	g_csgo::Engine->GetScreenSize(W, H);
	cW = W / 2; cH = H / 2;

	if (local_player->IsAlive())
	{
		auto accuracy = (weapon->GetInaccuracy() + weapon->GetSpreadCone()) * 500.f;

		float r;
		float alpha, newAlpha;

		for (r = accuracy; r > 0; r--)
		{
			if (!weapon->is_grenade() && !weapon->is_knife())


				alpha = r / accuracy;
			newAlpha = pow(alpha, 5);

			RENDER::DrawCircle(cW, cH, r, 60, CColor(spread_Col[0], spread_Col[1], spread_Col[2], newAlpha * 130));
		}
	}
}
void CVisuals::DrawIndicator()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;
	if (local_player->GetHealth() <= 0) return;

	float breaking_lby_fraction = fabs(MATH::NormalizeYaw(global::real_angles.y - local_player->GetLowerBodyYaw())) / 180.f;
	float lby_delta = abs(MATH::NormalizeYaw(global::real_angles.y - local_player->GetLowerBodyYaw()));

	int screen_width, screen_height;
	g_csgo::Engine->GetScreenSize(screen_width, screen_height);

	int iY = 88;
	if (settings::options.stop_bool)
	{
		iY += 22;
		RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, settings::options.stop_flip ? CColor(0, 255, 0) : CColor(255, 0, 0), "STOP");
	}
	if (settings::options.overrideenable)
	{
		iY += 22; bool overridekeyenabled;
		if (settings::options.overridemethod == 0)
			RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, settings::options.overridething ? CColor(0, 255, 0) : CColor(255, 0, 0), "OVERRIDE");
		else if (settings::options.overridemethod == 1)
		{
			GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(settings::options.overridekey)) ?
				RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor(0, 255, 0), "OVERRIDE") :
				RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor(255, 0, 0), "OVERRIDE");
		}
	}

	if (settings::options.aa_bool && settings::options.lbyenable)
	{
		iY += 22;
		RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor((1.f - breaking_lby_fraction) * 255.f, breaking_lby_fraction * 255.f, 0), "LBY");
	}
}

bool PrecacheModel(const char* szModelName)
{
	auto m_pModelPrecacheTable = g_csgo::ClientStringTableContainer()->FindTable("modelprecache");

	if (m_pModelPrecacheTable)
	{
		g_csgo::ModelInfo->FindOrLoadModel(szModelName);
		int idx = m_pModelPrecacheTable->AddString(false, szModelName);
		if (idx == INVALID_STRING_INDEX)
			return false;
	}
	return true;
}


void CVisuals::Drawmodels()
{
	if (settings::options.CUSTOMMODEL)
	{
		SDK::CBaseEntity* local = (SDK::CBaseEntity*)g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		for (int i = 0; i < g_csgo::ClientEntityList->GetHighestEntityIndex(); i++)
		{
			SDK::CBaseEntity *entity = (SDK::CBaseEntity*)g_csgo::ClientEntityList->GetClientEntity(i);
			SDK::player_info_t pinfo;
			if (entity == nullptr)
				continue;
			if (entity == local)
				continue;
			if (entity->GetIsDormant())
				continue;
			if (g_csgo::Engine->GetPlayerInfo(i, &pinfo))
			{
				//CustomModels(entity);
			}
		}
	}
}

void CVisuals::LogEvents()
{

}

void CVisuals::ModulateWorld()
{
	/*auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	static auto sv_skyname = g_csgo::cvar->FindVar("sv_skyname");
	sv_skyname->nFlags &= ~FCVAR_CHEAT;

	static auto static_val = 100;
	auto night_val = settings::options.daytimevalue;

	if (static_val != night_val)
	{
		static_val = night_val;
		for (auto i = g_csgo::MaterialSystem->FirstMaterial(); i != g_csgo::MaterialSystem->InvalidMaterial(); i = g_csgo::MaterialSystem->NextMaterial(i))
		{
			auto pMaterial = g_csgo::MaterialSystem->GetMaterial(i);
			if (!pMaterial) continue;

			auto finalnightval = night_val / 100.f;
			if (strstr(pMaterial->GetTextureGroupName(), "World"))
				pMaterial->ColorModulate(finalnightval, finalnightval, finalnightval);
			else if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
				night_val == 100.f ? sv_skyname->SetValue("vertigoblue_hdr") : sv_skyname->SetValue("sky_descent");

		}
	}
}
/*
void cplayer_esp::draw_beam() {

	for (int i = 1; i <= g_csgo::Globals->maxclients; i++) {

		auto entity = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(i));

		SDK::BeamInfo_t beam_info;
		beam_info.m_vecCenter = entity->GetVecOrigin();
		beam_info.m_flStartRadius = 10;
		beam_info.m_flEndRadius = 285;

		beam_info.m_pszModelName = "sprites/physbeam.vmt";
		beam_info.m_nModelIndex = -1;

		beam_info.m_nStartFrame = 0;
		beam_info.m_flFrameRate = 10;

		beam_info.m_flLife = 0.6;
		beam_info.m_flWidth = 5;
		beam_info.m_flAmplitude = 0;

		beam_info.m_flSpeed = 10;
		beam_info.m_nFlags = 0;
		beam_info.m_flFadeLength = 0;
		beam_info.m_bRenderable = true;


		SDK::Beam_t* beam = g_csgo::ViewRenderBeams->CreateBeamRingPoint(beam_info);
		g_csgo::ViewRenderBeams->DrawBeam(beam);
	}
}

void CVisuals::ModulateSky()
{
	/*static bool nightmode_performed = false, nightmode_lastsetting;

	if (!g_csgo::Engine->IsConnected() || !g_csgo::Engine->IsInGame())
	{
		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	if (nightmode_performed != settings::options.night_bool)
	{
		nightmode_lastsetting = settings::options.night_bool;
		nightmode_performed = false;
	}

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!local_player)
	{
		if (nightmode_performed) nightmode_performed = false;
		return;
	}

	if (nightmode_lastsetting != settings::options.night_bool)
	{
		nightmode_lastsetting = settings::options.night_bool;
		nightmode_performed = false;
	}

	if (!nightmode_performed)
	{

		if (settings::options.night_bool) sv_skyname->SetValue("sky_csgo_night02");
		else sv_skyname->SetValue("vertigoblue_hdr");
	}
}

void CVisuals::DrawHitmarker() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	static int lineSize = 6;

	static float alpha = 0;
	float step = 255.f / 0.3f * g_csgo::Globals->frametime;


	if (global::flHurtTime + 0.4f >= g_csgo::Globals->curtime)
		alpha = 255.f;
	else
		alpha -= step;

	if (alpha > 0) {

		int screenSizeX, screenCenterX;
		int screenSizeY, screenCenterY;
		g_csgo::Engine->GetScreenSize(screenSizeX, screenSizeY);

		screenCenterX = screenSizeX / 2;
		screenCenterY = screenSizeY / 2;

		CColor col = CColor(255, 255, 255, alpha);

		RENDER::DrawLine(screenCenterX - lineSize * 2, screenCenterY - lineSize * 2, screenCenterX - (lineSize), screenCenterY - (lineSize), col);
		RENDER::DrawLine(screenCenterX - lineSize * 2, screenCenterY + lineSize * 2, screenCenterX - (lineSize), screenCenterY + (lineSize), col);
		RENDER::DrawLine(screenCenterX + lineSize * 2, screenCenterY + lineSize * 2, screenCenterX + (lineSize), screenCenterY + (lineSize), col);
		RENDER::DrawLine(screenCenterX + lineSize * 2, screenCenterY - lineSize * 2, screenCenterX + (lineSize), screenCenterY - (lineSize), col);
	}
}

void CVisuals::DrawBorderLines() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	int width, height;
	g_csgo::Engine->GetScreenSize(width, height);

	static float rainbow;
	rainbow += 0.001;

	if (rainbow > 1.f)
		rainbow = 0.f;

	if (local_player->GetIsScoped()) {

		RENDER::DrawLine(width / 2, 0, 1, height, CColor::FromHSB(rainbow, 1.f, 1.f));
		RENDER::DrawLine(0, height / 2, width, 1, CColor::FromHSB(rainbow, 1.f, 1.f));
	}
}

void set_clan_tag(const char* tag, const char* name) {

	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)UTILS::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")));
	pSetClanTag(tag, name);
}

void CVisuals::Clantag()
{
	std::string clantagText = "wanheda";
	size_t clantagSize = clantagText.length();
	std::string spacesText(clantagSize, ' ');
	clantagText = spacesText + clantagText + spacesText;

	for (int i = 0; i < clantagSize * 2; i++)
		set_clan_tag(clantagText.substr(i, clantagSize).c_str(), "wanheda");
}

void CVisuals::misc_visuals() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	SDK::ConVar* sniper_crosshair = g_csgo::cvar->FindVar("weapon_debug_spread_show");

	if (!local_player->GetIsScoped())
		sniper_crosshair->SetValue(3);
	else
		sniper_crosshair->SetValue(0);
}

CVisuals* visuals = new CVisuals();*/

#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/IViewRenderBeams.h"
#include "../FEATURES/Backtracking.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseWeapon.h"
#include "../FEATURES/AutoWall.h"
#include "../SDK/CTrace.h"	
#include "../FEATURES/Resolver.h"
#include "../SDK/CGlobalVars.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Visuals.h"
#include "../UTILS/render.h"
#include "../SDK/IVDebugOverlay.h"
#include <string.h>

//--- Misc Variable Initalization ---//
float flPlayerAlpha[255];
CColor breaking;
CColor backtrack;
static bool bPerformed = false, bLastSetting;
float fade_alpha[65];
float dormant_time[65];
CColor main_color;
CColor ammo;
SDK::CBaseEntity *BombCarrier;

void CVisuals::set_hitmarker_time(float time)
{
	global::flHurtTime = time;
}
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
void CVisuals::Draw()
{
	if (!g_csgo::Engine->IsInGame()) {
		global::flHurtTime = 0.f;
		return;
	}
	DrawCrosshair();
	for (int i = 1; i <= 65; i++)
	{
		auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!entity) continue;
		if (!local_player) continue;

		bool is_local_player = entity == local_player;
		bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

		if (is_local_player) continue;
		if (entity->GetHealth() <= 0) continue;
		if (entity->GetVecOrigin() == Vector(0, 0, 0)) continue;

		//--- Colors ---//
		if (entity->GetIsDormant() && flPlayerAlpha[i] > 0) flPlayerAlpha[i] -= .3;
		else if (!entity->GetIsDormant() && flPlayerAlpha[i] < 255) flPlayerAlpha[i] = 255;

		float playerAlpha = flPlayerAlpha[i];
		int enemy_hp = entity->GetHealth();
		int hp_red = 255 - (enemy_hp * 2.55);
		int hp_green = enemy_hp * 2.55;
		CColor health_color = CColor(hp_red, hp_green, 1, playerAlpha);
		CColor dormant_color = CColor(100, 100, 100, playerAlpha);
		CColor box_color, still_health, alt_color, zoom_color, name_color, weapon_color, distance_color, arrow_col;

		static auto alpha = 0.f; static auto plus_or_minus = false;
		if (alpha <= 0.f || alpha >= 255.f) plus_or_minus = !plus_or_minus;
		alpha += plus_or_minus ? (255.f / 7 * 0.015) : -(255.f / 7 * 0.015); alpha = clamp(alpha, 0.f, 255.f);
		float arrow_colour[3] = { settings::options.fov_col.RGBA[0], settings::options.fov_col.RGBA[1], settings::options.fov_col.RGBA[2] };
		float arrowteam_colour[3] = { settings::options.arrowteam_col.RGBA[0], settings::options.arrowteam_col.RGBA[1], settings::options.arrowteam_col.RGBA[2] };

		if (entity->GetIsDormant())
		{
			main_color = dormant_color;
			still_health = health_color;
			alt_color = CColor(20, 20, 20, playerAlpha);
			zoom_color = dormant_color;
			breaking = dormant_color;
			backtrack = dormant_color;
			box_color = dormant_color;
			ammo = dormant_color;
			name_color = dormant_color;
			weapon_color = dormant_color;
			distance_color = dormant_color;
			arrow_col = dormant_color;
		}
		else if (!entity->GetIsDormant())
		{
			main_color = CColor(255, 255, 255, playerAlpha);
			still_health = health_color;
			alt_color = CColor(0, 0, 0, 165);
			zoom_color = CColor(150, 150, 220, 165);
			breaking = CColor(220, 150, 150, 165);
			backtrack = CColor(155, 220, 150, 165);
			box_color = settings::options.box_col;
			ammo = CColor(61, 135, 255, 165);
			name_color = settings::options.name_col;
			weapon_color = settings::options.weapon_col;
			distance_color = settings::options.distance_col;
			arrow_col = settings::options.fov_col;
		}
		Vector min, max, pos, pos3D, top, top3D; entity->GetRenderBounds(min, max);
		pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10); top3D = pos3D + Vector(0, 0, max.z + 10);

		if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
		{
			if (!is_teammate)
			{
				if (settings::options.box_bool) DrawBox(entity, box_color, pos, top);
				if (settings::options.name_bool) DrawName(entity, name_color, i, pos, top);
				if (settings::options.weap_bool) DrawWeapon(entity, weapon_color, i, pos, top);
				if (settings::options.health_bool) DrawHealth(entity, still_health, alt_color, pos, top);
				if (settings::options.ammo_bool) DrawAmmo(entity, ammo, alt_color, pos, top);
			}
			else if (is_teammate)
			{
				if (settings::options.boxteam) DrawBox(entity, settings::options.boxteam_col, pos, top);
				if (settings::options.nameteam) DrawName(entity, settings::options.nameteam_col, i, pos, top);
				if (settings::options.weaponteam) DrawWeapon(entity, settings::options.weaponteam_col, i, pos, top);
				if (settings::options.healthteam) DrawHealth(entity, still_health, alt_color, pos, top);
				if (settings::options.ammoteam) DrawAmmo(entity, ammo, alt_color, pos, top);
			}
			DrawInfo(entity, main_color, zoom_color, pos, top);
		}
	}
}

void CVisuals::ClientDraw()
{
	if (settings::options.spread_bool == 1) DrawInaccuracy();
	if (settings::options.spread_bool == 2) DrawInaccuracy1();
	if (settings::options.scope_bool) DrawBorderLines();
	penetration_reticle();
	DrawIndicator();
	DrawHitmarker();
	misc_visuals();
	//Clantag();
}

std::string str_to_upper(std::string strToConvert)
{
	std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

	return strToConvert;
}

void CVisuals::DrawBox(SDK::CBaseEntity* entity, CColor color, Vector pos, Vector top)
{
	float alpha = flPlayerAlpha[entity->GetIndex()];
	int height = (pos.y - top.y), width = height / 2;

	RENDER::DrawEmptyRect(pos.x - width / 2, top.y, (pos.x - width / 2) + width, top.y + height, color);
	RENDER::DrawEmptyRect((pos.x - width / 2) + 1, top.y + 1, (pos.x - width / 2) + width - 1, top.y + height - 1, CColor(20, 20, 20, alpha));
	RENDER::DrawEmptyRect((pos.x - width / 2) - 1, top.y - 1, (pos.x - width / 2) + width + 1, top.y + height + 1, CColor(20, 20, 20, alpha));
}

void CVisuals::DrawName(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top)
{
	SDK::player_info_t ent_info; g_csgo::Engine->GetPlayerInfo(index, &ent_info);

	int height = (pos.y - top.y), width = height / 2;
	RENDER::DrawF(pos.x, top.y - 7, FONTS::visuals_esp_font, true, true, color, ent_info.szName);
}

float CVisuals::resolve_distance(Vector src, Vector dest)
{
	Vector delta = src - dest;
	float fl_dist = ::sqrtf((delta.Length()));
	if (fl_dist < 1.0f) return 1.0f;
	return fl_dist;
}

void CVisuals::DrawDistance(SDK::CBaseEntity* entity, CColor color, Vector pos, Vector top)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	SDK::player_info_t ent_info;
	Vector vecOrigin = entity->GetVecOrigin(), vecOriginLocal = local_player->GetVecOrigin();

	char dist_to[32]; int height = (pos.y - top.y), width = height / 2;

	sprintf_s(dist_to, "%.0f FT", resolve_distance(vecOriginLocal, vecOrigin));
	RENDER::DrawF(pos.x, settings::options.ammo_bool ? pos.y + 12 : pos.y + 8, FONTS::visuals_esp_font, true, true, color, dist_to);
}

std::string fix_item_name(std::string name)
{
	if (name[0] == 'C')
		name.erase(name.begin());

	auto startOfWeap = name.find("Weapon");
	if (startOfWeap != std::string::npos)
		name.erase(name.begin() + startOfWeap, name.begin() + startOfWeap + 6);

	return name;
}

void CVisuals::DrawWeapon(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top)
{
	SDK::player_info_t ent_info; g_csgo::Engine->GetPlayerInfo(index, &ent_info);

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	if (!weapon) return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	if (!c_baseweapon) return;

	bool is_teammate = local_player->GetTeam() == entity->GetTeam(), distanceThing, distanceThing2;
	if (settings::options.ammo_bool) distanceThing = true; else distanceThing = false; if (settings::options.ammoteam) distanceThing2 = true; else distanceThing2 = false;
	int height = (pos.y - top.y), width = height / 2, distanceOn = distanceThing ? pos.y + 12 : pos.y + 8, distanceOn2 = distanceThing2 ? pos.y + 12 : pos.y + 8;

	if (c_baseweapon->is_revolver())
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "R8 REVOLVER");

	else if (c_baseweapon->GetItemDefenitionIndex() == SDK::WEAPON_USP_SILENCER)
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "USP-S");

	else if (c_baseweapon->GetItemDefenitionIndex() == SDK::WEAPON_M4A1_SILENCER)
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, "M4A1-S");
	else
		RENDER::DrawF(pos.x, is_teammate ? distanceOn2 : distanceOn, FONTS::visuals_esp_font, true, true, color, fix_item_name(weapon->GetClientClass()->m_pNetworkName));
}

void CVisuals::DrawHealth(SDK::CBaseEntity* entity, CColor color, CColor dormant, Vector pos, Vector top)
{
	int enemy_hp = entity->GetHealth(),
		hp_red = 255 - (enemy_hp * 2.55),
		hp_green = enemy_hp * 2.55,
		height = (pos.y - top.y),
		width = height / 2;

	float offset = (height / 4.f) + 5;
	UINT hp = height - (UINT)((height * enemy_hp) / 100);

	RENDER::DrawEmptyRect((pos.x - width / 2) - 6, top.y, (pos.x - width / 2) - 3, top.y + height, dormant);
	RENDER::DrawLine((pos.x - width / 2) - 4, top.y + hp, (pos.x - width / 2) - 4, top.y + height, color);
	RENDER::DrawLine((pos.x - width / 2) - 5, top.y + hp, (pos.x - width / 2) - 5, top.y + height, color);

	if (entity->GetHealth() < 100)
		RENDER::DrawF((pos.x - width / 2) - 4, top.y + hp, FONTS::visuals_esp_font, true, true, main_color, std::to_string(enemy_hp));
}
/*
SDK::C_CSPlayerResource** PlayerResource() {

	static C_CSPlayerResource ** result = nullptr;

	if (!result)
		result = *(C_CSPlayerResource***)(util::pattern_scan("client_panorama.dll", "A1 ? ? ? ? 57 85 C0 74 08") + 1);

	return result;
}

void CVisuals::rage_rs() {

	auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));

	int
		kills = (*PlayerResource())->GetKills(local_player->GetIndex()),
		deaths = (*PlayerResource())->GetDeaths(local_player->GetIndex()), x, y;

	float kdr = (kills / deaths);

	CColor colour((1.f - kdr) * 255.f, kdr * 255.f, 0);
	CColor clr(colour.RGBA[0], colour.RGBA[1], colour.RGBA[2]);

	g_csgo::Engine->GetScreenSize(x, y);

	if (kdr <= settings::options.kdr_reset)
		g_csgo::Engine->ClientCmd("rs");

	RENDER::DrawF(5, (y / 2) - 20, FONTS::visuals_lby_font, false, false, clr, "%.2f", kdr);
}*/

void CVisuals::penetration_reticle() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()); if (!local_player) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex())); if (!weapon) return;

	Vector aimPoint = local_player->GetEyePosition();

	int screen_x, screen_y;
	g_csgo::Engine->GetScreenSize(screen_x, screen_y);

	if (AutoWall->GetDamagez(aimPoint) >= 1.f)//settings::options.damage_val)
		RENDER::DrawFilledRect(screen_x / 2, screen_y / 2, 2, 2, CColor(0, 255, 0));
	else
		RENDER::DrawFilledRect(screen_x / 2, screen_y / 2, 2, 2, CColor(255, 0, 0));
}

void CVisuals::BombPlanted(SDK::CBaseEntity* entity)
{
	BombCarrier = nullptr;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	Vector vOrig; Vector vScreen;
	vOrig = entity->GetVecOrigin();
	SDK::CCSBomb* Bomb = (SDK::CCSBomb*)entity;

	float flBlow = Bomb->GetC4BlowTime();
	float TimeRemaining = flBlow;// -(g_csgo::Globals->interval_per_tick * local_player->GetTickBase());
	char buffer[64];
	sprintf_s(buffer, "B - %.1fs", TimeRemaining);
	RENDER::DrawF(10, 10, FONTS::visuals_lby_font, false, false, CColor(124, 195, 13, 255), buffer);
}

void CVisuals::DrawDropped(SDK::CBaseEntity* entity)
{
	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);

	SDK::CBaseWeapon* weapon_cast = (SDK::CBaseWeapon*)entity;

	if (!weapon_cast)
		return;

	auto weapon = g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex());
	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));

	if (!c_baseweapon)
		return;

	if (!weapon)
		return;

	SDK::CBaseEntity* plr = g_csgo::ClientEntityList->GetClientEntityFromHandle((HANDLE)weapon_cast->GetOwnerHandle());
	if (!plr && RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		std::string ItemName = fix_item_name(weapon->GetClientClass()->m_pNetworkName);
		int height = (pos.y - top.y);
		int width = height / 2;
		RENDER::DrawF(pos.x, pos.y, FONTS::visuals_esp_font, true, true, WHITE, ItemName.c_str()); //numpad_menu_font
	}
}

void CVisuals::DrawAmmo(SDK::CBaseEntity* entity, CColor color, CColor dormant, Vector pos, Vector top)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	if (!c_baseweapon) return;

	int height = (pos.y - top.y);

	float offset = (height / 4.f) + 5;
	UINT hp = height - (UINT)((height * 3) / 100);

	auto animLayer = entity->GetAnimOverlay(1);
	if (!animLayer.m_pOwner)
		return;

	auto activity = entity->GetSequenceActivity(animLayer.m_nSequence);

	int iClip = c_baseweapon->GetLoadedAmmo();
	int iClipMax = c_baseweapon->get_full_info()->max_clip;

	float box_w = (float)fabs(height / 2);
	float width;
	if (activity == 967 && animLayer.m_flWeight != 0.f)
	{
		float cycle = animLayer.m_flCycle;
		width = (((box_w * cycle) / 1.f));
	}
	else
		width = (((box_w * iClip) / iClipMax));

	RENDER::DrawFilledRect((pos.x - box_w / 2), top.y + height + 3, (pos.x - box_w / 2) + box_w + 2, top.y + height + 7, dormant); //outline
	RENDER::DrawFilledRect((pos.x - box_w / 2) + 1, top.y + height + 4, (pos.x - box_w / 2) + width + 1, top.y + height + 6, color); //ammo
}

void CVisuals::DrawInfo(SDK::CBaseEntity* entity, CColor color, CColor alt, Vector pos, Vector top)
{
	std::vector<std::pair<std::string, CColor>> stored_info;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	bool is_local_player = entity == local_player;
	bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

	if (settings::options.money_bool && !is_teammate)
		stored_info.push_back(std::pair<std::string, CColor>("$" + std::to_string(entity->GetMoney()), backtrack));
	else if (settings::options.moneyteam && is_teammate)
		stored_info.push_back(std::pair<std::string, CColor>("$" + std::to_string(entity->GetMoney()), backtrack));

	if (settings::options.info_bool && !is_teammate)
	{
		if (entity->GetArmor() > 0)
			stored_info.push_back(std::pair<std::string, CColor>(entity->GetArmorName(), color));

		if (entity->GetIsScoped())
			stored_info.push_back(std::pair<std::string, CColor>("zoom", alt));
	}
	else if (settings::options.flagsteam && is_teammate)
	{
		if (entity->GetArmor() > 0)
			stored_info.push_back(std::pair<std::string, CColor>(entity->GetArmorName(), color));

		if (entity->GetIsScoped())
			stored_info.push_back(std::pair<std::string, CColor>("zoom", alt));
	}

	int height = (pos.y - top.y), width = height / 2, i = 0;
	for (auto Text : stored_info)
	{
		RENDER::DrawF((pos.x + width / 2) + 5, top.y + i, FONTS::visuals_esp_font, false, false, Text.second, Text.first);
		i += 8;
	}
}

void CVisuals::DrawInaccuracy()
{

}
/*
void CVisuals::DrawFootSteps() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!g_csgo::Engine->IsInGame() || !local_player) { footsteps.clear(); return; }
	if (footsteps.size() > 5)
		footsteps.pop_back();

	for (int i = 0; i < footsteps.size(); i++)
	{
		auto current = footsteps.at(i);
		if (!current.pPlayer)
			continue;
		if (current.pPlayer->GetIsDormant())
			continue;

		SDK::BeamInfo_t beamInfo;
		beamInfo.m_nType = SDK::TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = settings::options.bulletlife;
		beamInfo.m_flWidth = settings::options.bulletsize;
		beamInfo.m_flEndWidth = 2.0f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current.color.RGBA[0];
		beamInfo.m_flGreen = current.color.RGBA[1];
		beamInfo.m_flBlue = current.color.RGBA[2];
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = SDK::FBEAM_ONLYNOISEONCE | SDK::FBEAM_NOTILE | SDK::FBEAM_HALOBEAM;

		beamInfo.m_vecStart = current.pPlayer->GetVecOrigin() + current.pPlayer->GetViewOffset();
		beamInfo.m_vecEnd = current.vecImpactPos;

		auto beam = g_csgo::ViewRenderBeams->CreateBeamPoints(beamInfo);
		if (beam) g_csgo::ViewRenderBeams->DrawBeam(beam);

		Impacts.erase(Impacts.begin() + i);
	}
}
*/
void CVisuals::DrawBulletBeams()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!g_csgo::Engine->IsInGame() || !local_player) { Impacts.clear(); return; }
	if (Impacts.size() > 30) Impacts.pop_back();

	for (int i = 0; i < Impacts.size(); i++)
	{
		auto current = Impacts.at(i);
		if (!current.pPlayer) continue;
		if (current.pPlayer->GetIsDormant()) continue;

		bool is_local_player = current.pPlayer == local_player;
		bool is_teammate = local_player->GetTeam() == current.pPlayer->GetTeam() && !is_local_player;

		if (current.pPlayer == local_player)
			current.color = CColor(settings::options.bulletlocal_col[0] * 255, settings::options.bulletlocal_col[1] * 255, settings::options.bulletlocal_col[2] * 255, settings::options.bulletlocal_col[3] * 255);
		else if (current.pPlayer != local_player && !is_teammate)
			current.color = CColor(settings::options.bulletenemy_col[0] * 255, settings::options.bulletenemy_col[1] * 255, settings::options.bulletenemy_col[2] * 255, settings::options.bulletenemy_col[3] * 255);
		else if (current.pPlayer != local_player && is_teammate)
			current.color = CColor(settings::options.bulletteam_col[0] * 255, settings::options.bulletteam_col[1] * 255, settings::options.bulletteam_col[2] * 255, settings::options.bulletteam_col[3] * 255);

		SDK::BeamInfo_t beamInfo;
		beamInfo.m_nType = SDK::TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = 3;
		beamInfo.m_flWidth = 1;
		beamInfo.m_flEndWidth = 2.0f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current.color.RGBA[0];
		beamInfo.m_flGreen = current.color.RGBA[1];
		beamInfo.m_flBlue = current.color.RGBA[2];
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = SDK::FBEAM_ONLYNOISEONCE | SDK::FBEAM_NOTILE | SDK::FBEAM_HALOBEAM;

		beamInfo.m_vecStart = current.pPlayer->GetVecOrigin() + current.pPlayer->GetViewOffset();
		beamInfo.m_vecEnd = current.vecImpactPos;

		auto beam = g_csgo::ViewRenderBeams->CreateBeamPoints(beamInfo);
		if (beam) g_csgo::ViewRenderBeams->DrawBeam(beam);

		Impacts.erase(Impacts.begin() + i);
	}
}

void CVisuals::DrawCrosshair()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto crosshair = g_csgo::cvar->FindVar("crosshair");
	if (settings::options.xhair_type == 0)
	{
		crosshair->SetValue("1");
		return;
	}
	else
		crosshair->SetValue("0");

	int W, H, cW, cH;
	g_csgo::Engine->GetScreenSize(W, H);

	cW = W / 2; cH = H / 2;

	int dX = W / 120.f, dY = H / 120.f;
	int drX, drY;

	if (settings::options.xhair_type == 2)
	{
		drX = cW - (int)(dX * (((local_player->GetPunchAngles().y * 2.f) * 0.45f) + local_player->GetPunchAngles().y));
		drY = cH + (int)(dY * (((local_player->GetPunchAngles().x * 2.f) * 0.45f) + local_player->GetPunchAngles().x));
	}
	else
	{
		drX = cW;
		drY = cH;
	}

	g_csgo::Surface->DrawSetColor(BLACK);
	g_csgo::Surface->DrawFilledRect(drX - 4, drY - 2, drX - 4 + 8, drY - 2 + 4);
	g_csgo::Surface->DrawFilledRect(drX - 2, drY - 4, drX - 2 + 4, drY - 4 + 8);

	g_csgo::Surface->DrawSetColor(WHITE);
	g_csgo::Surface->DrawFilledRect(drX - 3, drY - 1, drX - 3 + 6, drY - 1 + 2);
	g_csgo::Surface->DrawFilledRect(drX - 1, drY - 3, drX - 1 + 2, drY - 3 + 6);
}


void CVisuals::DrawInaccuracy1()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon) return;

	int spread_Col[3] = { settings::options.spreadcirclecol[0], settings::options.spreadcirclecol[1], settings::options.spreadcirclecol[2] };


	int W, H, cW, cH;
	g_csgo::Engine->GetScreenSize(W, H);
	cW = W / 2; cH = H / 2;

	if (local_player->IsAlive())
	{
		auto accuracy = (weapon->GetInaccuracy() + weapon->GetSpreadCone()) * 500.f;

		float r;
		float alpha, newAlpha;

		for (r = accuracy; r > 0; r--)
		{
			if (!weapon->is_grenade() && !weapon->is_knife())
				return;

			alpha = r / accuracy;

			RENDER::DrawCircle(cW, cH, r, 60, CColor(spread_Col[0], spread_Col[1], spread_Col[2], (pow(alpha, 5)) * 130));
		}
	}
}
void CVisuals::DrawIndicator()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;
	if (local_player->GetHealth() <= 0) return;

	float breaking_lby_fraction = fabs(MATH::NormalizeYaw(global::real_angles.y - local_player->GetLowerBodyYaw())) / 180.f;
	float lby_delta = abs(MATH::NormalizeYaw(global::real_angles.y - local_player->GetLowerBodyYaw()));

	int screen_width, screen_height;
	g_csgo::Engine->GetScreenSize(screen_width, screen_height);

	int iY = 88;
	//if (settings::options.stop_bool)
	//{
	//	iY += 22;
	//	RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, settings::options.stop_flip ? CColor(0, 255, 0) : CColor(255, 0, 0), "STOP");
	//}
	/*if (settings::options.overrideenable)
	{
		iY += 22; bool overridekeyenabled;
		if (settings::options.overridemethod == 0)
			RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, settings::options.overridething ? CColor(0, 255, 0) : CColor(255, 0, 0), "OVERRIDE");
		else if (settings::options.overridemethod == 1)
		{
			GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(settings::options.overridekey)) ?
				RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor(0, 255, 0), "OVERRIDE") :
				RENDER::DrawF(10, screen_height - iY, FONTS::visuals_lby_font, false, false, CColor(255, 0, 0), "OVERRIDE");
		}
	}*/

	if (settings::options.aa_bool && local_player->GetVelocity().Length2D() < 0.1f  && local_player->GetFlags() & FL_ONGROUND && global::breaking_lby && settings::options.lbyenable)
		RENDER::DrawF(10, 210, FONTS::visuals_lby_font, false, false, CColor(0, 255, 0), "LBY");
	else
		RENDER::DrawF(10, 210, FONTS::visuals_lby_font, false, false, CColor(255, 0, 0), "LBY");

	if (settings::options.rifk_arrow)
	{
		auto client_viewangles = Vector();
		g_csgo::Engine->GetViewAngles(client_viewangles);
		const auto screen_center = Vector2D(screen_width / 2.f, screen_height / 2.f);

		constexpr auto radius = 80.f;
		auto draw_arrow = [&](float rot, CColor color) -> void
		{
			std::vector<SDK::Vertex_t> vertices;
			vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot) * radius, screen_center.y + sinf(rot) * radius)));
			vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot + DEG2RAD(8)) * (radius - 12), screen_center.y + sinf(rot + DEG2RAD(8)) * (radius - 12)))); //25
			vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot - DEG2RAD(8)) * (radius - 12), screen_center.y + sinf(rot - DEG2RAD(8)) * (radius - 12)))); //25
			RENDER::TexturedPolygon(3, vertices, color);
		};

		static auto alpha = 0.f; static auto plus_or_minus = false;
		if (alpha <= 0.f || alpha >= 255.f) plus_or_minus = !plus_or_minus;
		alpha += plus_or_minus ? (255.f / 7 * 0.015) : -(255.f / 7 * 0.015); alpha = clamp(alpha, 0.f, 255.f);


		/*if (settings::options.desync_yaw && global::should_send_packet) {
			auto fake_color = CColor(255, 0, 0, alpha);
			const auto fake_rot = DEG2RAD(client_viewangles.y - global::fake_angles.y - 90);
			draw_arrow(fake_rot, fake_color);
		}*/


		auto real_color = CColor(0, 255, 0, alpha);
		const auto real_rot = DEG2RAD(client_viewangles.y - global::real_angles.y - 90);
		draw_arrow(real_rot, real_color);
	}
}

void CVisuals::LogEvents()
{

}

void CVisuals::ModulateWorld()
{
	/*auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	static auto sv_skyname = g_csgo::cvar->FindVar("sv_skyname");
	sv_skyname->nFlags &= ~FCVAR_CHEAT;

	static auto static_val = 100;
	auto night_val = settings::options.daytimevalue;

	if (static_val != night_val)
	{
		static_val = night_val;
		for (auto i = g_csgo::MaterialSystem->FirstMaterial(); i != g_csgo::MaterialSystem->InvalidMaterial(); i = g_csgo::MaterialSystem->NextMaterial(i))
		{
			auto pMaterial = g_csgo::MaterialSystem->GetMaterial(i);
			if (!pMaterial) continue;

			auto finalnightval = night_val / 100.f;
			if (strstr(pMaterial->GetTextureGroupName(), "World"))
				pMaterial->ColorModulate(finalnightval, finalnightval, finalnightval);
			else if (strstr(pMaterial->GetTextureGroupName(), "SkyBox"))
				night_val == 100.f ? sv_skyname->SetValue("vertigoblue_hdr") : sv_skyname->SetValue("sky_descent");

		}
	}*/
}
/*
void cplayer_esp::draw_beam() {

	for (int i = 1; i <= g_csgo::Globals->maxclients; i++) {

		auto entity = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(i));

		SDK::BeamInfo_t beam_info;
		beam_info.m_vecCenter = entity->GetVecOrigin();
		beam_info.m_flStartRadius = 10;
		beam_info.m_flEndRadius = 285;

		beam_info.m_pszModelName = "sprites/physbeam.vmt";
		beam_info.m_nModelIndex = -1;

		beam_info.m_nStartFrame = 0;
		beam_info.m_flFrameRate = 10;

		beam_info.m_flLife = 0.6;
		beam_info.m_flWidth = 5;
		beam_info.m_flAmplitude = 0;

		beam_info.m_flSpeed = 10;
		beam_info.m_nFlags = 0;
		beam_info.m_flFadeLength = 0;
		beam_info.m_bRenderable = true;


		SDK::Beam_t* beam = g_csgo::ViewRenderBeams->CreateBeamRingPoint(beam_info);
		g_csgo::ViewRenderBeams->DrawBeam(beam);
	}
}
*/

void CVisuals::ModulateSky()
{
	/*static bool nightmode_performed = false, nightmode_lastsetting;

	if (!g_csgo::Engine->IsConnected() || !g_csgo::Engine->IsInGame())
	{
		if (nightmode_performed)
			nightmode_performed = false;
		return;
	}

	if (nightmode_performed != settings::options.night_bool)
	{
		nightmode_lastsetting = settings::options.night_bool;
		nightmode_performed = false;
	}

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!local_player)
	{
		if (nightmode_performed) nightmode_performed = false;
		return;
	}

	if (nightmode_lastsetting != settings::options.night_bool)
	{
		nightmode_lastsetting = settings::options.night_bool;
		nightmode_performed = false;
	}

	if (!nightmode_performed)
	{

		if (settings::options.night_bool) sv_skyname->SetValue("sky_csgo_night02");
		else sv_skyname->SetValue("vertigoblue_hdr");
	}*/
}

void CVisuals::DrawHitmarker() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	static int lineSize = 6;

	static float alpha = 0;
	float step = 255.f / 0.3f * g_csgo::Globals->frametime;


	if (global::flHurtTime + 0.4f >= g_csgo::Globals->curtime)
		alpha = 255.f;
	else
		alpha -= step;

	if (alpha > 0) {
		int screenSizeX, screenCenterX;
		int screenSizeY, screenCenterY;
		g_csgo::Engine->GetScreenSize(screenSizeX, screenSizeY);

		screenCenterX = screenSizeX / 2;
		screenCenterY = screenSizeY / 2;

		CColor col = CColor(255, 255, 255, alpha);

		RENDER::DrawLine(screenCenterX - lineSize * 2, screenCenterY - lineSize * 2, screenCenterX - (lineSize), screenCenterY - (lineSize), col);
		RENDER::DrawLine(screenCenterX - lineSize * 2, screenCenterY + lineSize * 2, screenCenterX - (lineSize), screenCenterY + (lineSize), col);
		RENDER::DrawLine(screenCenterX + lineSize * 2, screenCenterY + lineSize * 2, screenCenterX + (lineSize), screenCenterY + (lineSize), col);
		RENDER::DrawLine(screenCenterX + lineSize * 2, screenCenterY - lineSize * 2, screenCenterX + (lineSize), screenCenterY - (lineSize), col);
	}
}

void CVisuals::DrawBorderLines() {

	/*auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	auto weapon = g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex());
	if (!weapon) return;

	int
		screen_x,
		screen_y,
		center_x,
		center_y;

	g_csgo::Engine->GetScreenSize(screen_x, screen_y); g_csgo::Engine->GetScreenSize(center_x, center_y);
	center_x /= 2; center_y /= 2;

	static float rainbow;
	rainbow += 0.001;

	if (rainbow > 1.f)
		rainbow = 0.f;

	if (local_player->GetIsScoped()) {

		RENDER::DrawF(screen_x / 2, screen_y / 2 + 50, FONTS::visuals_lby_font, true, true, CColor(100, 100, 100), "SCOPED");

		RENDER::DrawLine(0, center_y, screen_x, center_y, CColor::FromHSB(rainbow, 1.f, 1.f));
		RENDER::DrawLine(center_x, 0, center_x, screen_y, CColor::FromHSB(rainbow, 1.f, 1.f));
	}*/
}

void set_clan_tag(const char* tag, const char* name) {

	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)UTILS::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30", "xxxxxxxxx????xxxxxx")));
	pSetClanTag(tag, name);
}

void CVisuals::Clantag()
{
	std::string clantagText = "wanheda";
	size_t clantagSize = clantagText.length();
	std::string spacesText(clantagSize, ' ');
	clantagText = spacesText + clantagText + spacesText;

	for (int i = 0; i < clantagSize * 2; i++)
		set_clan_tag(clantagText.substr(i, clantagSize).c_str(), "wanheda");
}

void CVisuals::misc_visuals() {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	SDK::ConVar* sniper_crosshair = g_csgo::cvar->FindVar("weapon_debug_spread_show");

	if (!local_player->GetIsScoped())
		sniper_crosshair->SetValue(3);
	else
		sniper_crosshair->SetValue(0);
}

CVisuals* visuals = new CVisuals();