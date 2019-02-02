
#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../SDK/ISurface.h"
#include "../UTILS/render.h"
#include "NewEventLog.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/Visuals.h"
#include "../cheats/visuals/other_esp.h"
#include <playsoundapi.h>
#include <sstream> 
#include <string>

#pragma comment(lib, "Winmm.lib")

typedef void(__cdecl* MsgFn)(const char* msg, va_list);
void ConMsg(const char* msg, ...) {

	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything

	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg"); //This gets the address of export "Msg" in the dll "tier0.dll". The static keyword means it's only called once and then isn't called again (but the variable is still there)
	char buffer[989];
	va_list list; //Normal varargs stuff http://stackoverflow.com/questions/10482960/varargs-to-printf-all-arguments
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list); //Calls the function, we got the address above.

	if (!g_csgo::cvar->FindVar("developer")->GetInt())
		g_csgo::cvar->FindVar("developer")->SetValue(true);

	if (! g_csgo::cvar->FindVar( "con_filter_enable" )->GetInt( ) )
		g_csgo::cvar->FindVar( "con_filter_enable" )->SetValue( 2 );
}

namespace FEATURES
{
	namespace MISC
	{
		InGameLogger in_game_logger;
		void ColorLine::Draw(int x, int y, unsigned int font)
		{
			for (int i = 0; i < texts.size(); i++)
			{
				RENDER::DrawF(x, y, font, false, false, colors[i], texts[i]);
				x += RENDER::GetTextSize(font, texts[i]).x;
			}
		}

		void InGameLogger::Do()
		{
			if (log_queue.size() > max_lines_at_once)
				log_queue.erase(log_queue.begin() + max_lines_at_once, log_queue.end());

			for (int i = 0; i < log_queue.size(); i++)
			{
				auto log = log_queue[i];
				float time_delta = fabs(UTILS::GetCurtime() - log.time);

				int height = ideal_height + (16 * i);

				/// erase dead logs
				if (time_delta > text_time)
				{
					log_queue.erase(log_queue.begin() + i);
					break;
				}
				if (time_delta > text_time - slide_out_speed)
					height = height + (((slide_out_speed - (text_time - time_delta)) / slide_out_speed) * slide_out_distance);

				/// fade out
				if (time_delta > text_time - text_fade_out_time)
					log.color_line.ChangeAlpha(255 - (((time_delta - (text_time - text_fade_out_time)) / text_fade_out_time) * 255.f));
				/// fade in
				if (time_delta < text_fade_in_time)
					log.color_line.ChangeAlpha((time_delta / text_fade_in_time) * 255.f);

				int width = ideal_width;

				/// slide from left
				if (time_delta < text_fade_in_time)
					width = (time_delta / text_fade_in_time) * static_cast<float>(slide_in_distance) + (ideal_width - slide_in_distance);
				/// slider from right
				if (time_delta > text_time - text_fade_out_time)
					width = ideal_width + (((time_delta - (text_time - text_fade_out_time)) / text_fade_out_time) * static_cast<float>(slide_out_distance));

				
				log.color_line.Draw(width, height, FONTS::in_game_logging_font);
				
			}
		}
	}
}

bool did_hit = false;

namespace FEATURES
{
	namespace MISC
	{
		CREATE_EVENT_LISTENER(ItemPurchaseListener, "item_purchase");

		CREATE_EVENT_LISTENER(PlayerHurtListener, "player_hurt");

		CREATE_EVENT_LISTENER(BulletImpactListener, "bullet_impact");

		CREATE_EVENT_LISTENER(WeaponFireListener, "weapon_fire");

		CREATE_EVENT_LISTENER(RoundStartListener, "round_start");

		CREATE_EVENT_LISTENER(RoundPreStartListener, "round_prestart");

		CREATE_EVENT_LISTENER(PlayerDeathListener, "player_death");

		CREATE_EVENT_LISTENER(RoundEndListener, "round_end");


