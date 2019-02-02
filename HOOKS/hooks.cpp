#include "..\includes.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/dx9/imgui_impl_dx9.h"
#include "hooks.h"
#include "../UTILS/interfaces.h"
#include "../UTILS/offsets.h"
#include "../UTILS/NetvarHookManager.h"
#include "../UTILS/render.h"
#include "../FEATURES/NewEventLog.h"
#include "../SDK/CInput.h"
#include "../SDK/IClient.h"
#include "../SDK/CPanel.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlowObjectManager.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/RecvData.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/RenderView.h"
#include "../SDK/CTrace.h"
#include "../SDK/CViewSetup.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CPrediction.h"
#include "../FEATURES/Movement.h"
#include "../FEATURES/Visuals.h"
#include "../FEATURES/Chams.h"
#include "../FEATURES/AntiAim.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Backtracking.h"
#include "../FEATURES/FakeWalk.h"
#include "../cheats/visuals/player_esp.h"
#include "../cheats/visuals/other_esp.h"
#include "../cheats/exploits/exploits.h"
#include "../FEATURES/FakeLag.h"
#include "../FEATURES/EnginePred.h"
#include "../FEATURES/EventListener.h"
#include "../FEATURES/GrenadePrediction.h"
#include "../SDK/NetChannel.h"
#include "../FEATURES/Legitbot.h"
#include "../FEATURES/Flashlight.h"
#include "../FEATURES/GloveChanger.h"
#include "../FEATURES/SkinChanger.h"
#include "../FEATURES/D9Visuals.h"
#include "..\night_mode.h"
#include "..\FEATURES\custom_font.h"
#include "../shit.h"
#include "../configurations/configurations.hpp"
#include <intrin.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

CMenu* g_Menu = new CMenu();

namespace INIT
{
	HMODULE Dll;
	HWND Window;
	WNDPROC OldWindow;
}

namespace MenuTabs
{
	void Tab1();
	void Tab2();
	void Tab3();
	void Tab4();
	void Tab5();
}

template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(UTILS::FindSignature("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(UTILS::FindSignature("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}

static bool menu_open = false;
static bool tabsOnTop = false;
static bool d3d_init = false;
bool PressedKeys[256] = {};
const char* merixids[] =
{
	"1","2","3","4","5","6", "7", "8", "9",
	"Q","W","E","R","T","Y","U","I","O","P",
	"A","S","D","F","G","H","J","K","L",
	"Z","X","C","V","B","N","M",".","\\","|", "/","}","{","[","]",
	"<",">","?","'"
};
static char ConfigNamexd[64] = { 0 };
static char clantag_name[64] = { 0 };
static bool should_reset = false;
namespace ImGui
{

	static auto vector_getterxd = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	IMGUI_API bool ComboBoxArrayxd(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getterxd,
			static_cast<void*>(&values), values.size());
	}
}
ImFont* bigmenu_font;
ImFont* menu_font;
ImFont* smallmenu_font;
ImFont* font_menu;
//--- Other Globally Used Variables ---///
static bool tick = false;
static int ground_tick;
Vector vecAimPunch, vecViewPunch;
Vector* pAimPunch = nullptr;
Vector* pViewPunch = nullptr;

//--- Declare Signatures and Patterns Here ---///
static auto CAM_THINK = UTILS::FindSignature("client_panorama.dll", "85 C0 75 30 38 86");
static auto linegoesthrusmoke = UTILS::FindPattern("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");

//--- Tick Counting ---//
void ground_ticks()
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetFlags() & FL_ONGROUND)
		ground_tick++;
	else
		ground_tick = 0;
}

namespace HOOKS
{
	CreateMoveFn original_create_move;
	PaintTraverseFn original_paint_traverse;
	PaintFn original_paint;
	FrameStageNotifyFn original_frame_stage_notify;
	DrawModelExecuteFn original_draw_model_execute;
	SceneEndFn original_scene_end;
	TraceRayFn original_trace_ray;
	OverrideViewFn original_override_view;
	RenderViewFn original_render_view;
	SvCheatsGetBoolFn original_get_bool;
	GetViewmodelFOVFn original_viewmodel_fov;

	vfunc_hook fireevent;
	vfunc_hook directz;

	VMT::VMTHookManager iclient_hook_manager;
	VMT::VMTHookManager panel_hook_manager;
	VMT::VMTHookManager paint_hook_manager;
	VMT::VMTHookManager model_render_hook_manager;
	VMT::VMTHookManager scene_end_hook_manager;
	VMT::VMTHookManager render_view_hook_manager;
	VMT::VMTHookManager trace_hook_manager;
	VMT::VMTHookManager net_channel_hook_manager;
	VMT::VMTHookManager override_view_hook_manager;
	VMT::VMTHookManager input_table_manager;
	VMT::VMTHookManager get_bool_manager;
	std::string sPanel = ("FocusOverlayPanel");

	template<class T, class U>
	T fine(T in, U low, U high)
	{
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}

	bool __stdcall HookedCreateMove(float sample_input_frametime, SDK::CUserCmd* cmd)
	{
		if (!cmd || cmd->command_number == 0)
			return false;

		uintptr_t* FPointer; __asm { MOV FPointer, EBP }
		byte* SendPacket = (byte*)(*FPointer - 0x1C);
		if (!SendPacket) return false;

		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!local_player) return false;

		global::should_send_packet = *SendPacket;
		global::originalCMD = *cmd;
		if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
		{
			//GrenadePrediction::instance().Tick(cmd->buttons);
			if (settings::options.aim_type == 0)
				slidebitch->do_fakewalk(cmd);

			exploits->duck(cmd);

			if (!g_csgo::Engine->IsVoiceRecording())
				fakelag->do_fakelag(cmd);

			if (settings::options.bhop_bool) movement->bunnyhop(cmd);
			if (settings::options.duck_bool) movement->duckinair(cmd);
			if (settings::options.strafe_bool) movement->autostrafer(cmd);

			if (settings::options.misc_clantag)
			{
				int iLastTime;
				if (int(g_csgo::Globals->curtime) != iLastTime)
				{
					global::hasclantagged = true;
					static std::string cur_clantag = /*clantag_name*/ " wanheda.red ";
					//if (should_reset)
					//    cur_clantag = clantag_name;
					//todo: fix custom clantag staying static after force updating
					static int old_time,
						i = 0;

					if (i > 32)
					{
						auto marquee = [](std::string& panicova_zlomena_noha) -> void
						{
							std::string temp_string = panicova_zlomena_noha;
							panicova_zlomena_noha.erase(0, 1);
							panicova_zlomena_noha += temp_string[0];
						};
						marquee(cur_clantag);
						auto setclantag = [](const char* tag) -> void
						{
							static auto ClanTagOffset = UTILS::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx");
							if (ClanTagOffset)
							{
								auto tag_ = std::string(tag);
								if (strlen(tag) > 0) {
									auto newline = tag_.find("\\n");
									auto tab = tag_.find("\\t");
									if (newline != std::string::npos) {
										tag_.replace(newline, newline + 2, "\n");
									}
									if (tab != std::string::npos) {
										tag_.replace(tab, tab + 2, "\t");
									}
								}
								static auto dankesttSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(ClanTagOffset);
								dankesttSetClanTag(tag_.data(), tag_.data());
							}
						};
						setclantag(cur_clantag.c_str());
						i = 0;
					}
					else
					{
						i++;
					}
				}

				iLastTime = int(g_csgo::Globals->curtime);
			}
			else if (global::hasclantagged || should_reset && global::hasclantagged) {
				auto cl_clanid = g_csgo::cvar->FindVar("cl_clanid");
				cl_clanid->SetValue(-1);
				global::hasclantagged = false;
				should_reset = false;
			}
			prediction->run_prediction(cmd); {

				if (settings::options.aim_bool)
				{
					if (settings::options.fake_bool)
					{
						if (local_player->IsAlive())
						{
							for (int i = 1; i < g_csgo::Globals->maxclients; i++)
							{
								auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
								if (!entity || entity == local_player || entity->GetClientClass()->m_ClassID != !entity->IsAlive()) continue;



								float simtime_delta = entity->GetSimTime() - *reinterpret_cast<float*>(uintptr_t(entity) + OFFSETS::m_flSimulationTime) + 0x4;

								int choked_ticks = HOOKS::fine(TIME_TO_TICKS(simtime_delta), 1, 15);
								Vector lastOrig;

								if (lastOrig.Length() != entity->GetVecOrigin().Length())
									lastOrig = entity->GetVecOrigin();

								float delta_distance = (entity->GetVecOrigin() - lastOrig).LengthSqr();
								if (delta_distance > 4096.f)
								{
									Vector velocity_per_tick = entity->GetVelocity() * g_csgo::Globals->interval_per_tick;
									auto new_origin = entity->GetVecOrigin() + (velocity_per_tick * choked_ticks);
									entity->SetAbsOrigin(new_origin);
								}
							}
						}
					}
					aimbot->run_aimbot(cmd);
					backtracking->backtrack_player(cmd);
				}

				if (settings::options.aim_type == 1 && settings::options.aim_bool)
				{
					if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(settings::options.legittrigger_key)) && settings::options.legittrigger_bool)
						legitbot->triggerbot(cmd);
					backtracking->run_legit(cmd);
				}

