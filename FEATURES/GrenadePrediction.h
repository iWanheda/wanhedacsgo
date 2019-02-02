#pragma once
#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CViewSetup.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Aimbot.h"
#include "../HOOKS/hooks.h"

#include "../SDK/CTrace.h"	
#include "../FEATURES/Visuals.h"
#include "../UTILS/render.h"
#include <string.h>
#include "..\singleton.h"

enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

class GrenadePrediction : public singleton<GrenadePrediction>
{
private:
	std::vector<Vector> path;
	std::vector<std::pair<Vector, QAngle>> others;
	//std::vector<Vector> others;
	int type = 0;
	int act = 0;

	void Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles);
	void Simulate(SDK::CViewSetup* setup);

	int Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval);
	bool CheckDetonate(const Vector& vecThrow, const SDK::trace_t& tr, int tick, float interval);

	void TraceHull(Vector& src, Vector& end, SDK::trace_t& tr);
	void AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground);
	void PushEntity(Vector& src, const Vector& move, SDK::trace_t& tr);
	void ResolveFlyCollisionCustom(SDK::trace_t& tr, Vector& vecVelocity, float interval);
	int PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
public:
	void Tick(int buttons);
	void View(SDK::CViewSetup* setup);
	void Paint();
};