		ItemPurchaseListener item_purchase_listener;
		PlayerHurtListener player_hurt_listener;
		BulletImpactListener bullet_impact_listener;
		WeaponFireListener weapon_fire_listener;
		RoundStartListener round_start_listener;
		RoundPreStartListener round_prestart_listener;
		PlayerDeathListener player_death_listener;
		RoundEndListener round_end_listener;

		void InitializeEventListeners()
		{

			g_csgo::GameEventManager->AddListener(&item_purchase_listener, ("item_purchase"), false);
			g_csgo::GameEventManager->AddListener(&player_hurt_listener, ("player_hurt"), false);
			g_csgo::GameEventManager->AddListener(&bullet_impact_listener, ("bullet_impact"), false);
			g_csgo::GameEventManager->AddListener(&weapon_fire_listener, ("weapon_fire"), false);
			g_csgo::GameEventManager->AddListener(&round_start_listener, ("round_start"), false);
			g_csgo::GameEventManager->AddListener(&player_death_listener, ("player_death"), false);
			g_csgo::GameEventManager->AddListener(&round_prestart_listener, ("round_prestart"), false);
			g_csgo::GameEventManager->AddListener(&round_end_listener, ("round_end"), false);

		}

		void RemoveEventListeners()
		{
			g_csgo::GameEventManager->RemoveListener(&item_purchase_listener);
			g_csgo::GameEventManager->RemoveListener(&player_hurt_listener);
			g_csgo::GameEventManager->RemoveListener(&bullet_impact_listener);
			g_csgo::GameEventManager->RemoveListener(&weapon_fire_listener);
			g_csgo::GameEventManager->RemoveListener(&round_start_listener);
			g_csgo::GameEventManager->RemoveListener(&round_start_listener);
			g_csgo::GameEventManager->RemoveListener(&round_end_listener);
		}

		void ItemPurchaseListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			SDK::CBaseEntity* local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			SDK::CBaseEntity* entity = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(game_event->GetInt("userid")));
			if (!entity)
				return;

			if (entity->GetTeam() == local_player->GetTeam())
				return;

			SDK::player_info_t player_info;
			if (!g_csgo::Engine->GetPlayerInfo(entity->GetIndex(), &player_info))
				return;

			MISC::InGameLogger::Log log;

