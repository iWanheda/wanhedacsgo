#pragma once

#include "..\..\includes.h"

#include "..\..\UTILS/interfaces.h"
#include "..\..\SDK/CClientEntityList.h"
#include "..\..\SDK/IEngine.h"
#include "..\..\SDK/CBaseWeapon.h"
#include "..\..\SDK/CBaseEntity.h"
#include "..\..\SDK/CGlobalVars.h"
#include "..\..\SDK/ConVar.h"
#include "..\..\SDK/ISurface.h"
#include "..\..\UTILS/render.h"
#include "..\..\FEATURES/Resolver.h"
#include "..\..\FEATURES/Visuals.h"
#include "..\..\SDK\IVDebugOverlay.h"
#include "..\..\SDK\CTrace.h"

struct impact_info {
	float x, y, z;
	long long time;
};

struct hitmarker_info {
	impact_info impact;
	int alpha;
};

class cother_esp {

private:
	float hurt_time;

	class cbullet_tracer_info {

	public:
		cbullet_tracer_info(Vector src, Vector dst, float time, CColor color) {

			this->src = src;
			this->dst = dst;
			this->time = time;
			this->color = color;
		}

		Vector src, dst;
		float time;
		CColor color;
	}; std::vector<cbullet_tracer_info> logs;

public:
	void hurt(SDK::IGameEvent* event);
	void impacts(SDK::IGameEvent * event);
	void hitmarkerdynamic_paint();
	void viewmodel_changer();
	void damageesp_event(SDK::IGameEvent * event);
	void damageesp_draw();
	void log_tracer(SDK::IGameEvent* event);
	void render_tracer();
};

extern std::vector<impact_info> impacts;
extern std::vector<hitmarker_info> hitmarker;

class floating_text
{
public:
	floating_text() :
		valid(false)
	{ }

	bool valid;
	float start_time;
	int damage;
	int hitgroup;
	Vector hit_pos;
	int random_idx;
};

const int MAX_FLOATING_TEXTS = 50;
extern std::array<floating_text, MAX_FLOATING_TEXTS> floating_texts;