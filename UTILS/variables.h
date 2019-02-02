#pragma once

/*
global variables and other stuff that is probably too small to put in another 
file (and i'm too lazy to make files for all of them) would go in here.
Stuff like fonts and shit
*/

namespace global
{
	extern HWND csgo_hwnd;
	extern bool render_init;
	extern bool Should_Desync;
	extern int packets_choked;
	extern bool hasclantagged;

	extern std::string reason;

	extern bool should_send_packet;
	extern bool m_round_changed;
	extern bool is_fakewalking;
	extern bool breaking_lby;
	extern int choke_amount;

	extern Vector real_angles;
	extern Vector angles;
	extern Vector fake_angles;
	extern Vector strafe_angle;

	extern bool should_choke_packets;

	extern int randomnumber;
	extern float flHurtTime;
	extern bool DisableAA;
	extern bool Aimbotting;
	extern int missed_shots[65];

	using msg_t = void(__cdecl*)(const char*, ...);
	extern msg_t		Msg;

	extern Vector FakePosition;
	extern int ground_tickz;
	extern bool CircleStraferActive;
	extern SDK::CUserCmd originalCMD;
}
namespace FONTS
{
	extern unsigned int menu_tab_font;
	extern unsigned int menu_checkbox_font;
	extern unsigned int menu_slider_font;
	extern unsigned int menu_groupbox_font;
	extern unsigned int menu_combobox_font;
	extern unsigned int menu_window_font;
	extern unsigned int numpad_menu_font;
	extern unsigned int visuals_esp_font;
	extern unsigned int visuals_xhair_font;
	extern unsigned int visuals_side_font;
	extern unsigned int visuals_name_font;
	extern unsigned int visuals_lby_font;
	extern unsigned int visuals_grenade_pred_font;
	extern unsigned int  in_game_logging_font;
	bool ShouldReloadFonts();
	void InitFonts();
}
namespace settings
{
	class Sticker_t
	{
	public:
		int   iID = 0;
		float flWear = 0.f;
		float flScale = 1.f;
		int   iRotation = 0;
	};
	class Weapon_tTT
	{
	public:
		int SkinsWeapon;
		int SkinsKnife;
		int VremennyiWeapon;
		int VremennyiKnife;
		int Stikers1;
		int Stikers2;
		int Stikers3;
		int Stikers4;
		float ChangerWear = 0;
		int ChangerStatTrak = 0;
		char ChangerName[32] = "";
		bool ChangerEnabled;
	};

	class CSettings
	{
	public:
		// returns true/false whether the function succeeds, usually returns false if file doesn't exist
		bool Save(std::string file_name);
		bool Load(std::string file_name);

		bool Remove(std::string file_name);

		void CreateConfig(std::string name); // creates a blank config

		std::vector<std::string> GetConfigs();

		bool skinenabled;
		int Knife;

		Weapon_tTT weapons[520];

		struct wpnz
		{
			bool  EnabledStickers;
			bool  EnabledSkins;
			int   ChangerSkin;
			char  ChangerName[32] = "";
			int   ChangerStatTrak;
			int   ChangerSeed;
			float ChangerWear;
			Sticker_t Stickers[5];
		}Weapons[519];

		bool fix;
		bool backaimhead;
		int rage_lagcompensation_type=0;
		bool rage_lagcompensation;

		bool CUSTOMMODEL;
		int customct;
		int customtt;


		bool hitscan_head;
		bool hitscan_neck;
		bool hitscan_chest;
		bool hitscan_pelvis;
		bool hitscan_arms;
		bool hitscan_legs;
		bool smartbaim;
		bool weapon_esp;

		int 
			viewmodel_x, 
			viewmodel_y, 
			viewmodel_z;

		int gloves;
		int skingloves;
		int newconfigs;
		bool desync_yaw;
		float glovewear;
		bool glovesenabled;
		bool rankchanger;
		int KnifeSkin;
		int rank_id = 9;
		int wins = 100;
		int level = 40;
		int friendly = 999;
		int teaching = 999;
		int leader = 999;
		int AK47Skin;
		int GalilSkin;
		int M4A1SSkin;
		int M4A4Skin;
		int AUGSkin;
		int FAMASSkin;
		int AWPSkin;
		int SSG08Skin;
		bool fakelatency_enabled;
		bool fixshit;
		float fakelatency_amount;
		char clantag_name[64] = { 0 };
		int SCAR20Skin;
		int P90Skin;
		int Mp7Skin;
		int NovaSkin;
		int UMP45Skin;
		int GlockSkin;
		int SawedSkin;
		int USPSkin;
		int MagSkin;
		int XmSkin;
		int DeagleSkin;
		int DualSkin;
		int FiveSkin;
		int RevolverSkin;
		int Mac10Skin;
		int tec9Skin;
		int Cz75Skin;
		int NegevSkin;
		int M249Skin;
		int Mp9Skin;
		int P2000Skin;
		int BizonSkin;
		int Sg553Skin;
		int P250Skin;
		int G3sg1Skin;


