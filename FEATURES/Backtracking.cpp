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
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Backtracking.h"

#define TICK_INTERVAL			( g_pGlobalVarsBase->interval_per_tick )

#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )

template<class T> const T&
clamp(const T& x, const T& upper, const T& lower) { return min(upper, max(x, lower)); }

void CBacktrack::backtrack_player(SDK::CUserCmd* cmd)
{
	for (int i = 1; i < 65; ++i)
	{
		auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

		if (!entity)
			continue;

		if (!local_player)
			continue;

		bool is_local_player = entity == local_player;
		bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

		if (is_local_player)
			continue;

		if (is_teammate)
			continue;

		if (entity->GetHealth() <= 0)
			continue;

		if (local_player->GetHealth() <= 0)
			continue;

		if (entity->GetImmunity())
			continue;

		int index = entity->GetIndex();

		for (int i = 0; i < 20; i++)
		{
			backtrack_hitbox[index][i][cmd->command_number % 12] = aimbot->get_hitbox_pos(entity, i);
		}

		backtrack_simtime[index][cmd->command_number % 12] = entity->GetSimTime();

		for (int i = 0; i < 12; i++)
		{
			if (backtrack_simtime[index][i] != compensate[index][i])
			{
				if (i > 0 && i != 11)
				{
					oldest_tick[index] = i + 2;
				}
				else
				{
					oldest_tick[index] = 1;
				}
				compensate[index][i] = backtrack_simtime[index][i];
			}
		}
	}
}

inline Vector CBacktrack::angle_vector(Vector meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(M_PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(M_PI));

	auto sp = sin(meme.x / 180.f * static_cast<float>(M_PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(M_PI));

	return Vector(cp*cy, cp*sy, -sp);
}

inline float CBacktrack::point_to_line(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

void CBacktrack::run_legit(SDK::CUserCmd* cmd) //phook backtrack muahhahahahaaha
{
	int bestTargetIndex = -1;
	float bestFov = FLT_MAX;
	SDK::player_info_t info;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	for (int i = 1; i < 65; i++)
	{
		auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
		if (!entity) continue;
		if (!g_csgo::Engine->GetPlayerInfo(i, &info)) continue;
		if (entity->GetIsDormant()) continue;
		if (entity->GetTeam() == local_player->GetTeam()) continue;
		if (entity->GetHealth() <= 0) continue;
		if (local_player->GetHealth() <= 0) return;

		float simtime = entity->GetSimTime(); Vector hitboxPos = aimbot->get_hitbox_pos(entity, 0);
		headPositions[i][cmd->command_number % 12] = legit_backtrackdata{ simtime, hitboxPos };
		Vector ViewDir = angle_vector(cmd->viewangles + (local_player->GetPunchAngles() * 2.f));
		float FOVDistance = point_to_line(hitboxPos, local_player->GetEyePosition(), ViewDir);

		if (bestFov > FOVDistance)
		{
			bestFov = FOVDistance;
			bestTargetIndex = i;
		}
	}

	float bestTargetSimTime;
	if (bestTargetIndex != -1)
	{
		float tempFloat = FLT_MAX;
		Vector ViewDir = angle_vector(cmd->viewangles + (local_player->GetPunchAngles() * 2.f));

		for (int t = 0; t < 12; ++t)
		{
			float tempFOVDistance = point_to_line(headPositions[bestTargetIndex][t].hitboxPos, local_player->GetEyePosition(), ViewDir);
			if (tempFloat > tempFOVDistance && headPositions[bestTargetIndex][t].simtime > local_player->GetSimTime() - 1)
			{
				tempFloat = tempFOVDistance;
				bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
			}
		}

		if (cmd->buttons & IN_ATTACK)
			cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
	}
}
bool CBacktrack::IsValid(SDK::CBaseEntity * player)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return false;

	if (!player)
		return false;

	if (player->GetIsDormant() || player->GetHealth() == 0 || player->GetFlags() & FL_FROZEN)
		return false;

	if (player->GetTeam() == local_player->GetTeam())
		return false;

	if (player->GetClientClass()->m_ClassID != 38)
		return false;

	if (player == local_player)
		return false;

	if (player->GetImmunity())
		return false;

	return true;
}

CBacktrack* backtracking = new CBacktrack();
legit_backtrackdata headPositions[64][12];