#include "resolver.h"
/*
bool is_using_desync() {

	auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer())); if (!local_player) return;

	for (int i = 0; i <= g_csgo::Globals->maxclients; i++) {

		auto entity = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(i)); if (!entity) continue;

		if (!entity->IsAlive())
			continue;

		if (entity->GetIsDormant())
			continue;

		if (entity->GetEyeAngles().y ? 58 : -58)
			return true;
	}
	return false;
}

void c_resolver::run(SDK::CUserCmd* cmd) {

	auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer())); if (!local_player) return;

	for (int i = 0; i <= g_csgo::Globals->maxclients; i++) {

		auto entity = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(i)); if (!entity) continue;

		if (!entity->IsAlive())
			continue;

		if (entity->GetIsDormant())
			continue;

		//if (is_using_desync)
			//entity->GetEyeAngles().y = ;
	}
}*/