#include "../includes.h"
#include "NetvarHookManager.h"
#include "offsets.h"
#include "../SDK/ConVar.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
namespace OFFSETS
{
	uintptr_t m_iHealth;
	uintptr_t m_fFlags;
	uintptr_t m_vecVelocity;
	uintptr_t m_flLowerBodyYawTarget;
	uintptr_t deadflag;
	uintptr_t m_vecOrigin;
	uintptr_t m_flDuckAmount;
	uintptr_t m_iTeamNum;
	uintptr_t m_nTickBase;
	uintptr_t m_bDormant;
	uintptr_t animstate;
	uintptr_t m_Collision;
	uintptr_t m_angEyeAngles;
	uintptr_t m_flSimulationTime;
	uintptr_t m_vecViewOffset;
	uintptr_t m_dwBoneMatrix;
	uintptr_t m_aimPunchAngle;
	uintptr_t m_bGunGameImmunity;
	uintptr_t m_nForceBone;
	uintptr_t m_flPoseParameter;
	uintptr_t dwGlowObjectManager;
	uintptr_t m_flNextPrimaryAttack;
	uintptr_t m_flNextAttack;
	uintptr_t m_hActiveWeapon;
	uintptr_t m_ArmorValue;
	uintptr_t m_bHasHelmet;
	uintptr_t m_iObserverMode;
	uintptr_t m_bIsScoped;
	uintptr_t m_iAccount;
	uintptr_t m_iPlayerC4;
	uintptr_t dwPlayerResource;
	uintptr_t m_iItemDefinitionIndex;
	uintptr_t m_lifeState;
	uintptr_t m_flPostponeFireReadyTime;
	uintptr_t m_fThrowTime;
	uintptr_t m_bPinPulled;
	uintptr_t m_MoveType;
	uintptr_t m_viewPunchAngle;
	uintptr_t m_iItemIDHigh;
	uintptr_t m_iEntityQuality;
	uintptr_t m_iAccountID;
	uintptr_t m_flFallbackWear;
	uintptr_t m_nFallbackSeed;
	uintptr_t m_nFallbackStatTrak;
	uintptr_t m_nFallbackPaintKit;
	uintptr_t m_hMyWearables;
	uintptr_t m_nModelIndex;
	uintptr_t m_hOwner;
	uintptr_t m_iViewModelIndex;
	uintptr_t m_iWorldModelIndex;
	uintptr_t m_hWeaponWorldModel;
	uintptr_t m_Item;
	uintptr_t m_OriginalOwnerXuidLow;
	uintptr_t m_OriginalOwnerXuidHigh;
	uintptr_t m_iShotsFired;
	uintptr_t invalidateBoneCache;
	uintptr_t m_iClip1;
	uintptr_t m_hMyWeapons;
	uintptr_t animlayer;
	uintptr_t get_ragdoll_pos;
	uintptr_t m_bClientSideAnimation;
	uintptr_t m_iMatchStats_PlayersAlive_T;
	uintptr_t m_iMatchStats_PlayersAlive_CT;


