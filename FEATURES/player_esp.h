#pragma once

#include "..\UTILS\singleton.h"
#include "..\includes.h"

class Box;

class playeresp : public singleton<playeresp> {

	public:
		float esp_alpha_fade[64];
		void paint_traverse();

		void lby_indicator(SDK::CBaseEntity* m_entity, Box box);
		void draw_box(SDK::CBaseEntity* m_entity, Box box);
		void draw_name(SDK::CBaseEntity* m_entity, Box box);
		void draw_weapon(SDK::CBaseEntity* m_entity, Box box);
		void draw_health(SDK::CBaseEntity* m_entity, Box box);
		void footstep_esp();
		void draw_ammobar(SDK::CBaseEntity* m_entity, Box box);
		void draw_skeleton(SDK::CBaseEntity* e, CColor color, VMatrix matrix[128]);
		void draw_rank();
		void draw_flags(SDK::CBaseEntity* m_entity, Box box);
		void draw_pov_arrows();
};

struct store_bone_matrice { VMatrix bone_matrix[128]; };