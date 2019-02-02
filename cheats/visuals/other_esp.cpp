#include "other_esp.h"

SDK::CBaseEntity* get_player(int userid) {

	int i = g_csgo::Engine->GetPlayerForUserID(userid);
	return static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(i));
}

std::vector<impact_info> impact;
std::vector<hitmarker_info> hitmarker;

void cother_esp::hurt(SDK::IGameEvent* event) {

	auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));

		auto attacker = event->GetInt("attacker");
		auto victim = get_player(event->GetInt("userid"));

		if (g_csgo::Engine->GetPlayerForUserID(attacker) == g_csgo::Engine->GetLocalPlayer())
			hurt_time = g_csgo::Globals->curtime;

		impact_info best_impact;
		float best_impact_distance = -1;
		float time = g_csgo::Globals->curtime;


		for (int i = 0; i < impact.size(); i++) {

			auto iter = impact[i];

			if (time > iter.time + 1.f) {

				impact.erase(impact.begin() + i);
				continue;
			}

			Vector 
				position = Vector(iter.x, iter.y, iter.z), 
				enemy_pos = victim->GetVecOrigin();

			float distance = position.DistTo(enemy_pos);

			if (distance < best_impact_distance || best_impact_distance == -1) {

				best_impact_distance = distance;
				best_impact = iter;
			}
		}

		if (best_impact_distance == -1)
			return;

		hitmarker_info info;
		info.impact = best_impact;
		info.alpha = 255;
		hitmarker.push_back(info);
}

void cother_esp::impacts(SDK::IGameEvent* event) {

	auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));

	auto entity = get_player(event->GetInt("userid"));

	if (!entity || entity != local_player)
		return;

	impact_info info;

	info.x = event->GetFloat("x");
	info.y = event->GetFloat("y");
	info.z = event->GetFloat("z");

	info.time = g_csgo::Globals->curtime;

	impact.push_back(info);
}

void cother_esp::log_tracer(SDK::IGameEvent* event) {
	//if we receive bullet_impact event
		//get the user who fired the bullet
	auto index = g_csgo::Engine->GetPlayerForUserID(event->GetInt("userid"));

	//return if the userid is not valid or we werent the entity who was fireing
	if (index != g_csgo::Engine->GetLocalPlayer())
		return;

	//get local player
	auto local = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(index));
	if (!local)
		return;

	//get the bullet impact's position
	Vector position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

	SDK::Ray_t ray;
	ray.Init(local->GetEyePosition(), position);

	//skip local player
	SDK::CTraceFilter filter;
	filter.pSkip1 = local;

	//trace a ray
	SDK::trace_t tr;
	g_csgo::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	//use different color when we hit a player
	auto color = CColor(255, 255, 0, 220);

	//push info to our vector
	logs.push_back(cbullet_tracer_info(local->GetEyePosition(), position, g_csgo::Globals->curtime, color));
}

void cother_esp::render_tracer() {

	auto local = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));
	if (!local)
		return;

	for (size_t i = 0; i < logs.size(); i++) {

		auto current = logs.at(i);

		g_csgo::DebugOverlay->AddLineOverlay(current.src, current.dst, current.color.RGBA[0], current.color.RGBA[1], current.color.RGBA[2], true, -1.f);
		g_csgo::DebugOverlay->AddBoxOverlay(current.dst, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 127, -1.f);

		if (fabs(g_csgo::Globals->curtime - current.time) > 5.f)
			logs.erase(logs.begin() + i);
	}
}

void cother_esp::hitmarkerdynamic_paint() {

	float time = g_csgo::Globals->curtime;

	for (int i = 0; i < hitmarker.size(); i++) {

		bool expired = time >= hitmarker.at(i).impact.time + 3.f;

		if (expired)
			hitmarker.at(i).alpha -= 1;

		if (expired && hitmarker.at(i).alpha <= 0) {

			hitmarker.erase(hitmarker.begin() + i);
			continue;
		}

		Vector pos3D = Vector(hitmarker.at(i).impact.x, hitmarker.at(i).impact.y, hitmarker.at(i).impact.z), pos2D;

		if (!RENDER::WorldToScreen(pos3D, pos2D))
			continue;

		g_csgo::Surface->DrawSetColor(CColor(240, 200, 240, hitmarker.at(i).alpha));

		g_csgo::Surface->DrawOutlinedCircle(pos2D.x, pos2D.y, 8, 50); // pos2D.x - (linesize / 4), pos2D.y - (linesize / 4)
		g_csgo::Surface->DrawLine(pos2D.x - 6, pos2D.y - 6, pos2D.x - (3 / 4), pos2D.y - (3 / 4));
		g_csgo::Surface->DrawLine(pos2D.x - 6, pos2D.y + 6, pos2D.x - (3 / 4), pos2D.y + (3 / 4));
		g_csgo::Surface->DrawLine(pos2D.x + 6, pos2D.y + 6, pos2D.x + (3 / 4), pos2D.y + (3 / 4));
		g_csgo::Surface->DrawLine(pos2D.x + 6, pos2D.y - 6, pos2D.x + (3 / 4), pos2D.y - (3 / 4));
	}
}

