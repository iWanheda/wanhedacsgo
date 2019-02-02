#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/AntiAim.h"
#include "../FEATURES/AutoWall.h"
#include "../UTILS/render.h"
#include "../SDK/NetChannel.h"


#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

float randnum(float Min, float Max)
{
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

bool next_lby_update(const float yaw_to_break, SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = g_csgo::Globals->curtime;
	local_update = next_lby_update_time;

	auto animstate = local_player->GetAnimState();

	if (!animstate)
		return false;

	if (!(local_player->GetFlags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d > 0.1f && !fake_walk)
		next_lby_update_time = curtime + 0.22f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}

float fov_player(Vector ViewOffSet, Vector View, SDK::CBaseEntity* entity, int hitbox)
{
	const float MaxDegrees = 180.0f;
	Vector Angles = View, Origin = ViewOffSet;
	Vector Delta(0, 0, 0), Forward(0, 0, 0);
	Vector AimPos = aimbot->get_hitbox_pos(entity, hitbox);

	MATH::AngleVectors(Angles, &Forward);
	MATH::VectorSubtract(AimPos, Origin, Delta);
	MATH::NormalizeNum(Delta, Delta);

	float DotProduct = Forward.Dot(Delta);
	return (acos(DotProduct) * (MaxDegrees / M_PI));
}

int closest_to_crosshair()
{
	int index = -1;
	float lowest_fov = INT_MAX;

	SDK::CBaseEntity* local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return -1;

	Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();

	Vector angles;
	g_csgo::Engine->GetViewAngles(angles);

	for (int i = 1; i <= g_csgo::Globals->maxclients; i++)
	{
		SDK::CBaseEntity *entity = g_csgo::ClientEntityList->GetClientEntity(i);

		if (!entity || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || entity->GetIsDormant() || entity == local_player)
			continue;

		float fov = fov_player(local_position, angles, entity, 0);

		if (fov < lowest_fov)
		{
			lowest_fov = fov;
			index = i;
		}
	}

	return index;
}

void AngleVectorsp(const Vector angles, Vector& forward, Vector& right, Vector& up)
{
	float angle;
	static float sp, sy, cp, cy;

	angle = angles[0] * (M_PI / 180.f);
	sp = sin(angle);
	cp = cos(angle);

	angle = angles[1] * (M_PI / 180.f);
	sy = sin(angle);
	cy = cos(angle);


	forward[0] = cp * cy;
	forward[1] = cp * sy;
	forward[2] = -sp;


	static float sr, cr;

	angle = angles[2] * (M_PI / 180.f);
	sr = sin(angle);
	cr = cos(angle);


	right[0] = -1 * sr * sp * cy + -1 * cr * -sy;
	right[1] = -1 * sr * sp * sy + -1 * cr *cy;
	right[2] = -1 * sr * cp;

	up[0] = cr * sp *cy + -sr * -sy;
	up[1] = cr * sp *sy + -sr * cy;
	up[2] = cr * cp;


}
float detectdatwallbitch(float yaw) //actual freestanding part
{
	float Back, Right, Left;

	Vector src3D, dst3D, forward, right, up, src, dst;
	SDK::trace_t tr;
	SDK::Ray_t ray, ray2, ray3, ray4, ray5;
	SDK::CTraceFilter filter;

	Vector viewangles;
	g_csgo::Engine->GetViewAngles(viewangles);

	viewangles.x = 0;

	AngleVectorsp(viewangles, forward, right, up);
	int index = closest_to_crosshair();
	auto entity = g_csgo::ClientEntityList->GetClientEntity(index);
	auto local = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	filter.pSkip1 = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	src3D = local->GetEyePosition();
	dst3D = src3D + (forward * 384);

	ray.Init(src3D, dst3D);

	g_csgo::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	Back = (tr.end - tr.start).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);

	g_csgo::Trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);

	Right = (tr.end - tr.start).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);

	g_csgo::Trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);

	Left = (tr.end - tr.start).Length();

	if (Back < Right && Back < Left && Left == Right && !(entity == nullptr)) {
		return (yaw); //if left and right are equal and better than back
	}
	if (Back > Right && Back > Left && !(entity == nullptr)) {
		return (yaw - 180); //if back is the best angle
	}
	else if (Right > Left && Right > Back && !(entity == nullptr)) {
		return (yaw + 90); //if right is the best angle
	}
	else if (Right > Left && Right == Back && !(entity == nullptr)) {
		return (yaw + 135); //if right is equal to back
	}
	else if (Left > Right && Left > Back && !(entity == nullptr)) {
		return (yaw - 90); //if left is the best angle
	}
	else if (Left > Right && Left == Back && !(entity == nullptr)) {
		return (yaw - 135); //if left is equal to back
	}
	else { //if none of the above cases is correct
		if (entity == nullptr) //if there is no enemy / the enemy is null
			return (yaw - 180); //do backward antiaim
		else //if the entity isn't null
			return ((UTILS::CalcAngle(local->GetVecOrigin(), entity->GetVecOrigin()).y) + 180); //else do backward at target antiaim
	}

	return 0;
}

