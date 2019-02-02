#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CPrediction.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/EnginePred.h"

float flOldCurtime;
float flOldFrametime;

void CEnginePred::run_prediction(SDK::CUserCmd* cmd)
{
	static int flTickBase;
	static SDK::CUserCmd* pLastCmd;

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player) 
		return;

	if (pLastCmd) {

		if (pLastCmd->hasbeenpredicted)
			flTickBase = local_player->GetTickBase();
		else
			++flTickBase;
	}

	const auto getRandomSeed = [&]() {
		
		using MD5_PseudoRandomFn = unsigned long(__cdecl*)(std::uintptr_t);
		static auto offset = UTILS::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 70 6A 58");
		static auto MD5_PseudoRandom = reinterpret_cast<MD5_PseudoRandomFn>(offset);
		return MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
	};


	pLastCmd = cmd;
	flOldCurtime = g_csgo::Globals->curtime;
	flOldFrametime = g_csgo::Globals->frametime;
	SDK::CGV::uRandomSeed = getRandomSeed();
	g_csgo::Globals->curtime = flTickBase * g_csgo::Globals->interval_per_tick;
	g_csgo::Globals->frametime = g_csgo::Globals->interval_per_tick;

	g_csgo::pMovement->StartTrackPredictionErrors(local_player);

	SDK::CMoveData data;
	memset(&data, 0, sizeof(SDK::CMoveData));

	g_csgo::pMoveHelper->set_host(local_player);
	g_csgo::pPrediction->SetupMove(local_player, cmd, g_csgo::pMoveHelper, &data);
	g_csgo::pMovement->ProcessMovement(local_player, &data);
	g_csgo::pPrediction->FinishMove(local_player, cmd, &data);
}

void CEnginePred::end_prediction(SDK::CUserCmd * cmd) {

	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!local_player) 
		return

	g_csgo::pMovement->FinishTrackPredictionErrors(local_player); 
	g_csgo::pMoveHelper->set_host(nullptr);

	g_csgo::Globals->curtime = flOldCurtime;
	g_csgo::Globals->frametime = flOldFrametime;
}

CEnginePred* prediction = new CEnginePred();