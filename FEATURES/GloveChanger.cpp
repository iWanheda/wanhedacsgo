
#include "..\includes.h"
#include "../UTILS/interfaces.h"
#include "../UTILS/offsets.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/CInput.h"
#include "../SDK/IClient.h"
#include "../SDK/CPanel.h"
#include "../UTILS/render.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlowObjectManager.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/RenderView.h"
#include "../SDK/CTrace.h"
#include "../SDK/CViewSetup.h"
#include "../SDK/CGlobalVars.h"
#include "../UTILS/NetvarHookManager.h"
#include "../SDK/CBaseEntity.h"

#include "../SDK/RecvData.h"

#include "GloveChanger.h"



//typedef unsigned long CBaseHandle;
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

void GloveChanger()
{

	if (settings::options.glovesenabled)
	{
		SDK::player_info_t localPlayerInfo;

		SDK::CBaseEntity *pLocal = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
		if (pLocal)
		{
			if (g_csgo::Engine->GetPlayerInfo(g_csgo::Engine->GetLocalPlayer(), &localPlayerInfo))
			{
				DWORD* hMyWearables = (DWORD*)((size_t)pLocal + OFFSETS::m_hMyWearables);

				if (hMyWearables)
				{
					if (!g_csgo::ClientEntityList->GetClientEntityFromHandle((DWORD)hMyWearables[0]))
					{
						static SDK::ClientClass* pClass;

						if (!pClass)
							pClass = g_csgo::Client->GetAllClasses();
						while (pClass)
						{
							if (pClass->m_ClassID == 52)
								break;
							pClass = pClass->m_pNext;
						}

						int iEntry = g_csgo::ClientEntityList->GetHighestEntityIndex() + 1;
						int iSerial = RandomInt(0x0, 0xFFF);

						pClass->m_pCreateFn(iEntry, iSerial);
						hMyWearables[0] = iEntry | (iSerial << 16);

						SDK::CBaseEntity* pEnt = g_csgo::ClientEntityList->GetClientEntityFromHandle((DWORD)hMyWearables[0]);

						if (pEnt)
						{
							int modelindex = 0;

							if (settings::options.gloves == 1)
								modelindex = g_csgo::ModelInfo->GetModelIndex(("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
							else if (settings::options.gloves == 2)
								modelindex = g_csgo::ModelInfo->GetModelIndex(("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
							else if (settings::options.gloves == 3)
								modelindex = g_csgo::ModelInfo->GetModelIndex(("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
							else if (settings::options.gloves == 4)
								modelindex = g_csgo::ModelInfo->GetModelIndex(("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
							else if (settings::options.gloves == 5)
								modelindex = g_csgo::ModelInfo->GetModelIndex(("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
							else if (settings::options.gloves == 6)
								modelindex = g_csgo::ModelInfo->GetModelIndex(("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
							else if (settings::options.gloves == 7)
								modelindex = g_csgo::ModelInfo->GetModelIndex(("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"));

							int ItemDefinitionIndex;
							if (settings::options.gloves == 1)
							{
								ItemDefinitionIndex = 5027;
							}
							else if (settings::options.gloves == 2)
							{
								ItemDefinitionIndex = 5030;
							}
							else if (settings::options.gloves == 3)
							{
								ItemDefinitionIndex = 5031;
							}
							else if (settings::options.gloves == 4)
							{
								ItemDefinitionIndex = 5032;
							}
							else if (settings::options.gloves == 5)
							{
								ItemDefinitionIndex = 5033;
							}
							else if (settings::options.gloves == 6)
							{
								ItemDefinitionIndex = 5034;
							}
							else if (settings::options.gloves == 7)
							{
								ItemDefinitionIndex = 5035;
							}
							else
								ItemDefinitionIndex = 0;

							int paintkit;
							if (settings::options.gloves == 1)
							{
								switch (settings::options.skingloves)
								{
								case 0:
									paintkit = 10006;
									break;
								case 1:
									paintkit = 10007;
									break;
								case 2:
									paintkit = 10008;
									break;
								case 3:
									paintkit = 10039;
									break;
								default:
									paintkit = 0; break;
								}
							}
							else if (settings::options.gloves == 2)
							{
								switch (settings::options.skingloves)
								{
								case 0:
									paintkit = 10038; break;
								case 1:
									paintkit = 10037; break;
								case 2:
									paintkit = 10018; break;
								case 3:
									paintkit = 10019; break;
								case 4:
									paintkit = 10048; break;
								case 5:
									paintkit = 10047; break;
								case 6:
									paintkit = 10045; break;
								case 7:
									paintkit = 10046; break;
								default:
									paintkit = 0; break;
								}
							}
							else if (settings::options.gloves == 3)
							{
								switch (settings::options.skingloves)
								{
								case 0:
									paintkit = 10013; break;
								case 1:
									paintkit = 10015; break;
								case 2:
									paintkit = 10016; break;
								case 3:
									paintkit = 10040; break;
								case 4:
									paintkit = 10043; break;
								case 5:
									paintkit = 10044; break;
								case 6:
									paintkit = 10041; break;
								case 7:
									paintkit = 10042; break;
								default:
									paintkit = 0; break;
								}
							}
							else if (settings::options.gloves == 4)
							{
								switch (settings::options.skingloves)
								{
								case 0:
									paintkit = 10009; break;
								case 1:
									paintkit = 10010; break;
								case 2:
									paintkit = 10021; break;
								case 3:
									paintkit = 10036; break;
								case 4:
									paintkit = 10053; break;
								case 5:
									paintkit = 10054; break;
								case 6:
									paintkit = 10055; break;
								case 7:
									paintkit = 10056; break;
								default:
									paintkit = 0; break;
								}
							}
							else if (settings::options.gloves == 5)
							{
								switch (settings::options.skingloves)
								{
								case 0:
									paintkit = 10024; break;
								case 1:
									paintkit = 10026; break;
								case 2:
									paintkit = 10027; break;
								case 3:
									paintkit = 10028; break;
								case 4:
									paintkit = 10050; break;
								case 5:
									paintkit = 10051; break;
								case 6:
									paintkit = 10052; break;
								case 7:
									paintkit = 10049; break;
								default:
									paintkit = 0; break;
								}
							}
							else if (settings::options.gloves == 6)
							{
								switch (settings::options.skingloves)
								{
								case 0:
									paintkit = 10030; break;
								case 1:
									paintkit = 10033; break;
								case 2:
									paintkit = 10034; break;
								case 3:
									paintkit = 10035; break;
								case 4:
									paintkit = 10061; break;
								case 5:
									paintkit = 10062; break;
								case 6:
									paintkit = 10063; break;
								case 7:
									paintkit = 10064; break;
								default:
									paintkit = 0; break;
								}
							}
							else if (settings::options.gloves == 7)
							{
								switch (settings::options.skingloves)
								{
								case 0:
									paintkit = 10057; break;
								case 1:
									paintkit = 10058; break;
								case 2:
									paintkit = 10059; break;
								case 3:
									paintkit = 10060; break;
								default:
									paintkit = 0; break;
								}
							}
							*(int*)((DWORD)pEnt + OFFSETS::m_iItemDefinitionIndex) = ItemDefinitionIndex;
							*(int*)((DWORD)pEnt + OFFSETS::m_iItemIDHigh) = -1; //0x2FB0
							*(int*)((DWORD)pEnt + OFFSETS::m_iEntityQuality) = 4;
							*(int*)((DWORD)pEnt + OFFSETS::m_iAccountID) = localPlayerInfo.xuid_low;
							*(float*)((DWORD)pEnt + OFFSETS::m_flFallbackWear) = settings::options.glovewear;
							*(int*)((DWORD)pEnt + OFFSETS::m_nFallbackSeed) = 0;
							*(int*)((DWORD)pEnt + OFFSETS::m_nFallbackStatTrak) = -1;
							*(int*)((DWORD)pEnt + OFFSETS::m_nFallbackPaintKit) = paintkit;

							pEnt->SetModelIndex(modelindex);
							pEnt->PreDataUpdate(DATA_UPDATE_CREATEDxd);
						}
					}
				}

			}
		}
	}
}
