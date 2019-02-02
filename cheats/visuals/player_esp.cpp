#include "player_esp.h"
#include "..\..\UTILS\interfaces.h"
#include "..\..\SDK\CGlobalVars.h"
#include "..\..\SDK\CClientEntityList.h"
#include "..\..\SDK\IEngine.h"
#include "..\..\UTILS\vector3D.h"
#include "..\..\FEATURES\Aimbot.h"
#include "..\..\UTILS\general_utils.h"

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

/*bool get_bbox(SDK::CBaseEntity* e, Box& box) {

	Vector top, down, air, s[2];

	Vector adjust = Vector(0, 0, -18) * e->m_flDuckAmount();

	if (!(e->GetFlags() & FL_ONGROUND) && (e->GetMoveType() != 9)) // ladder
		air = Vector(0, 0, 10);
	else
		air = Vector(0, 0, 0);

	down = e->GetVecOrigin() + air;
	top = down + Vector(0, 0, 72) + adjust;

	if (RENDER::WorldToScreen(top, s[1]) && RENDER::WorldToScreen(down, s[0])) {

		Vector delta = s[1] - s[0];

		box.h = fabsf(delta.y);
		box.w = box.h / 2.0f;

		box.x = s[1].x - (box.w / 2);
		box.y = s[1].y;

		return true;
	} return false;
}*/

int alpha[65];

void cplayer_esp::paint_traverse() {

	//draw_watermark();

	if (!settings::options.Visuals.Enabled)
		return;

	auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer())); if (!local_player) return;

	for (int i = 1; i < g_csgo::Globals->maxclients; i++) {

		auto entity = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(i));

		if (!entity)
			continue;

		if (!entity->IsAlive())
			continue;

		bool is_local_player = entity == local_player;
		bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

		if (is_local_player)
			continue;

		if (is_teammate)
			continue;

		CColor 
			dormant_color = CColor(100, 100, 100, alpha[entity->GetIndex()]),
			health_color,
			name_color,
			weapons_color;

		if (entity->GetIsDormant()) {

			if (alpha[entity->GetIndex()] > 0)
				alpha[entity->GetIndex()] -= .0001;

			name_color =	dormant_color;
			weapons_color = dormant_color;
			health_color =	dormant_color;
		}
		else {

			if (alpha[entity->GetIndex()] <= 0)
				alpha[entity->GetIndex()] += 0.4;

			name_color =	CColor(255, 255, 255);
			weapons_color = CColor(255, 255, 255);
			health_color =	CColor(255, 255, 255);
		}

		if (settings::options.fov_bool) pov_arrow(entity);
		if (settings::options.name_bool) draw_name(entity);
		if (settings::options.weapon_esp) draw_weapon(entity);

		if (settings::options.skeleton) {

			CColor color = entity->GetIsDormant() ? CColor(130, 130, 130, 130) : GREY;
			color.SetAlpha(255 * esp_alpha_fade[entity->GetIndex()]);

			VMatrix matrix[128];

			//if (entity->SetupBones(matrix, 128, 0x100, 0.f))
				//draw_skeleton(entity, color, matrix);
		}
	}
}

