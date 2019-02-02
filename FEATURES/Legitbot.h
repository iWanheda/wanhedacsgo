#pragma once
#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Aimbot.h"
#include "../HOOKS/hooks.h"

//class CLegitBot
//{
//public:
//	void Run(SDK::CUserCmd* cmd);
//	bool EntityIsValid(int i);
//	void Triggerbot(SDK::CUserCmd* cmd);
//private:
//	void GoToTarget(SDK::CUserCmd* cmd);
//	void FindTarget();
//	void DropTarget();
//	void RCS(SDK::CUserCmd* cmd);
//
//	int m_fbestfov;
//	int m_ibesttarget;
//};
//
//extern CLegitBot* legitbot;

class CLegitBot
{
public:
	//CLegitBot();

	void weapon_settings(SDK::CBaseWeapon* weapon);
	void do_aimbot(SDK::CBaseEntity *local, SDK::CBaseWeapon* weapon, SDK::CUserCmd* cmd);
	Vector get_randomized_recoil(SDK::CBaseEntity* local);
	Vector get_randomized_angles(SDK::CBaseEntity* local);
	void triggerbot(SDK::CUserCmd* cmd);
	bool entity_is_valid(int i);

private:

	bool get_hitbox(SDK::CBaseEntity* local, SDK::CBaseEntity* entity, Vector& destination);
	int get_target(SDK::CBaseEntity* local, SDK::CBaseWeapon* weapon, SDK::CUserCmd* cmd, Vector& destination);

private:
	int best_target;
	bool shoot;
	Vector view_angle, aim_angle,
		delta_angle, final_angle, hitbox_position;

	float
		aim_smooth, aim_fov,
		randomized_smooth, randomized_angle,
		recoil_min, recoil_max;
};

extern CLegitBot* legitbot;