				if (settings::options.aa_bool)
				{
					antiaim->do_antiaim(cmd);
					antiaim->fix_movement(cmd);

					auto net_channel = g_csgo::Engine->GetNetChannel();
					if (!net_channel)
						return;

					if (!global::should_send_packet && settings::options.desync_yaw)
						cmd->viewangles.y += net_channel->m_nChokedPackets == 0 ? -58 : 180;
					//ground_ticks();
				}
			} prediction->end_prediction(cmd);

			if (!global::should_send_packet) {
				global::real_angles = cmd->viewangles;
				global::angles = cmd->viewangles;
			}
			else
			{
				global::FakePosition = local_player->GetAbsOrigin();
				global::fake_angles = cmd->viewangles;
			}

		}
		*SendPacket = global::should_send_packet;
		cmd->move = antiaim->fix_movement(cmd, global::originalCMD);
		if (settings::options.aa_pitch < 2 || settings::options.aa_pitch1_type < 2 || settings::options.aa_pitch2_type < 2 || settings::options.aa_pitch3_type < 2)
			UTILS::ClampLemon(cmd->viewangles);

		return false;
	}

	void render_scope() {

		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!local_player) return;

		int
			screen_x,
			screen_y,
			center_x,
			center_y;

		g_csgo::Engine->GetScreenSize(screen_x, screen_y); g_csgo::Engine->GetScreenSize(center_x, center_y);
		center_x /= 2; center_y /= 2;

		static float rainbow;
		rainbow += 0.00001;

		if (rainbow > 1.f)
			rainbow = 0.f;

		if (local_player->GetIsScoped()) {

			CColor color;

			if (settings::options.scope_type == 0)
				color = CColor::FromHSB(rainbow, 1.f, 1.f);
			else
				color = CColor(0, 0, 0);

			RENDER::DrawLine(0, center_y, screen_x, center_y, color);
			RENDER::DrawLine(center_x, 0, center_x, screen_y, color);
		}
	}

	void __stdcall HookedPaintTraverse(int VGUIPanel, bool ForceRepaint, bool AllowForce)
	{
		if (settings::options.scope_bool) render_scope();
		std::string panel_name = g_csgo::Panel->GetName(VGUIPanel);
		if (panel_name == "HudZoom" && settings::options.scope_bool) return;
		//player_visuals->paint_traverse();

		cother_esp other_esp;
		other_esp.hitmarkerdynamic_paint();
		other_esp.render_tracer();

		player_visuals->paint_traverse();

		auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));

		/*if (!local_player)
			return;

		if (local_player->IsAlive()) {

			if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame()) {

				static DWORD* _death_notice = FindHudElement<DWORD>("CCSGO_HudDeathNotice");
				static void(__thiscall *_clear_notices)(DWORD) = (void(__thiscall*)(DWORD))UTILS::FindSignature("client_panorama.dll", "55 8B EC 83 EC 0C 53 56 8B 71 58");

				if (global::m_round_changed) {

					_death_notice = FindHudElement<DWORD>("CCSGO_HudDeathNotice");
					if (_death_notice - 20)
						_clear_notices(((DWORD)_death_notice - 20));

					global::m_round_changed = false;
				}

				if (_death_notice)
					*(float*)((DWORD)_death_notice + 0x50) = settings::options.killfeed ? 100 : 1.5f;
			}
		}*/

		SDK::ConVar* mat_fullbright = g_csgo::cvar->FindVar("mat_fullbright");

		if (settings::options.full_bright)
			mat_fullbright->SetValue(true);
		else
			mat_fullbright->SetValue(false);

		if (panel_name == "FocusOverlayPanel")
		{
			if (FONTS::ShouldReloadFonts())
				FONTS::InitFonts();

			if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
			{
				//FEATURES::MISC::in_game_logger.Do();
				auto matpostprocess = g_csgo::cvar->FindVar("mat_postprocess_enable");
				matpostprocess->fnChangeCallback = 0;
				matpostprocess->SetValue(settings::options.matpostprocessenable);

				cnight_mode night;
				night.do_nightmode();

				if (settings::options.Visuals.Enabled)
				{
					visuals->Draw();
					visuals->ClientDraw();
					//visuals->Drawmodels();
				}
			}
		}


		//for (int i = 0; i < 4; i++)
		//{
			//for (int j = 0; j < 4; j++)
			//{
			//	viewMatrix[i][j] = g_csgo::Engine->WorldToScreenMatrix()[i][j];
		//	}
	//	}
		original_paint_traverse(g_csgo::Panel, VGUIPanel, ForceRepaint, AllowForce);

		const char* pszPanelName = g_csgo::Panel->GetName(VGUIPanel);

		if (!strstr(pszPanelName, sPanel.data()))
			return;

		g_csgo::Panel->SetMouseInputEnabled(VGUIPanel, menu_open);
	}

	void FixThemAnim(SDK::CBaseEntity* entity)
	{
		if (!entity)
			return;

		auto old_curtime = g_csgo::Globals->curtime;
		auto old_frametime = g_csgo::Globals->frametime;
		auto old_fraction = entity->GetAnimState()->m_flUnknownFraction = 0.f;

		g_csgo::Globals->curtime = entity->GetSimTime();
		g_csgo::Globals->frametime = g_csgo::Globals->interval_per_tick;

		SDK::CAnimationLayer backup_layers[15];
		std::memcpy(backup_layers, entity->GetAnimOverlays(), (sizeof(SDK::CAnimationLayer) * 15));

		if (entity->GetAnimState())
			entity->GetAnimState()->m_iLastClientSideAnimationUpdateFramecount = g_csgo::Globals->framecount - 1;

		entity->GetClientSideAnimation2() = true;
		entity->UpdateClientSideAnimation();
		entity->GetClientSideAnimation2() = false;

		std::memcpy(entity->GetAnimOverlays(), backup_layers, (sizeof(SDK::CAnimationLayer) * 15));

		g_csgo::Globals->curtime = old_curtime;
		g_csgo::Globals->frametime = old_frametime;

		/*if (global::is_fakewalking) {
			entity->GetAnimState()->m_vVelocityX = old_velocity;
			entity->GetAnimState()->m_vVelocityY = old_velocity2;
			entity->GetAnimState()->m_flFeetSpeedUnknownForwardOrSideways = old_velocity3;
			entity->GetAnimState()->m_flFeetSpeedForwardsOrSideWays = old_velocity4;
			entity->GetAnimState()->m_flUnknownFloat3 = old_velocity5;
			entity->GetAnimState()->m_flUnknownFloat2 = old_velocity6;
			entity->GetAnimState()->m_flUnknownFloat1 = old_velocity7;
		}*/

		entity->GetAnimState()->m_flUnknownFraction = old_fraction; //fix them legs

		entity->SetAngle2(Vector(0.f, entity->GetAnimState()->m_flGoalFeetYaw, 0.f));
	}

	void __fastcall HookedFrameStageNotify(void* ecx, void* edx, int stage)
	{

		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!local_player) return;
		Vector vecAimPunch, vecViewPunch;
		Vector* pAimPunch = nullptr; Vector* pViewPunch = nullptr;

		auto GetCorrectDistance = [&local_player](float ideal_distance) -> float {

			Vector inverse_angles;
			g_csgo::Engine->GetViewAngles(inverse_angles);

			inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

			Vector direction;
			MATH::AngleVectors(inverse_angles, &direction);

			SDK::CTraceWorldOnly filter;
			SDK::trace_t trace;
			SDK::Ray_t ray;

			ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), (local_player->GetVecOrigin() + local_player->GetViewOffset()) + (direction * (ideal_distance + 5.f)));
			g_csgo::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

			return ideal_distance * trace.flFraction;
		};

		switch (stage)
		{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
			if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
			{
				GloveChanger();
				xdSkinchanger();
				for (int i = 1; i <= 65; i++)
				{
					auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
					if (!entity) continue;

					bool is_local_player = entity == local_player;
					bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

					if (is_local_player) continue;
					if (is_teammate) continue;
					if (entity->GetHealth() <= 0) continue;
					if (entity->GetIsDormant()) continue;

					//player_resolver resolver;
					// new resolvo
					//resolver.run();
				}
			} break;
		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
			break;
		case FRAME_RENDER_START:
			if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
			{
				if (in_tp)
				{
					SDK::CBaseAnimState* animstate = local_player->GetAnimState();

					if (!animstate)
						return;

					if (animstate->m_bInHitGroundAnimation && animstate->m_flHeadHeightOrOffsetFromHittingGroundAnimation)
						*reinterpret_cast<Vector*>(reinterpret_cast<DWORD>(local_player) + 0x31D8) = Vector(-10, global::angles.y, 0.f);
					else
						*reinterpret_cast<Vector*>(reinterpret_cast<DWORD>(local_player) + 0x31D8) = Vector(global::angles.x, global::angles.y, 0.f);

					g_csgo::pPrediction->SetLocalViewAngles(global::real_angles);
					if (global::is_fakewalking) {
					    local_player->UpdateClientSideAnimation(); //update our client side animation
						local_player->GetAnimState()->m_flUnknownFraction = 0.f; //replace leg shuffling with leg sliding
					}
					else
					FixThemAnim(local_player); //do that premium anim fix m8
					//g_csgo::pPrediction->SetLocalViewAngles(global::real_angles);

				}
				for (int i = 1; i <= 65; i++)
				{
					auto entity = g_csgo::ClientEntityList->GetClientEntity(i);
					if (!entity) continue;
					if (entity == local_player) continue;

					*(int*)((uintptr_t)entity + 0xA30) = g_csgo::Globals->framecount;
					*(int*)((uintptr_t)entity + 0xA28) = 0;
				}
			} break;

		case FRAME_NET_UPDATE_START:
			if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
			{
				if (settings::options.beam_bool)
					visuals->DrawBulletBeams();
			} break;
		case FRAME_NET_UPDATE_END:
			if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
			{
				for (int i = 1; i < 65; i++)
				{
					auto entity = g_csgo::ClientEntityList->GetClientEntity(i);

					if (!entity)
						continue;

					if (!local_player)
						continue;

					bool is_local_player = entity == local_player;
					bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

					if (is_local_player)
						continue;

					if (is_teammate)
						continue;

					if (entity->GetHealth() <= 0)
						continue;

					if (settings::options.aim_type == 0)
						backtracking->DisableInterpolation(entity);
				}
			}
			break;
		case FRAME_RENDER_END:
			if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame()) {

			}
		}
		original_frame_stage_notify(ecx, stage);
	}

	void __fastcall HookedDrawModelExecute(void* ecx, void* edx, SDK::IMatRenderContext* context, const SDK::DrawModelState_t& state, const SDK::ModelRenderInfo_t& render_info, matrix3x4_t* matrix)
	{
		if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
		{
			std::string ModelName = g_csgo::ModelInfo->GetModelName(render_info.pModel);

			auto local_player = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));

			if (in_tp && render_info.entity_index == local_player->GetIndex() && local_player->GetIsScoped())
				g_csgo::RenderView->SetBlend(settings::options.transparency_amnt);

			if (ModelName.find("v_sleeve") != std::string::npos)
			{
				SDK::IMaterial* material = g_csgo::MaterialSystem->FindMaterial(ModelName.c_str(), TEXTURE_GROUP_MODEL);
				if (!material) return;
				material->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
				material->ColorModulate();
				g_csgo::ModelRender->ForcedMaterialOverride(material);
			}
			/*for (int i = 1; i <= g_csgo::Globals->maxclients; i++) {

				auto entity = static_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(i));

				if (!entity)
					continue;

				if (!entity->IsAlive())
					continue;

				glow g;

				if (entity->SetupBones(g.playerMatrix, 128, BONE_USED_BY_HITBOX, g_csgo::Globals->curtime)) {

					SDK::IMaterial* material = g_csgo::MaterialSystem->FindMaterial("dev/glow_armsrace.vmt", nullptr);

					g_csgo::RenderView->SetBlend(0.f);
					g_csgo::ModelRender->ForcedMaterialOverride(material);
					entity->DrawModel(0x1, 150);
					g_csgo::ModelRender->ForcedMaterialOverride(nullptr);
				}
			}*/
		}
		original_draw_model_execute(ecx, context, state, render_info, matrix);
		g_csgo::RenderView->SetBlend(1.f);
	}

	void __fastcall HookedSceneEnd(void* ecx, void* edx)
	{
		original_scene_end(ecx);
		static SDK::IMaterial* ignorez = chams->CreateMaterialBasic(true, true, false);
		static SDK::IMaterial* notignorez = chams->CreateMaterialBasic(false, true, false);
		static SDK::IMaterial* ignorez_metallic = chams->CreateMaterialMetallic(true, true, false);
		static SDK::IMaterial* notignorez_metallic = chams->CreateMaterialMetallic(false, true, false);

		if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
		{
			auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
			if (!local_player) return;
			CColor color = CColor(settings::options.glow_col[0] * 255, settings::options.glow_col[1] * 255, settings::options.glow_col[2] * 255, settings::options.glow_col[3] * 255), colorTeam = CColor(settings::options.teamglow_color[0] * 255, settings::options.teamglow_color[1] * 255, settings::options.teamglow_color[2] * 255, settings::options.teamglow_color[3] * 255), colorlocal = CColor(settings::options.glowlocal_col[0] * 255, settings::options.glowlocal_col[1] * 255, settings::options.glowlocal_col[2] * 255, settings::options.glowlocal_col[3] * 255);
			for (int i = 1; i < 65; i++)
			{
				if (settings::options.fakechams)
				{
					auto pLocal = reinterpret_cast<SDK::CBaseEntity*>(g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer()));
					if (pLocal && pLocal->IsAlive())
					{
						static SDK::IMaterial* mat = chams->CreateMaterialBasic(false, true, false);
						if (mat)
						{
							Vector OrigAng = pLocal->GetEyeAngles();
							pLocal->SetAngle2(Vector(0, global::fake_angles.y, 0));
							bool LbyColor = false;
							g_csgo::RenderView->SetColorModulation(CColor(255, 255, 255));
							g_csgo::RenderView->SetBlend(.5f);
							g_csgo::ModelRender->ForcedMaterialOverride(mat);
							pLocal->DrawModel(0x1, 150);
							g_csgo::ModelRender->ForcedMaterialOverride(nullptr);
							pLocal->SetAngle2(OrigAng);
						} original_scene_end(ecx);
					}
				}

				auto entity = g_csgo::ClientEntityList->GetClientEntity(i);

				if (!entity) continue;
				if (!local_player) continue;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;
				auto ignorezmaterial = settings::options.chamstype == 0 ? ignorez_metallic : ignorez;
				auto notignorezmaterial = settings::options.chamstype == 0 ? notignorez_metallic : notignorez;
				if (is_local_player)
				{
					switch (settings::options.localchams)
					{
					case 0: continue; break;
					case 1:
						local_player->SetAbsOrigin(global::FakePosition);
						local_player->DrawModel(0x1, 255);
						local_player->SetAbsOrigin(local_player->GetAbsOrigin());
						continue; break;
					case 2:
						g_csgo::RenderView->SetColorModulation(CColor(settings::options.localchams_col[0] * 255, settings::options.localchams_col[1] * 255, settings::options.localchams_col[2] * 255, settings::options.localchams_col[3] * 255));
						g_csgo::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						local_player->DrawModel(0x1, 255);
						g_csgo::ModelRender->ForcedMaterialOverride(nullptr);
						continue;  break;
					case 3:
						notignorezmaterial->ColorModulate(CColor(settings::options.localchams_col[0] * 255, settings::options.localchams_col[1] * 255, settings::options.localchams_col[2] * 255, settings::options.localchams_col[3] * 255));
						g_csgo::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						local_player->SetAbsOrigin(global::FakePosition);
						local_player->DrawModel(0x1, 255);
						local_player->SetAbsOrigin(local_player->GetAbsOrigin());
						g_csgo::ModelRender->ForcedMaterialOverride(nullptr);
						continue; break;
					}
				}

				if (entity->GetHealth() <= 0) continue;
				if (entity->GetIsDormant())	continue;
				if (entity->GetClientClass()->m_ClassID != 38) continue;

				if (is_teammate)
				{
					if (entity && settings::options.chamsteam == 2)
					{
						ignorezmaterial->ColorModulate(CColor(settings::options.teaminvis_color[0] * 255, settings::options.teaminvis_color[1] * 255, settings::options.teaminvis_color[2] * 255, settings::options.teaminvis_color[3] * 255));
						g_csgo::ModelRender->ForcedMaterialOverride(ignorezmaterial);
						entity->DrawModel(0x1, 255);

						notignorezmaterial->ColorModulate(CColor(settings::options.teamvis_color[0] * 255, settings::options.teamvis_color[1] * 255, settings::options.teamvis_color[2] * 255, settings::options.teamvis_color[3] * 255));
						g_csgo::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						entity->DrawModel(0x1, 255);

						g_csgo::ModelRender->ForcedMaterialOverride(nullptr);
					}
					else if (entity && settings::options.chamsteam == 1)
					{
						notignorezmaterial->ColorModulate(CColor(settings::options.teamvis_color[0] * 255, settings::options.teamvis_color[1] * 255, settings::options.teamvis_color[2] * 255, settings::options.teamvis_color[3] * 255));
						g_csgo::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						entity->DrawModel(0x1, 255);

						g_csgo::ModelRender->ForcedMaterialOverride(nullptr);
					} continue;
				}
				else if (is_teammate && settings::options.chamsteam)
					continue;

				if (entity && settings::options.chams_type == 2)
				{
					ignorezmaterial->ColorModulate(CColor(settings::options.imodel_col[0] * 255, settings::options.imodel_col[1] * 255, settings::options.imodel_col[2] * 255, settings::options.imodel_col[3] * 255));
					g_csgo::ModelRender->ForcedMaterialOverride(ignorezmaterial);
					entity->DrawModel(0x1, 255);

					notignorezmaterial->ColorModulate(CColor(settings::options.vmodel_col[0] * 255, settings::options.vmodel_col[1] * 255, settings::options.vmodel_col[2] * 255, settings::options.vmodel_col[3] * 255));
					g_csgo::ModelRender->ForcedMaterialOverride(notignorezmaterial);
					entity->DrawModel(0x1, 255);

					g_csgo::ModelRender->ForcedMaterialOverride(nullptr);
				}
				else if (entity && settings::options.chams_type == 1)
				{
					notignorezmaterial->ColorModulate(CColor(settings::options.vmodel_col[0] * 255, settings::options.vmodel_col[1] * 255, settings::options.vmodel_col[2] * 255, settings::options.vmodel_col[3] * 255));
					g_csgo::ModelRender->ForcedMaterialOverride(notignorezmaterial);
					entity->DrawModel(0x1, 255);

					g_csgo::ModelRender->ForcedMaterialOverride(nullptr);
				}
			}

			for (auto i = 0; i < g_csgo::GlowObjManager->GetSize(); i++)
			{
				auto &glowObject = g_csgo::GlowObjManager->m_GlowObjectDefinitions[i];
				auto entity = reinterpret_cast<SDK::CBaseEntity*>(glowObject.m_pEntity);

				if (!entity) continue;
				if (!local_player) continue;

				if (glowObject.IsUnused()) continue;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

				if (is_local_player && in_tp && settings::options.glowlocal)
				{
					glowObject.m_nGlowStyle = settings::options.glowstylelocal;
					glowObject.m_flRed = colorlocal.RGBA[0] / 255.0f;
					glowObject.m_flGreen = colorlocal.RGBA[1] / 255.0f;
					glowObject.m_flBlue = colorlocal.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = colorlocal.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
					continue;
				}
				else if (!settings::options.glowlocal && is_local_player)
					continue;

				if (entity->GetHealth() <= 0) continue;
				if (entity->GetIsDormant())	continue;
				if (entity->GetClientClass()->m_ClassID != 38) continue;

				if (is_teammate && settings::options.glowteam)
				{
					glowObject.m_nGlowStyle = settings::options.glowstyle; //0;
					glowObject.m_flRed = colorTeam.RGBA[0] / 255.0f;
					glowObject.m_flGreen = colorTeam.RGBA[1] / 255.0f;
					glowObject.m_flBlue = colorTeam.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = colorTeam.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
					continue;
				}
				else if (is_teammate && !settings::options.glowteam)
					continue;

				if (settings::options.glowenable)
				{
					glowObject.m_nGlowStyle = settings::options.glowstyle;//0;
					glowObject.m_flRed = color.RGBA[0] / 255.0f;
					glowObject.m_flGreen = color.RGBA[1] / 255.0f;
					glowObject.m_flBlue = color.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = color.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
				}
			}

			SDK::IMaterial* flash = g_csgo::MaterialSystem->FindMaterial("effects/flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);
			SDK::IMaterial* flashWhite = g_csgo::MaterialSystem->FindMaterial("effects/flashbang_white", TEXTURE_GROUP_CLIENT_EFFECTS);

			flash->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, settings::options.no_flash ? true : false);
			flashWhite->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, settings::options.no_flash ? true : false);

			if (settings::options.smoke_bool)
			{
				std::vector<const char*> vistasmoke_wireframe = { "particle/vistasmokev1/vistasmokev1_smokegrenade" };

				std::vector<const char*> vistasmoke_nodraw =
				{
					"particle/vistasmokev1/vistasmokev1_fire",
					"particle/vistasmokev1/vistasmokev1_emods",
					"particle/vistasmokev1/vistasmokev1_emods_impactdust",
				};

				for (auto mat_s : vistasmoke_wireframe)
				{
					SDK::IMaterial* mat = g_csgo::MaterialSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, true); //wireframe
				}

				for (auto mat_n : vistasmoke_nodraw)
				{
					SDK::IMaterial* mat = g_csgo::MaterialSystem->FindMaterial(mat_n, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
				}

				static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);
				*(int*)(smokecout) = 0;
			}
		}
	}
	void __fastcall HookedOverrideView(void* ecx, void* edx, SDK::CViewSetup* pSetup)
	{
		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!local_player) return;

		auto animstate = local_player->GetAnimState();
		if (!animstate) return;

		if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(settings::options.thirdperson_int)) & 1)
			in_tp = !in_tp;

		if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
		{
			//GrenadePrediction::instance().View(pSetup);
			auto GetCorrectDistance = [&local_player](float ideal_distance) -> float //lambda
			{
				Vector inverse_angles;
				g_csgo::Engine->GetViewAngles(inverse_angles);

				inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

				Vector direction;
				MATH::AngleVectors(inverse_angles, &direction);

				SDK::CTraceWorldOnly filter;
				SDK::trace_t trace;
				SDK::Ray_t ray;

				ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), (local_player->GetVecOrigin() + local_player->GetViewOffset()) + (direction * (ideal_distance + 5.f)));
				g_csgo::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

				return ideal_distance * trace.flFraction;
			};

			if (settings::options.tp_bool && in_tp) {
				
				if (local_player->GetHealth() <= 0)
					local_player->SetObserverMode(5);

				if (!g_csgo::Input->m_fCameraInThirdPerson)
				{
					g_csgo::Input->m_fCameraInThirdPerson = true;

					SDK::CBaseAnimState* animstate = local_player->GetAnimState();

					if (!animstate)
						return;

					if (animstate->m_bInHitGroundAnimation && animstate->m_flHeadHeightOrOffsetFromHittingGroundAnimation) {
						g_csgo::Input->m_vecCameraOffset = Vector(-10, global::angles.y, GetCorrectDistance(100));
					}
					else {
						g_csgo::Input->m_vecCameraOffset = Vector(global::angles.x, global::angles.y, GetCorrectDistance(100));
					}
					Vector camForward;

					MATH::AngleVectors(Vector(g_csgo::Input->m_vecCameraOffset.x, g_csgo::Input->m_vecCameraOffset.y, 0), &camForward);
				}
			}
			else {
				g_csgo::Input->m_fCameraInThirdPerson = false;
				g_csgo::Input->m_vecCameraOffset = Vector(global::angles.x, global::angles.y, 0);
			}

			auto zoomsensration = g_csgo::cvar->FindVar("zoom_sensitivity_ratio_mouse");
			if (settings::options.fixscopesens)
				zoomsensration->SetValue("0");
			else
				zoomsensration->SetValue("1");

			if (settings::options.aim_type == 0) {

				if (!local_player->GetIsScoped())
					pSetup->fov = settings::options.fov_val;
				else if (local_player->GetIsScoped() && settings::options.removescoping)
					pSetup->fov = settings::options.fov_val;
			}
			else if (!(settings::options.aim_type == 0) && !local_player->GetIsScoped())
				pSetup->fov = 90;
		}
		original_override_view(ecx, pSetup);
	}
	void __fastcall HookedTraceRay(void *thisptr, void*, const SDK::Ray_t &ray, unsigned int fMask, SDK::ITraceFilter *pTraceFilter, SDK::trace_t *pTrace)
	{
		original_trace_ray(thisptr, ray, fMask, pTraceFilter, pTrace);
		if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
			pTrace->surface.flags |= SURF_SKY;
	}
	bool __fastcall HookedGetBool(void* pConVar, void* edx)
	{
		if ((uintptr_t)_ReturnAddress() == CAM_THINK)
			return true;

		return original_get_bool(pConVar);
	}
	float __fastcall GetViewmodelFOV()
	{
		if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
		{
			float player_fov = original_viewmodel_fov();

			if (settings::options.esp_bool)
				player_fov = settings::options.viewfov_val;

			return player_fov;
		}
	}






	void OpenMenu()
	{
		static bool is_down = false;
		static bool is_clicked = false;
		if (GetAsyncKeyState(VK_INSERT))
		{
			is_clicked = false;
			is_down = true;
		}
		else if (!GetAsyncKeyState(VK_INSERT) && is_down)
		{
			is_clicked = true;
			is_down = false;
		}
		else
		{
			is_clicked = false;
			is_down = false;
		}

		if (is_clicked)
		{
			menu_open = !menu_open;

		}
	}




	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {
		case WM_LBUTTONDOWN:
			PressedKeys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			PressedKeys[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			PressedKeys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			PressedKeys[VK_RBUTTON] = false;
			break;
		case WM_MBUTTONDOWN:
			PressedKeys[VK_MBUTTON] = true;
			break;
		case WM_MBUTTONUP:
			PressedKeys[VK_MBUTTON] = false;
			break;
		case WM_XBUTTONDOWN:
		{
			UINT button = GET_XBUTTON_WPARAM(wParam);
			if (button == XBUTTON1)
			{
				PressedKeys[VK_XBUTTON1] = true;
			}
			else if (button == XBUTTON2)
			{
				PressedKeys[VK_XBUTTON2] = true;
			}
			break;
		}
		case WM_XBUTTONUP:
		{
			UINT button = GET_XBUTTON_WPARAM(wParam);
			if (button == XBUTTON1)
			{
				PressedKeys[VK_XBUTTON1] = false;
			}
			else if (button == XBUTTON2)
			{
				PressedKeys[VK_XBUTTON2] = false;
			}
			break;
		}
		case WM_KEYDOWN:
			PressedKeys[wParam] = true;
			break;
		case WM_KEYUP:
			PressedKeys[wParam] = false;
			break;
		default: break;
		}

		OpenMenu();

		if (d3d_init && menu_open && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProc(INIT::OldWindow, hWnd, uMsg, wParam, lParam);
	}


	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		ImGui_ImplDX9_Init(INIT::Window, pDevice);


		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 15.0f);


		ImGuiStyle &styled = ImGui::GetStyle();
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.45f, 0.00f, 0.29f, 1.00f);
		colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.45f, 0.00f, 0.29f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.71f, 0.00f, 0.45f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.28f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.28f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.28f);
		colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 0.00f, 0.64f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 0.00f, 0.64f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.00f, 0.64f, 0.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(1.00f, 0.00f, 0.64f, 0.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.85f, 0.85f, 0.85f, 0.12f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 0.56f, 0.81f, 0.30f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.19f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.75f, 0.08f, 0.53f, 0.30f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.75f, 0.08f, 0.53f, 0.30f);
		colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.28f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.49f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.71f, 0.71f, 0.71f, 0.61f);
		colors[ImGuiCol_Header] = ImVec4(1.00f, 0.00f, 0.70f, 0.77f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.70f, 0.57f);
		colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.00f, 0.70f, 0.38f);
		colors[ImGuiCol_Separator] = ImVec4(0.71f, 0.00f, 0.45f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.71f, 0.00f, 0.45f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.71f, 0.00f, 0.45f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);

		style.WindowRounding = 7.f;

		/*style.WindowPadding = ImVec2(32, 32);
		style.WindowRounding = 5.0f;
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.ChildWindowRounding = 0.0f;
		style.FramePadding = ImVec2(5, 5);
		style.FrameRounding = 4.0f;
		style.ItemSpacing = ImVec2(12, 8);
		style.ItemInnerSpacing = ImVec2(8, 6);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 3.0f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = true;
		style.CurveTessellationTol = 1.25f;*/

		pDevice->GetViewport(&Menuxd::viewPort);
		D3DXCreateFont(pDevice, 9, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("Verdana"), &Menuxd::fntVerdana9);
		D3DXCreateFont(pDevice, 10, 5, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("Verdana"), &Menuxd::fntVerdana10);
		D3DXCreateFont(pDevice, 11, 5, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("Verdana"), &Menuxd::fntVerdana11);
		//D3DXCreateFont(pDevice, 12, 5, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("Verdana"), &Menuxd::fntVerdana12);
		D3DXCreateFont(pDevice, 11, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, ("csgo_icons"), &Menuxd::fntWeaponIcon);
		font_menu = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(gamer, 34952, 24.f);

		d3d_init = true;

		
	}
	long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		static auto ofunc = directz.get_original<EndSceneFn>(42);

		D3DCOLOR rectColor = D3DCOLOR_XRGB(255, 0, 0);
		D3DRECT BarRect = { 1, 1, 1, 1 };
		ImGuiStyle& style = ImGui::GetStyle();
		pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, rectColor, 0, 0);

		if (!d3d_init)
			GUI_Init(pDevice);

		//ImGui::GetIO().MouseDrawCursor = menu_open;

		static const D3DRENDERSTATETYPE backupStates[] = { D3DRS_COLORWRITEENABLE, D3DRS_ALPHABLENDENABLE, D3DRS_SRCBLEND, D3DRS_DESTBLEND, D3DRS_BLENDOP, D3DRS_FOGENABLE };
		static const int size = sizeof(backupStates) / sizeof(DWORD);

		DWORD oldStates[size] = { 0 };

		for (int i = 0; i < size; i++)
			pDevice->GetRenderState(backupStates[i], &oldStates[i]);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

		ImGui_ImplDX9_NewFrame();

		g_Menu->Menu(); //THIS IS THE ENTRY POINT FOR THE MENU
		
		ImGui::Render();

		//if (settings::options.Visuals.Enabled)
			//D9Visuals::Render(pDevice);

		if (settings::options.spread_bool == 3)
			Menuxd::drawfatalpricel(pDevice);

		for (int i = 0; i < size; i++)
			pDevice->SetRenderState(backupStates[i], oldStates[i]);

		return ofunc(pDevice);

	}

	HRESULT __stdcall hooked_present(IDirect3DDevice9* device, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion) {

		static auto ofunc = directz.get_original<hooked_presentFn>(17);

		return ofunc(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

	long __stdcall Hooked_EndScene_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto ofunc = directz.get_original<EndSceneResetFn>(16);

		if (!d3d_init)
			return ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();

		auto hr = ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_CreateDeviceObjects();

		return hr;
	}

	typedef bool(__thiscall *FireEventClientSide)(void*, SDK::IGameEvent*);

	bool __fastcall Hooked_FireEventClientSide(void *ecx, void* edx, SDK::IGameEvent* pEvent) {

		static auto ofunct = fireevent.get_original<FireEventClientSide>(9);

		if (!pEvent)
			return ofunct(ecx, pEvent);

		DamageESP::HandleGameEvent(pEvent);

		/*
		if (g_Options.NewLegitbot.Aimbot.bKillDelay && g_Options.NewLegitbot.Aimbot.Enabled)
		{

		if (strcmp(pEvent->GetName(), "player_death") == 0)
		{
		int attacker = Interfaces.pEngine->GetPlayerForUserID(pEvent->GetInt("attacker"));
		int iLocalPlayer = Interfaces.pEngine->GetLocalPlayer();
		if (attacker == iLocalPlayer)
		{
		G::KillStopDelay = Interfaces.pGlobalVars->curtime + g_Options.NewLegitbot.Aimbot.iKillDelay;
		}
		}
		}
		*/


		return ofunct(ecx, pEvent);
	}




	void InitHooks()
	{
		iclient_hook_manager.Init(g_csgo::Client);
		original_frame_stage_notify = reinterpret_cast<FrameStageNotifyFn>(iclient_hook_manager.HookFunction<FrameStageNotifyFn>(37, HookedFrameStageNotify));

		panel_hook_manager.Init(g_csgo::Panel);
		original_paint_traverse = reinterpret_cast<PaintTraverseFn>(panel_hook_manager.HookFunction<PaintTraverseFn>(41, HookedPaintTraverse));

		model_render_hook_manager.Init(g_csgo::ModelRender);
		original_draw_model_execute = reinterpret_cast<DrawModelExecuteFn>(model_render_hook_manager.HookFunction<DrawModelExecuteFn>(21, HookedDrawModelExecute));

		scene_end_hook_manager.Init(g_csgo::RenderView);
		original_scene_end = reinterpret_cast<SceneEndFn>(scene_end_hook_manager.HookFunction<SceneEndFn>(9, HookedSceneEnd));

		trace_hook_manager.Init(g_csgo::Trace);
		original_trace_ray = reinterpret_cast<TraceRayFn>(trace_hook_manager.HookFunction<TraceRayFn>(5, HookedTraceRay));

		override_view_hook_manager.Init(g_csgo::ClientMode);
		original_override_view = reinterpret_cast<OverrideViewFn>(override_view_hook_manager.HookFunction<OverrideViewFn>(18, HookedOverrideView));
		original_create_move = reinterpret_cast<CreateMoveFn>(override_view_hook_manager.HookFunction<CreateMoveFn>(24, HookedCreateMove));
		original_viewmodel_fov = reinterpret_cast<GetViewmodelFOVFn>(override_view_hook_manager.HookFunction<GetViewmodelFOVFn>(35, GetViewmodelFOV));

		auto sv_cheats = g_csgo::cvar->FindVar("sv_cheats");
		get_bool_manager = VMT::VMTHookManager(reinterpret_cast<DWORD**>(sv_cheats));
		original_get_bool = reinterpret_cast<SvCheatsGetBoolFn>(get_bool_manager.HookFunction<SvCheatsGetBoolFn>(13, HookedGetBool));

		fireevent.setup(g_csgo::GameEventManager);
		fireevent.hook_index(9, Hooked_FireEventClientSide);


		if (!(INIT::Window = FindWindowA("Valve001", nullptr)))
			Sleep(100);

		if (INIT::Window)
			INIT::OldWindow = (WNDPROC)SetWindowLongPtr(INIT::Window, GWL_WNDPROC, (LONG_PTR)Hooked_WndProc);

		DWORD DeviceStructureAddress = **(DWORD**)(UTILS::FindSignature("shaderapidx9.dll", "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C") + 1);

		// fluxi if (devicestructureaddress)

		directz.setup((DWORD**)DeviceStructureAddress);
		directz.hook_index(16, Hooked_EndScene_Reset);
		directz.hook_index(17, hooked_present);
		directz.hook_index(42, Hooked_EndScene);

	}
	void EyeAnglesPitchHook(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;

	}
	void EyeAnglesYawHook(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;
	}
	void InitNetvarHooks()
	{
		UTILS::netvar_hook_manager.Hook("DT_CSPlayer", "m_angEyeAngles[0]", EyeAnglesPitchHook);
		UTILS::netvar_hook_manager.Hook("DT_CSPlayer", "m_angEyeAngles[1]", EyeAnglesYawHook);
	}
}