	void InitOffsets()
	{
	
		UTILS::netvar_hook_manager.Initialize();
	//	g_csgo::Engine->ExecuteClientCmd("clear");
		m_iItemIDHigh = UTILS::netvar_hook_manager.GetOffset(("DT_BaseAttributableItem"), ("m_iItemIDHigh"));
		m_iEntityQuality = UTILS::netvar_hook_manager.GetOffset("DT_BaseAttributableItem", "m_iEntityQuality");
		m_Item = UTILS::netvar_hook_manager.GetOffset("DT_BaseAttributableItem", "m_Item");
		m_OriginalOwnerXuidLow = UTILS::netvar_hook_manager.GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
		m_OriginalOwnerXuidHigh = UTILS::netvar_hook_manager.GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
		m_iAccountID = UTILS::netvar_hook_manager.GetOffset("DT_BaseCombatWeapon", "m_iAccountID");
		m_flFallbackWear = UTILS::netvar_hook_manager.GetOffset("DT_BaseAttributableItem", "m_flFallbackWear");
		m_nFallbackSeed = UTILS::netvar_hook_manager.GetOffset("DT_BaseAttributableItem", "m_nFallbackSeed");
		m_nFallbackStatTrak = UTILS::netvar_hook_manager.GetOffset("DT_BaseAttributableItem", "m_nFallbackStatTrak");
		m_nFallbackPaintKit = UTILS::netvar_hook_manager.GetOffset("DT_BaseAttributableItem", "m_nFallbackPaintKit");
		m_hMyWearables = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_hMyWearables"));
		m_nModelIndex = UTILS::netvar_hook_manager.GetOffset(("DT_BaseViewModel"), ("m_nModelIndex"));
		m_hOwner = UTILS::netvar_hook_manager.GetOffset(("DT_BaseViewModel"), ("m_hOwner"));
		m_iViewModelIndex = UTILS::netvar_hook_manager.GetOffset("DT_BaseCombatWeapon", "m_iViewModelIndex");
		m_iWorldModelIndex = UTILS::netvar_hook_manager.GetOffset("DT_BaseCombatWeapon", "m_iWorldModelIndex");
		m_hWeaponWorldModel = UTILS::netvar_hook_manager.GetOffset("DT_BaseCombatWeapon", "m_hWeaponWorldModel");
		m_iShotsFired = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_iShotsFired"));
		invalidateBoneCache = (DWORD)UTILS::FindSignature("client_panorama.dll", "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");
		m_iHealth = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_iHealth"));;
		m_fFlags = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_fFlags")); 
		m_vecVelocity = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_vecVelocity[0]"));
		m_flLowerBodyYawTarget = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_flLowerBodyYawTarget"));
		m_vecOrigin = UTILS::netvar_hook_manager.GetOffset(("DT_BaseEntity"), ("m_vecOrigin"));  
		m_iTeamNum = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_iTeamNum"));
		m_flDuckAmount = UTILS::netvar_hook_manager.GetOffset(("CCSPlayer"), ("m_flDuckAmount"));
		m_nTickBase = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_nTickBase"));
		m_Collision = UTILS::netvar_hook_manager.GetOffset(("DT_BaseEntity"), ("m_Collision"));
		m_angEyeAngles = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_angEyeAngles[0]"));
		m_flSimulationTime = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_flSimulationTime"));
		m_vecViewOffset = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_vecViewOffset[0]")); 
		m_aimPunchAngle = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_aimPunchAngle")); 
		m_bGunGameImmunity = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_bGunGameImmunity")); 
		m_nForceBone = UTILS::netvar_hook_manager.GetOffset(("DT_BaseAnimating"), ("m_nForceBone")); 
		m_flPoseParameter = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_flPoseParameter"));
		dwGlowObjectManager = UTILS::FindSignature("client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01" + 0x3);
		m_flNextPrimaryAttack = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCombatWeapon"), ("m_flNextPrimaryAttack")); 
		m_flNextAttack = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCombatCharacter"), ("m_flNextAttack"));
		m_hActiveWeapon = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_hActiveWeapon")); 
		m_ArmorValue = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_ArmorValue"));  
		m_bHasHelmet = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_bHasHelmet")); 
		m_iObserverMode = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_iObserverMode")); 
		m_bIsScoped = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_bIsScoped")); 
		m_iAccount = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_iAccount")); 
		m_iItemDefinitionIndex = UTILS::netvar_hook_manager.GetOffset(("DT_BaseAttributableItem"), ("m_iItemDefinitionIndex")); 
		m_lifeState = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_lifeState")); 
		m_flPostponeFireReadyTime = UTILS::netvar_hook_manager.GetOffset(("DT_WeaponCSBaseGun"), ("m_flPostponeFireReadyTime")); 
		m_fThrowTime = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCSGrenade"), ("m_fThrowTime"));
		m_viewPunchAngle = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_viewPunchAngle")); 
		m_iClip1 = UTILS::netvar_hook_manager.GetOffset("DT_BaseCombatWeapon", "m_iClip1");
		get_ragdoll_pos = UTILS::netvar_hook_manager.GetOffset("DT_Ragdoll", "m_ragPos");
		m_bClientSideAnimation = UTILS::netvar_hook_manager.GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
		m_iMatchStats_PlayersAlive_T = UTILS::netvar_hook_manager.GetOffset("DT_CSGameRulesProxy", "m_iMatchStats_PlayersAlive_T");
		m_iMatchStats_PlayersAlive_CT = UTILS::netvar_hook_manager.GetOffset("DT_CSGameRulesProxy", "m_iMatchStats_PlayersAlive_CT");
		m_bDormant = 0xED;
		animstate = 0x3900;
		m_dwBoneMatrix = 0x26A8;
		dwPlayerResource = 0x30F2F7C;
		m_MoveType = 0x25C;
		animlayer = 0x2980;
		m_hMyWeapons = 0x2DF8;
	}
}