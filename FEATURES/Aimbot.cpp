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
#include "../FEATURES/Movement.h"

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

int bestHitbox = -1, mostDamage, multipointCount = 0;;
bool lag_comp;

Vector multipoints[128];

void CAimbot::rotate_movement(float yaw, SDK::CUserCmd* cmd) {

	Vector viewangles;
	g_csgo::Engine->GetViewAngles(viewangles);

	float rotation = DEG2RAD(viewangles.y - yaw);

	float cos_rot = cos(rotation);
	float sin_rot = sin(rotation);

	float new_forwardmove = (cos_rot * cmd->move.x) - (sin_rot * cmd->move.y);
	float new_sidemove = (sin_rot * cmd->move.x) + (cos_rot * cmd->move.y);

	cmd->move.x = new_forwardmove;
	cmd->move.y = new_sidemove;
}

int lerped_ticks() {

	static const auto cl_interp_ratio = g_csgo::cvar->FindVar("cl_interp_ratio");
	static const auto cl_updaterate = g_csgo::cvar->FindVar("cl_updaterate");
	static const auto cl_interp = g_csgo::cvar->FindVar("cl_interp");

	return TIME_TO_TICKS(max(cl_interp->GetFloat(), cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat()));
}

static SDK::ConVar *big_ud_rate = nullptr, 
*min_ud_rate = nullptr, *max_ud_rate = nullptr, 
*interp_ratio = nullptr, *cl_interp = nullptr, 
*cl_min_interp = nullptr, *cl_max_interp = nullptr;

float LerpTime() {

	static SDK::ConVar* updaterate = g_csgo::cvar->FindVar("cl_updaterate");
	static SDK::ConVar* minupdate = g_csgo::cvar->FindVar("sv_minupdaterate");
	static SDK::ConVar* maxupdate = g_csgo::cvar->FindVar("sv_maxupdaterate");
	static SDK::ConVar* lerp = g_csgo::cvar->FindVar("cl_interp");
	static SDK::ConVar* cmin = g_csgo::cvar->FindVar("sv_client_min_interp_ratio");
	static SDK::ConVar* cmax = g_csgo::cvar->FindVar("sv_client_max_interp_ratio");
	static SDK::ConVar* ratio = g_csgo::cvar->FindVar("cl_interp_ratio");

	float lerpurmom = lerp->GetFloat(), maxupdateurmom = maxupdate->GetFloat(), 
	ratiourmom = ratio->GetFloat(), cminurmom = cmin->GetFloat(), cmaxurmom = cmax->GetFloat();
	int updaterateurmom = updaterate->GetInt(), 
	sv_maxupdaterate = maxupdate->GetInt(), sv_minupdaterate = minupdate->GetInt();

	if (sv_maxupdaterate && sv_minupdaterate) 
		updaterateurmom = maxupdateurmom;

	if (ratiourmom == 0) 
		ratiourmom = 1.0f;

	if (cmin && cmax && cmin->GetFloat() != 1) 
		ratiourmom = clamp(ratiourmom, cminurmom, cmaxurmom);

	return max(lerpurmom, ratiourmom / updaterateurmom);
}

bool CAimbot::good_backtrack_tick(int tick)
{
	auto nci = g_csgo::Engine->GetNetChannelInfo();
	if (!nci) return false;

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + LerpTime(), 0.f, 1.f);
	float delta_time = correct - (g_csgo::Globals->curtime - TICKS_TO_TIME(tick));
	return fabsf(delta_time) < 0.2f;
}

void CAimbot::run_aimbot(SDK::CUserCmd* cmd) 
{
	Entities.clear();
	SelectTarget();
	shoot_enemy(cmd);
}