const char* key_binds[] = { "none", "mouse1", "mouse2", "mouse3", "mouse4", "mouse5", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12" };

void MenuTabs::Tab1() {
	
	
}

void MenuTabs::Tab2() {


}

void MenuTabs::Tab3() {

	ImGui::BeginChild("sa3rqewfadfwe", ImVec2(500, 0), true); {

		const char* fakelag_mode[] = { "factor", "adaptive" };

		ImGui::Checkbox(("auto bunnyhop"), &settings::options.bhop_bool);
		ImGui::Checkbox(("auto strafer"), &settings::options.strafe_bool);
		ImGui::Checkbox(("duck in air"), &settings::options.duck_bool);
		ImGui::Checkbox(("animated clantag"), &settings::options.misc_clantag);
		ImGui::Checkbox(("hit sound"), &settings::options.hitsound);
		ImGui::Checkbox(("preserve killfeed"), &settings::options.killfeed);
		//ImGui::InputText("##clantag_text", clantag_name, 64);
		//if (ImGui::Button("force update clantag"))
		//	should_reset = true;
	//	else
		//	should_reset = false;

		//ImGui::Hotkey(("circle strafer"), &settings::options.circlestrafekey, key_binds, ARRAYSIZE(key_binds));
		ImGui::Checkbox(("enable fakeLag"), &settings::options.lag_bool);
		ImGui::Combo(("fakelag type"), &settings::options.lag_type, fakelag_mode, ARRAYSIZE(fakelag_mode));
		ImGui::SliderInt("standing lag", &settings::options.stand_lag, 1, 14);
		ImGui::SliderInt("moving lag", &settings::options.move_lag, 1, 14);
		ImGui::SliderInt("jumping lag", &settings::options.jump_lag, 1, 14);
		ImGui::SliderInt("fakewalk lag", &settings::options.fakewalk_lag, 1, 14);
	} ImGui::EndChild();
}

struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};

