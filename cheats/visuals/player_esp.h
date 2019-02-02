#pragma once

#include "..\..\includes.h"
#include "..\..\csgo_sdk\utils\utils.h"
#include "..\..\UTILS\render.h"

//class Box;

class cplayer_esp {

public:
	float esp_alpha_fade[64];
	void paint_traverse();

	void draw_watermark();
	void draw_box(SDK::CBaseEntity* m_entity, Box box);
	void draw_health(SDK::CBaseEntity* entity);
	void draw_ammobar(SDK::CBaseEntity* m_entity, Box box);
	void draw_name(SDK::CBaseEntity* m_entity);
	void draw_flags(SDK::CBaseEntity* e, Box box);
	//void draw_rank(C_CSPlayerResource* playerResource, SDK::CBaseEntity* entity);
	void draw_skeleton(SDK::CBaseEntity* e, CColor color, VMatrix matrix[128]);
	void draw_lbybar(SDK::CBaseEntity* m_entity, Box box);
	void bomb_timer(SDK::CBaseEntity * entity);
	void pov_arrow(SDK::CBaseEntity* entity);
	void draw_damage();
	void draw_weapon(SDK::CBaseEntity* m_entity);
}; extern cplayer_esp* player_visuals;