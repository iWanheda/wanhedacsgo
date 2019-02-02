#pragma once

namespace SDK
{
	class CUserCmd;
	class CBaseWeapon;
}

class CMovement
{
public:
	void bunnyhop(SDK::CUserCmd* cmd);
	void autostrafer(SDK::CUserCmd* cmd);
	void duckinair(SDK::CUserCmd* cmd);
	void circle_strafe(SDK::CUserCmd* cmd, float* circle_yaw);
};

extern CMovement* movement;