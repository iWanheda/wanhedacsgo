#include "../includes.h"

#include "general_utils.h"

#include "interfaces.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CUserCmd.h"
#include "../UTILS/render.h"
#include "../SDK/CTrace.h"
#include "../cheats/visuals/player_esp.h"

#include <corecrt.h>
#include <iosfwd>
namespace UTILS
{
	HANDLE _out = NULL;

	bool ConsolePrint(bool logToFile, const char *fmt, ...)
	{
		char buf[1024] = { 0 };
		va_list va;

		va_start(va, fmt);
		_vsnprintf_s(buf, 1024, fmt, va);
		va_end(va);

		std::string outRes = buf;

		OutputDebugString(outRes.c_str());

		

		if (!_out)
			return false;

		return !!WriteConsoleA(_out, outRes.c_str(), static_cast<DWORD>(strlen(outRes.c_str())), nullptr, nullptr);
	}

	Vector TraceToEnd(Vector start, Vector end)
	{
		SDK::trace_t trace;
		SDK::CTraceWorldOnly filter;
		SDK::Ray_t ray;

		ray.Init(start, end);
		g_csgo::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

		return trace.end;
	}

	const char* GetHitgroupName(int hitgroup)
	{
		switch (hitgroup)
		{
		case 1:
			return "HEAD";
		case 2:
			return "CHEST";
		case 3:
			return "PELVIS";
		case 4:
			return "LEFT ARM";
		case 5:
			return "RIGHT ARM";
		case 6:
			return "LEFT LEG";
		case 7:
			return "RIGHT LEG";
		default:
			return "";
		}
	}
	bool is_point_in_range(Vector2D point, Vector2D position, Vector2D size)
	{
		return (point.x >= position.x && point.x <= position.x + size.x)
			&& (point.y >= position.y && point.y <= position.y + size.y);
	}
	float GetFraction(float value, float min, float max)
	{
		float ratio = 1.f / (max - min);
		return ratio * (value - min);
	}
	float GetValueFromFraction(float fraction, float min, float max)
	{
		return ((max - min) * fraction) + min;
	}
	unsigned int GetNumberOfDigits(int i)
	{
		i = i < 0 ? -(i * 10) : i;
		return i > 0 ? (int)log10((double)i) + 1 : 1;
	}
	float RoundToDecimalPlaces(float value, int decimals)
	{
		float yeet = pow(10.f, decimals);
		return roundf(value * yeet) / yeet;
	}
	std::string FloatToString(float value, int decimals) // this shit is ugly as fOK
	{
		std::string ret = std::to_string(value);

		ret.erase(ret.begin() + clamp<unsigned int>(GetNumberOfDigits(value)
			+ decimals + 1, 0, ret.size()), ret.end());

		if (decimals <= 0)
			ret.pop_back();

		return ret;
	}
	Vector CalcAngle(Vector src, Vector dst)
	{
		Vector ret;
		MATH::VectorAngles(dst - src, ret);
		return ret;
	}

	bool IsOnScreen(Vector origin, Vector& screen)
	{
		if (!RENDER::WorldToScreen(origin, screen)) return false;
		int iScreenWidth, iScreenHeight;
		g_csgo::Engine->GetScreenSize(iScreenWidth, iScreenHeight);
		bool xOk = iScreenWidth > screen.x > 0, yOk = iScreenHeight > screen.y > 0;
		return xOk && yOk;
	}
	float GetCurtime()
	{
		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (!local_player)
			return -1;

		return static_cast<float>(local_player->GetTickBase()) * g_csgo::Globals->interval_per_tick;
	}
	float Lerp(float fraction, float min, float max)
	{
		return min + (fraction * (max - min));
	}
	uint64_t FindSignature(const char* szModule, const char* szSignature)
	{
		MODULEINFO modInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
		DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
		DWORD endAddress = startAddress + modInfo.SizeOfImage;
		const char* pat = szSignature;
		DWORD firstMatch = 0;
		for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;    //one ?
			}
			else {
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}
	DWORD WaitOnModuleHandle(std::string moduleName)
	{
		DWORD ModuleHandle = NULL;
		while (!ModuleHandle)
		{
			ModuleHandle = (DWORD)GetModuleHandle(moduleName.c_str());
			if (!ModuleHandle)
				Sleep(50);
		}
		return ModuleHandle;
	}
	bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask)
	{
		for (; *szMask; ++szMask, ++Mask, ++Data)
		{
			if (*szMask == 'x' && *Mask != *Data)
			{
				return false;
			}
		}
		return (*szMask) == 0;
	}
	DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask)
	{
		DWORD Address = WaitOnModuleHandle(moduleName.c_str());
		MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
		DWORD Length = ModInfo.SizeOfImage;
		for (DWORD c = 0; c < Length; c += 1)
		{
			if (bCompare((BYTE*)(Address + c), Mask, szMask))
			{
				return (DWORD)(Address + c);
			}
		}
		std::cout << "FAIL LMAO" << std::endl;
		return 0;
	}

	void NormalizeLemon(Vector& angle)
	{
		while (angle.x > 89.0f) {
			angle.x -= 180.f;
		}
		while (angle.x < -89.0f) {
			angle.x += 180.f;
		}
		while (angle.y > 180.f) {
			angle.y -= 360.f;
		}
		while (angle.y < -180.f) {
			angle.y += 360.f;
		}
	}

	void ClampAnglesLemon(Vector &angles)
	{
		if (angles.y > 180.0f)
			angles.y = 180.0f;
		else if (angles.y < -180.0f)
			angles.y = -180.0f;

		if (angles.x > 89.0f)
			angles.x = 89.0f;
		else if (angles.x < -89.0f)
			angles.x = -89.0f;

		angles.z = 0;
	}

	bool ClampLemon(Vector &angles)
	{
		Vector a = angles;
		NormalizeLemon(a);
		ClampAnglesLemon(a);

		if (isnan(a.x) || isinf(a.x) ||
			isnan(a.y) || isinf(a.y) ||
			isnan(a.z) || isinf(a.z)) {
			return false;
		}
		else {
			angles = a;
			return true;
		}
	}
	void SetClanTag(const char* tag, const char* name)
	{
		static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(reinterpret_cast<uintptr_t>(GetModuleHandleA("engine.dll")) + 0x87060);
		pSetClanTag(tag, name);
	}
}