//void cplayer_esp::draw_watermark() {  RENDER::DrawF(30, 30, FONTS::visuals_name_font, false, false, WHITE, "wanheda framework"); }
/*
void cplayer_esp::draw_box(SDK::CBaseEntity* m_entity, Box box) {


}

void cplayer_esp::draw_health(SDK::CBaseEntity* m_entity, Box box) {


}

void cplayer_esp::draw_ammobar(SDK::CBaseEntity* m_entity, Box box) {


}

void cplayer_esp::draw_name(SDK::CBaseEntity* m_entity, Box box) {

	static auto sanitize = [](char* name) -> std::string {

		name[127] = '\0';

		std::string tmp(name);

		if (tmp.length() > 20) {

			tmp.erase(20, (tmp.length() - 20));
			tmp.append("...");
		} return tmp;
	};

	if (!settings::options.name_bool)
		return;

	SDK::player_info_t player_info;

	if (g_csgo::Engine->GetPlayerInfo(m_entity->GetIndex(), &player_info)) {

		std::string name = sanitize(player_info.szName);

		auto color = m_entity->GetIsDormant() ? CColor(130, 130, 130, 130) : settings::options.name_col;
		color.SetAlpha(255 * esp_alpha_fade[m_entity->GetIndex()]);

		RENDER::DrawF(box.x + (box.w / 2), box.y - 13, FONTS::visuals_esp_font, true, true, color, name.c_str());
	}
}
/*
void cplayer_esp::draw_weapon(SDK::CBaseEntity* m_entity, Box box) {

	SDK::player_info_t player_info;
	g_csgo::Engine->GetPlayerInfo(m_entity->GetIndex(), &player_info);

	if (!settings::options.weapons)
		return;

	int bottom_pos = 0;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(m_entity->GetActiveWeaponIndex()));

	int _y;

	if (!weapon)
		return;

	bool drawing_ammo_bar = settings::options.ammo_bool == 1;
	bool drawing_lby_bar = settings::options.lby_bar;

	if (weapon->is_non_aim() || !settings::options.lby_bar || player_info.fakeplayer)
		drawing_lby_bar = false;

	if (!drawing_lby_bar)
		_y = box.y + box.h + (drawing_ammo_bar ? 10 : 5);
	else
		_y = box.y + box.h + (drawing_ammo_bar ? 10 : 5);

	if (weapon->is_non_aim())
		drawing_ammo_bar = false;

	auto color = m_entity->GetIsDormant() ? CColor(130, 130, 130, 130) : settings::options.weapon_col;
	color.SetAlpha(255 * esp_alpha_fade[m_entity->GetIndex()]);

	std::string weapon_name = weapon->get_name();
	std::transform(weapon_name.begin(), weapon_name.end(), weapon_name.begin(), toupper);

	RENDER::DrawF(box.x + (box.w / 2), _y, FONTS::visuals_esp_font, true, true, color, weapon_name.c_str());
}

void cplayer_esp::draw_flags(SDK::CBaseEntity* e, Box box) {

	int _x = box.x + box.w + 1, _y = box.y;
	SDK::player_info_t player_info;
	g_csgo::Engine->GetPlayerInfo(e->GetIndex(), &player_info);

	auto weapon = e->GetActiveWeaponIndex();

	if (!weapon)
		return;

	auto draw_flag = [&](CColor color, const char* text, ...) -> void { RENDER::DrawF(_x + 3, _y, FONTS::visuals_esp_font, false, false, color, text); _y += 9; };

	auto color = CColor(255, 255, 255);
	color.SetAlpha(255 * esp_alpha_fade[e->GetIndex()]);

	if (e->GetIsScoped()) {

		auto scope_color = CColor(150, 150, 220);
		scope_color.SetAlpha(255 * esp_alpha_fade[e->GetIndex()]);
		draw_flag(scope_color, "scoped");
	}

	if (player_info.fakeplayer)
		draw_flag(color, "bot");

	if (e->GetMoney() > -1) {

		auto money_color = CColor(155, 220, 150, 165);
		money_color.SetAlpha(255 * esp_alpha_fade[e->GetIndex()]);
		draw_flag(money_color, "$%d", e->GetMoney());
	}
}

void cplayer_esp::draw_skeleton(SDK::CBaseEntity* e, CColor color, VMatrix matrix[128]) {


}

void cplayer_esp::draw_lbybar(SDK::CBaseEntity* m_entity, Box box) {

	SDK::player_info_t player_info;
	g_csgo::Engine->GetPlayerInfo(m_entity->GetIndex(), &player_info);

	if (player_info.fakeplayer || !settings::options.lbyenable)
		return;

	int
		alpha = 255 * esp_alpha_fade[m_entity->GetIndex()],
		outline_alpha = alpha * 0.7f,
		inner_back_alpha = alpha * 0.6f;

	CColor
		outline_color{ 0, 0, 0, outline_alpha },
		inner_back_color{ 0, 0, 0, inner_back_alpha },
		text_color = m_entity->GetIsDormant() ? CColor(130, 130, 130, alpha) : CColor(255, 255, 255, alpha),
		color = m_entity->GetIsDormant() ? CColor(130, 130, 130, 130) : CColor(255, 20, 20, alpha);

	color.SetAlpha(alpha);

	/*bool drawing_ammo_bar = g_cfg.player.ammo == 1;

	Box n_box = {

		box.x + 1,
		box.y + box.h + 8,
		box.w - 1,
		2
	};

	if (!drawing_ammo_bar)
		n_box.y -= 5;

	float cycle = global::next_update[m_entity->GetIndex()] - m_entity->GetSimTime();
	float bar_width = (((cycle * (n_box.w / 1.1f))));

	UTILS::clamp<float>(g_ctx.m_globals.add_time[m_entity->EntIndex()], 0.f, 1.1f);

	RENDER::DrawFilledRect(n_box.x - 1, n_box.y - 1, box.w + 2, 4, inner_back_color);
	RENDER::DrawFilledRect(n_box.x, n_box.y, bar_width, 2, color);
}

void cplayer_esp::draw_damage() {

	
}

void cplayer_esp::bomb_timer(SDK::CBaseEntity* entity) {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	Vector vOrig; Vector vScreen;
	vOrig = entity->GetVecOrigin();
	SDK::CCSBomb* bomb = (SDK::CCSBomb*)entity;

	float flBlow = bomb->GetC4BlowTime();
	float TimeRemaining = flBlow;
	char buffer[64];
	sprintf_s(buffer, "B - %.1fs", TimeRemaining);
	RENDER::DrawF(10, 10, FONTS::visuals_lby_font, false, false, CColor(124, 195, 13, 255), buffer);
}*/