void KnifeApplyCallbk() {

	static auto clear_hud_weapon_icon_fn =
		reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
			UTILS::FindSignature("client_panorama.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C 89 5D FC"));

	auto element = FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");

	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);
	if (hud_weapons == nullptr)
		return;

	if (!*hud_weapons->get_weapon_count())
		return;

	for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = clear_hud_weapon_icon_fn(hud_weapons, i);

	static SDK::ConVar* Meme = g_csgo::cvar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	g_csgo::Engine->ClientCmd_Unrestricted("cl_fullupdate");

}

SDK::CBaseWeapon* xd(SDK::CBaseEntity* xz)
{
	if (!g_csgo::Engine->IsConnected())
		return nullptr;
	if (!xz->IsAlive())
		return nullptr;

	HANDLE weaponData = *(HANDLE*)((DWORD)xz + OFFSETS::m_hActiveWeapon);
	return (SDK::CBaseWeapon*)g_csgo::ClientEntityList->GetClientEntityFromHandle(weaponData);
}

short SafeWeaponID()
{
	SDK::CBaseEntity* local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!(local_player))
		return 0;

	SDK::CBaseWeapon* WeaponC = xd(local_player);

	if (!(WeaponC))
		return 0;

	return WeaponC->GetItemDefenitionIndex();
}