void ZAnti(SDK::CUserCmd* cmd)
{
	constexpr auto maxRange = 90.0f;

	// where you want your head to go
	constexpr auto angleAdditive = 180.0f;

	// to make it equal on both sides / by 2
	cmd->viewangles.y += angleAdditive - maxRange / 2.f + std::fmodf(g_csgo::Globals->curtime * 150, maxRange);
}

void CAntiAim::freestand(SDK::CUserCmd* cmd) //setting up aa
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	//float reverse_delta = settings::options.delta_val * (-1);

	if (!(local_player->GetFlags() & FL_ONGROUND)) {
		ZAnti(cmd);
		global::breaking_lby = false;
	}
	else if (local_player->GetVelocity().Length2D() > 0.1 && !global::is_fakewalking) {
		cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y);
		global::breaking_lby = false;
	}
	else {
		if (next_lby_update(cmd->viewangles.y, cmd)) {
			cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) + settings::options.delta_val;
			global::breaking_lby = true;
		}
		//else if ((local_update + 0.05f) <= 0 && settings::options.cradle)
		//	cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) + reverse_delta;
		else {
			cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) - 35.0f / 2.f + std::fmodf(g_csgo::Globals->curtime * 150, 35.0f);
			global::breaking_lby = false;
		}
	}
}

void autoDirection(SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	static float last_real;
	bool no_active = true;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	Vector besthead;

	auto leyepos = local_player->GetVecOrigin() + local_player->GetViewOffset();
	auto headpos = aimbot->get_hitbox_pos(local_player, 0);
	auto origin = local_player->GetAbsOrigin();

	auto checkWallThickness = [&](SDK::CBaseEntity* pPlayer, Vector newhead) -> float
	{
		Vector endpos1, endpos2;
		Vector eyepos = pPlayer->GetVecOrigin() + pPlayer->GetViewOffset();

		SDK::Ray_t ray;
		ray.Init(newhead, eyepos);

		SDK::CTraceFilterSkipTwoEntities filter(pPlayer, local_player);

		SDK::trace_t trace1, trace2;
		g_csgo::Trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.end;
		else
			return 0.f;

		ray.Init(eyepos, newhead);
		g_csgo::Trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

		if (trace2.DidHit())
			endpos2 = trace2.end;

		float add = newhead.DistTo(eyepos) - leyepos.DistTo(eyepos) + 3.f;
		return endpos1.DistTo(endpos2) + add / 3;
	};

	int index = closest_to_crosshair();
	auto entity = g_csgo::ClientEntityList->GetClientEntity(index);

	float step = (2 * M_PI) / 18.f;
	float radius = fabs(Vector(headpos - origin).Length2D());

	if (index == -1)
	{
		no_active = true;
	}
	else
	{
		for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
		{
			Vector newhead(radius * cos(rotation) + leyepos.x, radius * sin(rotation) + leyepos.y, leyepos.z);

			float totalthickness = 0.f;

			no_active = false;

			totalthickness += checkWallThickness(entity, newhead);

			if (totalthickness > highestthickness)
			{
				highestthickness = totalthickness;
				bestrotation = rotation;
				besthead = newhead;
			}
		}
	}
	if (no_active)
		cmd->viewangles.y += 180.f;
	else
		cmd->viewangles.y = RAD2DEG(bestrotation);

	last_real = cmd->viewangles.y;
}