/*void cplayer_esp::draw_weapon(SDK::CBaseEntity* m_entity) {

	SDK::player_info_t player_info;
	g_csgo::Engine->GetPlayerInfo(m_entity->GetIndex(), &player_info);

	if (!settings::options.weapons)
		return;

	int bottom_pos = 0;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(m_entity->GetActiveWeaponIndex()));

	int _y;

	if (!weapon)
		return;

	bool drawing_ammo_bar = settings::options.ammo_bool == 1;
	bool drawing_lby_bar = settings::options.lby_bar;

	_y = box.y + box.h + (5);

	auto color = m_entity->GetIsDormant() ? CColor(130, 130, 130, 130) : settings::options.weapon_col;
	color.SetAlpha(255 * esp_alpha_fade[m_entity->GetIndex()]);

	std::string weapon_name = weapon->get_name();
	std::transform(weapon_name.begin(), weapon_name.end(), weapon_name.begin(), toupper);

	RENDER::DrawF(box.x + (box.w / 2), _y, FONTS::visuals_esp_font, true, true, color, weapon_name.c_str());
}

void cplayer_esp::draw_flags(SDK::CBaseEntity* e) {

	int _x = box.x + box.w + 1, _y = box.y;
	SDK::player_info_t player_info;
	g_csgo::Engine->GetPlayerInfo(e->GetIndex(), &player_info);

	auto weapon = e->GetActiveWeaponIndex();

	if (!weapon)
		return;

	auto draw_flag = [&](CColor color, const char* text, ...) -> void { RENDER::DrawF(_x + 3, _y, FONTS::visuals_esp_font, false, false, color, text); _y += 9; };

	auto color = CColor(255, 255, 255);
	color.SetAlpha(255 * esp_alpha_fade[e->GetIndex()]);

	if (e->GetIsScoped()) {

		auto scope_color = CColor(150, 150, 220);
		scope_color.SetAlpha(255 * esp_alpha_fade[e->GetIndex()]);
		draw_flag(scope_color, "scoped");
	}

	if (player_info.fakeplayer)
		draw_flag(color, "bot");

	if (e->GetMoney() > -1) {

		auto money_color = CColor(155, 220, 150, 165);
		money_color.SetAlpha(255 * esp_alpha_fade[e->GetIndex()]);
		draw_flag(money_color, "$%d", e->GetMoney());
	}
}*/

void cplayer_esp::draw_name(SDK::CBaseEntity* m_entity) {

	static auto sanitize = [](char* name) -> std::string {

		name[127] = '\0';

		std::string tmp(name);

		if (tmp.length() > 20) {

			tmp.erase(20, (tmp.length() - 20));
			tmp.append("...");
		} return tmp;
	};

	if (!settings::options.name_bool)
		return;

	SDK::player_info_t player_info;

	if (g_csgo::Engine->GetPlayerInfo(m_entity->GetIndex(), &player_info)) {

		std::string name = sanitize(player_info.szName);

		auto color = m_entity->GetIsDormant() ? CColor(130, 130, 130, 130) : settings::options.name_col;
		color.SetAlpha(255 * esp_alpha_fade[m_entity->GetIndex()]);

		Vector min, max, pos, pos3D, top, top3D; m_entity->GetRenderBounds(min, max);
		pos3D = m_entity->GetAbsOrigin() - Vector(0, 0, 10); top3D = pos3D + Vector(0, 0, max.z + 10);

		if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
			RENDER::DrawF(pos.x, top.y - 7, FONTS::visuals_esp_font, true, true, color, name.c_str());
	}
}