void CAimbot::SelectTarget()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;
	for (int index = 1; index <= 65; index++)
	{
		auto entity = g_csgo::ClientEntityList->GetClientEntity(index);
		if (!entity) continue;
		if (entity->GetTeam() == local_player->GetTeam()) continue;
		if (entity->GetHealth() <= 0) continue;
		if (entity->GetClientClass()->m_ClassID != 38) continue;
		if (entity->GetVecOrigin() == Vector(0, 0, 0)) continue;
		if (entity->GetImmunity()) continue;
		if (entity->GetIsDormant())	continue;
		AimbotData_t data = AimbotData_t(entity, index);
		Entities.push_back(data);
	}
}
void CAimbot::lby_backtrack(SDK::CUserCmd *pCmd, SDK::CBaseEntity* pLocal, SDK::CBaseEntity* pEntity) {

	int index = pEntity->GetIndex();
	float PlayerVel = abs(pEntity->GetVelocity().Length2D());

	bool playermoving;

	if (PlayerVel > 0.f)
		playermoving = true;
	else
		playermoving = false;

	float lby = pEntity->GetLowerBodyYaw();
	static float lby_timer[65], lby_proxy[65];

	if (lby_proxy[index] != pEntity->GetLowerBodyYaw() && !playermoving) {

		lby_timer[index] = 0;
		lby_proxy[index] = pEntity->GetLowerBodyYaw();
	}

	if (!playermoving) {

		if (pEntity->GetSimTime() >= lby_timer[index]) {

			tick_to_back[index] = pEntity->GetSimTime();
			lby_to_back[index] = pEntity->GetLowerBodyYaw();
			lby_timer[index] = pEntity->GetSimTime() + g_csgo::Globals->interval_per_tick + 1.1;
		}
	}
	else {

		tick_to_back[index] = 0;
		lby_timer[index] = 0;
	}

	if (good_backtrack_tick(TIME_TO_TICKS(tick_to_back[index])))
		backtrack_tick[index] = true;
	else
		backtrack_tick[index] = false;
}

void auto_revolver(SDK::CUserCmd* cmd) {

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer())->GetActiveWeaponIndex());

	if (weapon->GetItemDefenitionIndex() == SDK::WEAPON_REVOLVER) {

		cmd->buttons |= IN_ATTACK;

		float flPostponeFireReady = weapon->GetPostponeFireReadyTime();

		if (flPostponeFireReady > 0 && flPostponeFireReady < g_csgo::Globals->curtime)
			cmd->buttons &= ~IN_ATTACK;
	}
}
inline float FastSqrt(float x)
{
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}
#define square( x ) ( x * x )
void quickstop(SDK::CUserCmd* cmd, float fMaxSpeed) {
	if (fMaxSpeed <= 0.f)
		return;
	float fSpeed = (float)(FastSqrt(square(global::originalCMD.move.x) + square(global::originalCMD.move.y) + square(global::originalCMD.move.z)));
	if (fSpeed <= 0.f)
		return;
	if (cmd->buttons & IN_DUCK)
		fMaxSpeed *= 2.94117647f;
	if (fSpeed <= fMaxSpeed)
		return;
	float fRatio = fMaxSpeed / fSpeed;
	global::originalCMD.move.x *= fRatio;
	global::originalCMD.move.y *= fRatio;
	global::originalCMD.move.z *= fRatio;
}