void CAntiAim::do_antiaim(SDK::CUserCmd* cmd) {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;
	if (local_player->GetHealth() <= 0) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon) return; auto grenade = (SDK::CBaseCSGrenade*)weapon;
	if (local_player->GetMoveType() == SDK::MOVETYPE_LADDER) return;
	if (cmd->buttons & IN_USE) return;

	if (cmd->buttons & IN_ATTACK && aimbot->can_shoot(cmd)) return;
	if (weapon->get_full_info()->m_WeaponType == 9) return;
	if (!settings::options.aa_bool) return;

	if (weapon->is_grenade() && grenade->pin_pulled()) return;

	if (global::packets_choked > 1)
		global::Should_Desync = true;
	else
		global::Should_Desync = false;

	auto get_max_desync_delta = [](void) {

		auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));

		auto animstate = local_player->GetAnimState();

		float duckammount = *(float *)(animstate + 0xA4);
		float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));

		float speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));

		float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
		float unk2 = unk1 + 1.f;

		if (duckammount > 0)
			unk2 += ((duckammount * speedfactor) * (0.5f - unk2));

		float unk3 = *(float *)(animstate + 0x334) * unk2;

		return unk3;
	};

	auto get_fixed_feet_yaw = [](void) {

		auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));

		auto state = local_player->GetAnimState();

		float current_feet_yaw = *(float *)(state + 0x80);

		if (current_feet_yaw >= -360)
			current_feet_yaw = min(current_feet_yaw, 360.f);

		return current_feet_yaw;
	};

	static float last_real; int local_flags = local_player->GetFlags();
	if ((local_player->GetVelocity().Length2D() < 80) && !(cmd->buttons & IN_JUMP) && !fake_walk) {

		switch (settings::options.aa_pitch_type) {

		case 0:								break;
		case 1: cmd->viewangles.x = 85.f;	break;
		case 2: cmd->viewangles.x = -179.f; break;
		case 3: cmd->viewangles.x = 179.f;	break;
		case 4: cmd->viewangles.x = 1080.f; break;
		}

		auto max = get_max_desync_delta() + get_fixed_feet_yaw();

		RENDER::DrawF(10, 10, FONTS::visuals_name_font, false, false, WHITE, std::to_string(get_max_desync_delta()));
		RENDER::DrawF(10, 30, FONTS::visuals_name_font, false, false, WHITE, std::to_string(get_fixed_feet_yaw()));

		auto net_channel = g_csgo::Engine->GetNetChannel();
		if (!net_channel)
			return;

		if (!global::should_send_packet && net_channel->m_nChokedPackets == 0)
			cmd->viewangles.y += -58;
		else {
			switch (settings::options.aa_real_type) {

			case 0: break;

			case 1: {
				if (next_lby_update(cmd->viewangles.y, cmd)) {
					cmd->viewangles.y += settings::options.delta_val;
					global::breaking_lby = true;
				}
				else cmd->viewangles.y += 180.f + settings::options.aa_realadditive_val;
			} break;

			case 2: {
				if (settings::options.flip_bool) {
					if (next_lby_update(cmd->viewangles.y + settings::options.delta_val, cmd)) {
						global::breaking_lby = true;
						cmd->viewangles.y = last_real + settings::options.delta_val;
					}
					else {
						global::breaking_lby = false;
						cmd->viewangles.y -= 90 + settings::options.aa_realadditive_val;
						last_real = cmd->viewangles.y;
					}
				}
				else {
					if (next_lby_update(cmd->viewangles.y - settings::options.delta_val, cmd)) {
						global::breaking_lby = true;
						cmd->viewangles.y = last_real - settings::options.delta_val;
					}
					else {
						global::breaking_lby = false;
						cmd->viewangles.y += 90 + settings::options.aa_realadditive_val;
						last_real = cmd->viewangles.y;
					}
				}
			}break;
			case 3: {
				if (next_lby_update(cmd->viewangles.y, cmd)) {
					cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) + settings::options.delta_val;
					global::breaking_lby = true;
				}
				else {
					cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) - (settings::options.spinangle + 1) / 2.f + std::fmodf(g_csgo::Globals->curtime * ((settings::options.spinspeed1 * 5) + 1), (settings::options.spinangle + 1));
					global::breaking_lby = false;
				}
			} break;
			}
		}
	}
	else if ((local_player->GetVelocity().Length2D() > 80) && (!(cmd->buttons & IN_JUMP) && (local_flags & FL_ONGROUND) && !fake_walk)) {

		switch (settings::options.aa_pitch1_type) {

		case 0: break;
		case 1: cmd->viewangles.x = 85.f; break;
		case 2: cmd->viewangles.x = -179.f; break;
		case 3: cmd->viewangles.x = 179.f; break;
		case 4: cmd->viewangles.x = 1080.f; break;
		}
		switch (settings::options.aa_real1_type)
		{
		case 0: break;
		case 1: {
			if (next_lby_update(cmd->viewangles.y, cmd)) {
				cmd->viewangles.y += settings::options.delta1_val;
				global::breaking_lby = true;
			}
			else
				cmd->viewangles.y += 180.f + settings::options.aa_realadditive1_val;
		} break;
		case 2: {
			if (settings::options.flip_bool)
			{
				if (next_lby_update(cmd->viewangles.y + settings::options.delta1_val, cmd))
				{
					global::breaking_lby = true;
					cmd->viewangles.y = last_real + settings::options.delta1_val;
				}
				else
				{
					//global::breaking_lby = false;
					cmd->viewangles.y -= 90 + settings::options.aa_realadditive1_val;
					last_real = cmd->viewangles.y;
				}
			}
			else
			{
				if (next_lby_update(cmd->viewangles.y - settings::options.delta1_val, cmd))
				{
					cmd->viewangles.y = last_real - settings::options.delta1_val;
					global::breaking_lby = true;
				}
				else
				{
					//global::breaking_lby = false;
					cmd->viewangles.y += 90 + settings::options.aa_realadditive1_val;
					last_real = cmd->viewangles.y;
				}
			} }break;
		case 3: //freestand(cmd); break;
		{
			cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) - (settings::options.spinangle1 + 1) / 2.f + std::fmodf(g_csgo::Globals->curtime * ((settings::options.spinspeed1 * 5) + 1), (settings::options.spinangle1 + 1));
			//cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y);
		//	global::breaking_lby = false;
		} break;
		//}
		}
	}
	else if (!fake_walk)
	{
		switch (settings::options.aa_pitch2_type)
		{
		case 0: break;
		case 1: cmd->viewangles.x = 85.f; break;
		case 2: cmd->viewangles.x = -179.f; break;
		case 3: cmd->viewangles.x = 179.f; break;
		case 4: cmd->viewangles.x = 1080.f; break;
		}
		/*if (global::should_send_packet)
		{
			switch (settings::options.aa_fake2_type)
			{
			case 0: break;
			case 1: cmd->viewangles.y += 180 + ((rand() % 15) - (15 * 0.5f)) + settings::options.aa_fakeadditive2_val; break;
			case 2: cmd->viewangles.y += randnum(-180, 180) + settings::options.aa_fakeadditive2_val; break;
			case 3: cmd->viewangles.y += 0 + settings::options.aa_fakeadditive2_val; break;
			case 4: cmd->viewangles.y = global::real_angles.y + 180.f + settings::options.aa_fakeadditive2_val; break;
			case 5: {
				static int y2 = -settings::options.spinanglefake;
				int speed = settings::options.spinspeedfake;
				y2 += speed; if (y2 >= settings::options.spinanglefake) y2 = -settings::options.spinanglefake;
				cmd->viewangles.y = y2 + settings::options.aa_fakeadditive2_val;
			} break;
			}
		}
		else
		{*/
		switch (settings::options.aa_real2_type)
		{
		case 0: break;
		case 1: {
			if (next_lby_update(cmd->viewangles.y, cmd)) {
				cmd->viewangles.y += settings::options.delta2_val;
				global::breaking_lby = true;
			}
			else
				cmd->viewangles.y += 180.f + settings::options.aa_realadditive2_val;
		} break;
		case 2: {
			if (settings::options.flip_bool)
			{
				if (next_lby_update(cmd->viewangles.y + settings::options.delta2_val, cmd))
				{
					global::breaking_lby = true;
					cmd->viewangles.y = last_real + settings::options.delta2_val;
				}
				else
				{
					global::breaking_lby = false;
					cmd->viewangles.y -= 90 + settings::options.aa_realadditive2_val;
					last_real = cmd->viewangles.y;
				}
			}
			else
			{
				if (next_lby_update(cmd->viewangles.y - settings::options.delta2_val, cmd))
				{
					cmd->viewangles.y = last_real - settings::options.delta2_val;
					global::breaking_lby = true;
				}
				else
				{
					global::breaking_lby = false;
					cmd->viewangles.y += 90 + settings::options.aa_realadditive2_val;
					last_real = cmd->viewangles.y;
				}
			} }break;
		case 3: //freestand(cmd); break;
		{
			cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) - settings::options.spinangle2 / 2.f + std::fmodf(g_csgo::Globals->curtime * (settings::options.spinspeed2 * 5), settings::options.spinangle2);
			//global::breaking_lby = false;
		} break;
		//	}
		}
	}
	else
	{
		switch (settings::options.aa_pitch3_type)
		{
		case 0: break;
		case 1: cmd->viewangles.x = 85.f; break;
		case 2: cmd->viewangles.x = -179.f; break;
		case 3: cmd->viewangles.x = 179.f; break;
		case 4: cmd->viewangles.x = 1080.f; break;
		}
		/*if (global::should_send_packet)
		{
			switch (settings::options.aa_fake2_type)
			{
			case 0: break;
			case 1: cmd->viewangles.y += 180 + ((rand() % 15) - (15 * 0.5f)) + settings::options.aa_fakeadditive2_val; break;
			case 2: cmd->viewangles.y += randnum(-180, 180) + settings::options.aa_fakeadditive2_val; break;
			case 3: cmd->viewangles.y += 0 + settings::options.aa_fakeadditive2_val; break;
			case 4: cmd->viewangles.y = global::real_angles.y + 180.f + settings::options.aa_fakeadditive2_val; break;
			case 5: {
				static int y2 = -settings::options.spinanglefake;
				int speed = settings::options.spinspeedfake;
				y2 += speed; if (y2 >= settings::options.spinanglefake) y2 = -settings::options.spinanglefake;
				cmd->viewangles.y = y2 + settings::options.aa_fakeadditive2_val;
			} break;
			}
		}
		else
		{*/
		if (global::should_send_packet && settings::options.desync_yaw)
			cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) + get_max_desync_delta();
		else {
			switch (settings::options.aa_real3_type)
			{
			case 0: break;
			case 1: {
				if (next_lby_update(cmd->viewangles.y, cmd))
					cmd->viewangles.y += settings::options.delta3_val;
				else
					cmd->viewangles.y += 180.f + settings::options.aa_realadditive3_val;
			} break;
			case 2: {
				if (settings::options.flip_bool)
				{
					if (next_lby_update(cmd->viewangles.y + settings::options.delta3_val, cmd))
					{
						global::breaking_lby = true;
						cmd->viewangles.y = last_real + settings::options.delta3_val;
					}
					else
					{
						global::breaking_lby = false;
						cmd->viewangles.y -= 90 + settings::options.aa_realadditive3_val;
						last_real = cmd->viewangles.y;
					}
				}
				else
				{
					if (next_lby_update(cmd->viewangles.y - settings::options.delta3_val, cmd))
					{
						cmd->viewangles.y = last_real - settings::options.delta3_val;
						global::breaking_lby = true;
					}
					else
					{
						global::breaking_lby = false;
						cmd->viewangles.y += 90 + settings::options.aa_realadditive3_val;
						last_real = cmd->viewangles.y;
					}
				} }break;
			case 3: //freestand(cmd); break;
			{
				if (next_lby_update(cmd->viewangles.y, cmd)) {
					cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) + settings::options.delta3_val;
					global::breaking_lby = true;
				}
				else {
					cmd->viewangles.y = detectdatwallbitch(cmd->viewangles.y) - settings::options.spinangle3 / 2.f + std::fmodf(g_csgo::Globals->curtime * (settings::options.spinspeed3 * 5), settings::options.spinangle3);
					global::breaking_lby = false;
				}
			} break;
			//	}
			}
		}
	}
}