			std::string name = player_info.szName, weapon_name = game_event->GetString("weapon");
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);
			std::transform(weapon_name.begin(), weapon_name.end(), weapon_name.begin(), ::toupper);

			log.color_line.PushBack(name, CColor(255, 255, 255,255));
			log.color_line.PushBack(" bought ", CColor(255, 255, 255, 255));
			log.color_line.PushBack(weapon_name, CColor(255, 255, 255, 255));

			//g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
			//g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "%s bought %s \n", name.c_str(), weapon_name.c_str());

			//g_csgo::cvar->FindVar("con_filter_text")->SetValueChar("bought");
			//ConMsg("%s bought %s", name.c_str(), weapon_name.c_str());

			MISC::in_game_logger.AddLog(log);
		};

		void RoundPreStartListener::FireGameEvent(SDK::IGameEvent* event) {

			if (!event)
				return;

			auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer())); if (!local_player) return;

			global::DisableAA = true;
		}

		/*auto is_enemy_team_dead() -> bool {

			auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));

			
		}*/

		void RoundEndListener::FireGameEvent(SDK::IGameEvent* event) {

			if (!event)
				return;

			auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer())); if (!local_player) return;

			/*if (is_enemy_team_dead)
				global::DisableAA = true;
			else
				global::DisableAA = false;*/

			global::DisableAA = false;

			global::m_round_changed = true;
		}

		void PlayerHurtListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;
			
			SDK::CBaseEntity* local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
			if (!local_player)
				return;


			if (!strcmp(game_event->GetName(), "player_hurt")) {
			
				global::missed_shots[local_player->GetIndex()]++;
			}


			SDK::CBaseEntity* attacker = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(game_event->GetInt("attacker")));
			if (!attacker)
				return;

			if (attacker != local_player)
				return;

			SDK::player_info_t player_info;
			g_csgo::Engine->GetPlayerInfo(g_csgo::Engine->GetPlayerForUserID(game_event->GetInt("userid")), &player_info);

			int hitgroup = game_event->GetInt("hitgroup");

			MISC::InGameLogger::Log log;

			std::string name = player_info.szName;
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			log.color_line.PushBack("-", CColor(255, 255, 255, 255));
			log.color_line.PushBack(std::to_string(game_event->GetInt("dmg_health")), CColor(255, 255, 255, 255));
			log.color_line.PushBack(" in ", CColor(255, 255, 255, 255));
			log.color_line.PushBack(UTILS::GetHitgroupName(hitgroup), CColor(255, 255, 255, 255));
			log.color_line.PushBack(" to ", CColor(255, 255, 255, 255));
			log.color_line.PushBack(name, CColor(255, 255, 255, 255));

			glow g; g.playerMatrix[128];

			g_csgo::cvar->FindVar("con_filter_text")->SetValueChar("hurt");
			ConMsg("hurt %s for %i in %s \n", name.c_str(), game_event->GetInt("dmg_health"), UTILS::GetHitgroupName(hitgroup));

			visuals->set_hitmarker_time(g_csgo::Globals->curtime);

			cother_esp other_esp;
			other_esp.hurt(game_event);
			other_esp.log_tracer(game_event);

			if (settings::options.hitsound) 
				g_csgo::Surface->IPlaySound("training\\timer_bell.wav");

			did_hit = true;

			//g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
			//g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "-%i in %s to %s \n", game_event->GetInt("dmg_health"), UTILS::GetHitgroupName(hitgroup), name.c_str());

			shots_hit[attacker->GetIndex()]++;
			MISC::in_game_logger.AddLog(log);
		};

		void BulletImpactListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			if (!(g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame()))
				return;

			int iUser = g_csgo::Engine->GetPlayerForUserID(game_event->GetInt("userid"));
			auto entity = reinterpret_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(iUser));
			if (!entity) return;

			if (entity->GetIsDormant()) return;

			float x, y, z;
			x = game_event->GetFloat("x");
			y = game_event->GetFloat("y");
			z = game_event->GetFloat("z");

			UTILS::BulletImpact_t impact(entity, Vector(x, y, z), g_csgo::Globals->curtime, iUser == g_csgo::Engine->GetLocalPlayer() ? BLUE : RED);

			cother_esp other_esp;
			other_esp.impacts(game_event);
			other_esp.log_tracer(game_event);

			visuals->Impacts.push_back(impact);
			//cbullet_tracer tracer;
			//tracer.log(game_event);
		}

		void PlayerDeathListener::FireGameEvent(SDK::IGameEvent* event) {

			if (!event)
				return;

			SDK::CBaseEntity* local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			SDK::CBaseEntity* attacker = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(event->GetInt("attacker")));
			if (!attacker)
				return;

			if (attacker != local_player)
				return;
			if (settings::options.hitsound)
				g_csgo::Surface->IPlaySound("ui\\deathnotice.wav");

			SDK::player_info_t player_info;
			g_csgo::Engine->GetPlayerInfo(g_csgo::Engine->GetPlayerForUserID(event->GetInt("userid")), &player_info);

			std::string name = player_info.szName;
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			//g_csgo::cvar->FindVar("con_filter_text")->SetValueChar("killed");
			//ConMsg("killed %s \n", name.c_str());
		}

		void WeaponFireListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			bool missed;

			if (!did_hit)
				missed = true;

			if (missed) {
				ConMsg("missed shot due to spread \n");
				missed = false;
			}
			did_hit = false;
		}

		void RoundStartListener::FireGameEvent(SDK::IGameEvent* game_event)
		{
			if (!game_event)
				return;

			if (!(g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame()))
				return;

		
			
			SDK::CBaseEntity* local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
			if (!local_player)
				return;

			global::DisableAA = false;
		}
	}
}