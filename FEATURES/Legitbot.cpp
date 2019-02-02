#include "Legitbot.h"

//int m_ibesttargetlegit = -1;
//float m_fbestfov = 20.0f, deltaTime, curAimTime;
//void CLegitBot::Run(SDK::CUserCmd* cmd)
//{
//	if (!EntityIsValid(m_ibesttargetlegit)) return;
//	DropTarget();
//
//	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
//	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
//
//	static float oldServerTime = local_player->GetTickBase() * g_csgo::Globals->interval_per_tick;
//	deltaTime = G::ServerTime - oldServerTime;
//	oldServerTime = G::ServerTime;
//
//	if (!weapon) return;
//	if (weapon->get_full_info()->WeaponType == 9) return;
//	if (weapon->GetLoadedAmmo() == 0) return;
//
//	if (m_ibesttargetlegit == -1 && settings::options.legit_bool)
//		FindTarget();
//
//	if ((m_ibesttargetlegit != -1) && GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(settings::options.legit_key)))
//		GoToTarget(cmd);
//}
//void CLegitBot::FindTarget()
//{
//	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
//	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
//	m_fbestfov = settings::options.legitfov_val;
//
//	for (int i = 0; i <= 65; i++)
//	{
//		auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
//		if (!EntityIsValid(i)) continue;
//		if (!entity) continue;
//		Vector start; g_csgo::Engine->GetViewAngles(start);
//		float fov = MATH::GetFov(start, MATH::CalcAngle((local_player->GetVecOrigin() + local_player->GetViewOffset()), aimbot->get_hitbox_pos(entity, 0)));
//		if (fov < m_fbestfov)
//		{
//			m_fbestfov = fov;
//			m_ibesttargetlegit = i;
//		}
//	}
//}
//void CLegitBot::GoToTarget(SDK::CUserCmd* cmd)
//{
//	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
//	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
//	auto entity = g_csgo::ClientEntityList->GetClientEntity(m_ibesttargetlegit);
//	if (!(EntityIsValid(m_ibesttargetlegit))) return;
//
//	Vector dst = MATH::CalcAngle(local_player->GetEyePosition(), aimbot->get_hitbox_pos(entity, 0));
//	Vector src = cmd->viewangles;
//	dst -= local_player->GetPunchAngles() * (settings::options.rcsamount_val / 50.f);
//	Vector delta = dst - src;
//
//	delta.Clamp();
//	if (!delta.IsZero())
//	{
//		float finalTime = delta.Length() / (settings::options.legitaim_val / 10);
//		curAimTime += deltaTime;
//		if (curAimTime > finalTime) curAimTime = finalTime;
//		float percent = curAimTime / finalTime;
//		delta *= percent;
//		dst = src + delta;
//	}
//	cmd->viewangles = dst.Clamp();
//}
//
//void CLegitBot::DropTarget()
//{
//	if ((m_ibesttargetlegit == -1) || !(EntityIsValid(m_ibesttargetlegit)))
//	{
//		m_ibesttargetlegit = -1;
//		curAimTime = 0.f;
//	}
//}

bool CLegitBot::entity_is_valid(int i)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	auto pEntity = g_csgo::ClientEntityList->GetClientEntity(i);
	if (!pEntity) return false;
	if (pEntity == local_player) return false;
	if (pEntity->GetTeam() == local_player->GetTeam()) return false;
	if (!pEntity->GetIsDormant()) return false;
	if (!pEntity->GetImmunity()) return false;
	if (pEntity->GetHealth() <= 0) return false;
	return true;
}