void CAimbot::shoot_enemy(SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player || local_player->GetHealth() <= 0) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon || weapon->GetLoadedAmmo() == 0) return;
	if (weapon->get_full_info()->m_WeaponType == 9) return;
	if (weapon->GetItemDefenitionIndex() == SDK::WEAPON_C4 || weapon->is_grenade() || weapon->is_knife()) return;
	if (!can_shoot(cmd)) { cmd->buttons &= ~IN_ATTACK;	return; }
	if (GetAsyncKeyState(VK_LBUTTON)) return;
	Vector aim_angles;
	for (auto players : Entities)
	{
		auto entity = players.pPlayer;
		auto class_id = entity->GetClientClass()->m_ClassID;

		if (!entity) 
			continue;

		if (entity->GetTeam() == local_player->GetTeam()) 
			continue;

		if (entity->GetHealth() <= 0) 
			continue;

		if (class_id != 38) 
			continue;

		if (entity->GetVecOrigin() == Vector(0, 0, 0)) 
			continue;

		if (entity->GetImmunity()) 
			continue;

		if (entity->GetIsDormant()) 
			continue;

		Vector where2Shoot;

		if (settings::options.multi_bool) where2Shoot = aimbot->multipoint(entity, settings::options.acc_type);
		else where2Shoot = aimbot->point(entity, settings::options.acc_type);
		if (where2Shoot == Vector(0, 0, 0)) continue;

		static auto auto_stop = [&cmd]()
		{
			if (settings::options.stop_bool)
				quickstop(cmd, 1);

		};
		auto_stop();

		aim_angles = MATH::NormalizeAngle(UTILS::CalcAngle(local_player->GetEyePosition(), where2Shoot));
		if (aim_angles == Vector(0, 0, 0)) continue;

		if (weapon->GetItemDefenitionIndex() == SDK::WEAPON_AWP || weapon->GetItemDefenitionIndex() == SDK::WEAPON_SSG08 ||
			weapon->GetItemDefenitionIndex() == SDK::WEAPON_SCAR20 || weapon->GetItemDefenitionIndex() == SDK::WEAPON_G3SG1 ||
			weapon->GetItemDefenitionIndex() == SDK::WEAPON_AUG || weapon->GetItemDefenitionIndex() == SDK::WEAPON_SG553)
			if (!local_player->GetIsScoped())
				cmd->buttons |= IN_ATTACK2;

		auto_revolver(cmd);

		Vector vec_position[65], origin_delta[65];
		if (entity->GetVecOrigin() != vec_position[entity->GetIndex()])
		{
			origin_delta[entity->GetIndex()] = entity->GetVecOrigin() - vec_position[entity->GetIndex()];
			vec_position[entity->GetIndex()] = entity->GetVecOrigin();

			lag_comp = fabs(origin_delta[entity->GetIndex()].Length()) > 64;
		}

		if (lag_comp && entity->GetVelocity().Length2D() > 300 && settings::options.delay_shot == 1) return;

		if (accepted_inaccuracy(weapon) < settings::options.chance_val) continue;

		if (good_backtrack_tick(TIME_TO_TICKS(entity->GetSimTime() + LerpTime())))
			cmd->tick_count = TIME_TO_TICKS(entity->GetSimTime() + LerpTime());

		cmd->buttons |= IN_ATTACK;
		shots_fired[entity->GetIndex()]++;
		break;
	}

	if (cmd->buttons & IN_ATTACK)
	{
		float recoil_scale = g_csgo::cvar->FindVar("weapon_recoil_scale")->GetFloat(); global::should_send_packet = true;
		aim_angles -= local_player->GetPunchAngles() * recoil_scale; cmd->viewangles = aim_angles;
	}
}

float CAimbot::accepted_inaccuracy(SDK::CBaseWeapon* weapon)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return 0;

	if (!weapon) return 0;
	if (weapon->GetItemDefenitionIndex() == SDK::WEAPON_TASER) return 0;

	float inaccuracy = weapon->GetInaccuracy();
	if (inaccuracy == 0) inaccuracy = 0.0000001;
	inaccuracy = 1 / inaccuracy;
	return inaccuracy;
}