void cplayer_esp::draw_weapon(SDK::CBaseEntity* m_entity) {

	SDK::player_info_t player_info;
	g_csgo::Engine->GetPlayerInfo(m_entity->GetIndex(), &player_info);

	if (!settings::options.weapons)
		return;

	int bottom_pos = 0;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(m_entity->GetActiveWeaponIndex()));

	if (!weapon)
		return;

	bool drawing_ammo_bar = settings::options.ammo_bool == 1;
	bool drawing_lby_bar = settings::options.lby_bar;

	auto color = m_entity->GetIsDormant() ? CColor(130, 130, 130, 130) : settings::options.weapon_col;
	color.SetAlpha(255 * esp_alpha_fade[m_entity->GetIndex()]);

	std::string weapon_name = weapon->get_name();
	std::transform(weapon_name.begin(), weapon_name.end(), weapon_name.begin(), toupper);

	Vector min, max, pos, pos3D, top, top3D; m_entity->GetRenderBounds(min, max);
	pos3D = m_entity->GetAbsOrigin() - Vector(0, 0, 10); top3D = pos3D + Vector(0, 0, max.z + 10);

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
		RENDER::DrawF(pos.x, top.y + 14, FONTS::visuals_esp_font, true, true, color, weapon_name.c_str());
}

void cplayer_esp::pov_arrow(SDK::CBaseEntity* entity) {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()); if (!local_player) return;

	Vector screen_pos, client_viewangles;
	int screen_width = 0, screen_height = 0;
	float radius = 360.f;

	if (entity->GetVecOrigin() == Vector(0, 0, 0))
		return;

	if (UTILS::IsOnScreen(aimbot->get_hitbox_pos(entity, SDK::HITBOX_HEAD), screen_pos)) return;

	g_csgo::Engine->GetViewAngles(client_viewangles);
	g_csgo::Engine->GetScreenSize(screen_width, screen_height);

	const auto screen_center = Vector(screen_width / 2.f, screen_height / 2.f, 0);
	const auto rot = DEG2RAD(client_viewangles.y - UTILS::CalcAngle(local_player->GetEyePosition(), aimbot->get_hitbox_pos(entity, SDK::HitboxList::HITBOX_HEAD)).y - 90);

	std::vector<SDK::Vertex_t> vertices;

	vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot) * radius, screen_center.y + sinf(rot) * radius)));
	vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot + DEG2RAD(2)) * (radius - 16), screen_center.y + sinf(rot + DEG2RAD(2)) * (radius - 16))));
	vertices.push_back(SDK::Vertex_t(Vector2D(screen_center.x + cosf(rot - DEG2RAD(2)) * (radius - 16), screen_center.y + sinf(rot - DEG2RAD(2)) * (radius - 16))));

	static auto alpha = 0.f; static auto plus_or_minus = false;
	if (alpha <= 0.f || alpha >= 255.f) plus_or_minus = !plus_or_minus;
	alpha += plus_or_minus ? (255.f / 7 * 0.015) : -(255.f / 7 * 0.015); alpha = clamp(alpha, 0.f, 255.f);

	float arrow_colour[3] = { 205, 45, 80 };

	RENDER::TexturedPolygon(3, vertices, CColor(arrow_colour[0], arrow_colour[1], arrow_colour[2], alpha)); //CColor(205, 45, 80, alpha));
}

void cplayer_esp::draw_health(SDK::CBaseEntity* entity) {

	Vector min, max;
	entity->GetRenderBounds(min, max);
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin() - Vector(0, 0, 10);
	top3D = pos3D + Vector(0, 0, max.z + 10);

	int enemy_hp = entity->GetHealth();
	int hp_red = 255 - (enemy_hp * 2.55);
	int hp_green = enemy_hp * 2.55;
	CColor health_color = CColor(hp_red, hp_green, 1, alpha[entity->GetIndex()]);

	if (RENDER::WorldToScreen(pos3D, pos) && RENDER::WorldToScreen(top3D, top))
	{
		int height = (pos.y - top.y);
		int width = height / 2;

		float offset = (height / 4.f) + 5;
		UINT hp = height - (UINT)((height * enemy_hp) / 100);

		RENDER::DrawEmptyRect((pos.x - width / 2) - 6, top.y - 1, (pos.x - width / 2) - 3, top.y + height + 1, CColor(0, 0, 0, 100)); //intense maths
		RENDER::DrawLine((pos.x - width / 2) - 4, top.y + hp, (pos.x - width / 2) - 4, top.y + height, CColor(0, 0, 255)); //could have done a rect here,
		RENDER::DrawLine((pos.x - width / 2) - 5, top.y + hp, (pos.x - width / 2) - 5, top.y + height, CColor(0, 0, 255)); //but fuck it

		if (entity->GetHealth() < 100)
			RENDER::DrawF((pos.x - width / 2) - 4, top.y + hp, FONTS::visuals_esp_font, true, true, CColor(255, 255, 255), std::to_string(enemy_hp));
	}
}

cplayer_esp* player_visuals = new cplayer_esp();