void CLegitBot::triggerbot(SDK::CUserCmd* cmd)
{
	Vector src, dst, forward;
	SDK::trace_t tr;
	SDK::Ray_t ray;
	SDK::CTraceFilter filter;
	Vector viewangle = cmd->viewangles;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;
	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon || weapon->is_grenade() || weapon->is_knife() || weapon->GetItemDefenitionIndex() == SDK::WEAPON_C4) return;

	viewangle += local_player->GetPunchAngles() * 2.f;
	MATH::AngleVectors(viewangle, &forward);
	forward *= weapon->get_full_info()->m_Range;
	filter.pSkip1 = local_player;

	src = local_player->GetEyePosition();
	dst = src + forward;

	ray.Init(src, dst);

	g_csgo::Trace->TraceRay(ray, 0x46004003, &filter, &tr);

	if (!tr.m_pEnt)
		return;

	int hitgroup = tr.hitGroup;
	bool didHit = false;
	didHit = false;

	if (hitgroup == HITGROUP_HEAD || hitgroup == HITGROUP_CHEST || hitgroup == HITGROUP_STOMACH || hitgroup == HITGROUP_LEFTLEG || hitgroup == HITGROUP_RIGHTLEG)
		didHit = true;
	else
		didHit = false;

	if (tr.m_pEnt->GetTeam() == local_player->GetTeam()) return;
	if (tr.m_pEnt->GetHealth() <= 0) return;
	if (tr.m_pEnt->GetImmunity()) return;
	if (didHit) cmd->buttons |= IN_ATTACK;
}

