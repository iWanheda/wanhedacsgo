#pragma once

#include "../UTILS/offsets.h"
#include "../UTILS/interfaces.h"


namespace SDK
{

	enum definition_index : int {
		WEAPON_NONE = 0,
		WEAPON_DEAGLE,
		WEAPON_ELITE,
		WEAPON_FIVESEVEN,
		WEAPON_GLOCK,
		WEAPON_AK47 = 7,
		WEAPON_AUG,
		WEAPON_AWP,
		WEAPON_FAMAS,
		WEAPON_G3SG1,
		WEAPON_GALILAR = 13,
		WEAPON_M249,
		WEAPON_M4A1 = 16,
		WEAPON_MAC10,
		WEAPON_P90 = 19,
		WEAPON_MP5SD = 23,
		WEAPON_UMP45,
		WEAPON_XM1014,
		WEAPON_BIZON,
		WEAPON_MAG7,
		WEAPON_NEGEV,
		WEAPON_SAWEDOFF,
		WEAPON_TEC9,
		WEAPON_TASER,
		WEAPON_HKP2000,
		WEAPON_MP7,
		WEAPON_MP9,
		WEAPON_NOVA,
		WEAPON_P250,
		WEAPON_SCAR20 = 38,
		WEAPON_SG553,
		WEAPON_SSG08,
		WEAPON_KNIFEGG,
		WEAPON_KNIFE,
		WEAPON_FLASHBANG,
		WEAPON_HEGRENADE,
		WEAPON_SMOKEGRENADE,
		WEAPON_MOLOTOV,
		WEAPON_DECOY,
		WEAPON_INCGRENADE,
		WEAPON_C4,
		WEAPON_HEALTHSHOT = 57,
		WEAPON_KNIFE_T = 59,
		WEAPON_M4A1_SILENCER,
		WEAPON_USP_SILENCER,
		WEAPON_CZ75A = 63,
		WEAPON_REVOLVER,
		WEAPON_TAGRENADE = 68,
		WEAPON_FISTS,
		WEAPON_BREACHCHARGE,
		WEAPON_TABLET = 72,
		WEAPON_MELEE = 74,
		WEAPON_AXE,
		WEAPON_HAMMER,
		WEAPON_SPANNER = 78,
		WEAPON_KNIFE_GHOST = 80,
		WEAPON_FIREBOMB,
		WEAPON_DIVERSION,
		WEAPON_FRAG_GRENADE,
		WEAPON_KNIFE_BAYONET = 500,
		WEAPON_KNIFE_FLIP = 505,
		WEAPON_KNIFE_GUT,
		WEAPON_KNIFE_KARAMBIT,
		WEAPON_KNIFE_M9_BAYONET,
		WEAPON_KNIFE_TACTICAL,
		WEAPON_KNIFE_FALCHION = 512,
		WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
		WEAPON_KNIFE_BUTTERFLY,
		WEAPON_KNIFE_PUSH,
		WEAPON_KNIFE_URSUS = 519,
		WEAPON_KNIFE_GYPSY_JACKKNIFE,
		WEAPON_KNIFE_STILETTO = 522,
		WEAPON_KNIFE_WIDOWMAKER
	};


	class CSWeaponInfo
	{
	public:
		char _0x0000[20];
		__int32 max_clip;			//0x0014 
		char _0x0018[12];
		__int32 max_reserved_ammo;	//0x0024 
		char _0x0028[96];
		char* hud_name;				//0x0088 
		char* weapon_name;			//0x008C 
		char _0x0090[60];
		__int32 m_WeaponType;				//0x00CC 
		__int32 price;				//0x00D0 
		__int32 reward;				//0x00D4 
		char _0x00D8[20];
		BYTE full_auto;				//0x00EC 
		char _0x00ED[3];
		__int32 m_Damage;				//0x00F0 
		float m_ArmorRatio;			//0x00F4 
		__int32 bullets;			//0x00F8 
		float m_Penetration;			//0x00FC 
		char _0x0100[8];
		float m_Range;				//0x0108 
		float m_RangeModifier;		//0x010C 
		char _0x0110[16];
		BYTE m_HasSilencer;				//0x0120 
		char _0x0121[15];
		float max_speed;			//0x0130 
		float max_speed_alt;		//0x0134 
		char _0x0138[76];
		__int32 recoil_seed;		//0x0184 
		char _0x0188[32];
	};