/*void cother_esp::bomb_info() {


}*/

void cother_esp::viewmodel_changer() {

		g_csgo::cvar->FindVar("viewmodel_offset_x")->SetValue(settings::options.viewmodel_x);
		g_csgo::cvar->FindVar("viewmodel_offset_y")->SetValue(settings::options.viewmodel_y);
		g_csgo::cvar->FindVar("viewmodel_offset_z")->SetValue(settings::options.viewmodel_z - 10);
}

/*std::array<floating_text, MAX_FLOATING_TEXTS> floating_texts;

void cother_esp::damageesp_event(SDK::IGameEvent* event) {

	int floatingTextsIdx = 0;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!settings::options.Visuals.Visuals_DamageESP || !(g_csgo::Engine->IsInGame() && g_csgo::Engine->IsConnected() && local_player))
		return;

	const char* name = event->GetName();

	static Vector lastImpactPos = Vector(0, 0, 0);

		float curTime = g_csgo::Globals->curtime;

		int userid = event->GetInt(("userid"));
		int attackerid = event->GetInt(("attacker"));
		int dmg_health = event->GetInt(("dmg_health"));
		int hitgroup = event->GetInt(("hitgroup"));

		SDK::CBaseEntity *entity = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(userid));
		SDK::CBaseEntity *attacker = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(attackerid));

		if (!entity || attacker != local_player)
			return;

		floating_text txt;
		txt.start_time = curTime;
		txt.hitgroup = hitgroup;
		txt.hit_pos = lastImpactPos;
		txt.damage = dmg_health;
		txt.random_idx = rand() % 5;
		txt.valid = true;

		floating_texts[floatingTextsIdx++ % MAX_FLOATING_TEXTS] = txt;

		int uid = event->GetInt(("userid"));
		float x = event->GetFloat(("x"));
		float y = event->GetFloat(("y"));
		float z = event->GetFloat(("z"));

		SDK::CBaseEntity *entity = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(uid));

		if (!entity || entity != local_player)
			return;

		lastImpactPos = Vector(x, y, z);
}

void cother_esp::damageesp_draw() {

	if (!settings::options.Visuals.Visuals_DamageESP)
		return;


	for (int i = 0; i < MAX_FLOATING_TEXTS; i++)
	{
		floating_text *txt = &floating_texts[i % MAX_FLOATING_TEXTS];

		if (!txt->valid)
			continue;

		float endTime = txt->start_time + 1.1f;

		if (endTime < g_csgo::Globals->curtime)
		{
			txt->valid = false;
			continue;
		}

		Vector screen;

		if (RENDER::WorldToScreen(txt->hit_pos, screen)) {

			float t = 1.0f - (endTime - g_csgo::Globals->curtime) / (endTime - txt->start_time);

			screen.y -= t * (35.0f);
			screen.x -= (float)txt->random_idx * t * 3.0f;

			char msg[12];
			sprintf_s(msg, 12, "-%dHP", txt->damage);

			Menuxd::DrawStringWithFont(Menuxd::fntVerdana10, screen.x / 2, screen.y - 11 + 1, D3D_COLOR_BLACK((int)((1.0f - t) * (float)255)), msg);

			Menuxd::DrawStringWithFont(Menuxd::fntVerdana10, screen.x / 2, screen.y - 11, D3DCOLOR_ARGB(int(settings::options.Visuals.DamageESPColor[3] * 255), int(settings::options.Visuals.DamageESPColor[0] * 255), int(settings::options.Visuals.DamageESPColor[1] * 255), int(settings::options.Visuals.DamageESPColor[2] * 255)), msg);
		}
	}
}*/