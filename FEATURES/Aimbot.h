#pragma once

namespace SDK
{
	class CUserCmd;
	class CBaseEntity;
	class CBaseWeapon;
	struct mstudiobbox_t;
}

struct AimbotData_t
{
	AimbotData_t(SDK::CBaseEntity* player, const int& idx)
	{
		this->pPlayer = player;
		this->index = idx;
	}
	SDK::CBaseEntity*	pPlayer;
	int					index;
};

class CAimbot
{
public:
	void shoot_enemy(SDK::CUserCmd* cmd);
	bool good_backtrack_tick(int tick);
	void run_aimbot(SDK::CUserCmd * cmd);
	void SelectTarget();
	void lby_backtrack(SDK::CUserCmd * pCmd, SDK::CBaseEntity * pLocal, SDK::CBaseEntity * pEntity);
	float accepted_inaccuracy(SDK::CBaseWeapon * weapon);
	bool can_shoot(SDK::CUserCmd * cmd);
	void fix_recoil(SDK::CUserCmd* cmd);
	void rotate_movement(float yaw, SDK::CUserCmd * cmd);
	Vector get_hitbox_pos(SDK::CBaseEntity* entity, int hitbox_id);
	SDK::mstudiobbox_t* get_hitbox(SDK::CBaseEntity* entity, int hitbox_index);
	Vector multipoint(SDK::CBaseEntity* entity, int option);
	Vector point(SDK::CBaseEntity* entity, int option);
	void auto_revolver(SDK::CUserCmd* cmd);

	std::vector<AimbotData_t>	Entities;
private:
	std::vector<Vector> GetMultiplePointsForHitbox(SDK::CBaseEntity * local, SDK::CBaseEntity * entity, int iHitbox, VMatrix BoneMatrix[128]);
	int get_damage(Vector position);
};

extern CAimbot* aimbot;