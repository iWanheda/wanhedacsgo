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
#include "../FEATURES/Fakewalk.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/FakeLag.h"

int CFakeLag::lag_comp_break()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player || local_player->GetHealth() <= 0)
		return 1;	

	auto velocity = local_player->GetVelocity();
	velocity.z = 0.f;
	auto speed = velocity.Length();
	auto distance_per_tick = speed * g_csgo::Globals->interval_per_tick;
	int choked_ticks = std::ceilf(64.f / distance_per_tick);
	return std::min<int>(choked_ticks, 14.f);
}

void CFakeLag::do_fakelag(SDK::CUserCmd* cmd)
{
	global::should_send_packet = true;
	int choke_amount;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player || local_player->GetHealth() <= 0)
		return;

	auto net_channel = g_csgo::Engine->GetNetChannel();
	if (!net_channel)
		return;


	if (settings::options.lag_bool)
	{
		if (fake_walk) {
			if ((local_update - g_csgo::Globals->curtime) > 0.22) {
				choke_amount = settings::options.fakewalk_lag;
			}
			else {
				choke_amount = 1;
			}
			
	    }
		else
  	    {
			choke_amount = 1;
			if (!(local_player->GetFlags() & FL_ONGROUND))
			{
				switch (settings::options.lag_type % 2)
				{
					case 0: choke_amount = settings::options.jump_lag; break;
					case 1: choke_amount = lag_comp_break(); break;
				}
			}
			else if (local_player->GetVelocity().Length2D() > 0.1)
			{
				switch (settings::options.lag_type % 2)
				{
					case 0: choke_amount = settings::options.move_lag; break;
					case 1: choke_amount = lag_comp_break(); break;
				}
			}
			else //we don't need a velocity check since we've already eliminated other cases
			{
				if ((local_update - g_csgo::Globals->curtime) > 0.22) {
					switch (settings::options.lag_type % 2)
					{
					case 0: choke_amount = settings::options.stand_lag; break;
					case 1: choke_amount = settings::options.stand_lag; break;
					}
				}
				else
					choke_amount = 1;
			}
		}
	}
	else
		choke_amount = 1;

	global::packets_choked = choke_amount;

	if (net_channel->m_nChokedPackets >= min(14, choke_amount))
		global::should_send_packet = true;
	else
		global::should_send_packet = false;
}

CFakeLag* fakelag = new CFakeLag();