void CAntiAim::fix_movement(SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (settings::options.aa_pitch > 1 || settings::options.aa_pitch > 1 || settings::options.aa_pitch > 1)
	{
		cmd->move.x = clamp(cmd->move.x, -450.f, 450.f);
		cmd->move.y = clamp(cmd->move.y, -450.f, 450.f);
		cmd->move.z = clamp(cmd->move.z, -320.f, 320.f);

		cmd->viewangles.x = clamp(cmd->viewangles.x, -89.f, 89.f);
	}

	Vector real_viewangles;
	g_csgo::Engine->GetViewAngles(real_viewangles);

	Vector vecMove(cmd->move.x, cmd->move.y, cmd->move.z);
	float speed = sqrt(vecMove.x * vecMove.x + vecMove.y * vecMove.y);

	Vector angMove;
	MATH::VectorAngles(vecMove, angMove);

	float yaw = DEG2RAD(cmd->viewangles.y - real_viewangles.y + angMove.y);

	cmd->move.x = cos(yaw) * speed;
	cmd->move.y = sin(yaw) * speed;

	cmd->viewangles = MATH::NormalizeAngle(cmd->viewangles);

	if (cmd->viewangles.x < -89.f || cmd->viewangles.x > 89.f) cmd->move.x *= -1;
}

