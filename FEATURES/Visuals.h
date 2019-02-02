#pragma once

#include "..\SDK\GameEvents.h"

namespace SDK
{
	class CUserCmd;
	class CBaseEntity;
}

class CVisuals {
public:
	void Drawmodels();
	void CustomModels(SDK::CBaseEntity* entity);
	void Draw();
	void ClientDraw();
	void apply_clantag();
	void DrawInaccuracy();
	void DrawBulletBeams();
	void ModulateWorld();
	void ModulateSky();
	void set_hitmarker_time( float time );
	void LogEvents();
	void LagCompHitbox(SDK::CBaseEntity * entity);
	void CollectFootstep(int iEntIndex, const char * pSample);
	void Clantag();
	void misc_visuals();
	void DrawSounds();
	void EmitSound(int iEntIndex, const char * pSample);
private:
	void DrawBox(SDK::CBaseEntity* entity, CColor color, Vector pos, Vector top);
	void DrawName(SDK::CBaseEntity* entity, CColor color, int index, Vector pos, Vector top);
	void DrawWeapon(SDK::CBaseEntity * entity, CColor color, int index, Vector pos, Vector top);
	void DrawHealth(SDK::CBaseEntity * entity, CColor color, CColor dormant, Vector pos, Vector top);
	void rage_rs();
	void penetration_reticle();
	void BombPlanted(SDK::CBaseEntity * entity);
	void DrawDropped(SDK::CBaseEntity * entity);
	void DrawAmmo(SDK::CBaseEntity * entity, CColor color, CColor dormant, Vector pos, Vector top);
	float resolve_distance(Vector src, Vector dest);
	void DrawDistance(SDK::CBaseEntity * entity, CColor color, Vector pos, Vector top);
	void DrawInfo(SDK::CBaseEntity * entity, CColor color, CColor alt, Vector pos, Vector top);
	void DrawInaccuracy1();
	void DrawCrosshair();
	void DrawIndicator();
	void DrawHitmarker();
	void DrawBorderLines();
public:
	std::vector<std::pair<int, float>>				Entities;
	std::deque<UTILS::BulletImpact_t>				Impacts;
};

extern CVisuals* visuals;