void MenuTabs::Tab4() {

	ImGui::BeginChild(("#skinchanger"), ImVec2(ImGui::GetWindowWidth() / 3 - 10, 0), true, true ? ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar : ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders); {

		ImGui::Text("DO NOT CLICK FORCE UPDATE WITHOUT \nENABLING SKIN CHANGER!", CColor(255, 255, 255));

		ImGui::Checkbox("skin changer", &settings::options.skinenabled);
		ImGui::Checkbox("glove changer", &settings::options.glovesenabled);
		//ImGui::Checkbox("model changer", &settings::options.CUSTOMMODEL);
		/*if (settings::options.CUSTOMMODEL)
		{
			ImGui::Combo(("ct"), &settings::options.customct, playermodels, ARRAYSIZE(playermodels));
			ImGui::Combo(("t"), &settings::options.customtt, playermodels, ARRAYSIZE(playermodels));
		}*/
		if (ImGui::Button(("Force update")))
			KnifeApplyCallbk();

	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild(("#skinchanger_pages"), ImVec2(320, 0), true, true ? ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar : ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders); {

		static bool Main = true;
		static bool Colors = false;

		static int page = 0;

		ImGuiStyle& style = ImGui::GetStyle();


		style.ItemSpacing = ImVec2(1, 1);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.f));

		const char* meme = "page : error";

		switch (page) {

			case 0: meme = "page : 1";  break;
			case 1: meme = "page : 2";  break;
			case 2: meme = "page : 3";  break;
			case 3: meme = "page : 4";  break;
			case 4: meme = "page : 5";  break;
			default: break;
		}

		ImGui::Text(meme); ImGui::SameLine(); ImGui::Text(("                  ")); ImGui::SameLine();
		if (ImGui::Button(("-"), ImVec2(22, 22))) {

			if (page != 0)
				page--;
		};

		ImGui::SameLine();

		if (ImGui::Button(("+"), ImVec2(22, 22))) {

			if (page != 3)
				page++;
		};

		ImGui::Text(("        "));

		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();

		style.ItemSpacing = ImVec2(8, 4);

		switch (page) {

		case 0: {

			ImGui::PushItemWidth(150.0f);
			ImGui::Combo(("knife model"), &settings::options.Knife, KnifeModel, ARRAYSIZE(KnifeModel));
			ImGui::PushItemWidth(150.0f);
			ImGui::Combo(("knife skin"), &settings::options.KnifeSkin, knifeskins, ARRAYSIZE(knifeskins));
			ImGui::PushItemWidth(150.0f);
			ImGui::Combo(("glove model"), &settings::options.gloves, GloveModel, ARRAYSIZE(GloveModel));
			ImGui::PushItemWidth(150.0f);

			switch (settings::options.gloves) {

				case 0: {
					const char* glovesskins[] = { "" };
					ImGui::Combo(("glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
					break;
				}
				case 1: {
					const char* glovesskins[] = { "charred", "snakebite", "bronzed", "guerilla" };
					ImGui::Combo(("glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
					break;
				}
				case 2: {
					const char* glovesskins[] = { "hedge maze", "andoras box", "superconductor", "arid", "omega", "amphibious", "bronze morph" };
					ImGui::Combo(("glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
					break;
				}
				case 3: {
					const char* glovesskins[] = { "lunar weave", "convoy", "crimson weave", "diamondback", "overtake", "racing green", "king snake", "imperial plaid" };
					ImGui::Combo(("glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
					break;
				}
				case 4: {
					const char* glovesskins[] = { "leather", "spruce ddpat", "slaughter", "cobalt skulls", "overprint", "duct tape", "arboreal" };
					ImGui::Combo(("glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
					break;
				}
				case 5: {
					const char* glovesskins[] = { "eclipse", "spearmint", "boom", "cool mint", "turtle", "transport", "polygon", "pow" };
					ImGui::Combo(("glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
					break;
				}
				case 6: {
					const char* glovesskins[] = { "forest ddpat", "crimson kimono", "emerald web", "foundation", "crimson web", "buckshot", "fade", "mogul" };
					ImGui::Combo(("glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
					break;
				}
				case 7: {
					const char* glovesskins[] = { "emerald", "mangrove", "rattler", "case hardened" };
					ImGui::Combo(("glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
					break;
				}
			}

		} break;
		case 1:
		{
			ImGui::Combo(("AK-47"), &settings::options.AK47Skin, ak47, ARRAYSIZE(ak47));
			ImGui::Combo(("M4A1-S"), &settings::options.M4A1SSkin, m4a1s, ARRAYSIZE(m4a1s));
			ImGui::Combo(("M4A4"), &settings::options.M4A4Skin, m4a4, ARRAYSIZE(m4a4));
			ImGui::Combo(("Galil AR"), &settings::options.GalilSkin, galil, ARRAYSIZE(galil));
			ImGui::Combo(("AUG"), &settings::options.AUGSkin, aug, ARRAYSIZE(aug));
			ImGui::Combo(("FAMAS"), &settings::options.FAMASSkin, famas, ARRAYSIZE(famas));
			ImGui::Combo(("SG-553"), &settings::options.Sg553Skin, sg553, ARRAYSIZE(sg553));
			ImGui::Combo(("UMP45"), &settings::options.UMP45Skin, ump45, ARRAYSIZE(ump45));
			ImGui::Combo(("MAC-10"), &settings::options.Mac10Skin, mac10, ARRAYSIZE(mac10));
			ImGui::Combo(("PP-Bizon"), &settings::options.BizonSkin, bizon, ARRAYSIZE(bizon));
			ImGui::Combo(("TEC-9"), &settings::options.tec9Skin, tec9, ARRAYSIZE(tec9));
			ImGui::Combo(("P2000"), &settings::options.P2000Skin, p2000, ARRAYSIZE(p2000));
			ImGui::Combo(("P250"), &settings::options.P250Skin, p250, ARRAYSIZE(p250));
			ImGui::Combo(("Dual-Barettas"), &settings::options.DualSkin, dual, ARRAYSIZE(dual));
			ImGui::Combo(("CZ75-Auto"), &settings::options.Cz75Skin, cz75, ARRAYSIZE(cz75));
			ImGui::Combo(("Nova"), &settings::options.NovaSkin, nova, ARRAYSIZE(nova));


		} break;
		case 2:
		{
			ImGui::Combo(("XM1014"), &settings::options.XmSkin, xm, ARRAYSIZE(xm));
			ImGui::Combo(("AWP"), &settings::options.AWPSkin, awp, ARRAYSIZE(awp));
			ImGui::Combo(("SSG08"), &settings::options.SSG08Skin, ssg08, ARRAYSIZE(ssg08));
			ImGui::Combo(("SCAR20"), &settings::options.SCAR20Skin, scar20, ARRAYSIZE(scar20));
			ImGui::Combo(("G3SG1"), &settings::options.G3sg1Skin, g3sg1, ARRAYSIZE(g3sg1));
			ImGui::Combo(("MP9"), &settings::options.Mp9Skin, mp9, ARRAYSIZE(mp9));
			ImGui::Combo(("Glock-18"), &settings::options.GlockSkin, glock, ARRAYSIZE(glock));
			ImGui::Combo(("USP-S"), &settings::options.USPSkin, usp, ARRAYSIZE(usp));
			ImGui::Combo(("Desert Eagle"), &settings::options.DeagleSkin, deagle, ARRAYSIZE(deagle));
			ImGui::Combo(("Five-Seven"), &settings::options.FiveSkin, five, ARRAYSIZE(five));
			ImGui::Combo(("Revolver"), &settings::options.RevolverSkin, revolver, ARRAYSIZE(revolver));
			ImGui::Combo(("Negev"), &settings::options.NegevSkin, negev, ARRAYSIZE(negev));
			ImGui::Combo(("M249"), &settings::options.M249Skin, m249, ARRAYSIZE(m249));
			ImGui::Combo(("SAWED-Off"), &settings::options.SawedSkin, sawed, ARRAYSIZE(sawed));
			ImGui::Combo(("Mag-7"), &settings::options.MagSkin, mag, ARRAYSIZE(mag));
		} break;
		default:break;
		}
	} ImGui::EndChild();
}

void MenuTabs::Tab5() {

	
}



template<typename T, int N>
inline size_t array_size(T(&)[N])
{
	return N;
}

#define ARRAY_SIZE(X)   (sizeof(array_size(X)) ? (sizeof(X) / sizeof((X)[0])) : -1)

const char* tabNames[] = { "Ragebot", "Visuals", "Misc", "Skins", "Configs" };

void CMenu::Menu() {

	ImGui::GetIO().MouseDrawCursor = menu_open;

	if (!menuVars.set)
		SetupVariables();

	CheckForResolutionChange();

	if (menu_open) {

		MenuAnimations();
		/*DrawTabsOnTop();
		DrawMenu();*/

		if (tabsOnTop) { //OLD MENU CODE WITH CHANGABLE TABS : TOP TABS LOOK BETTER
			DrawTabsOnTop();
			DrawMenu();
		}
		else {
			DrawTabBackground();
			DrawTabs();
			DrawMenu();
		}
	}else {
		menuVars.alpha = 0;
		tabMenuVars.position = Vector2D(-tabMenuVars.size.x, 0);
	}
}

void CMenu::DrawTabBackground() {
	TabStyle();

	ImGui::SetNextWindowPos(ImVec2(tabMenuVars.position.x, tabMenuVars.position.y));
	ImGui::SetNextWindowSize(ImVec2(tabMenuVars.size.x, tabMenuVars.size.y));

	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 0.411f, 0.705f, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

	ImGui::Begin("##tab_background", &menu_open, ImVec2(tabMenuVars.size.x, tabMenuVars.size.y), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
	{
		TabStyle();
		ImGui::SetWindowPos(ImVec2(tabMenuVars.position.x, tabMenuVars.position.y));
		ImGui::SetWindowSize(ImVec2(tabMenuVars.size.x, tabMenuVars.size.y));

		ImGui::BeginChild("##childdd", ImVec2(tabMenuVars.size.x*0.98, tabMenuVars.size.y));
		{
		}
		ImGui::EndChild();
	}
	ImGui::End();

}

void CMenu::DrawTabs() {
	TabStyle();


	int buttonHeight = tabMenuVars.size.y * 0.075;
	int childHeight = buttonHeight * array_size(tabNames); // * by number of tabs
	int positionY = (menuVars.screenSize.y - childHeight) / 2;

	ImGui::SetNextWindowPos(ImVec2(tabMenuVars.position.x, positionY));
	ImGui::SetNextWindowSize(ImVec2(tabMenuVars.size.x*0.98, childHeight));

	ImGui::Begin("##tab_buttons", &menu_open, ImVec2(tabMenuVars.size.x*0.98, tabMenuVars.size.y), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	{
		ImGui::SetWindowPos(ImVec2(tabMenuVars.position.x, positionY));
		ImGui::SetWindowSize(ImVec2(tabMenuVars.size.x*0.98, childHeight));

		ImGui::BeginChild("##tab_child", ImVec2(tabMenuVars.size.x, childHeight));
		{

			for (auto i = 0; i < array_size(tabNames); i++)
			{
				if (currentTab == i) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.09f, 0.12f, 1.00f));
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
					if (ImGui::Button(tabNames[i], ImVec2(tabMenuVars.size.x*0.98, buttonHeight))) {

					}
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					continue;
				}

				if (ImGui::Button(tabNames[i], ImVec2(tabMenuVars.size.x, buttonHeight))) {
					currentTab = i;
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void CMenu::DrawTabsOnTop() {
	TabStyle();

	int buttonHeight = tabMenuVars.size.y * 0.05;
	int buttonWidth = menuVars.size.x / array_size(tabNames);

	ImGui::SetNextWindowPos(ImVec2(menuVars.position.x, 0));
	ImGui::SetNextWindowSize(ImVec2(menuVars.size.x, buttonHeight));

	ImGui::Begin("##tab_buttons", &menu_open, ImVec2(menuVars.size.x, buttonHeight), menuVars.alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	{
		ImGui::SetWindowPos(ImVec2(menuVars.position.x, 0));
		ImGui::SetWindowSize(ImVec2(menuVars.size.x, buttonHeight));
		for (auto i = 0; i < array_size(tabNames); i++)
		{
			if (currentTab == i) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				if (ImGui::Button(tabNames[i], ImVec2(buttonWidth, buttonHeight))) {

				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				continue;
			}

			if (ImGui::Button(tabNames[i], ImVec2(buttonWidth, buttonHeight))) {
				currentTab = i;
			}
			ImGui::SameLine();
		}
	}
	ImGui::End();
}

void CMenu::DrawMenu() {
	MenuStyle();

	ImGui::SetNextWindowSize(ImVec2(menuVars.size.x, menuVars.size.y));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
	ImGui::Begin("Wanheda", &menu_open, ImVec2(menuVars.position.x, menuVars.position.y), menuVars.alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	{
		MenuStyle();
		ImGui::SetWindowSize(ImVec2(menuVars.size.x, menuVars.size.y));			

		if (menuVars.alpha >= 1.0f) { //only draw menu items when alpha is 1, fixes fade in bug
			if (currentTab == 0)
				RagebotTab();
			else if (currentTab == 1)
				VisualsTab();
			else if (currentTab == 2)
				MiscTab();
			else if (currentTab == 3)
				SkinsTab();
			else if (currentTab == 4)
				ConfigTab();
			else
				currentTab = 0; //incase of bug somewhere will reset tab
		}

		if (ImGui::GetWindowPos().x < tabMenuVars.size.x) {
			ImGui::SetWindowPos(ImVec2(tabMenuVars.size.x, ImGui::GetWindowPos().y));
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void CMenu::MenuAnimations() {
	// TAB ANIMATION
	if ((tabMenuVars.position.x += tabMenuVars.speed) > 0)
		tabMenuVars.position.x = 0;
	else
		tabMenuVars.position.x += tabMenuVars.speed;

	//MENU ALPHA ANIM
	if (menuVars.alpha < 1.0f) {
		menuVars.alpha += menuVars.alphaSpeed;
	}

	// RAINBOW ANIM FROM LEXIS RAINBOW SCOPE
	rainbow += 0.001;

	if (rainbow > 1.f)
		rainbow = 0.f;

	
}

void CMenu::MenuStyle() {
	ImGuiStyle style = ImGui::GetStyle();

	CColor color = CColor::FromHSB(rainbow, 1.f, 1.f);
	ImVec4 col = ImVec4{ (float)color.RGBA[0]/255, (float)color.RGBA[1]/255, (float)color.RGBA[2]/255, (float)color.RGBA[3]/255};

	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.FramePadding = ImVec2(5, 5);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;

	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.0f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.02f, 0.04f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.03f, 0.02f, 0.04f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(1.0f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	/*style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.0f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(1.0f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);*/

	ImGui::GetStyle() = style;
}

void CMenu::TabStyle() {
	ImGuiStyle style = ImGui::GetStyle();

	style.WindowPadding = ImVec2(0, 0);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.FramePadding = ImVec2(0, 0);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(0, 0);
	style.ItemInnerSpacing = ImVec2(0, 0);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;


	style.Colors[ImGuiCol_Text] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);

	ImGui::GetStyle() = style;
}

void CMenu::SetupVariables() {
	int sX, sY;
	g_csgo::Engine->GetScreenSize(sX, sY);

	menuVars.screenSize = Vector2D(sX, sY);
	menuVars.size = Vector2D(sX*menuVars.scale.x, sY*menuVars.scale.y);
	menuVars.position = Vector2D((sX - menuVars.size.x) / 2, (sY - menuVars.size.y) / 2);


	tabMenuVars.size = Vector2D(menuVars.screenSize.x * 0.15, menuVars.screenSize.y);
	tabMenuVars.position = Vector2D(-tabMenuVars.size.x, 0);

	menuVars.set = true;
}

void CMenu::CheckForResolutionChange() {
	int sX, sY;

	g_csgo::Engine->GetScreenSize(sX, sY);

	if (Vector2D(sX, sY) != menuVars.screenSize)
		SetupVariables();
}

/*MENU TABS*/

void CMenu::RagebotTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + (ImGui::GetStyle().ItemSpacing.x*2))) / 3;

	const char* hitboxes[] = { "Head", "Body Aim", "Hitscan" };
	ImGui::BeginChild("#aimbot_setting", ImVec2(childX, 0), true); {

		ImGui::Checkbox("Enable Aimbot", &settings::options.aim_bool);

		ImGui::Text("Hitboxes");
		ImGui::Combo("", &settings::options.acc_type, hitboxes, ARRAYSIZE(hitboxes));
		//ImGui::ListBoxHeader(""); {
		//ImGui::Selectable("head", &settings::options.hitscan_head);
		//ImGui::Selectable("neck", &settings::options.hitscan_neck);
		//ImGui::Selectable("chest", &settings::options.hitscan_chest);
		//ImGui::Selectable("pelvis", &settings::options.hitscan_pelvis);
		//ImGui::Selectable("arms", &settings::options.hitscan_arms);
		//ImGui::Selectable("legs", &settings::options.hitscan_legs);
		//}
		//ImGui::ListBoxFooter();
		//ImGui::Checkbox(("smart body-aim"), &settings::options.smartbaim);

		ImGui::SliderInt("Hit-Chance", &settings::options.chance_val, 0, 100);
		ImGui::SliderInt("Minimum Damage", &settings::options.damage_val, 1, 100);
		ImGui::Checkbox(("More Aimpoints"), &settings::options.multi_bool);
		if (settings::options.multi_bool)
		{
			ImGui::SliderFloat("Head Scale", &settings::options.point_val, 0, 1);
			ImGui::SliderFloat("Body Scale", &settings::options.body_val, 0, 1);
		}
		//ImGui::Combo("baim on key", &settings::options.baim_key, key_binds, sizeof(key_binds));
		ImGui::Checkbox(("Auto Stop"), &settings::options.stop_bool);
		ImGui::Checkbox(("Lag Compensation"), &settings::options.rage_lagcompensation);
		ImGui::Checkbox(("Fakelag Prediction"), &settings::options.fake_bool);
		ImGui::Checkbox("Fake Walk", &settings::options.fakewalk);
		ImGui::SliderInt("Fake Walk Speed", &settings::options.fakewalkspeed, 1, 14);
	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("#antiaims", ImVec2(childX, 0), true); {

		const char* antiaimmode[] = { "Standing", "Moving", "Jumping" ,"Fakewalking" };
		const char* aa_pitch[] = { "None", "Emotion", "Fake Down", "Fake Up", "Fake Zero" };
		const char* aa_mode[] = { "None", "Backwards", "Manual", "Freestanding" };
		const char* aa_fake[] = { "None", "Backjitter", "Random", "Local View", "Opposite", "Rotational" };

		ImGui::Checkbox(("Enable Hentai-Aim"), &settings::options.aa_bool);
		ImGui::Text("Mode");
		ImGui::Combo(("##mode"), &settings::options.aa_mode, antiaimmode, ARRAYSIZE(antiaimmode));

		switch (settings::options.aa_mode) {

		case 0: {

			ImGui::Text("Pitch");
			ImGui::Combo(("pitch##st"), &settings::options.aa_pitch_type, aa_pitch, ARRAYSIZE(aa_pitch));
			ImGui::Text("Real");
			ImGui::Combo(("real##st"), &settings::options.aa_real_type, aa_mode, ARRAYSIZE(aa_mode));
			//ImGui::Combo(("fake##st"), &settings::options.aa_fake_type, aa_fake, ARRAYSIZE(aa_fake));
			break;
		}

		case 1: {

			ImGui::Text("Pitch");
			ImGui::Combo(("pitch##moving"), &settings::options.aa_pitch1_type, aa_pitch, ARRAYSIZE(aa_pitch));
			ImGui::Text("Real");
			ImGui::Combo(("real##moving"), &settings::options.aa_real1_type, aa_mode, ARRAYSIZE(aa_mode));
			//ImGui::Combo(("fake##moving"), &settings::options.aa_fake1_type, aa_fake, ARRAYSIZE(aa_fake));
			break;
		}

		case 2: {

			ImGui::Text("Pitch");
			ImGui::Combo(("pitch##jumping"), &settings::options.aa_pitch2_type, aa_pitch, ARRAYSIZE(aa_pitch));
			ImGui::Text("Real");
			ImGui::Combo(("real##jumping"), &settings::options.aa_real2_type, aa_mode, ARRAYSIZE(aa_mode));
			//ImGui::Combo(("fake##jumping"), &settings::options.aa_fake2_type, aa_fake, ARRAYSIZE(aa_fake));
			break;
		}

		case 3: {
			ImGui::Text("Pitch");
			ImGui::Combo(("##fakewalking"), &settings::options.aa_pitch3_type, aa_pitch, ARRAYSIZE(aa_pitch));
			ImGui::Text("Real");
			ImGui::Combo(("##fakewalking"), &settings::options.aa_real3_type, aa_mode, ARRAYSIZE(aa_mode));

		}
		}
	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("#antiaimbuilder", ImVec2(childX, 0), true); {

		ImGui::Checkbox(("Hentai-Aim Arrows"), &settings::options.rifk_arrow);
		ImGui::Checkbox(("Desync Yaw"), &settings::options.desync_yaw);
		//ImGui::Combo(("Flip Key"), &settings::options.flip_int, key_binds, ARRAYSIZE(key_binds));
		switch (settings::options.aa_mode) {

			case 0: {
				if (settings::options.aa_real_type < 3) {
					ImGui::SliderInt("Real Additive", &settings::options.aa_realadditive_val, -180, 180);
					ImGui::SliderInt("LBY Delta", &settings::options.delta_val, -119, 119);
				}
				else {
					ImGui::SliderInt("Freestanding LBY Delta", &settings::options.delta_val, -119, 119);
					ImGui::SliderInt("Freestanding Spin Range", &settings::options.spinangle, 0.f, 360.f);
					ImGui::SliderInt("Spins Per Tick", &settings::options.spinspeed, 0.f, 60.f);
				}

				if (settings::options.aa_real_type == 2)
					ImGui::Text("Manual Flip Key");
					ImGui::Combo("##manual_flip_key", &settings::options.flip_int, key_binds, ARRAYSIZE(key_binds));
				break;
			}

			case 1: {
				if (settings::options.aa_real1_type < 3) {
					ImGui::SliderInt("Real Additive", &settings::options.aa_realadditive1_val, -180, 180);
					ImGui::SliderInt("LBY delta", &settings::options.delta1_val, -119, 119);
				}
				else {
					ImGui::SliderInt("Freestanding Spin Range", &settings::options.spinangle1, 0.f, 360.f);
					ImGui::SliderInt("Spins Per Tick", &settings::options.spinspeed1, 0.f, 60.f);
				}
				if (settings::options.aa_real1_type == 2)
					ImGui::Text("Manual Flip Key");
					ImGui::Combo("##manual_flip_key", &settings::options.flip_int, key_binds, ARRAYSIZE(key_binds));
				break;
			}

			case 2: {
				if (settings::options.aa_real2_type < 3) {
					ImGui::SliderInt("Real Additive", &settings::options.aa_realadditive2_val, -180, 180);
					ImGui::SliderInt("LBY Delta", &settings::options.delta2_val, -119, 119);
				}
				else {
					ImGui::SliderInt("Freestanding Spin Range", &settings::options.spinangle2, 0.f, 360.f);
					ImGui::SliderInt("Spins Per Tick", &settings::options.spinspeed2, 0.f, 60.f);
				}
				if (settings::options.aa_real2_type == 2)
					ImGui::Text("Manual Flip Key");
					ImGui::Combo("##manual_flip_key", &settings::options.flip_int, key_binds, ARRAYSIZE(key_binds));
				break;
			}

			case 3: {
				if (settings::options.aa_real3_type < 3) {
					ImGui::SliderInt("Real Additive", &settings::options.aa_realadditive3_val, -180, 180);
					ImGui::SliderInt("LBY Delta", &settings::options.delta3_val, -119, 119);
				}
				else {
					ImGui::SliderInt("Freestanding LBY delta", &settings::options.delta3_val, -119, 119);
					ImGui::SliderInt("Freestanding Spin Range", &settings::options.spinangle3, 0.f, 360.f);
					ImGui::SliderInt("Spins Per Tick", &settings::options.spinspeed3, 0.f, 60.f);
				}
				if (settings::options.aa_real3_type == 2)
					ImGui::Text("Manual Flip Key");
					ImGui::Combo("##manual_flip_key", &settings::options.flip_int, key_binds, ARRAYSIZE(key_binds));

					break;
			}
		} 
		ImGui::Spacing();

		/*switch (settings::options.aa_mode) {

			case 0: {
				ImGui::SliderInt("Standing Angle °", &settings::options.spinangle, 0, 180);
				ImGui::SliderInt("Standing Speed %", &settings::options.spinspeed, 0, 100);
				break;
			}
			case 1: {
				ImGui::SliderInt("Moving Angle °", &settings::options.spinangle1, 0, 180);
				ImGui::SliderInt("Moving Speed %", &settings::options.spinspeed1, 0, 100);
				break;
			}
			case 2: {
				ImGui::SliderInt("Jumping Angle °", &settings::options.spinangle2, 0, 180);
				ImGui::SliderInt("Jumping Speed %", &settings::options.spinspeed2, 0, 100);
				break;
			}
		}*/
	} 
	ImGui::EndChild();

}

void CMenu::VisualsTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + (ImGui::GetStyle().ItemSpacing.x * 2))) / 3;

	ImGui::BeginChild("#visuals", ImVec2(childX, 0), true); {

		const char* weptype[] = { "Type1", "Type2" };

		ImGui::Checkbox(("Enabled"), &settings::options.Visuals.Enabled);
		ImGui::Checkbox(("Box"), &settings::options.Visuals.Visuals_BoxEnabled);
		ImGui::Text("Box Type");
		ImGui::Combo(("##box Type"), &settings::options.Visuals.Visuals_BoxType, weptype, ARRAYSIZE(weptype));
		ImGui::Checkbox(("Esp team"), &settings::options.Visuals.Visuals_EspTeam);
		ImGui::Checkbox(("Name"), &settings::options.name_bool);
		ImGui::Checkbox(("Health"), &settings::options.Visuals.Visuals_HealthBar);
		ImGui::Text("Health Type");
		ImGui::Combo(("##health type"), &settings::options.Visuals.Visuals_HealthBarType, weptype, ARRAYSIZE(weptype));

		ImGui::Checkbox(("Aim Lines"), &settings::options.Visuals.Visuals_AimLines);
		ImGui::Checkbox(("Weapons"), &settings::options.weap_bool);
		ImGui::Text("Weapon Type");
		ImGui::Combo(("##weaponType"), &settings::options.Visuals.Visuals_WeaponsType, weptype, ARRAYSIZE(weptype));
		ImGui::Checkbox(("Ammo"), &settings::options.ammo_bool);
		ImGui::Text("Ammo Type");
		ImGui::Combo(("##ammoType"), &settings::options.Visuals.Visuals_AmmoESPType, weptype, ARRAYSIZE(weptype));

		ImGui::Checkbox(("Damage Esp"), &settings::options.Visuals.Visuals_DamageESP);
		ImGui::Checkbox(("POV Arrows"), &settings::options.fov_bool);

	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("#visuals2", ImVec2(childX, 0), true); {


		const char* chams_type[] = { "Metallic", "Material", "Flat" };
		const char* chams_mode[] = { "None", "Visible", "Visible + Invisible" };
		const char* glow_styles[] = { "Regular", "Pulsing", "Outline" };
		const char* local_chams[] = { "None", "Material", "Metallic" };

		ImGui::Text("Enemy Colored Models");
		ImGui::Combo(("##enemy coloured models"), &settings::options.chams_type, chams_mode, ARRAYSIZE(chams_mode));
		ImGui::Text("Team Colored Models");
		ImGui::Combo(("##team coloured models"), &settings::options.chamsteam, chams_mode, ARRAYSIZE(chams_mode));
		ImGui::Text("Chams Type");
		ImGui::Combo(("##chams type"), &settings::options.chamstype, chams_type, ARRAYSIZE(chams_type));
		ImGui::Text("Local Chams");
		ImGui::Combo(("##local chams"), &settings::options.localchams, local_chams, ARRAYSIZE(local_chams));

		ImGui::Checkbox(("Fake Chams"), &settings::options.fakechams);
		ImGui::Checkbox(("Enemy Glow Enable"), &settings::options.glowenable);
		ImGui::Checkbox(("Team Glow Enable"), &settings::options.glowteam);
		ImGui::Text("Glow Style");
		ImGui::Combo(("##glow style"), &settings::options.glowstyle, glow_styles, ARRAYSIZE(glow_styles));

		ImGui::Checkbox(("Local Glow"), &settings::options.glowlocal);
		ImGui::Text("Local Glow Style");
		ImGui::Combo(("##local glow style"), &settings::options.glowstylelocal, glow_styles, ARRAYSIZE(glow_styles));
	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("#visuals3", ImVec2(childX, 0), true); {

		const char* crosshair_select[] = { "None", "Static", "Recoil" };
		const char* hitmarker[] = { "None", "Gamesense", "Bameware", "Custom" };
		const char* spreadshit[] = { "Off", "Standart", "Another", "Rainbow" };
		const char* scope_lines[] = { "Rainbow Lines", "Black Lines" };

		ImGui::Checkbox("Night Mode", &settings::options.night_bool);
		ImGui::Checkbox(("Bullet Tracers"), &settings::options.beam_bool);
		//ImGui::SliderFloat("bullet life", &settings::options.bulletlife, 0, 30);
		//ImGui::SliderFloat("bullet size", &settings::options.bulletsize, 0, 20);

		ImGui::Checkbox(("Thirdperson"), &settings::options.tp_bool);
		ImGui::Text("Thirdperson Key");
		ImGui::Combo(("##thirdperson_key"), &settings::options.thirdperson_int, key_binds, ARRAYSIZE(key_binds));
		ImGui::SliderFloat(("Thirdperson Transparency"), &settings::options.transparency_amnt, 0, 1);
		ImGui::Text("Crosshair");
		ImGui::Combo(("##crosshair"), &settings::options.xhair_type, crosshair_select, ARRAYSIZE(crosshair_select));
		ImGui::Text("Spread Circle");
		ImGui::Combo(("##spread_circle"), &settings::options.spread_bool, spreadshit, ARRAYSIZE(spreadshit));

		ImGui::Checkbox(("Remove Smoke"), &settings::options.smoke_bool);
		ImGui::Checkbox(("Remove Flash"), &settings::options.no_flash);
		ImGui::Checkbox(("Remove Scope"), &settings::options.scope_bool);
		if (settings::options.scope_bool)
			ImGui::Combo(("Remove Scope"), &settings::options.scope_type, scope_lines, ARRAYSIZE(scope_lines));
		ImGui::Checkbox(("Remove Zoom"), &settings::options.removescoping);
		ImGui::Checkbox(("Fix Zoom Sensitivity"), &settings::options.fixscopesens);

		ImGui::Checkbox(("Enable Postprocessing"), &settings::options.matpostprocessenable);
		//ImGui::Hotkey(("flashlight key"), &settings::options.flashlightkey);
		ImGui::SliderFloat("Render Fov", &settings::options.fov_val, 0, 179);
		ImGui::SliderFloat("Viewmodel Fov", &settings::options.viewfov_val, 0, 179);

		ImGui::Checkbox(("LBY Indicator"), &settings::options.lbyenable);
	} ImGui::EndChild();
}

void CMenu::MiscTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;
	ImGui::BeginChild("##misc", ImVec2(childX, 0), true); {

		const char* fakelag_mode[] = { "Factor", "Adaptive" };

		ImGui::Checkbox(("Auto Bunnyhop"), &settings::options.bhop_bool);
		ImGui::Checkbox(("Auto Strafer"), &settings::options.strafe_bool);
		ImGui::Checkbox(("Duck in Air"), &settings::options.duck_bool);
		ImGui::Checkbox(("Animated Clantag"), &settings::options.misc_clantag);
		ImGui::Checkbox(("hit sound"), &settings::options.hitsound);
		ImGui::Checkbox(("full bright"), &settings::options.full_bright);
		ImGui::Checkbox(("preserve killfeed"), &settings::options.killfeed);
		//ImGui::SliderInt(("viewmodel x"), &settings::options.viewmodel_x, 1, 10);
		//ImGui::SliderInt(("viewmodel y"), &settings::options.viewmodel_y, 1, 10);
		//ImGui::SliderInt(("viewmodel z"), &settings::options.viewmodel_z, 1, 20);
		//ImGui::InputText("##clantag_text", clantag_name, 64);
		//if (ImGui::Button("force update clantag"))
		//	should_reset = true;
		//	else
		//	should_reset = false;

		//ImGui::Hotkey(("circle strafer"), &settings::options.circlestrafekey, key_binds, ARRAYSIZE(key_binds));
		ImGui::Checkbox(("Enable Fakelag"), &settings::options.lag_bool);
		ImGui::Text("Fakelag Type");
		ImGui::Combo(("##fakelag type"), &settings::options.lag_type, fakelag_mode, ARRAYSIZE(fakelag_mode));
		ImGui::SliderInt("Standing Lag", &settings::options.stand_lag, 1, 14);
		ImGui::SliderInt("Moving Lag", &settings::options.move_lag, 1, 14);
		ImGui::SliderInt("Jumping Lag", &settings::options.jump_lag, 1, 14);
		ImGui::SliderInt("Fakewalk Lag", &settings::options.fakewalk_lag, 1, 14);
	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("##misc2", ImVec2(childX, 0), true); {
		ImGui::Checkbox(("Menu Tabs on Top"), &tabsOnTop);
	} ImGui::EndChild();
}

void CMenu::SkinsTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;

	ImGui::BeginChild(("#skinchanger"), ImVec2(childX, 0), true, true ? ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar : ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders); {

		ImGui::Text("DO NOT CLICK FORCE UPDATE WITHOUT \nENABLING SKIN CHANGER!", CColor(255, 255, 255));

		ImGui::Checkbox("skin changer", &settings::options.skinenabled);
		ImGui::Checkbox("glove changer", &settings::options.glovesenabled);
		//ImGui::Checkbox("model changer", &settings::options.CUSTOMMODEL);
		/*if (settings::options.CUSTOMMODEL)
		{
		ImGui::Combo(("ct"), &settings::options.customct, playermodels, ARRAYSIZE(playermodels));
		ImGui::Combo(("t"), &settings::options.customtt, playermodels, ARRAYSIZE(playermodels));
		}*/
		if (ImGui::Button(("Force update")))
			KnifeApplyCallbk();

	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild(("#skinchanger_pages"), ImVec2(childX, 0), true, true ? ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar : ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders); {

		static bool Main = true;
		static bool Colors = false;

		static int page = 0;

		ImGuiStyle& style = ImGui::GetStyle();


		style.ItemSpacing = ImVec2(1, 1);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.f));

		const char* meme = "page : error";

		switch (page) {

		case 0: meme = "page : 1";  break;
		case 1: meme = "page : 2";  break;
		case 2: meme = "page : 3";  break;
		case 3: meme = "page : 4";  break;
		case 4: meme = "page : 5";  break;
		default: break;
		}

		ImGui::Text(meme); ImGui::SameLine(); ImGui::Text(("                  ")); ImGui::SameLine();
		if (ImGui::Button(("-"), ImVec2(22, 22))) {

			if (page != 0)
				page--;
		};

		ImGui::SameLine();

		if (ImGui::Button(("+"), ImVec2(22, 22))) {

			if (page != 3)
				page++;
		};

		ImGui::Text(("        "));

		ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();

		style.ItemSpacing = ImVec2(8, 4);

		switch (page) {

		case 0: {

			ImGui::PushItemWidth(150.0f);
			ImGui::Text("Knife Model");
			ImGui::Combo(("##knife model"), &settings::options.Knife, KnifeModel, ARRAYSIZE(KnifeModel));
			ImGui::PushItemWidth(150.0f);
			ImGui::Text("Knife Skin");
			ImGui::Combo(("##knife skin"), &settings::options.KnifeSkin, knifeskins, ARRAYSIZE(knifeskins));
			ImGui::PushItemWidth(150.0f);
			ImGui::Text("Glove Model");
			ImGui::Combo(("##glove model"), &settings::options.gloves, GloveModel, ARRAYSIZE(GloveModel));
			ImGui::PushItemWidth(150.0f);

			switch (settings::options.gloves) {

			case 0: {
				const char* glovesskins[] = { "" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 1: {
				const char* glovesskins[] = { "charred", "snakebite", "bronzed", "guerilla" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 2: {
				const char* glovesskins[] = { "hedge maze", "andoras box", "superconductor", "arid", "omega", "amphibious", "bronze morph" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 3: {
				const char* glovesskins[] = { "lunar weave", "convoy", "crimson weave", "diamondback", "overtake", "racing green", "king snake", "imperial plaid" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 4: {
				const char* glovesskins[] = { "leather", "spruce ddpat", "slaughter", "cobalt skulls", "overprint", "duct tape", "arboreal" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 5: {
				const char* glovesskins[] = { "eclipse", "spearmint", "boom", "cool mint", "turtle", "transport", "polygon", "pow" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 6: {
				const char* glovesskins[] = { "forest ddpat", "crimson kimono", "emerald web", "foundation", "crimson web", "buckshot", "fade", "mogul" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			case 7: {
				const char* glovesskins[] = { "emerald", "mangrove", "rattler", "case hardened" };
				ImGui::Text("Glove Skin");
				ImGui::Combo(("##glove skin"), &settings::options.skingloves, glovesskins, ARRAYSIZE(glovesskins));
				break;
			}
			}

		} break;
		case 1:
		{
			ImGui::Combo(("AK-47"), &settings::options.AK47Skin, ak47, ARRAYSIZE(ak47));
			ImGui::Combo(("M4A1-S"), &settings::options.M4A1SSkin, m4a1s, ARRAYSIZE(m4a1s));
			ImGui::Combo(("M4A4"), &settings::options.M4A4Skin, m4a4, ARRAYSIZE(m4a4));
			ImGui::Combo(("Galil AR"), &settings::options.GalilSkin, galil, ARRAYSIZE(galil));
			ImGui::Combo(("AUG"), &settings::options.AUGSkin, aug, ARRAYSIZE(aug));
			ImGui::Combo(("FAMAS"), &settings::options.FAMASSkin, famas, ARRAYSIZE(famas));
			ImGui::Combo(("SG-553"), &settings::options.Sg553Skin, sg553, ARRAYSIZE(sg553));
			ImGui::Combo(("UMP45"), &settings::options.UMP45Skin, ump45, ARRAYSIZE(ump45));
			ImGui::Combo(("MAC-10"), &settings::options.Mac10Skin, mac10, ARRAYSIZE(mac10));
			ImGui::Combo(("PP-Bizon"), &settings::options.BizonSkin, bizon, ARRAYSIZE(bizon));
			ImGui::Combo(("TEC-9"), &settings::options.tec9Skin, tec9, ARRAYSIZE(tec9));
			ImGui::Combo(("P2000"), &settings::options.P2000Skin, p2000, ARRAYSIZE(p2000));
			ImGui::Combo(("P250"), &settings::options.P250Skin, p250, ARRAYSIZE(p250));
			ImGui::Combo(("Dual-Barettas"), &settings::options.DualSkin, dual, ARRAYSIZE(dual));
			ImGui::Combo(("CZ75-Auto"), &settings::options.Cz75Skin, cz75, ARRAYSIZE(cz75));
			ImGui::Combo(("Nova"), &settings::options.NovaSkin, nova, ARRAYSIZE(nova));


		} break;
		case 2:
		{
			ImGui::Combo(("XM1014"), &settings::options.XmSkin, xm, ARRAYSIZE(xm));
			ImGui::Combo(("AWP"), &settings::options.AWPSkin, awp, ARRAYSIZE(awp));
			ImGui::Combo(("SSG08"), &settings::options.SSG08Skin, ssg08, ARRAYSIZE(ssg08));
			ImGui::Combo(("SCAR20"), &settings::options.SCAR20Skin, scar20, ARRAYSIZE(scar20));
			ImGui::Combo(("G3SG1"), &settings::options.G3sg1Skin, g3sg1, ARRAYSIZE(g3sg1));
			ImGui::Combo(("MP9"), &settings::options.Mp9Skin, mp9, ARRAYSIZE(mp9));
			ImGui::Combo(("Glock-18"), &settings::options.GlockSkin, glock, ARRAYSIZE(glock));
			ImGui::Combo(("USP-S"), &settings::options.USPSkin, usp, ARRAYSIZE(usp));
			ImGui::Combo(("Desert Eagle"), &settings::options.DeagleSkin, deagle, ARRAYSIZE(deagle));
			ImGui::Combo(("Five-Seven"), &settings::options.FiveSkin, five, ARRAYSIZE(five));
			ImGui::Combo(("Revolver"), &settings::options.RevolverSkin, revolver, ARRAYSIZE(revolver));
			ImGui::Combo(("Negev"), &settings::options.NegevSkin, negev, ARRAYSIZE(negev));
			ImGui::Combo(("M249"), &settings::options.M249Skin, m249, ARRAYSIZE(m249));
			ImGui::Combo(("SAWED-Off"), &settings::options.SawedSkin, sawed, ARRAYSIZE(sawed));
			ImGui::Combo(("Mag-7"), &settings::options.MagSkin, mag, ARRAYSIZE(mag));
		} break;
		default:break;
		}
	} ImGui::EndChild();
}

void CMenu::ConfigTab() {

	auto childX = (menuVars.size.x - ((ImGui::GetStyle().WindowPadding.x * 2) + ImGui::GetStyle().ItemSpacing.x)) / 2;

	ImGui::BeginChild("#configs", ImVec2(childX, 0), true); {
		const char* configFiles[] =
		{
			" Legit 1" ,
			" Legit 2" ,
			" Rage 1" ,
			" Rage 2" ,
			" HvH 1" ,
			" HvH 2" ,
			" HvH_Scout 1" ,
			" HvH_Scout 2" ,
			" HvH_Auto 1" ,
			" HvH_Auto 2" ,
			" HvH_Pistol 1" ,
			" HvH_Pistol 2"
		};
		ImGui::Combo(("File"), &settings::options.newconfigs, configFiles, ARRAYSIZE(configFiles));
		ImGui::Spacing();
		if (ImGui::Button("Save Config", ImVec2(100, 25)))
			Config->Save();
		if (ImGui::Button("Load Config", ImVec2(100, 25)))
			Config->Load();

		/*ImGui::InputText("##configname", ConfigNamexd, 64);
		static int sel;
		std::string config;

		std::vector<std::string> configs = settings::options.GetConfigs();

		if (configs.size() > 0) {

		ImGui::ComboBoxArrayxd("configs", &sel, configs);
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::PushItemWidth(220.f);
		config = configs[settings::options.config_sel];
		} settings::options.config_sel = sel;

		if (configs.size() > 0) {

		if (ImGui::Button("load", ImVec2(50, 20))) {

		settings::options.Load(config);
		g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
		g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "loaded config: %s\n", config.c_str());
		}
		} ImGui::SameLine();

		if (configs.size() >= 1) {

		if (ImGui::Button("save", ImVec2(50, 20))) {

		settings::options.Save(config);
		g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
		g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "saved config: %s\n", config.c_str());
		}
		} ImGui::SameLine();

		if (ImGui::Button("create", ImVec2(50, 20))) {

		std::string ConfigFileName = ConfigNamexd;

		if (ConfigFileName.size() < 1)
		ConfigFileName = "settings";

		settings::options.CreateConfig(ConfigFileName);
		g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
		g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "created config: %s\n", ConfigFileName.c_str());
		} ImGui::SameLine();

		if (config.size() >= 1) {

		if (ImGui::Button("delete", ImVec2(50, 20))) {

		settings::options.Remove(config);
		g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
		g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "removed config: %s\n", config.c_str());
		}
		}*/
	} ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("#colors", ImVec2(childX, 0), true); {

		ImGui::Text(("box colors"));
		ImGui::ColorEdit4(("ct Visible##box"), settings::options.Visuals.BoxColorPickCTVIS, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("ct invisible##box"), settings::options.Visuals.BoxColorPickCTINVIS, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("t Visible##box"), settings::options.Visuals.BoxColorPickTVIS, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("t invisible##box"), settings::options.Visuals.BoxColorPickTINVIS, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);


		ImGui::Text(("chams"));
		ImGui::ColorEdit4(("enemy visible##chams"), settings::options.vmodel_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("enemy invisible##chams"), settings::options.imodel_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("team visible##chams"), settings::options.teamvis_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("team invisible##chams"), settings::options.teaminvis_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("local##chams"), settings::options.localchams_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);


		ImGui::Text(("glow"));
		ImGui::ColorEdit4(("enemy##glow"), settings::options.glow_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("team##glow"), settings::options.teamglow_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("local##glow"), settings::options.glowlocal_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::Text(("bullet tracers"));

		ImGui::ColorEdit4(("local##tracer"), settings::options.bulletlocal_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("enemy##tracer"), settings::options.bulletenemy_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("team##tracer"), settings::options.bulletteam_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);

		ImGui::Text(("other"));
		ImGui::ColorEdit4(("damage esp##DamageEspColor"), settings::options.Visuals.DamageESPColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		//ImGui::ColorEdit4(("grenade prediction##xd"), settings::options.grenadepredline_col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		ImGui::ColorEdit4(("spread circle##xd"), settings::options.spreadcirclecol, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
		//ImGui::ColorEdit4(("fake chams##xd"), settings::options.fakechamscol, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar);
	} ImGui::EndChild();
}