		struct Visualsxd
		{
			bool Enabled = true;
			bool Visuals_BoxEnabled = true;
			int Visuals_BoxType = 1;
			int Visuals_HealthBarType = 1;
			bool Visuals_Scoped = false;
			bool Visuals_AmmoESP = true;
			int Visuals_AmmoESPType = 1;

			bool Visuals_EspTeam = false;
			bool Visuals_VisableOnly = false;

			float BoxColorPickCTVIS[4] = { 1.0f,0.0f,0.0f,1.0f };
			float BoxColorPickCTINVIS[4] = { 1.0f,1.0f,0.0f,1.0f };


			float BoxColorPickTVIS[4] = { 0.0f,1.0f,0.0f,1.0f };
			float BoxColorPickTINVIS[4] = { 0.0f,1.0f,1.0f,1.0f };


			float LagCompHitboxes[4] = { 0.0f,1.0f,0.0f,1.0f };



			float SkeletonCTVIS[4] = { 1.0f,0.0f,0.0f,1.0f };
			float SkeletonCTINVIS[4] = { 1.0f,1.0f,0.0f,1.0f };

			float SkeletonTVIS[4] = { 0.0f,1.0f,0.0f,1.0f };
			float SkeletonTINVIS[4] = { 0.0f,1.0f,1.0f,1.0f };




			float CTChamsVisable[4] = { 1.0f,0.0f,0.0f,1.0f };

			float TChamsVisable[4] = { 1.0f,1.0f,0.0f,1.0f };


			float CTChamsInVisable[4] = { 0.0f,1.0f,0.0f,1.0f };
			float TChamsInVisable[4] = { 1.0f,1.0f,0.0f,1.0f };

			float beamtrace[4] = { 1.0f,0.0f,0.0f,1.0f };
			int thirdkey;
			int Hitsound;
			bool tp_bool;
			bool Monitor;
			bool Visuals_HealthBar = true;
			bool Visuals_Name = true;
			bool Visuals_ArmorBar = false;
			bool Visuals_Flashed = false;
			bool Visuals_Defuser = false;
			bool Visuals_Weapons = false;
			int Visuals_WeaponsType = 0;
			bool Visuals_AimLines = false;
			bool Visuals_Skeltal = false;
			bool Visuals_EngineRadar = false;
			bool Visuals_DrawLinesAA = false;
			bool Visuals_DroppedWeapons = false;
			bool Visuals_NoRecoil = false;
			bool Visuals_NoFlash = false;
			bool Visuals_NoSmoke = false;
			bool Visuals_NoSmokeVar = false;
			bool Visuals_NoScope = false;
			bool Visuals_NoZoom = false;
			bool Visuals_NoPostProcess = false;


			bool Visuals_Chams = true;
			bool Visuals_ChamsTeam = false;
			bool Visuals_ChamsXQZ = false;
			int  Visuals_ChamsMaterial = 1;
			bool Visuals_GhostAngle = false;
			bool Visuals_ChamsGuns = false;
			bool Visuals_Crosshair = false;
			bool Visuals_CrosshairDynamic = false;


			float CTGlowVisable[4] = { 1.f,1.0f,1.0f,1.0f };
			float TGlowVisable[4] = { 1.f,1.0f,1.0f,1.0f };
			float CTGlowInVisable[4] = { 1.f,1.0f,1.0f,1.0f };
			float TGlowInVisable[4] = { 1.f,1.0f,1.0f,1.0f };

			bool Visuals_Hitmarker = false;
			bool Visuals_Spread = false;
			bool Visuals_GrenadePrediction = false;
			bool Visuals_DamageESP = false;
			bool Visuals_C4 = false;
			bool eventlog_enabled = false;
			float SpreadColor[4] = { 1.0f,0.0f,0.0f,1.0f };

			float DamageESPColor[4] = { 1.0f,0.0f,0.0f,1.0f };
			bool Vis_Glow = true;
			bool Vis_Glow_Team = false;
			bool Vis_Glow_Vis = false;