	struct Weapon_Info
	{
		char _0x0000[20];
		__int32 max_clip;			//0x0014 
		char _0x0018[12];
		__int32 max_reserved_ammo;	//0x0024 
		char _0x0028[96];
		char* hud_name;				//0x0088 
		char* weapon_name;			//0x008C 
		char _0x0090[60];
		__int32 m_WeaponType;				//0x00CC 
		__int32 price;				//0x00D0 
		__int32 reward;				//0x00D4 
		char _0x00D8[20];
		BYTE full_auto;				//0x00EC 
		char _0x00ED[3];
		__int32 m_Damage;				//0x00F0 
		float m_ArmorRatio;			//0x00F4 
		__int32 bullets;			//0x00F8 
		float m_Penetration;			//0x00FC 
		char _0x0100[8];
		float m_Range;				//0x0108 
		float m_RangeModifier;		//0x010C 
		char _0x0110[16];
		BYTE m_HasSilencer;				//0x0120 
		char _0x0121[15];
		float max_speed;			//0x0130 
		float max_speed_alt;		//0x0134 
		char _0x0138[76];
		__int32 recoil_seed;		//0x0184 
		char _0x0188[32];
	};

	
	class CBaseWeapon
	{
	public:
		inline int* FallbackPaintKit() {
			return (int*)((DWORD)this + OFFSETS::m_nFallbackPaintKit);
		}
		HANDLE m_hWeaponWorldModel()
		{
			return *(HANDLE*)((uintptr_t)this + OFFSETS::m_hWeaponWorldModel);
		}
		int* GetEntityQuality()
		{
			return (int*)((uintptr_t)this + OFFSETS::m_iEntityQuality);
		}
		int* OwnerXuidLow()
		{
			return (int*)((uintptr_t)this + OFFSETS::m_OriginalOwnerXuidLow);
		}
		float* FallbackWear()
		{
			return (float*)((uintptr_t)this + OFFSETS::m_flFallbackWear);
		}
		int* ItemIDHigh()
		{
			return (int*)((uintptr_t)this + OFFSETS::m_iItemIDHigh);
		}
		int* OwnerXuidHigh()
		{
			return (int*)((uintptr_t)this + OFFSETS::m_OriginalOwnerXuidHigh);
		}
		int* ModelIndex()
		{
			return (int*)((uintptr_t)this + OFFSETS::m_nModelIndex);
		}
		short* fixskins()
		{
			return (short*)((uintptr_t)this + OFFSETS::m_iItemDefinitionIndex);
		}
		int* ViewModelIndex()
		{
			return (int*)((uintptr_t)this + OFFSETS::m_iViewModelIndex);
		}
		float GetNextPrimaryAttack()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flNextPrimaryAttack);
		}
		short GetItemDefenitionIndex()
		{
			return *reinterpret_cast<short*>(uintptr_t(this) + OFFSETS::m_iItemDefinitionIndex);
		}
		float GetPostponeFireReadyTime()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_flPostponeFireReadyTime);
		}
		float GetSpreadCone()
		{
			typedef float(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 439)(this);
		}
		float GetInaccuracy()
		{
			typedef float(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 471)(this); //469
		}
		HANDLE GetOwnerHandle()
		{
			return *(HANDLE*)((uintptr_t)this + 0x031D0);
		}
		Vector GetOrigin()
		{
			return *(Vector*)((uintptr_t)this + OFFSETS::m_vecOrigin);
		}
		bool is_revolver()
		{
			int iWeaponID = this->GetItemDefenitionIndex();
			return (iWeaponID == 64);
		}

		bool is_knife()
		{
			int iWeaponID = this->GetItemDefenitionIndex();
			return (
				iWeaponID	 == 42 
				|| iWeaponID == 59 
				|| iWeaponID == 41
				|| iWeaponID == 500 
				|| iWeaponID == 505 
				|| iWeaponID == 506
				|| iWeaponID == 507 
				|| iWeaponID == 508 
				|| iWeaponID == 509
				|| iWeaponID == 515
				|| iWeaponID == 523
				|| iWeaponID == 522
				|| iWeaponID == 520
				|| iWeaponID == 519
				|| iWeaponID == 516);
		}
		
		char* get_name() {

			switch (definition_index()) {

			case WEAPON_KNIFE_BAYONET:
				return "BAYONET";
			case WEAPON_KNIFE_SURVIVAL_BOWIE:
				return "BOWIE";
			case WEAPON_KNIFE_BUTTERFLY:
				return "BUTTERFLY";
			case WEAPON_KNIFE:
				return "KNIFE";
			case WEAPON_KNIFE_FALCHION:
				return "FALCHION";
			case WEAPON_KNIFE_FLIP:
				return "FLIP";
			case WEAPON_KNIFE_GUT:
				return "GUT";
			case WEAPON_KNIFE_KARAMBIT:
				return "KARAMBIT";
			case WEAPON_KNIFE_M9_BAYONET:
				return "M9 BAYONET";
			case WEAPON_KNIFE_T:
				return "KNIFE";
			case WEAPON_KNIFE_TACTICAL:
				return "HUNTSMAN";
			case WEAPON_KNIFE_PUSH:
				return "SHADDOW DAGGERS";
			case WEAPON_DEAGLE:
				return "DESERT EAGLE";
			case WEAPON_ELITE:
				return "DUAL BERRETAS";
			case WEAPON_FIVESEVEN:
				return "FIVE SEVEN";
			case WEAPON_GLOCK:
				return "GLOCK";
			case WEAPON_HKP2000:
				return "P2000";
			case WEAPON_P250:
				return "P250";
			case WEAPON_USP_SILENCER:
				return "USP-S";
			case WEAPON_TEC9:
				return "TEC9";
			case WEAPON_REVOLVER:
				return "REVOLVER";
			case WEAPON_MAC10:
				return "MAC10";
			case WEAPON_UMP45:
				return "UMP45";
			case WEAPON_BIZON:
				return "BIZON";
			case WEAPON_MP7:
				return "MP7";
			case WEAPON_MP5SD:
				return "MP5";
			case WEAPON_MP9:
				return "MP9";
			case WEAPON_P90:
				return "P90";
			case WEAPON_GALILAR:
				return "GALIL AR";
			case WEAPON_FAMAS:
				return "FAMAS";
			case WEAPON_M4A1_SILENCER:
				return "M4A1-S";
			case WEAPON_M4A1:
				return "M4A1";
			case WEAPON_AUG:
				return "AUG";
			case WEAPON_SG553:
				return "SG556";
			case WEAPON_AK47:
				return "AK47";
			case WEAPON_G3SG1:
				return "G3SG1";
			case WEAPON_SCAR20:
				return "SCAR20";
			case WEAPON_AWP:
				return "AWP";
			case WEAPON_SSG08:
				return "SCOUT";
			case WEAPON_XM1014:
				return "XM1014";
			case WEAPON_SAWEDOFF:
				return "SAWED OFF";
			case WEAPON_MAG7:
				return "MAG7";
			case WEAPON_NOVA:
				return "NOVA";
			case WEAPON_NEGEV:
				return "NEGEV";
			case WEAPON_M249:
				return "M249";
			case WEAPON_TASER:
				return "ZEUS";
			case WEAPON_FLASHBANG:
				return "FLASH";
			case WEAPON_HEGRENADE:
				return "FRAG";
			case WEAPON_SMOKEGRENADE:
				return "SMOKE";
			case WEAPON_MOLOTOV:
				return "MOLOTOV";
			case WEAPON_DECOY:
				return "DECOY";
			case WEAPON_INCGRENADE:
				return "INCENDIARY";
			case WEAPON_C4:
				return "BOMB";
			case WEAPON_CZ75A:
				return "CZ75A";
			default:
				return "  ";
			}
		}

		/*bool is_non_aim() {
			int idx = definition_index();

			return (idx == WEAPON_C4 
				|| idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY || idx == WEAPON_KNIFE_FALCHION
				|| idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_GUT || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_M9_BAYONET || idx == WEAPON_KNIFE_PUSH
				|| idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_TACTICAL || idx == WEAPON_FLASHBANG || idx == WEAPON_HEGRENADE
				|| idx == WEAPON_SMOKEGRENADE || idx == WEAPON_MOLOTOV || idx == WEAPON_DECOY || idx == WEAPON_INCGRENADE);
		}*/

		bool is_grenade()
		{
			if (!this)
				return false;

			int WeaponId = this->GetItemDefenitionIndex();

			if (!WeaponId)
				return false;

			if (WeaponId == 43 || WeaponId == 44 || WeaponId == 45 || WeaponId == 46 || WeaponId == 47 || WeaponId == 48)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		int GetLoadedAmmo() 
		{
			return *(int*)((DWORD)this + OFFSETS::m_iClip1);
		}
		void UpdateAccuracyPenalty()
		{
			typedef void(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 472)(this); //470
		}
		Weapon_Info* GetWeaponInfo()
		{
			if (!this) return nullptr;
			typedef Weapon_Info*(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 448)(this);
		}
		CSWeaponInfo* get_full_info()
		{
			if (!this) return nullptr;
			typedef CSWeaponInfo*(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 448)(this);
		}
	};

	class CCSBomb
	{
	public:
		HANDLE GetOwnerHandle()
		{
			return *(HANDLE*)((uintptr_t)this + 0x000031D0);
		}

		float GetC4BlowTime()
		{
			return *(float*)((uintptr_t)this + 0x0000297C);
		}

		float GetC4DefuseCountDown()
		{
			return *(float*)((uintptr_t)this + 0x00002994);
		}

		int GetBombDefuser()
		{
			return *(int*)((uintptr_t)this + 0x000017B8);
		}

		bool IsBombDefused()
		{
			return *(bool*)((uintptr_t)this + 0x00002998);
		}
	};

	class CBaseCSGrenade : CBaseWeapon
	{
	public:
		float pin_pulled()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_bPinPulled);
		}

		float throw_time()
		{
			return *reinterpret_cast<float*>(uintptr_t(this) + OFFSETS::m_fThrowTime);
		}
	};
}