std::vector<Vector> CAimbot::GetMultiplePointsForHitbox(SDK::CBaseEntity* local, SDK::CBaseEntity* entity, int iHitbox, VMatrix BoneMatrix[128])
{
	auto VectorTransform_Wrapper = [](const Vector& in1, const VMatrix &in2, Vector &out)
	{
		auto VectorTransform = [](const float *in1, const VMatrix& in2, float *out)
		{
			auto DotProducts = [](const float *v1, const float *v2)
			{
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = DotProducts(in1, in2[0]) + in2[0][3];
			out[1] = DotProducts(in1, in2[1]) + in2[1][3];
			out[2] = DotProducts(in1, in2[2]) + in2[2][3];
		};
		VectorTransform(&in1.x, in2, &out.x);
	};

	SDK::studiohdr_t* pStudioModel = g_csgo::ModelInfo->GetStudioModel(entity->GetModel());
	SDK::mstudiohitboxset_t* set = pStudioModel->pHitboxSet(0);
	SDK::mstudiobbox_t *hitbox = set->GetHitbox(iHitbox);

	std::vector<Vector> vecArray;

	Vector max;
	Vector min;
	VectorTransform_Wrapper(hitbox->bbmax, BoneMatrix[hitbox->bone], max);
	VectorTransform_Wrapper(hitbox->bbmin, BoneMatrix[hitbox->bone], min);

	auto center = (min + max) * 0.5f;

	Vector CurrentAngles = UTILS::CalcAngle(center, local->GetEyePosition());

	Vector Forward;
	MATH::AngleVectors(CurrentAngles, &Forward);

	Vector Right = Forward.Cross(Vector(0, 0, 1));
	Vector Left = Vector(-Right.x, -Right.y, Right.z);

	Vector Top = Vector(0, 0, 1);
	Vector Bot = Vector(0, 0, -1);

	switch (iHitbox) {
	case 0:
		for (auto i = 0; i < 4; ++i)
			vecArray.emplace_back(center);

		vecArray[1] += Top * (hitbox->radius * settings::options.point_val);
		vecArray[2] += Right * (hitbox->radius * settings::options.point_val);
		vecArray[3] += Left * (hitbox->radius * settings::options.point_val);
		break;

	default:

		for (auto i = 0; i < 3; ++i)
			vecArray.emplace_back(center);

		vecArray[1] += Right * (hitbox->radius * settings::options.body_val);
		vecArray[2] += Left * (hitbox->radius * settings::options.body_val);
		break;
	}
	return vecArray;
}
Vector CAimbot::get_hitbox_pos(SDK::CBaseEntity* entity, int hitbox_id)
{
	auto getHitbox = [](SDK::CBaseEntity* entity, int hitboxIndex) -> SDK::mstudiobbox_t*
	{
		if (entity->GetIsDormant() || entity->GetHealth() <= 0) return NULL;

		const auto pModel = entity->GetModel();
		if (!pModel) return NULL;

		auto pStudioHdr = g_csgo::ModelInfo->GetStudioModel(pModel);
		if (!pStudioHdr) return NULL;

		auto pSet = pStudioHdr->pHitboxSet(0);
		if (!pSet) return NULL;

		if (hitboxIndex >= pSet->numhitboxes || hitboxIndex < 0) return NULL;

		return pSet->GetHitbox(hitboxIndex);
	};

	auto hitbox = getHitbox(entity, hitbox_id);
	if (!hitbox) return Vector(0, 0, 0);

	auto bone_matrix = entity->GetBoneMatrix(hitbox->bone);

	Vector bbmin, bbmax;
	MATH::VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
	MATH::VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

	return (bbmin + bbmax) * 0.5f;
}
int HitscanCombo()
{
	if (settings::options.hitscan_head)
		return SDK::HitboxList::HITBOX_HEAD;
	if (settings::options.hitscan_neck)
		return SDK::HitboxList::HITBOX_NECK;
	if (settings::options.hitscan_chest)
		return SDK::HitboxList::HITBOX_UPPER_CHEST, SDK::HitboxList::HITBOX_BODY, SDK::HitboxList::HITBOX_CHEST;
	if (settings::options.hitscan_pelvis)
		return SDK::HitboxList::HITBOX_PELVIS;
	if (settings::options.hitscan_legs)
		return SDK::HitboxList::HITBOX_LEFT_CALF, SDK::HitboxList::HITBOX_RIGHT_CALF, SDK::HitboxList::HITBOX_LEFT_FOOT, SDK::HitboxList::HITBOX_RIGHT_FOOT;
	if (settings::options.hitscan_arms)
		return SDK::HitboxList::HITBOX_RIGHT_UPPER_ARM, SDK::HitboxList::HITBOX_LEFT_UPPER_ARM, SDK::HitboxList::HITBOX_RIGHT_HAND, SDK::HitboxList::HITBOX_LEFT_HAND, SDK::HitboxList::HITBOX_RIGHT_FOREARM, SDK::HitboxList::HITBOX_LEFT_FOREARM;
}
Vector CAimbot::multipoint(SDK::CBaseEntity* entity, int option)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return Vector(0, 0, 0);

	Vector vector_best_point = Vector(0, 0, 0);
	int maxDamage = settings::options.damage_val;

	VMatrix matrix[128];
	if (!entity->SetupBones(matrix, 128, 256, 0)) return Vector(0, 0, 0);

	int hitboxes[5] = { HitscanCombo() };
	for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
	{
		for (auto point : GetMultiplePointsForHitbox(local_player, entity, hitboxes[i], matrix))
		{
			int damage = AutoWall->GetDamagez(point);
			if (damage > maxDamage)
			{
				bestHitbox = hitboxes[i];
				maxDamage = damage;
				vector_best_point = point;
			}
			if (damage >= entity->GetHealth())
			{
				return vector_best_point;
			}
		}
	}
	switch (option)
	{
	case 0:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_HEAD };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			for (auto point : GetMultiplePointsForHitbox(local_player, entity, hitboxes[i], matrix))
			{
				int damage = AutoWall->GetDamagez(point);
				if (damage > maxDamage)
				{
					bestHitbox = hitboxes[i];
					maxDamage = damage;
					vector_best_point = point;

					if (maxDamage >= entity->GetHealth())
						return vector_best_point;
				}
			}
		}
	} break;
	case 1:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_PELVIS, SDK::HitboxList::HITBOX_UPPER_CHEST, SDK::HitboxList::HITBOX_THORAX, SDK::HitboxList::HITBOX_MAX };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			for (auto point : GetMultiplePointsForHitbox(local_player, entity, hitboxes[i], matrix))
			{
				int damage = AutoWall->GetDamagez(point);
				if (damage > maxDamage)
				{
					bestHitbox = hitboxes[i];
					maxDamage = damage;
					vector_best_point = point;

					if (maxDamage >= entity->GetHealth())
						return vector_best_point;
				}
			}
		}
	} break;
	case 2:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_HEAD, SDK::HitboxList::HITBOX_BODY, SDK::HitboxList::HITBOX_LEFT_THIGH, SDK::HitboxList::HITBOX_RIGHT_THIGH, SDK::HitboxList::HITBOX_LEFT_CALF, SDK::HitboxList::HITBOX_RIGHT_CALF, SDK::HitboxList::HITBOX_LEFT_FOOT, SDK::HitboxList::HITBOX_RIGHT_FOOT };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			for (auto point : GetMultiplePointsForHitbox(local_player, entity, hitboxes[i], matrix))
			{
				int damage = AutoWall->GetDamagez(point);
				if (damage > maxDamage)
				{
					bestHitbox = hitboxes[i];
					maxDamage = damage;
					vector_best_point = point;

					if (maxDamage >= entity->GetHealth())
						return vector_best_point;
				}
			}
		}
	} break;
	case 3:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_HEAD, SDK::HitboxList::HITBOX_NECK, SDK::HitboxList::HITBOX_BODY, SDK::HitboxList::HITBOX_UPPER_CHEST, SDK::HitboxList::HITBOX_PELVIS, SDK::HitboxList::HITBOX_LEFT_THIGH, SDK::HitboxList::HITBOX_RIGHT_THIGH, SDK::HitboxList::HITBOX_LEFT_CALF, SDK::HitboxList::HITBOX_RIGHT_CALF, SDK::HitboxList::HITBOX_LEFT_FOOT, SDK::HitboxList::HITBOX_RIGHT_FOOT };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			for (auto point : GetMultiplePointsForHitbox(local_player, entity, hitboxes[i], matrix))
			{
				int damage = AutoWall->GetDamagez(point);
				if (damage > maxDamage)
				{
					bestHitbox = hitboxes[i];
					maxDamage = damage;
					vector_best_point = point;

					if (maxDamage >= entity->GetHealth())
						return vector_best_point;
				}
			}
		}
	} break;
	case 4:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_HEAD, SDK::HitboxList::HITBOX_NECK, SDK::HitboxList::HITBOX_PELVIS, SDK::HitboxList::HITBOX_UPPER_CHEST, SDK::HitboxList::HITBOX_BODY, SDK::HitboxList::HITBOX_PELVIS , SDK::HitboxList::HITBOX_LEFT_THIGH, SDK::HitboxList::HITBOX_RIGHT_THIGH , SDK::HitboxList::HITBOX_LEFT_FOOT, SDK::HitboxList::HITBOX_RIGHT_FOOT , SDK::HitboxList::HITBOX_NECK, SDK::HitboxList::HITBOX_UPPER_CHEST , SDK::HitboxList::HITBOX_RIGHT_HAND, SDK::HitboxList::HITBOX_LEFT_HAND };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			for (auto point : GetMultiplePointsForHitbox(local_player, entity, hitboxes[i], matrix))
			{
				int damage = AutoWall->GetDamagez(point);
				if (damage > maxDamage)
				{
					bestHitbox = hitboxes[i];
					maxDamage = damage;
					vector_best_point = point;

					if (maxDamage >= entity->GetHealth())
						return vector_best_point;
				}
			}
		}
	} break;
	case 5:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_HEAD, SDK::HitboxList::HITBOX_BODY, SDK::HitboxList::HITBOX_PELVIS, SDK::HitboxList::HITBOX_LEFT_THIGH, SDK::HitboxList::HITBOX_RIGHT_THIGH, SDK::HitboxList::HITBOX_LEFT_CALF, SDK::HitboxList::HITBOX_RIGHT_CALF, SDK::HitboxList::HITBOX_LEFT_FOOT, SDK::HitboxList::HITBOX_RIGHT_FOOT, SDK::HitboxList::HITBOX_NECK };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			for (auto point : GetMultiplePointsForHitbox(local_player, entity, hitboxes[i], matrix))
			{
				int damage = AutoWall->GetDamagez(point);
				if (damage > maxDamage)
				{
					bestHitbox = hitboxes[i];
					maxDamage = damage;
					vector_best_point = point;

					if (maxDamage >= entity->GetHealth())
						return vector_best_point;
				}
			}
		}
	} break;
	}

	/*if (settings::options.smartbaim) {
		if (global::is_fakewalking)
		{
			int hitboxes[] = { SDK::HitboxList::HITBOX_PELVIS, SDK::HitboxList::HITBOX_UPPER_CHEST, SDK::HitboxList::HITBOX_CHEST };
		}
		auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
		if (settings::options.baimifawp && weapon->GetItemDefenitionIndex() == SDK::WEAPON_AWP) //men why rageawp reeeeee
		{
			int hitboxes[] = { SDK::HitboxList::HITBOX_PELVIS,
				SDK::HitboxList::HITBOX_BODY,
				SDK::HitboxList::HITBOX_THORAX,
				SDK::HitboxList::HITBOX_UPPER_CHEST,
				SDK::HitboxList::HITBOX_RIGHT_THIGH,
				SDK::HitboxList::HITBOX_LEFT_THIGH,
				SDK::HitboxList::HITBOX_RIGHT_CALF,
				SDK::HitboxList::HITBOX_LEFT_CALF,
				SDK::HitboxList::HITBOX_RIGHT_FOOT,
				SDK::HitboxList::HITBOX_LEFT_FOOT };
		}

		if (entity->GetFlags() & !FL_ONGROUND)
		{
			int hitboxes[] = { SDK::HitboxList::HITBOX_PELVIS,
				SDK::HitboxList::HITBOX_BODY,
				SDK::HitboxList::HITBOX_THORAX,
				SDK::HitboxList::HITBOX_UPPER_CHEST,
				SDK::HitboxList::HITBOX_RIGHT_THIGH,
				SDK::HitboxList::HITBOX_LEFT_THIGH,
				SDK::HitboxList::HITBOX_RIGHT_CALF,
				SDK::HitboxList::HITBOX_LEFT_CALF,
				SDK::HitboxList::HITBOX_RIGHT_FOOT,
				SDK::HitboxList::HITBOX_LEFT_FOOT };
		}

		if (entity->GetHealth() < 20 && local_player->GetHealth() < 29)
		{
			int hitboxes[] = { SDK::HitboxList::HITBOX_PELVIS,
				SDK::HitboxList::HITBOX_THORAX,
				SDK::HitboxList::HITBOX_BODY,
				SDK::HitboxList::HITBOX_UPPER_CHEST,
				SDK::HitboxList::HITBOX_RIGHT_THIGH,
				SDK::HitboxList::HITBOX_LEFT_THIGH,
				SDK::HitboxList::HITBOX_RIGHT_CALF,
				SDK::HitboxList::HITBOX_LEFT_CALF,
				SDK::HitboxList::HITBOX_RIGHT_FOOT,
				SDK::HitboxList::HITBOX_LEFT_FOOT,
				SDK::HitboxList::HITBOX_LEFT_FOREARM,
				SDK::HitboxList::HITBOX_RIGHT_FOREARM,
				SDK::HitboxList::HITBOX_LEFT_HAND,
				SDK::HitboxList::HITBOX_RIGHT_HAND };
		}
	}*/

	return vector_best_point;
}
Vector CAimbot::point(SDK::CBaseEntity* entity, int option)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return Vector(0, 0, 0);

	Vector vector_best_point = Vector(0, 0, 0);
	int maxDamage = settings::options.damage_val;

	switch (option)
	{
	case 0:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_HEAD };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			
			int damage = AutoWall->GetDamagez(entity->GetBonePosition(hitboxes[i]));
			if (damage > maxDamage)
			{
				bestHitbox = hitboxes[i];
				maxDamage = damage;
				vector_best_point = get_hitbox_pos(entity, bestHitbox);

				if (maxDamage >= entity->GetHealth())
					return vector_best_point;
			}
		}
	} break;
	case 1:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_BODY, SDK::HitboxList::HITBOX_PELVIS, SDK::HitboxList::HITBOX_UPPER_CHEST, SDK::HitboxList::HITBOX_THORAX, SDK::HitboxList::HITBOX_MAX };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			int damage = AutoWall->GetDamagez(entity->GetBonePosition(hitboxes[i]));
			if (damage > maxDamage)
			{
				bestHitbox = hitboxes[i];
				maxDamage = damage;
				vector_best_point = get_hitbox_pos(entity, bestHitbox);

				if (maxDamage >= entity->GetHealth())
					return vector_best_point;
			}
		}
	} break;
	case 2:
	{
		int hitboxes[] = { SDK::HitboxList::HITBOX_HEAD, SDK::HitboxList::HITBOX_BODY, SDK::HitboxList::HITBOX_PELVIS, SDK::HitboxList::HITBOX_LEFT_UPPER_ARM, SDK::HitboxList::HITBOX_LEFT_FOREARM, SDK::HitboxList::HITBOX_RIGHT_UPPER_ARM, SDK::HitboxList::HITBOX_RIGHT_FOREARM, SDK::HitboxList::HITBOX_UPPER_CHEST, SDK::HitboxList::HITBOX_CHEST, SDK::HitboxList::HITBOX_LEFT_CALF, SDK::HitboxList::HITBOX_RIGHT_CALF,SDK::HitboxList::HITBOX_LEFT_FOOT, SDK::HitboxList::HITBOX_RIGHT_FOOT };
		for (int i = 0; i < ARRAYSIZE(hitboxes); i++)
		{
			int damage = AutoWall->GetDamagez(entity->GetBonePosition(hitboxes[i]));
			if (damage > maxDamage)
			{
				bestHitbox = hitboxes[i];
				maxDamage = damage;
				vector_best_point = get_hitbox_pos(entity, bestHitbox);

				if (maxDamage >= entity->GetHealth())
					return vector_best_point;
			}
		}
	} break;
	}
	return vector_best_point;
}

