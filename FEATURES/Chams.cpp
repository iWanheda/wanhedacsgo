#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IMaterial.h"
#include "../FEATURES/Chams.h"
#include "../UTILS/render.h"
#include "../SDK/IVDebugOverlay.h"

void CChams::InitKeyValues(SDK::KeyValues* kv_, std::string name_)
{
	static auto address = UTILS::FindSignature("client_panorama.dll", "55 8B EC 51 33 C0 C7 45");
	using Fn = void(__thiscall*)(void* thisptr, const char* name);
	reinterpret_cast< Fn >(address)(kv_, name_.c_str());
}

void CChams::LoadFromBuffer(SDK::KeyValues* vk_, std::string name_, std::string buffer_)
{
	static auto address = UTILS::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04");
	using Fn = void(__thiscall*)(
		void* thisptr, const char* resourceName,
		const char* pBuffer, void* pFileSystem,
		const char* pPathID, void* pfnEvaluateSymbolProc);

	reinterpret_cast< Fn >(address)(
		vk_, name_.c_str(), buffer_.c_str(), nullptr, nullptr, nullptr);
}

SDK::IMaterial* CChams::CreateMaterialMetallic(bool ignorez, bool lit, bool wireframe)
{
	static auto created = 0;

	std::string type = lit ? "VertexLitGeneric" : "UnlitGeneric";
	auto matdata =
		"\"" + type + "\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"env_cubemap\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$normalmapalphaenvmapmask\" \"1\"\
		\n\t\"$envmapcontrast\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"" + std::to_string(ignorez) + "\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"" + std::to_string(wireframe) + "\"\
		\n}\n";

	auto matname = "custom_" + std::to_string(created);
	++created;

	auto keyValues = static_cast< SDK::KeyValues* >(malloc(sizeof(SDK::KeyValues)));

	InitKeyValues(keyValues, type.c_str());
	LoadFromBuffer(keyValues, matname, matdata);

	auto material =
		g_csgo::MaterialSystem->CreateMaterial(matname.c_str(), keyValues);

	material->IncrementReferenceCount();
	return material;
}

SDK::IMaterial* CChams::CreateMaterialBasic(bool ignorez, bool lit, bool wireframe)
{
	static auto created = 0;

	std::string type = lit ? "VertexLitGeneric" : "UnlitGeneric";
	auto matdata =
		"\"" + type + "\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"" + std::to_string(ignorez) + "\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"" + std::to_string(wireframe) + "\"\
		\n}\n";

	auto matname = "custom_" + std::to_string(created);
	++created;

	auto keyValues = static_cast< SDK::KeyValues* >(malloc(sizeof(SDK::KeyValues)));

	InitKeyValues(keyValues, type.c_str());
	LoadFromBuffer(keyValues, matname, matdata);

	auto material =
		g_csgo::MaterialSystem->CreateMaterial(matname.c_str(), keyValues);

	material->IncrementReferenceCount();
	return material;
}

CChams* chams = new CChams();