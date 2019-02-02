#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Movement.h"

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)

void CMovement::bunnyhop(SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (cmd->buttons & IN_JUMP)
	{
		int local_flags = local_player->GetFlags();
		if (!(local_flags & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;
	}
}

float get_move_angle(float speed)
{
	auto move_angle = RAD2DEG(asin(15.f / speed));

	if (!isfinite(move_angle) || move_angle > 90.f)
		move_angle = 90.f;
	else if (move_angle < 0.f)
		move_angle = 0.f;

	return move_angle;
}

bool get_closest_plane(Vector* plane)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return false;

	SDK::trace_t trace; SDK::CTraceWorldOnly filter; SDK::Ray_t ray;

	auto start = local_player->GetVecOrigin(), mins = local_player->GetVecMins(), maxs = local_player->GetVecMaxs();

	Vector planes;
	auto count = 0;

	for (auto step = 0.f; step <= M_PI * 2.f; step += M_PI / 10.f)
	{
		auto end = start;

		end.x += cos(step) * 64.f;
		end.y += sin(step) * 64.f;

		ray.Init(start, end, mins, maxs);
		g_csgo::Trace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		if (trace.flFraction < 1.f)
		{
			planes += trace.plane.normal;
			count++;
		}
	}

	planes /= count;

	if (planes.z < 0.1f) { *plane = planes; return true; }
	return false;
}

bool will_hit_obstacle_in_future(float predict_time, float step)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return false;

	static auto sv_gravity = g_csgo::cvar->FindVar("sv_gravity");
	static auto sv_jump_impulse = g_csgo::cvar->FindVar("sv_jump_impulse");

	bool ground = local_player->GetFlags() & FL_ONGROUND;
	auto gravity_per_tick = sv_gravity->GetFloat() * g_csgo::Globals->interval_per_tick;

	auto start = local_player->GetVecOrigin(), end = start;
	auto velocity = local_player->GetVelocity();

	auto min = local_player->GetVecMins(), maxs = local_player->GetVecMaxs();

	SDK::trace_t trace;
	SDK::CTraceWorldOnly filter;
	SDK::Ray_t ray;

	auto predicted_ticks_needed = TIME_TO_TICKS(predict_time);
	auto velocity_rotation_angle = RAD2DEG(atan2(velocity.y, velocity.x));
	auto ticks_done = 0;

	if (predicted_ticks_needed <= 0)
		return false;

	while (true)
	{
		auto rotation_angle = velocity_rotation_angle + step;

		velocity.x = cos(DEG2RAD(rotation_angle)) * velocity.Length2D();
		velocity.y = sin(DEG2RAD(rotation_angle)) * velocity.Length2D();
		velocity.z = ground ? sv_jump_impulse->GetFloat() : velocity.z - gravity_per_tick;

		end += velocity * g_csgo::Globals->interval_per_tick;

		ray.Init(start, end, min, maxs);
		g_csgo::Trace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		if (trace.flFraction != 1.f && trace.plane.normal.z <= 0.9f || trace.startSolid || trace.allsolid)
			break;

		end = trace.end;
		end.z -= 2.f;

		ray.Init(trace.end, end, min, maxs);
		g_csgo::Trace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		ground = (trace.flFraction < 1.f || trace.allsolid || trace.startSolid) && trace.plane.normal.z >= 0.7f;

		if (++ticks_done >= predicted_ticks_needed)
			return false;

		velocity_rotation_angle = rotation_angle;
	}
	return true;
}

