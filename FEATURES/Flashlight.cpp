#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/Flashlight.h"
#include "../SDK/IMemAlloc.h"

CFlashLight Flashlight;
CFlashLightEffect* CFlashLight::CreateFlashLight(int nEntIndex, const char *pszTextureName, float flFov, float flFarZ, float flLinearAtten)
{
	static DWORD oConstructor = UTILS::FindSignature("client_panorama.dll", "55 8B EC F3 0F 10 45 ? B8");

	// we need to use the engine memory management if we are calling the destructor later
	CFlashLightEffect *pFlashLight = new CFlashLightEffect; //(CFlashLightEffect*)) g_csgo::MemAlloc->Alloc(sizeof(CFlashLightEffect));

	if (!pFlashLight)
		return NULL;

	// we need to call this function on a non standard way
	__asm
	{
		movss xmm3, flFov
		mov ecx, pFlashLight
		push flLinearAtten
		push flFarZ
		push pszTextureName
		push nEntIndex
		call oConstructor
	}

	return pFlashLight;
}

void CFlashLight::DestroyFlashLight(CFlashLightEffect *pFlashLight)
{
	static DWORD oDestructor = UTILS::FindSignature("client_panorama.dll", "56 8B F1 E8 ? ? ? ? 8B 4E 28");
	__asm
	{
		mov ecx, pFlashLight
		push ecx
		call oDestructor
	}
}

void CFlashLight::UpdateFlashLight(CFlashLightEffect *pFlashLight, const Vector &vecPos, const Vector &vecForward, const Vector &vecRight, const Vector &vecUp)
{
	// here we tell to the compiler wich calling convention we will use to call the function and the paramaters needed (note the __thiscall*)
	typedef void(__thiscall* UpdateLight_t)(void*, int, const Vector&, const Vector&, const Vector&, const Vector&, float, float, float, bool, const char*);

	static UpdateLight_t oUpdateLight = NULL;

	if (!oUpdateLight)
	{
		DWORD callInstruction = UTILS::FindSignature("client_panorama.dll", "E8 ? ? ? ? 8B 06 F3 0F 10 46"); // get the instruction address
		DWORD relativeAddress = *(DWORD*)(callInstruction + 1); // read the rel32
		DWORD nextInstruction = callInstruction + 5; // get the address of next instruction
		oUpdateLight = (UpdateLight_t)(nextInstruction + relativeAddress); // our function address will be nextInstruction + relativeAddress
	}

	oUpdateLight(pFlashLight, pFlashLight->m_nEntIndex, vecPos, vecForward, vecRight, vecUp, pFlashLight->m_flFov, pFlashLight->m_flFarZ, pFlashLight->m_flLinearAtten, pFlashLight->m_bCastsShadows, pFlashLight->m_szTextureName);
}

void CFlashLight::RunFrame()
{
	static CFlashLightEffect *pFlashLight = NULL;
	static float x = 0;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (UTILS::INPUT::input_handler.GetKeyState(UTILS::INPUT::input_handler.keyBindings(settings::options.flashlightkey)) & 1)
	{
		if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame())
		{
			if (!pFlashLight)
				pFlashLight = CreateFlashLight(local_player->GetIndex(), "effects/flashlight001", 35, 1000, 1000);
			else
				pFlashLight = NULL;
		}
		else
			pFlashLight = NULL;
	}

	if (pFlashLight)
	{
		Vector f, r, u;
		Vector viewAngles;

		g_csgo::Engine->GetViewAngles(viewAngles);
		MATH::AngleVectors2(viewAngles, &f, &r, &u);

		pFlashLight->m_bIsOn = true;
		pFlashLight->m_bCastsShadows = false;
		pFlashLight->m_flFov = 35;//fabsf(13 + 37 * cosf(x += 0.002f));
		UpdateFlashLight(pFlashLight, local_player->GetEyePosition(), f, r, u);
	}
}