#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/IEngine.h"
#include "../SDK/SurfaceData.h"
#include "../SDK/CTrace.h"

#include "../FEATURES/AutoWall.h"

static bool is_autowalling = false;


CAutowall* AutoWall = new CAutowall;

float CAutowall::GetHitgroupDamageMultiplier(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_HEAD:
	{
		return 4.0f;
	}
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
	{
		return 1.0f;
	}
	case HITGROUP_STOMACH:
	{
		return 1.25f;
	}
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
	{
		return 0.75f;
	}
	}
	return 1.0f;
}

void CAutowall::ScaleDamage(int hitgroup, SDK::CBaseEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
	current_damage *= GetHitgroupDamageMultiplier(hitgroup);
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!settings::options.friendfire && enemy->GetTeam() == local_player->GetTeam())
	{
		current_damage = 0;
		return;
	}

	int armor = enemy->GetArmor();
	int helmet = enemy->HasHelmet();

	if (armor > 0)
	{
		if (hitgroup == HITGROUP_HEAD)
		{
			if (helmet)
				current_damage *= (weapon_armor_ratio * .5f);
		}
		else
		{
			current_damage *= (weapon_armor_ratio * .5f);
		}
	}
}

bool CAutowall::DidHitNonWorldEntity(SDK::CBaseEntity* m_pEnt)
{
	return m_pEnt != NULL && m_pEnt == g_csgo::ClientEntityList->GetClientEntity(0);
}

bool CAutowall::TraceToExit(Vector& end, SDK::trace_t& tr, float x, float y, float z, float x2, float y2, float z2, SDK::trace_t* trace)
{

	typedef bool(__fastcall* TraceToExitFn)(Vector&, SDK::trace_t&, float, float, float, float, float, float, SDK::trace_t*);
	static TraceToExitFn TraceToExit = (TraceToExitFn)UTILS::FindSignature("client_panorama.dll", "55 8B EC 83 EC 30 F3 0F 10 75");
	if (!TraceToExit)
	{
		return false;
	}//(Vector&, trace_t&, float, float, float, float, float, float, trace_t*);
	_asm
	{
		push trace
		push z2
		push y2
		push x2
		push z
		push y
		push x
		mov edx, tr
		mov ecx, end
		call TraceToExit
		add esp, 0x1C
	}
}

bool CAutowall::HandleBulletPenetration(SDK::CSWeaponInfo *wpn_data, FireBulletData &data)
{
	SDK::surfacedata_t *enter_surface_data = g_csgo::PhysicsProps->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = *(float*)((DWORD)enter_surface_data + 80);

	data.trace_length += data.enter_trace.flFraction * data.trace_length_remaining;
	data.current_damage *= pow((wpn_data->m_RangeModifier), (data.trace_length * 0.002));

	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;

	if (data.penetrate_count <= 0)
		return false;

	Vector dummy;
	SDK::trace_t trace_exit;
	if (!TraceToExit(dummy, data.enter_trace, data.enter_trace.end.x, data.enter_trace.end.y, data.enter_trace.end.z, data.direction.x, data.direction.y, data.direction.z, &trace_exit))
		return false;




	SDK::surfacedata_t *exit_surface_data = g_csgo::PhysicsProps->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = *(float*)((DWORD)exit_surface_data + 76);
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->m_Penetration) * 1.25f);
	float thickness = (trace_exit.end - data.enter_trace.end).Length();

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;

	float lost_damage = fmaxf(0.0f, v35 + thickness);

	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage >= 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.end;
	data.penetrate_count--;

	return true;
}

bool CAutowall::SimulateFireBullet(FireBulletData &data)
{
	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));


	// weapon data
	auto weaponData = weapon->get_full_info();

	if (weaponData == NULL)
		return false;

	data.current_damage = (float)weaponData->m_Damage;

	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = weaponData->m_Range - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;



		UTIL_TraceLine(data.src, end, 0x4600400B, local_player, &data.enter_trace);
		UTIL_ClipTraceToPlayers(data.src, end + data.direction * 40.f, 0x4600400B, &data.filter, &data.enter_trace);

		if (data.enter_trace.flFraction == 1.0f)
			break;

		if ((data.enter_trace.hitGroup <= 7) && (data.enter_trace.hitGroup > 0))
		{
			data.trace_length += data.enter_trace.flFraction * data.trace_length_remaining;
			data.current_damage *= pow(weaponData->m_RangeModifier, data.trace_length * 0.002);
			ScaleDamage(data.enter_trace.hitGroup, data.enter_trace.m_pEnt, weaponData->m_ArmorRatio, data.current_damage);

			return true;
		}

		if (!HandleBulletPenetration(weaponData, data))
			break;
	}

	return false;
}
void VectorAnglesxd(const Vector& forward, QAngle &angles)
{
	if (forward[1] == 0.0f && forward[0] == 0.0f)
	{
		angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
		angles[1] = 0.0f;  //yaw left/right
	}
	else
	{
		angles[0] = atan2(-forward[2], forward.Length2D()) * -180 / M_PI;
		angles[1] = atan2(forward[1], forward[0]) * 180 / M_PI;

		if (angles[1] > 90) angles[1] -= 180;
		else if (angles[1] < 90) angles[1] += 180;
		else if (angles[1] == 90) angles[1] = 0;
	}

	angles[2] = 0.0f;
}
QAngle CalcAngle(Vector src, Vector dst)
{
	QAngle angles;
	Vector delta = src - dst;
	VectorAnglesxd(delta, angles);
	delta.Normalize();
	return angles;
}
void inline SinCos(float radians, float *sine, float *cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);
}
void AngleVectors(const QAngle &angles, Vector *forward)
{
	float sp, sy, cp, cy;

	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

float CAutowall::GetDamagez(const Vector& point)
{
	float damage = 0.f;
	Vector dst = point;
	FireBulletData data;
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	data.src = local_player->GetEyePosition();
	data.filter.pSkip1 = local_player;

	//data.filter.pSkip = G::LocalPlayer;

	QAngle angles = CalcAngle(data.src, dst);
	AngleVectors(angles, &data.direction);
	Vector dataNormalized;

	data.direction.Normalize();

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (SimulateFireBullet(data))
	{
		damage = data.current_damage;
	}

	return damage;
}