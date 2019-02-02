#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "..\SDK\IEngineSound.h"
#include "..\SDK\IMaterial.h"

namespace SDK
{
	class IMatRenderContext;
	class Ray_t;
	class ITraceFilter;

	struct CViewSetup;
	struct DrawModelState_t;
	struct ModelRenderInfo_t;
	struct trace_t;
}

namespace HOOKS
{
	typedef bool(__stdcall* CreateMoveFn)(float , SDK::CUserCmd*);
	typedef void(__thiscall* FrameStageNotifyFn)(void*, int);
	typedef void(__thiscall* PaintFn)(void*, void*, int);
	typedef void(__thiscall* PaintTraverseFn)(void*, unsigned int, bool, bool);
	typedef void(__thiscall* DrawModelExecuteFn)(void*, SDK::IMatRenderContext*, const SDK::DrawModelState_t&, const SDK::ModelRenderInfo_t&, matrix3x4_t*);
	typedef void(__thiscall* SceneEndFn)(void*);
	typedef void(__thiscall* TraceRayFn)(void *thisptr, const SDK::Ray_t &ray, unsigned int fMask, SDK::ITraceFilter* pTraceFilter, SDK::trace_t *pTrace);
	typedef void(__thiscall* OverrideViewFn)(void* ecx, SDK::CViewSetup* pSetup);
	typedef void(__thiscall* RenderViewFn)(void*, SDK::CViewSetup&, SDK::CViewSetup&, unsigned int nClearFlags, int whatToDraw);
	typedef int(__thiscall* SendDatagramFn)(void*, void*);
	typedef bool(__thiscall* SvCheatsGetBoolFn)(void*);
	typedef float(__thiscall* GetViewmodelFOVFn)();
	typedef SDK::IMaterial*(__fastcall* FindMaterialFn)(void*, const char*, const char*, bool, const char*);
	typedef void(*EmitSound1Fn) (void*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int);
	typedef void(*EmitSound2Fn) (void*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, int, soundlevel_t, int, int, const Vector*, const Vector*, void*, bool, float, int);

	typedef long(__stdcall *EndSceneFn)(IDirect3DDevice9* device);
	typedef long(__stdcall *EndSceneResetFn)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	typedef long(__stdcall *hooked_presentFn)(IDirect3DDevice9* device, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion);

	extern CreateMoveFn original_create_move;
	extern PaintFn original_paint;
	extern PaintTraverseFn original_paint_traverse;
	extern FrameStageNotifyFn original_frame_stage_notify;
	extern DrawModelExecuteFn original_draw_model_execute;
	extern SceneEndFn original_scene_end;
	extern TraceRayFn original_trace_ray;
	extern SendDatagramFn original_send_datagram;
	extern OverrideViewFn original_override_view;
	extern RenderViewFn original_render_view;
	extern SvCheatsGetBoolFn original_get_bool;
	extern GetViewmodelFOVFn original_viewmodel_fov;
	extern FindMaterialFn original_find_material;

	extern VMT::VMTHookManager iclient_hook_manager;
	extern VMT::VMTHookManager panel_hook_manager;
	extern VMT::VMTHookManager paint_hook_manager;
	extern VMT::VMTHookManager model_render_hook_manager;
	extern VMT::VMTHookManager scene_end_hook_manager;
	extern VMT::VMTHookManager render_view_hook_manager;
	extern VMT::VMTHookManager trace_hook_manager;
	extern VMT::VMTHookManager net_channel_hook_manager;
	extern VMT::VMTHookManager override_view_hook_manager;
	extern VMT::VMTHookManager input_table_manager;
	extern VMT::VMTHookManager get_bool_manager;
	extern VMT::VMTHookManager sound_hook_manager;
	extern VMT::VMTHookManager find_material_hook_manager;

	void InitHooks();

	// virtual function hooks
	bool __stdcall   HookedCreateMove(float sample_input_frametime, SDK::CUserCmd* cmd);
	void __fastcall  HookedFrameStageNotify(void*, void*, int);
	void __stdcall   HookedPaintTraverse(int VGUIPanel, bool ForceRepaint, bool AllowForce);
	void __fastcall  HookedDrawModelExecute(void*, void*, SDK::IMatRenderContext*, const SDK::DrawModelState_t&, const SDK::ModelRenderInfo_t&, matrix3x4_t*);
	void __fastcall  HookedSceneEnd(void*, void*);
	void __fastcall  HookedTraceRay(void *thisptr, void*, const SDK::Ray_t &ray, unsigned int fMask, SDK::ITraceFilter *pTraceFilter, SDK::trace_t *pTrace);
	void __fastcall  HookedSendDatagram(void* ecx, void* data);
	void __fastcall  HookedOverrideView(void* ecx, void* edx, SDK::CViewSetup* pSetup);
	void __fastcall  HookedRenderView(void* thisptr, void*, SDK::CViewSetup& setup, SDK::CViewSetup& hudViewSetup, unsigned int nClearFlags, int whatToDraw);
	bool __fastcall  HookedGetBool(void* pConVar, void* edx);
	SDK::IMaterial* __fastcall hooked_findmaterial(void* thisptr, void*, const char* pMaterialName, const char* pTextureGroupName, bool complain, const char* pComplainPrefix);
	float __fastcall GetViewmodelFOV();
	void EmitSound1(void* thisptr, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity);
	void EmitSound2(void* thisptr, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, soundlevel_t iSoundLevel, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity);
	void hookKnife();
	// netvar hooks
	void InitNetvarHooks();
}
namespace RecVarShit
{
	void hookKnife();
}

class CMenu
{
public:
	void Menu();

private:
	void DrawTabBackground();
	void DrawTabs();
	void DrawTabsOnTop();
	void DrawMenu();

	void MenuStyle();
	void TabStyle();
	void SetupVariables();
	void CheckForResolutionChange();

	void RagebotTab();
	void VisualsTab();
	void MiscTab();
	void SkinsTab();
	void ConfigTab();

	void MenuAnimations();


	/*MENU VARS*/

	int currentTab = 0;
	int menuTabStyle = 0;

	float rainbow = 0.0f;



	struct menuVars {
		//this determines the size of the menu
		Vector2D scale = Vector2D(0.4, 0.5);

		Vector2D screenSize;
		Vector2D size;
		Vector2D position;

		float alpha = 0.0f;
		float alphaSpeed = 0.02f;
		bool set = false;
	}menuVars;

	struct tabMenuVars {
		Vector2D size;
		Vector2D position;
		float speed = 5.0f;
	}tabMenuVars;

}; 
extern CMenu* g_Menu;


extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);