//static int custom_delay = 0;
//#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
//#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
//CLegitBot::CLegitBot()
//{
//	best_target = -1;
//
//	view_angle = Vector(0.0f, 0.0f, 0.0f);
//	aim_angle = Vector(0.0f, 0.0f, 0.0f);
//	delta_angle = Vector(0.0f, 0.0f, 0.0f);
//	final_angle = Vector(0.0f, 0.0f, 0.0f);
//	hitbox_position = Vector(0.0f, 0.0f, 0.0f);
//
//	aim_smooth = 1;
//	aim_fov = 0;
//	randomized_smooth = 0;
//	recoil_min = 0;
//	recoil_max = 0;
//	randomized_angle = 0;
//	shoot = false;
//}
//
//void CLegitBot::do_aimbot(SDK::CBaseEntity *local, SDK::CBaseWeapon *weapon, SDK::CUserCmd  *cmd)
//{
//	if (!local) return;
//	if (!weapon) return;
//	if (!local->GetHealth() > 0) return;
//	if (weapon->get_full_info()->WeaponType == 9) return;
//	if (weapon->GetLoadedAmmo() == 0) return;
//
//	weapon_settings(weapon);
//	best_target = get_target(local, weapon, cmd, hitbox_position);
//
//	if (best_target == -1) return;
//
//	auto entity = g_csgo::ClientEntityList->GetClientEntity(best_target);
//	if (!entity) return;
//
//	if (MATH::get_distance(local->GetEyePosition(), hitbox_position) > 8192.0f)
//		return;
//
//	MATH::compute_angle(local->GetEyePosition(), hitbox_position, aim_angle);
//	MATH::NormalizeAngle(aim_angle);
//
//	aim_angle -= get_randomized_recoil(local);
//	aim_angle += get_randomized_angles(local);
//
//	MATH::NormalizeAngle(view_angle);
//
//	delta_angle = view_angle - aim_angle;
//	MATH::NormalizeAngle(delta_angle);
//
//	float randomSmoothing = 1.0f;
//
//	if (randomized_smooth > 1.0f)
//		randomSmoothing = UTILS::random_number_range(randomized_smooth / 10.0f, 1.0f);
//
//	final_angle = view_angle - delta_angle / aim_smooth * randomSmoothing;
//	MATH::NormalizeAngle(final_angle);
//
//	if (!MATH::sanitize_angles(final_angle))
//		return;
//
//	cmd->viewangles = final_angle;
//	g_csgo::Engine->SetViewAngles(cmd->viewangles);
//}
//
//void CLegitBot::weapon_settings(SDK::CBaseWeapon* weapon)
//{
//	if (!weapon)
//		return;
//
//
//	//if (weapon->IsPistol())
//	//{
//	aim_smooth = settings::options.legitaim_val;
//	aim_fov = settings::options.legitfov_val;
//	randomized_smooth = 1;
//	recoil_min = settings::options.rcsamount_min;
//	recoil_max = settings::options.rcsamount_max;
//	randomized_angle = 1;
//	//}
//	/*else if (weapon->IsRifle())
//	{
//	aim_key = g_Options.MainKey;
//	aim_smooth = g_Options.aim_RiflesSmooth;
//	aim_fov = g_Options.aim_RiflesFOV;
//	randomized_smooth = 1;
//	recoil_min = g_Options.aim_RiflesRCSMin;
//	recoil_max = g_Options.aim_RiflesRCSMax;
//	randomized_angle = 1;
//	}*/
//}
//
//Vector CLegitBot::get_randomized_recoil(SDK::CBaseEntity *local)
//{
//	Vector compensatedAngles = (local->GetPunchAngles() * 2.0f) * (UTILS::random_number_range(recoil_min, recoil_max) / 100.0f);
//	MATH::NormalizeAngle(compensatedAngles);
//
//	return (local->ShotsFired() > 1 ? compensatedAngles : Vector(0.0f, 0.0f, 0.0f));
//}
//
//Vector CLegitBot::get_randomized_angles(SDK::CBaseEntity *local)
//{
//	Vector randomizedValue = Vector(0.0f, 0.0f, 0.0f);
//
//	float randomRate = UTILS::random_number_range(-randomized_angle, randomized_angle);
//	float randomDeviation = UTILS::random_number_range(-randomized_angle, randomized_angle);
//
//	switch (rand() % 2)
//	{
//	case 0:
//		randomizedValue.x = (randomRate * cos(randomDeviation));
//		randomizedValue.y = (randomRate * cos(randomDeviation));
//		randomizedValue.z = (randomRate * cos(randomDeviation));
//		break;
//	case 1:
//		randomizedValue.x = (randomRate * sin(randomDeviation));
//		randomizedValue.y = (randomRate * sin(randomDeviation));
//		randomizedValue.z = (randomRate * sin(randomDeviation));
//		break;
//	}
//
//	MATH::NormalizeAngle(randomizedValue);
//
//	return (local->ShotsFired() > 1 ? randomizedValue : Vector(0.0f, 0.0f, 0.0f));
//}
//
//bool CLegitBot::get_hitbox(SDK::CBaseEntity *local, SDK::CBaseEntity *entity, Vector &destination)
//{
//	int bestHitbox = -1;
//	float best_fov = aim_fov;
//
//	int hitboxspots;
//
//	/*switch (g_Options.hitbox)
//	{
//	case 0:
//	break;
//	case 1:
//	hitboxspots = HITBOX_HEAD;
//	break;
//	case 2:
//	hitboxspots = HITBOX_NECK;
//	break;
//	case 3:
//	hitboxspots = HITBOX_CHEST;
//	break;
//	case 4:
//	hitboxspots = HITBOX_STOMACH;
//	break;
//	case 5:*/
//	hitboxspots = SDK::HITBOX_HEAD, SDK::HITBOX_NECK, SDK::HITBOX_CHEST, SDK::HITBOX_STOMACH;
//	/*	break;
//	}*/
//
//	std::vector<int> hitboxes = { hitboxspots };
//	for (auto hitbox : hitboxes)
//	{
//		Vector temp;
//		if (!entity->GetHitboxPos(hitbox, temp))
//			continue;
//
//		float fov = MATH::GetFov(view_angle, MATH::CalcAngle(local->GetEyePosition(), temp));
//		if (fov < best_fov)
//		{
//			best_fov = fov;
//			bestHitbox = hitbox;
//		}
//	}
//
//	if (bestHitbox != -1)
//	{
//		if (!entity->GetHitboxPos(bestHitbox, destination))
//			return true;
//	}
//
//	return false;
//}
//
//int CLegitBot::get_target(SDK::CBaseEntity *local, SDK::CBaseWeapon *weapon, SDK::CUserCmd *cmd, Vector &destination)
//{
//	int best_target = -1;
//	float best_fov = FLT_MAX;
//
//	g_csgo::Engine->GetViewAngles(view_angle);
//
//	for (int i = 1; i <= 65; i++)
//	{
//		auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
//		if (!entity_is_valid(i)) continue;
//
//		Vector hitbox;
//		if (get_hitbox(local, entity, hitbox)) continue;
//
//		float fov = MATH::GetFov(view_angle + (local->GetPunchAngles() * 2.0f), MATH::CalcAngle(local->GetEyePosition(), hitbox));
//		if (fov < best_fov && fov < aim_fov)
//		{
//			if (local->CanSeePlayer(entity, hitbox))
//			{
//				best_fov = fov;
//				destination = hitbox;
//				best_target = i;
//			}
//		}
//	}
//
//	return best_target;
//}
CLegitBot* legitbot = new CLegitBot();