void CMovement::circle_strafe(SDK::CUserCmd* cmd, float* circle_yaw)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	const auto min_step = 2.25f;
	const auto max_step = 5.f;

	auto velocity_2d = local_player->GetVelocity().Length2D();
	auto velocity = local_player->GetVelocity();
	velocity.z = 0.f;

	auto circlemin = settings::options.circlemin / 10;
	auto circlemax = settings::options.circlemax / 10;
	auto circlevel = settings::options.circlevel;
	auto circlemulti = settings::options.circlenormalizemultiplier / 1000;

	auto ideal_strafe = clamp(get_move_angle(velocity_2d) * 2.f, min_step, max_step);
	auto predict_time = clamp(295.f / velocity_2d, 0.25f, 1.15f);
	//auto predict_time = clamp(circlevel / velocity_2d, circlemin, circlemax);

	auto step = ideal_strafe;

	while (true)
	{
		if (!will_hit_obstacle_in_future(predict_time, step) || step > max_step)
			break;

		step += 0.2f;
	}

	if (step > max_step)
	{
		step = ideal_strafe;

		while (true)
		{
			if (!will_hit_obstacle_in_future(predict_time, step) || step < -min_step)
				break;

			step -= 0.2f;
		}

		if (step < -min_step)
		{
			Vector plane;
			if (get_closest_plane(&plane))
				//step = -MATH::NormalizeAngle(*circle_yaw - RAD2DEG(atan2(plane.y, plane.x))) * circlemulti;
				step = -MATH::NormalizeAngle(*circle_yaw - RAD2DEG(atan2(plane.y, plane.x))) * 0.03f;
		}
		else
			step -= 0.2f;
	}
	else
		step += 0.2f;

	global::originalCMD.viewangles.y = *circle_yaw = MATH::NormalizeAngle(*circle_yaw + step);
	global::originalCMD.move.y = copysign(450.f, -step);
}

void CMovement::autostrafer(SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!settings::options.strafe_bool)
		return;

	static auto switch_key = 1.f;
	static auto circle_yaw = 0.f;
	static auto old_yaw = 0.f;

	auto velocity = local_player->GetVelocity();
	velocity.z = 0.f;

	auto speed = velocity.Length2D();
	auto ideal_strafe = get_move_angle(speed);

	switch_key *= -1.f;

	if (!(local_player->GetFlags() & FL_ONGROUND))
	{
		if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(settings::options.circlestrafekey)))
		{
			circle_strafe(cmd, &circle_yaw);
			return;
		}

		auto yaw_delta = MATH::NormalizeAngle(global::originalCMD.viewangles.y - old_yaw);
		auto absolute_yaw_delta = abs(yaw_delta);

		circle_yaw = old_yaw = global::originalCMD.viewangles.y;

		if (yaw_delta > 0.f)
			global::originalCMD.move.y = -450.f;
		else if (yaw_delta < 0.f)
			global::originalCMD.move.y = 450.f;

		if (absolute_yaw_delta <= ideal_strafe || absolute_yaw_delta >= 30.f)
		{
			Vector velocity_angles;
			MATH::VectorAngles2(velocity, velocity_angles);

			auto velocity_delta = MATH::NormalizeAngle(global::originalCMD.viewangles.y - velocity_angles.y);
			auto retrack = 2.f * (ideal_strafe * 2.f);

			if (velocity_delta <= retrack || speed <= 15.f)
			{
				if (-retrack <= velocity_delta || speed <= 15.f)
				{
					global::originalCMD.viewangles.y += ideal_strafe * switch_key;
					global::originalCMD.move.y = switch_key * 450.f;
				}
				else
				{
					global::originalCMD.viewangles.y = velocity_angles.y - retrack;
					global::originalCMD.move.y = 450.f;
				}
			}
			else
			{
				global::originalCMD.viewangles.y = velocity_angles.y + retrack;
				global::originalCMD.move.y = -450.f;
			}
		}
	}

	/*if (GetAsyncKeyState(0x57) || GetAsyncKeyState(0x41) || GetAsyncKeyState(0x53) || GetAsyncKeyState(0x44)) return;
	int local_flags = local_player->GetFlags();
	if (!(local_flags & FL_ONGROUND))
	{
		if (cmd->mousedx > 1 || cmd->mousedx < -1)
			cmd->sidemove = cmd->mousedx < 0.0f ? -450 : 450;
		else
		{
			cmd->forwardmove = 4200.0f / local_player->GetVelocity().Length();
			cmd->sidemove = (cmd->command_number % 2) == 0 ? -450 : 450;
			if (cmd->forwardmove >  450)
				cmd->forwardmove = 450;
		}
	}
	else if (cmd->buttons & IN_JUMP)
		cmd->forwardmove = 450;*/
}

void CMovement::duckinair(SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	int local_flags = local_player->GetFlags();
	if (!(local_flags & FL_ONGROUND))
		cmd->buttons |= IN_DUCK;
}

CMovement* movement = new CMovement();