#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/ConVar.h"
#include "../HOOKS/hooks.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Fakewalk.h"
#include "../FEATURES/Aimbot.h"
#include "AntiAim.h"
#include <time.h>
#include <iostream>

void CFakewalk::do_fakewalk(SDK::CUserCmd* cmd) {

	if (!settings::options.fakewalk) {
		fake_walk = false;
		global::is_fakewalking = false;
		return;
	}

	if (GetAsyncKeyState(VK_SHIFT))
	{
		uintptr_t* FPointer; __asm { MOV FPointer, EBP }
		byte* *SendPacket = (byte*)(*FPointer - 0x1C);
		static int choked = 0;
		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!local_player || local_player->GetHealth() <= 0) return;

		auto net_channel = g_csgo::Engine->GetNetChannel();
		if (!net_channel) return;

		auto animstate = local_player->GetAnimState();
		if (!animstate) return;

		fake_walk = true;
		global::is_fakewalking = true;
		auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(g_csgo::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
		int ClampedSpeed = settings::options.fakewalkspeed / 2;
		if (weapon->is_knife() || weapon->GetItemDefenitionIndex() == SDK::WEAPON_TASER || weapon->GetItemDefenitionIndex() == SDK::WEAPON_C4 || weapon->is_grenade()) {
			if ((local_update - g_csgo::Globals->curtime) > 0.22) {
				choked = choked > 7 ? 0 : choked + 1;
				global::originalCMD.move.x = choked < 2 || choked > settings::options.fakewalkspeed ? 0 : global::originalCMD.move.x;
				global::originalCMD.move.y = choked < 2 || choked > settings::options.fakewalkspeed ? 0 : global::originalCMD.move.y;
				*SendPacket = choked < 1;
			}
			else {
				global::originalCMD.move.y = 0;
				global::originalCMD.move.x = 0;
			}
		}
		else {
			if ((local_update - g_csgo::Globals->curtime) > 0.22) {
				choked = choked > 7 ? 0 : choked + 1;
				global::originalCMD.move.x = choked < 2 || choked > ClampedSpeed ? 0 : global::originalCMD.move.x;
				global::originalCMD.move.y = choked < 2 || choked > ClampedSpeed ? 0 : global::originalCMD.move.y;
				*SendPacket = choked < 1;
			}
			else {
				global::originalCMD.move.y = 0;
				global::originalCMD.move.x = 0;
			}
		}

		global::should_send_packet = *SendPacket;

	}
	else {
		fake_walk = false;
		global::is_fakewalking = false;
	}
}

/*
void CFakewalk::do_fakewalk(SDK::CUserCmd* cmd) {

	global::is_fakewalking = false;

	if (!settings::options.fakewalk)
		return;

	if (GetAsyncKeyState(VK_SHIFT)) {

		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!local_player || local_player->GetHealth() <= 0) return;

		auto net_channel = g_csgo::Engine->GetNetChannel();
		if (!net_channel) return;

		auto animstate = local_player->GetAnimState();
		if (!animstate) return;

		global::is_fakewalking = true;

		int32_t ticks_to_update = (antiaim->m_next_lby_update_time - 1);

		const int32_t max_ticks = std::min<int32_t>(7, ticks_to_update);

		const int32_t chocked = net_channel->m_nChokedPackets;

		int32_t ticks_left = max_ticks - chocked;

		if (!net_channel->m_nChokedPackets) {

			if (antiaim->TimeUntilNextLBYUpdate() > 0.15)
				global::should_choke_packets = true;
			else
				global::should_choke_packets = false;
		}

		global::originalCMD.move.y *= settings::options.fakewalkspeed / 233.33;
		global::originalCMD.move.x *= settings::options.fakewalkspeed / 233.33;

		if (global::should_choke_packets) {

			const int choked_ticks = net_channel->m_nChokedPackets;

			if (!choked_ticks || ticks_left > (animstate->speed_2d < 115.f ? 9 : 8)) {

				global::originalCMD.move.y = 0;
				global::originalCMD.move.x = 450;

				if (!choked_ticks || animstate->speed_2d < 20.f)
					global::originalCMD.move.x = 0;

				aimbot->rotate_movement(UTILS::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f, cmd);
			}
		} else {
			global::originalCMD.move.y = 0;
			global::originalCMD.move.x = 0;
		}
	}
}
*/
CFakewalk* slidebitch = new CFakewalk();