			bool Visuals_NightMode = false;
			bool Visuals_Asus = false;

			bool Visuals_DrawBeams = false;
			float Visuals_DrawBeamsDuration = 3.f;
			float Visuals_DrawBeamsSize = 3.f;

			bool Visuals_DrawCapsules = false;
			bool Visuals_DrawEventLog = false;
			float capsulecolor[4] = { 1.0,0.0f,0.0f,1.0f };
			float Visuals_DrawCapsulesDuration = 5.f;
			int Visuals_Spread_Type = 0;
			struct
			{
				bool Enabled = false;
				float Distance = 800.f;
				int type = 0;
				float Radius = 10.f;
				bool Animated = false;
				bool visonly = true;
			}SoundESP;
			struct
			{
				struct
				{
					bool	ExternalRadar;
					int		RadarStyle;
					float	RadarDistance;
					bool	RadarVisibleOnly;
					bool	RadarSmoke;
				}Radar;
				bool Monitor = false;
			}Panels;

			bool lbyIndicator = false;
			bool strelkiIndicator = false;
		}Visuals;


		bool friendfire;
		bool bhop_bool;
		bool strafe_bool;
		bool esp_bool=true;
		int chams_type;
		int xhair_type;
		bool skeleton;
		bool tp_bool;
		bool aim_bool;
		int aim_type;
		bool aa_bool;
		int aa_pitch;
		int aa_type;
		int thirdperson_int;
		int baim_key;
		int acc_type;
		bool up_bool;
		bool misc_bool;
		int config_sel;
		bool beam_bool;
		bool stop_bool;
		int fakewalk_lag;
		bool night_bool;
		bool box_bool;
		bool name_bool;
		bool weap_bool;
		bool health_bool;
		bool info_bool;
		bool back_bool;
		bool lag_bool;
		int box_type;
		bool reverse_bool;
		bool multi_bool=true;
		bool fakefix_bool;
		bool angle_bool;
		bool tp_angle_bool;
		bool glow_bool;
		bool dist_bool;
		bool fov_bool;
		bool smoke_bool;
		bool scope_bool;
		bool predict_bool;
		bool fake_bool;
		int media_type;
		bool novis_bool;
		bool localglow_bool;
		bool duck_bool;
		bool money_bool;
		int delay_shot;
		int lag_type;
		bool cham_bool;
		bool resolve_bool;
		bool ammo_bool;
		bool lby_bar;
		int spread_bool;


		float spreadcirclecol[4] = { 1.0f,0.0f,0.0f,0.3f };

		int stand_lag;
		int move_lag;
		int jump_lag;

		bool debug_bool;


		float vmodel_col[4] = { 1.0f,1.0f,0.0f,0.5f };

		float imodel_col[4] = { 0.0f,1.0f,0.0f,0.5f };
		
	
		
		CColor box_col;
		CColor name_col;
		CColor weapon_col;
		CColor distance_col;

		
		float localchams_col[4] = { 1.0f,0.0f,1.0f,0.5f };
		float grenadepredline_col[4] = { 1.0f,0.0f,1.0f,0.5f };

		float bulletlocal_col[4] = { 1.0f,0.0f,1.0f,1.0f };
		float bulletenemy_col[4] = { 0.0f,1.0f,1.0f,1.0f };
		float bulletteam_col[4]{ 1.0f,1.0f,1.0f,1.0f };

		float bulletlife = 3.0f;
		float bulletsize = 2.0f;
	
		CColor menu_col = CColor(60, 60, 60);
		CColor checkbox_col = CColor(5, 135, 5);
		CColor slider_col = CColor(5, 135, 5);
		CColor tab_col = CColor(91, 91, 91);
		
		bool no_flash;

		float glow_col[4] = { 0.5f,0.5f,1.0f,1.0f };

		float transparency_amnt = .5f;

		float glowlocal_col[4] = { 0.3f,0.3f,0.9f,0.7f };
		
		CColor fov_col;

		bool cradle;

		int chance_val;
		int damage_val = 1;
		int delta_val;
		float point_val;
		bool hitsound;
		float body_val;
		bool misc_clantag;

		bool localesp;
		int localchams;
		float fov_val= 90;
		float viewfov_val= 68;

		bool fakechams;
		float fakechamscol[4] = { 1.0f,1.0f,1.0f,1.0f };


		bool flip_bool;
		int aa_side;

		bool legit_bool;
		int legit_key;
		bool rcs_bool;
		float legitfov_val;
		int legitbone_int;
		float rcsamount_min;
		float rcsamount_max;
		float legitaim_val;
		bool legittrigger_bool;
		int legittrigger_key;