bool CAimbot::can_shoot(SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return false;
	if (local_player->GetHealth() <= 0) return false;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon || weapon->GetLoadedAmmo() == 0) return false;

	return (weapon->GetNextPrimaryAttack() < UTILS::GetCurtime()) && (local_player->GetNextAttack() < UTILS::GetCurtime());
}

void CAimbot::auto_revolver(SDK::CUserCmd* cmd)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player && local_player->GetHealth() <= 0) return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon || weapon->GetLoadedAmmo() == 0) return;

	if (weapon->GetItemDefenitionIndex() == SDK::WEAPON_REVOLVER) {
		cmd->buttons |= IN_ATTACK;
		float flPostponeFireReady = weapon->GetPostponeFireReadyTime();
		if (flPostponeFireReady > 0 && flPostponeFireReady < g_csgo::Globals->curtime) {
			cmd->buttons &= ~IN_ATTACK;
		}
	}
}

int CAimbot::get_damage(Vector position)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return 0;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));

	if (!weapon)
		return 0;

	SDK::trace_t trace;
	SDK::Ray_t ray;
	SDK::CTraceWorldOnly filter;
	ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), position);

	g_csgo::Trace->TraceRay(ray, MASK_ALL, (SDK::ITraceFilter*)&filter, &trace);

	if (trace.flFraction == 1.f)
	{
		auto weapon_info = weapon->get_full_info();
		if (!weapon_info)
			return -1;

		return weapon_info->m_Damage;
		return 1;
	}
	else
		return 0;
}

CAimbot* aimbot = new CAimbot();