Vector CAntiAim::fix_movement(SDK::CUserCmd* cmd, SDK::CUserCmd originalCMD)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return Vector(0, 0, 0);

	Vector wish_forward, wish_right, wish_up, cmd_forward, cmd_right, cmd_up;

	auto viewangles = cmd->viewangles;
	auto movedata = originalCMD.move;
	viewangles.Normalize();

	if (!(local_player->GetFlags() & FL_ONGROUND) && viewangles.z != 0.f)
		movedata.y = 0.f;

	MATH::AngleVectors2(originalCMD.viewangles, &wish_forward, &wish_right, &wish_up);
	MATH::AngleVectors2(viewangles, &cmd_forward, &cmd_right, &cmd_up);

	auto v8 = sqrt(wish_forward.x * wish_forward.x + wish_forward.y * wish_forward.y), v10 = sqrt(wish_right.x * wish_right.x + wish_right.y * wish_right.y), v12 = sqrt(wish_up.z * wish_up.z);

	Vector wish_forward_norm(1.0f / v8 * wish_forward.x, 1.0f / v8 * wish_forward.y, 0.f),
		wish_right_norm(1.0f / v10 * wish_right.x, 1.0f / v10 * wish_right.y, 0.f),
		wish_up_norm(0.f, 0.f, 1.0f / v12 * wish_up.z);

	auto v14 = sqrt(cmd_forward.x * cmd_forward.x + cmd_forward.y * cmd_forward.y), v16 = sqrt(cmd_right.x * cmd_right.x + cmd_right.y * cmd_right.y), v18 = sqrt(cmd_up.z * cmd_up.z);

	Vector cmd_forward_norm(1.0f / v14 * cmd_forward.x, 1.0f / v14 * cmd_forward.y, 1.0f / v14 * 0.0f),
		cmd_right_norm(1.0f / v16 * cmd_right.x, 1.0f / v16 * cmd_right.y, 1.0f / v16 * 0.0f),
		cmd_up_norm(0.f, 0.f, 1.0f / v18 * cmd_up.z);

	auto v22 = wish_forward_norm.x * movedata.x, v26 = wish_forward_norm.y * movedata.x, v28 = wish_forward_norm.z * movedata.x, v24 = wish_right_norm.x * movedata.y, v23 = wish_right_norm.y * movedata.y, v25 = wish_right_norm.z * movedata.y, v30 = wish_up_norm.x * movedata.z, v27 = wish_up_norm.z * movedata.z, v29 = wish_up_norm.y * movedata.z;

	Vector correct_movement;
	correct_movement.x = cmd_forward_norm.x * v24 + cmd_forward_norm.y * v23 + cmd_forward_norm.z * v25
		+ (cmd_forward_norm.x * v22 + cmd_forward_norm.y * v26 + cmd_forward_norm.z * v28)
		+ (cmd_forward_norm.y * v30 + cmd_forward_norm.x * v29 + cmd_forward_norm.z * v27);
	correct_movement.y = cmd_right_norm.x * v24 + cmd_right_norm.y * v23 + cmd_right_norm.z * v25
		+ (cmd_right_norm.x * v22 + cmd_right_norm.y * v26 + cmd_right_norm.z * v28)
		+ (cmd_right_norm.x * v29 + cmd_right_norm.y * v30 + cmd_right_norm.z * v27);
	correct_movement.z = cmd_up_norm.x * v23 + cmd_up_norm.y * v24 + cmd_up_norm.z * v25
		+ (cmd_up_norm.x * v26 + cmd_up_norm.y * v22 + cmd_up_norm.z * v28)
		+ (cmd_up_norm.x * v30 + cmd_up_norm.y * v29 + cmd_up_norm.z * v27);

	correct_movement.x = clamp(correct_movement.x, -450.f, 450.f);
	correct_movement.y = clamp(correct_movement.y, -450.f, 450.f);
	correct_movement.z = clamp(correct_movement.z, -320.f, 320.f);

	return correct_movement;
}

CAntiAim* antiaim = new CAntiAim();