		char thirdperson;
		int flip_int;

		bool glowenable;
		int glowstyle;
		bool glowlocal;
		int glowstylelocal;
		int hitmarker_val;

		int aa_mode;

		int aa_real_type;
		int aa_real1_type;
		int aa_real2_type;
		int aa_real3_type;

		int aa_fake_type;
		int aa_fake1_type;
		int aa_fake2_type;

		int aa_pitch_type;
		int aa_pitch1_type;
		int aa_pitch2_type;
		int scope_type;
		int aa_pitch3_type;

		int aa_realadditive_val;
		float aa_fakeadditive_val;

		int aa_realadditive1_val;
		float aa_fakeadditive1_val;
		int delta1_val;

		int aa_realadditive2_val;
		float aa_fakeadditive2_val;
		int delta2_val;

		int aa_realadditive3_val;
		int delta3_val;

		int spinangle;
		int spinspeed;

		int spinangle1;
		int spinspeed1;
		int spinspeed3;

		int spinangle2;
		int spinspeed2;
		int spinangle3;

		float spinanglefake;
		float spinspeedfake;

		float spinanglefake1;
		float spinspeedfake1;

		float spinanglefake2;
		float spinspeedfake2;

		bool lbyflickup;
		bool lbyflickup1;
		bool lbyflickup2;

		bool aa_fakeangchams_bool;

		int chamstype;
		float fov_time;
		bool rifk_arrow;

		int glowteamselection;
		bool glowteam;

		int chamsteamselection;
		int chamsteam;

		int espteamselection;
		int espteamcolourselection;
		bool boxteam;
		bool nameteam;
		bool weaponteam;
		bool flagsteam;
		bool healthteam;
		bool moneyteam;
		bool ammoteam;
		bool arrowteam;
		CColor boxteam_col;
		CColor nameteam_col;
		CColor weaponteam_col;
		CColor arrowteam_col;


		float teamvis_color[4] = { 1.0f,0.5f,0.5f,0.5f };
		float teaminvis_color[4] = { 1.0f,1.0f,0.5f,0.5f };


	
		


		float teamglow_color[4] = { 1.0f,1.0f,0.5f,0.5f };

		bool matpostprocessenable;
		bool removescoping;
		bool fixscopesens;
		bool forcecrosshair;

		bool killfeed;
		bool full_bright;

		int quickstopkey;
		bool stop_flip;
		bool chamsmetallic;
		char flashlightkey;
		int overridekey;
		int autostopmethod;
		int overridemethod;
		bool overridething;
		bool overrideenable;
		bool lbyenable;
		int circlestrafekey;
		float circlstraferetract;
		int fakewalkspeed = 1;
		bool fakewalk;
		float daytimevalue = 98;
		bool footsteps;

		bool test;

		float circlemin;
		float circlemax;
		float circlevel;
		float circlenormalizemultiplier;

		bool skinchangerenable;

		int knifeToUse;
		int bayonetID, karambitID, flipID, gutID, m9ID, huntsmanID;
		int gloveToUse;
		int bloodhoundID, driverID, handwrapsID, motoID, specialistID, sportID, hydraID;

		int uspID, p2000ID, glockID, dualberettaID, p250ID, fivesevenID, tech9ID, r8ID, deagleID;
		int novaID, xm1014ID, mag7ID, sawedoffID, m249ID, negevID;
		int mp9ID, mac10ID, mp7ID, ump45ID, p90ID, ppbizonID;
		int famasID, galilID, ak47ID, m4a4ID, m4a1sID, ssg08ID, augID, sg553ID, awpID, scar20ID, g3sg1ID;


	private:
	}; extern CSettings options;
}

extern bool using_fake_angles[65];
extern bool full_choke;
extern bool is_shooting;

extern bool in_tp;
extern bool fake_walk;

extern int resolve_type[65];

extern int target;
extern int shots_fired[65];
extern int shots_hit[65];
extern int shots_missed[65];
extern int backtrack_missed[65];

extern bool didShot;
extern bool didMiss;

extern float tick_to_back[65];
extern float lby_to_back[65];
extern bool backtrack_tick[65];

extern float lby_delta;
extern float update_time[65];
extern float walking_time[65];

extern float local_update;

extern int hitmarker_time;
extern int random_number;

extern bool menu_hide;

extern int oldest_tick[65];
extern float compensate[65][12];
extern Vector backtrack_hitbox[65][20][12];
extern float backtrack_simtime[65][12];