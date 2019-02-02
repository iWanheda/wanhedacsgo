#include "includes.h"

#include "UTILS\interfaces.h"
#include "HOOKS\hooks.h"
#include "UTILS\offsets.h"
#include "FEATURES\EventListener.h"
#include "SDK\RecvData.h"
#include "SDK\CClientEntityList.h"
#include "SDK\IEngine.h"
#include "FEATURES/recv.h"
#include "FEATURES/NewEventLog.h"
#include <chrono>
#include <thread>
#include <map>
#include "external dependencies/steam api/isteamfriends.h"
#include "SDK/ConVar.h"
#include "SDK/IClient.h"
#include <string>
#include <sstream>

using namespace std::literals::chrono_literals;

bool using_fake_angles[65];
bool full_choke;
bool is_shooting;

bool in_tp;
bool fake_walk;

int resolve_type[65];

int target;
int shots_fired[65];
int shots_hit[65];
int shots_missed[65];
bool didMiss = true;
bool didShot = false;
int backtrack_missed[65];

float tick_to_back[65];
float lby_to_back[65];
bool backtrack_tick[65];

float lby_delta;
float update_time[65];
float walking_time[65];

float local_update;

int hitmarker_time;
int random_number;

bool menu_hide;

int oldest_tick[65];
float compensate[65][12];
Vector backtrack_hitbox[65][20][12];
float backtrack_simtime[65][12];
bool fake = false;

HMODULE get_module_handle(wchar_t* name) {

	static std::map<wchar_t*, HMODULE> modules = { };

	if (modules.count(name) == 0)
		modules[name] = GetModuleHandleW(name);

	return modules[name];
}

ISteamFriends* SteamFriends() {

	static ISteamFriends* result;

	if (!result) {

		auto steam_api = get_module_handle(L"steam_api.dll");
		HSteamUser hSteamUser = ((HSteamUser(__cdecl*)(void))GetProcAddress(steam_api, "SteamAPI_GetHSteamUser"))();
		HSteamPipe hSteamPipe = ((HSteamPipe(__cdecl*)(void))GetProcAddress(steam_api, "SteamAPI_GetHSteamPipe"))();
		ISteamClient* g_SteamClient = ((ISteamClient*(__cdecl*)(void))GetProcAddress(steam_api, "SteamClient"))();
		result = g_SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
	} return result;
}

// CRASH LIST

/*
0x4E414542 - Don't know yet

Exception thrown at 0x5A8B925B (vstdlib.dll) in csgo.exe: 0xC0000005: Access violation reading location 0xCC00443A.

Exception thrown at 0x2E55842B (client_panorama.dll) in csgo.exe: 0xC5: Access violation reading location 0x24.

*/

class netvar_manager {

public:
	std::string DumpTable(SDK::RecvTable * table, int depth);
	void DumpOffsets(std::string fileName);
};

std::string netvar_manager::DumpTable(SDK::RecvTable* table, int depth) {

	std::string pre("");
	std::stringstream ss;

	for (int i = 0; i < depth; i++)
		pre.append("\t");

	ss << pre << table->m_pNetTableName << "\n";

	for (int i = 0; i < table->m_nProps; i++) {

		SDK::RecvProp* prop = &table->m_pProps[i];
		if (!prop)
			continue;

		std::string varName(prop->m_pVarName);

		if (!(varName.find("baseclass") || varName.find("0") || varName.find("1") || varName.find("2")))
			continue;

		ss << pre << "\t" << varName << " [0x" << std::hex << prop->m_Offset << "]\n";

		if (prop->m_pDataTable)
			ss << DumpTable(prop->m_pDataTable, depth + 1);
	}

	ss << std::endl;

	return ss.str();
}

void netvar_manager::DumpOffsets(std::string fileName) {

	std::ofstream output(fileName);
	output.clear();
	std::stringstream ss;

	for (SDK::ClientClass* pClass = g_csgo::Client->GetAllClasses(); pClass != NULL; pClass = pClass->m_pNext) {

		SDK::RecvTable* table = pClass->m_pRecvTable;
		ss << this->DumpTable(table, false);
	}

	output << "[WANHEDA OFFSET DUMPER] \n\n Last checked: " << __TIME__ << "\n\n";
	output << ss.str();
	output.close();

	UTILS::ConsolePrint("[Wanheda] Dumped offsets to: (%s)", fileName.c_str());
}

void patchLogs() {

	//if (!g_csgo::Engine->Con_BoneIsVisible()) //seems to cause crashes, so commented it. Use it if you want
		//g_csgo::Engine->ExecuteClientCmd("toggleconsole");

	//g_csgo::Engine->ExecuteClientCmd("clear");

	/*std::string playerName = SteamFriends()->GetPersonaName();

	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "\n[Wanheda] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Welcome back, ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(20, 255, 20), playerName.c_str()); /// Welcome back, lexi
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "\n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "This cheat was reworked in C++ by ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "lexi ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "& ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "SkymeR \n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Menu made by ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "IcePixelx ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "if you want one make sure to ask for a ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "paid ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "request with ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "@IcePixelx#4931 ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "on our Discord. \n\n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Discord Link:");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), " https://discord.gg/b6nnV48 \n\n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "If you are running into trouble or crashes please tag us with ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(33, 202, 218), "@SUPPORT ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "on Discord to get our attention. \n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "[Wanheda] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Use the ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 20, 255), "INSERT ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "key to open the menu! (NOTE: make sure to be tabbed in the game when you inject to prevent crashes (use windowed fullscreen when you inject)) \n\n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(215, 210, 120), "Changelog: \n\n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(170, 55, 70), " + [RAGEBOT] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Added fakelag prediction \n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Bug fixes and improvements \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Added animated clantag \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Remade animfix \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Remade fakewalk \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed standing fakelag breaking when using adaptive fakelag mode \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Standing fakelag now disables on lby break \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed crashes when using rainbow spread crosshair \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed auto-stop \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(15, 10, 20), " + [MISC] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed some crashes on injection \n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(220, 145, 30), " ~ [ANTI-AIM] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Completely remade antiaims and freestanding \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(220, 145, 30), " ~ [ANTI-AIM] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed LBY breaker \n");

	g_csgo::cvar->ConsoleColorPrintf(CColor(90, 245, 90), " + [VISUALS] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Remade thirdperson \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(90, 245, 90), " + [VISUALS] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Fixed antiaim arrows \n");
	g_csgo::cvar->ConsoleColorPrintf(CColor(90, 245, 90), " + [VISUALS] ");
	g_csgo::cvar->ConsoleColorPrintf(CColor(255, 255, 255), "Made LBY indicator infallible \n");*/
}

void Start() {

	g_csgo::InitInterfaces();
	OFFSETS::InitOffsets();
	NetvarHook();
	UTILS::INPUT::input_handler.Init();
	FONTS::InitFonts();
	HOOKS::InitHooks();
	HOOKS::InitNetvarHooks();
	FEATURES::MISC::InitializeEventListeners();
	patchLogs();

	netvar_manager offsets;

	offsets.DumpOffsets("offsets.txt");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {

	if (dwReason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Start, NULL, NULL, NULL);

	return true;
}