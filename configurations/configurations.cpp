#include "../configurations/configurations.hpp"
#include "../UTILS/variables.h"
#include <winerror.h>
#pragma warning( disable : 4091)
#include <ShlObj.h>
#include <string>
#include <sstream>
#include "../configurations/XorStr.hpp"

void CConfig::Setup()
{
	//RAGEBOT
	SetupValue(settings::options.aim_bool, false, ("Ragebot"), ("RageToggle"));
	SetupValue(settings::options.hitscan_head, false, ("Ragebot"), ("Scan_head"));
	SetupValue(settings::options.hitscan_neck, false, ("Ragebot"), ("Scan_neck"));
	SetupValue(settings::options.hitscan_chest, false, ("Ragebot"), ("Scan_chest"));
	SetupValue(settings::options.hitscan_pelvis, false, ("Ragebot"), ("Scan_pelvis"));
	SetupValue(settings::options.hitscan_arms, false, ("Ragebot"), ("Scan_arms"));
	SetupValue(settings::options.hitscan_legs, false, ("Ragebot"), ("Scan_legs"));
	SetupValue(settings::options.smartbaim, false, ("Ragebot"), ("Smart_scan"));
	SetupValue(settings::options.chance_val, 0, ("Ragebot"), ("Hitchance"));
	SetupValue(settings::options.damage_val, 0, ("Ragebot"), ("Damage"));
	SetupValue(settings::options.multi_bool, false, ("Ragebot"), ("Multipoint"));
	SetupValue(settings::options.point_val, 0, ("Ragebot"), ("Head_scale"));
	SetupValue(settings::options.body_val, 0, ("Ragebot"), ("Body_scale"));
	SetupValue(settings::options.stop_bool, false, ("Ragebot"), ("Autostop"));
	SetupValue(settings::options.rage_lagcompensation, false, ("Ragebot"), ("Lagcomp"));
	SetupValue(settings::options.fake_bool, false, ("Ragebot"), ("Fakelag_prediction"));
	SetupValue(settings::options.fakewalk, false, ("Ragebot"), ("Fakewalk"));
	SetupValue(settings::options.fakewalkspeed, 0, ("Ragebot"), ("Fakewalk_speed"));
	SetupValue(settings::options.aa_bool, false, ("Ragebot"), ("EnableAA"));
	SetupValue(settings::options.aa_mode, 0, ("Ragebot"), ("ModeAA"));
	SetupValue(settings::options.aa_pitch_type, 0, ("Ragebot"), ("Pitchstand"));
	SetupValue(settings::options.aa_real_type, 0, ("Ragebot"), ("Yawstand"));
	SetupValue(settings::options.aa_pitch1_type, 0, ("Ragebot"), ("Pitchmove"));
	SetupValue(settings::options.aa_real1_type, 0, ("Ragebot"), ("Yawmove"));
	SetupValue(settings::options.aa_pitch2_type, 0, ("Ragebot"), ("Pitchair"));
	SetupValue(settings::options.aa_real2_type, 0, ("Ragebot"), ("Yawair"));
	SetupValue(settings::options.aa_pitch3_type, 0, ("Ragebot"), ("Pitchfakewalk"));
	SetupValue(settings::options.aa_real3_type, 0, ("Ragebot"), ("Yawfakewalk"));
	SetupValue(settings::options.acc_type, 0, ("Ragebot"), ("AimbotMode"));
	SetupValue(settings::options.aa_realadditive_val, 0, ("Ragebot"), ("Yawadditive_stand"));
	SetupValue(settings::options.aa_realadditive1_val, 0, ("Ragebot"), ("Yawadditive_move"));
	SetupValue(settings::options.aa_realadditive2_val, 0, ("Ragebot"), ("Yawadditive_air"));
	SetupValue(settings::options.aa_realadditive3_val, 0, ("Ragebot"), ("Yawadditive_fakewalk"));
	SetupValue(settings::options.delta_val, 0, ("Ragebot"), ("Delta_stand"));
	SetupValue(settings::options.delta1_val, 0, ("Ragebot"), ("Delta_move"));
	SetupValue(settings::options.delta2_val, 0, ("Ragebot"), ("Delta_air"));
	SetupValue(settings::options.delta3_val, 0, ("Ragebot"), ("Delta_fakewalk"));
	SetupValue(settings::options.spinangle2, 0, ("Ragebot"), ("Spinspeed_air"));
	SetupValue(settings::options.spinangle3, 0, ("Ragebot"), ("Spinspeed_fakewalk"));
	SetupValue(settings::options.spinspeed2, 0, ("Ragebot"), ("Spinangle_air"));
	SetupValue(settings::options.spinspeed3, 0, ("Ragebot"), ("Spinangle_fakewalk"));
	SetupValue(settings::options.spinangle1, 0, ("Ragebot"), ("Spinspeed_move"));
	SetupValue(settings::options.spinangle, 0, ("Ragebot"), ("Spinspeed_stand"));
	SetupValue(settings::options.spinspeed1, 0, ("Ragebot"), ("Spinangle_move"));
	SetupValue(settings::options.spinspeed, 0, ("Ragebot"), ("Spinangle_stand"));
	SetupValue(settings::options.flip_int, 0, ("Ragebot"), ("Flipkey"));
	SetupValue(settings::options.rifk_arrow, false, ("Ragebot"), ("ArrowsAA"));
	SetupValue(settings::options.desync_yaw, false, ("Ragebot"), ("Desync"));

	//VISUALS
	SetupValue(settings::options.Visuals.Enabled, false, ("Visuals"), ("Enabled"));
	SetupValue(settings::options.Visuals.Visuals_EspTeam, false, ("Visuals"), ("TeamESP"));
	SetupValue(settings::options.Visuals.Visuals_BoxEnabled, false, ("Visuals"), ("BoxEnabled"));
	SetupValue(settings::options.Visuals.Visuals_BoxType, 0, ("Visuals"), ("BoxType"));
	SetupValue(settings::options.Visuals.Visuals_Name, false, ("Visuals"), ("Name"));
	SetupValue(settings::options.Visuals.Visuals_HealthBar, false, ("Visuals"), ("Health"));
	SetupValue(settings::options.Visuals.Visuals_HealthBarType, 0, ("Visuals"), ("HealthType"));
	SetupValue(settings::options.Visuals.Visuals_AimLines, false, ("Visuals"), ("Aimlines"));        //at this point i got bored and started putting
	SetupValue(settings::options.Visuals.Visuals_Weapons, false, ("Visuals"), ("Visuals_Weapons"));  //variable names instead of normal names
	SetupValue(settings::options.Visuals.Visuals_WeaponsType, 0, ("Visuals"), ("Visuals_WeaponsType"));
	SetupValue(settings::options.Visuals.Visuals_AmmoESP, false, ("Visuals"), ("Visuals_AmmoESP"));
	SetupValue(settings::options.Visuals.Visuals_AmmoESPType, 0, ("Visuals"), ("Visuals_AmmoESPType"));
	SetupValue(settings::options.Visuals.Visuals_DamageESP, false, ("Visuals"), ("Visuals_DamageESP"));
	SetupValue(settings::options.fov_bool, false, ("Visuals"), ("fov_bool"));
	SetupValue(settings::options.chams_type, 0, ("Visuals"), ("chams_type"));
	SetupValue(settings::options.chamsteam, 0, ("Visuals"), ("chamsteam"));
	SetupValue(settings::options.chamstype, 0, ("Visuals"), ("chamstype"));
	SetupValue(settings::options.localchams, 0, ("Visuals"), ("localchams"));
	SetupValue(settings::options.glowenable, false, ("Visuals"), ("glowenable"));
	SetupValue(settings::options.glowteam, false, ("Visuals"), ("glowteam"));
	SetupValue(settings::options.glowstyle, 0, ("Visuals"), ("glowstyle"));
	SetupValue(settings::options.glowlocal, false, ("Visuals"), ("glowlocal"));
	SetupValue(settings::options.glowstylelocal, 0, ("Visuals"), ("glowstylelocal"));
	SetupValue(settings::options.night_bool, false, ("Visuals"), ("night_bool"));
	SetupValue(settings::options.beam_bool, false, ("Visuals"), ("beam_bool"));
	SetupValue(settings::options.tp_bool, false, ("Visuals"), ("tp_bool"));
	SetupValue(settings::options.thirdperson_int, 0, ("Visuals"), ("thirdperson_int"));
	SetupValue(settings::options.transparency_amnt, 0, ("Visuals"), ("transparency_amnt"));
	SetupValue(settings::options.xhair_type, 0, ("Visuals"), ("xhair_type"));
	SetupValue(settings::options.spread_bool, 0, ("Visuals"), ("spread_bool")); //why bool lol idk but too bored to change ok bye
	SetupValue(settings::options.smoke_bool, false, ("Visuals"), ("smoke_bool"));
	SetupValue(settings::options.no_flash, false, ("Visuals"), ("no_flash"));
	SetupValue(settings::options.scope_bool, false, ("Visuals"), ("scope_bool"));
	SetupValue(settings::options.removescoping, false, ("Visuals"), ("removescoping"));
	SetupValue(settings::options.fixscopesens, false, ("Visuals"), ("fixscopesens"));
	SetupValue(settings::options.matpostprocessenable, false, ("Visuals"), ("matpostprocessenable"));
	SetupValue(settings::options.fov_val, 90, ("Visuals"), ("fov_val"));
	SetupValue(settings::options.viewfov_val, 68, ("Visuals"), ("viewfov_val"));
	SetupValue(settings::options.lbyenable, false, ("Visuals"), ("lbyenable"));
	SetupValue(settings::options.scope_type, false, ("Visuals"), ("scope_lines"));
	SetupValue(settings::options.fov_bool, false, ("Visuals"), ("fov_arrows"));
	SetupValue(settings::options.name_bool, false, ("Visuals"), ("name_esp"));
	SetupValue(settings::options.weapon_esp, false, ("Visuals"), ("weapon_bool"));
	




	//MISC
	SetupValue(settings::options.bhop_bool, false, ("Misc"), ("Bhop"));
	SetupValue(settings::options.misc_clantag, false, ("Misc"), ("Clantag"));
	SetupValue(settings::options.strafe_bool, false, ("Misc"), ("Autostrafe"));
	SetupValue(settings::options.duck_bool, false, ("Misc"), ("duck_bool"));
	SetupValue(settings::options.lag_bool, false, ("Misc"), ("lag_bool"));
	SetupValue(settings::options.lag_type, 0, ("Misc"), ("lag_type"));
	SetupValue(settings::options.stand_lag, 0, ("Misc"), "stand_lag");
	SetupValue(settings::options.move_lag, 0, ("Misc"), ("move_lag"));
	SetupValue(settings::options.jump_lag, 0, ("Misc"), ("jump_lag"));
	SetupValue(settings::options.fakewalk_lag, 0, ("Misc"), ("fakewalk_lag"));
	SetupValue(settings::options.hitsound, false, ("Misc"), ("hitsound"));
	SetupValue(settings::options.full_bright, false, ("Misc"), ("full_bright"));


	//SKIN + GLOVE CHANGER
	SetupValue(settings::options.skinenabled, false, ("Skins"), ("skinenabled"));
	SetupValue(settings::options.glovesenabled, false, ("Skins"), ("glovesenabled"));
	SetupValue(settings::options.Knife, 0, ("Skins"), ("Knife"));
	SetupValue(settings::options.KnifeSkin, 0, ("Skins"), ("KnifeSkin"));
	SetupValue(settings::options.gloves, 0, ("Skins"), ("gloves"));
	SetupValue(settings::options.skingloves, 0, ("Skins"), ("skingloves"));
	SetupValue(settings::options.AK47Skin, 0, ("Skins"), ("AK47Skin"));
	SetupValue(settings::options.M4A1SSkin, 0, ("Skins"), ("M4A1SSkin"));
	SetupValue(settings::options.M4A4Skin, 0, ("Skins"), ("M4A4Skin"));
	SetupValue(settings::options.GalilSkin, 0, ("Skins"), ("GalilSkin"));
	SetupValue(settings::options.AUGSkin, 0, ("Skins"), ("AUGSkin"));
	SetupValue(settings::options.FAMASSkin, 0, ("Skins"), ("FAMASSkin"));
	SetupValue(settings::options.Sg553Skin, 0, ("Skins"), ("Sg553Skin"));
	SetupValue(settings::options.UMP45Skin, 0, ("Skins"), ("UMP45Skin"));
	SetupValue(settings::options.Mac10Skin, 0, ("Skins"), ("Mac10Skin"));
	SetupValue(settings::options.BizonSkin, 0, ("Skins"), ("BizonSkin"));
	SetupValue(settings::options.tec9Skin, 0, ("Skins"), ("tec9Skin"));
	SetupValue(settings::options.P2000Skin, 0, ("Skins"), ("P2000Skin"));
	SetupValue(settings::options.P250Skin, 0, ("Skins"), ("P250Skin"));
	SetupValue(settings::options.DualSkin, 0, ("Skins"), ("DualSkin"));
	SetupValue(settings::options.Cz75Skin, 0, ("Skins"), ("Cz75Skin"));
	SetupValue(settings::options.NovaSkin, 0, ("Skins"), ("NovaSkin"));
	SetupValue(settings::options.XmSkin, 0, ("Skins"), ("XmSkin"));
	SetupValue(settings::options.AWPSkin, 0, ("Skins"), ("AWPSkin"));
	SetupValue(settings::options.SSG08Skin, 0, ("Skins"), ("SSG08Skin"));
	SetupValue(settings::options.SCAR20Skin, 0, ("Skins"), ("SCAR20Skin"));
	SetupValue(settings::options.G3sg1Skin, 0, ("Skins"), ("G3sg1Skin"));
	SetupValue(settings::options.Mp9Skin, 0, ("Skins"), ("Mp9Skin"));
	SetupValue(settings::options.GlockSkin, 0, ("Skins"), ("GlockSkin"));
	SetupValue(settings::options.USPSkin, 0, ("Skins"), ("USPSkin"));
	SetupValue(settings::options.DeagleSkin, 0, ("Skins"), ("DeagleSkin"));
	SetupValue(settings::options.FiveSkin, 0, ("Skins"), ("FiveSkin"));
	SetupValue(settings::options.RevolverSkin, 0, ("Skins"), ("RevolverSkin"));
	SetupValue(settings::options.NegevSkin, 0, ("Skins"), ("NegevSkin"));
	SetupValue(settings::options.M249Skin, 0, ("Skins"), ("M249Skin"));
	SetupValue(settings::options.SawedSkin, 0, ("Skins"), ("SawedSkin"));
	SetupValue(settings::options.MagSkin, 0, ("Skins"), ("MagSkin"));


	//COLORS
	SetupValue(settings::options.Visuals.BoxColorPickCTVIS[1], 0.0f, ("Colors"), ("BoxColorPickCTVIS"));
	SetupValue(settings::options.Visuals.BoxColorPickCTVIS[2], 0.0f, ("Colors"), ("BoxColorPickCTVIS1"));
	SetupValue(settings::options.Visuals.BoxColorPickCTVIS[3], 0.0f, ("Colors"), ("BoxColorPickCTVIS2"));
	SetupValue(settings::options.Visuals.BoxColorPickCTVIS[4], 0.0f, ("Colors"), ("BoxColorPickCTVIS3"));
	SetupValue(settings::options.Visuals.BoxColorPickCTINVIS[1], 0.0f, ("Colors"), ("BoxColorPickCTINVIS"));
	SetupValue(settings::options.Visuals.BoxColorPickCTINVIS[2], 0.0f, ("Colors"), ("BoxColorPickCTINVIS1"));
	SetupValue(settings::options.Visuals.BoxColorPickCTINVIS[3], 0.0f, ("Colors"), ("BoxColorPickCTINVIS2"));
	SetupValue(settings::options.Visuals.BoxColorPickCTINVIS[4], 0.0f, ("Colors"), ("BoxColorPickCTINVIS3"));
	SetupValue(settings::options.Visuals.BoxColorPickTVIS[1], 0.0f, ("Colors"), ("BoxColorPickTVIS"));
	SetupValue(settings::options.Visuals.BoxColorPickTVIS[2], 0.0f, ("Colors"), ("BoxColorPickTVIS1"));
	SetupValue(settings::options.Visuals.BoxColorPickTVIS[3], 0.0f, ("Colors"), ("BoxColorPickTVIS2"));
	SetupValue(settings::options.Visuals.BoxColorPickTVIS[4], 0.0f, ("Colors"), ("BoxColorPickTVIS3"));
	SetupValue(settings::options.Visuals.BoxColorPickTINVIS[1], 0.0f, ("Colors"), ("BoxColorPickTINVIS"));
	SetupValue(settings::options.Visuals.BoxColorPickTINVIS[2], 0.0f, ("Colors"), ("BoxColorPickTINVIS1"));
	SetupValue(settings::options.Visuals.BoxColorPickTINVIS[3], 0.0f, ("Colors"), ("BoxColorPickTINVIS2"));
	SetupValue(settings::options.Visuals.BoxColorPickTINVIS[4], 0.0f, ("Colors"), ("BoxColorPickTINVIS3"));
	SetupValue(settings::options.vmodel_col[1], 0.0f, "Colors", "vmodel_col");
	SetupValue(settings::options.vmodel_col[2], 0.0f, "Colors", "vmodel_col1");
	SetupValue(settings::options.vmodel_col[3], 0.0f, "Colors", "vmodel_col2");
	SetupValue(settings::options.vmodel_col[4], 0.0f, "Colors", "vmodel_col3");
	SetupValue(settings::options.imodel_col[1], 0.0f, "Colors", "imodel_col");
	SetupValue(settings::options.imodel_col[2], 0.0f, "Colors", "imodel_col1");
	SetupValue(settings::options.imodel_col[3], 0.0f, "Colors", "imodel_col2");
	SetupValue(settings::options.imodel_col[4], 0.0f, "Colors", "imodel_col3");
	SetupValue(settings::options.teamvis_color[1], 0.0f, "Colors", "teamvis_color");
	SetupValue(settings::options.teamvis_color[2], 0.0f, "Colors", "teamvis_color1");
	SetupValue(settings::options.teamvis_color[3], 0.0f, "Colors", "teamvis_color2");
	SetupValue(settings::options.teamvis_color[4], 0.0f, "Colors", "teamvis_color3");
	SetupValue(settings::options.teaminvis_color[1], 0.0f, "Colors", "teaminvis_color");
	SetupValue(settings::options.teaminvis_color[2], 0.0f, "Colors", "teaminvis_color1");
	SetupValue(settings::options.teaminvis_color[3], 0.0f, "Colors", "teaminvis_color2");
	SetupValue(settings::options.teaminvis_color[4], 0.0f, "Colors", "teaminvis_color3");
	SetupValue(settings::options.localchams_col[1], 0.0f, "Colors", "localchams_col");
	SetupValue(settings::options.localchams_col[2], 0.0f, "Colors", "localchams_col1");
	SetupValue(settings::options.localchams_col[3], 0.0f, "Colors", "localchams_col2");
	SetupValue(settings::options.localchams_col[4], 0.0f, "Colors", "localchams_col3");
	SetupValue(settings::options.Visuals.DamageESPColor[1], 0.0f, "Colors", "DamageESPColor");
	SetupValue(settings::options.Visuals.DamageESPColor[2], 0.0f, "Colors", "DamageESPColor1");
	SetupValue(settings::options.Visuals.DamageESPColor[3], 0.0f, "Colors", "DamageESPColor2");
	SetupValue(settings::options.Visuals.DamageESPColor[4], 0.0f, "Colors", "DamageESPColor3");
	SetupValue(settings::options.glow_col[1], 0.0f, "Colors", "glow_col");
	SetupValue(settings::options.glow_col[2], 0.0f, "Colors", "glow_col1");
	SetupValue(settings::options.glow_col[3], 0.0f, "Colors", "glow_col2");
	SetupValue(settings::options.glow_col[4], 0.0f, "Colors", "glow_col3");
	SetupValue(settings::options.teamglow_color[1], 0.0f, "Colors", "teamglow_color");
	SetupValue(settings::options.teamglow_color[2], 0.0f, "Colors", "teamglow_color1");
	SetupValue(settings::options.teamglow_color[3], 0.0f, "Colors", "teamglow_color2");
	SetupValue(settings::options.teamglow_color[4], 0.0f, "Colors", "teamglow_color3");
	SetupValue(settings::options.glowlocal_col[1], 0.0f, "Colors", "glowlocal_col");
	SetupValue(settings::options.glowlocal_col[2], 0.0f, "Colors", "glowlocal_col1");
	SetupValue(settings::options.glowlocal_col[3], 0.0f, "Colors", "glowlocal_col2");
	SetupValue(settings::options.glowlocal_col[4], 0.0f, "Colors", "glowlocal_col3");
	SetupValue(settings::options.bulletlocal_col[1], 0.0f, "Colors", "bulletlocal_col");
	SetupValue(settings::options.bulletlocal_col[2], 0.0f, "Colors", "bulletlocal_col1");
	SetupValue(settings::options.bulletlocal_col[3], 0.0f, "Colors", "bulletlocal_col2");
	SetupValue(settings::options.bulletlocal_col[4], 0.0f, "Colors", "bulletlocal_col3");
	SetupValue(settings::options.bulletenemy_col[1], 0.0f, "Colors", "bulletenemy_col");
	SetupValue(settings::options.bulletenemy_col[2], 0.0f, "Colors", "bulletenemy_col1");
	SetupValue(settings::options.bulletenemy_col[3], 0.0f, "Colors", "bulletenemy_col2");
	SetupValue(settings::options.bulletenemy_col[4], 0.0f, "Colors", "bulletenemy_col3");
	SetupValue(settings::options.bulletteam_col[1], 0.0f, "Colors", "bulletteam_col");
	SetupValue(settings::options.bulletteam_col[2], 0.0f, "Colors", "bulletteam_col1");
	SetupValue(settings::options.bulletteam_col[3], 0.0f, "Colors", "bulletteam_col2");
	SetupValue(settings::options.bulletteam_col[4], 0.0f, "Colors", "bulletteam_col3");
	SetupValue(settings::options.spreadcirclecol[1], 0.0f, "Colors", "spreadcirclecol");
	SetupValue(settings::options.spreadcirclecol[2], 0.0f, "Colors", "spreadcirclecol1");
	SetupValue(settings::options.spreadcirclecol[3], 0.0f, "Colors", "spreadcirclecol2");
	SetupValue(settings::options.spreadcirclecol[4], 0.0f, "Colors", "spreadcirclecol3");
}

void CConfig::SetupValue(int &value, int def, std::string category, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue<int>(category, name, &value));
}

void CConfig::SetupValue(float &value, float def, std::string category, std::string name)
{
	value = def;
	floats.push_back(new ConfigValue<float>(category, name, &value));
}

void CConfig::SetupValue(bool &value, bool def, std::string category, std::string name)
{
	value = def;
	bools.push_back(new ConfigValue<bool>(category, name, &value));
}



void CConfig::Save()

{

	static TCHAR path[MAX_PATH];

	std::string folder, file;



	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path)))

	{





		folder = std::string(path) + ("\\wanheda sdk\\");

		switch (settings::options.newconfigs)

		{

		case 0: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Legit") + XorStr(".wanheda"); break;
		case 1: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Legit2") + XorStr(".wanheda"); break;
		case 2: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Rage") + XorStr(".wanheda"); break;
		case 3: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Rage2") + XorStr(".wanheda"); break;
		case 4: file = std::string(path) + ("\\wanheda sdk\\") + std::string("HvH") + XorStr(".wanheda"); break;
		case 5: file = std::string(path) + ("\\wanheda sdk\\") + std::string("HvH2") + XorStr(".wanheda"); break;
		case 6: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Scout") + XorStr(".wanheda"); break;
		case 7: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Scout2") + XorStr(".wanheda"); break;
		case 8: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Auto") + XorStr(".wanheda"); break;
		case 9: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Auto2") + XorStr(".wanheda"); break;
		case 10: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Pistol") + XorStr(".wanheda"); break;
		case 11: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Pistol2") + XorStr(".wanheda"); break;

		}



	}



	CreateDirectory(folder.c_str(), NULL);



	for (auto value : ints)

		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());



	for (auto value : floats)

		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());



	for (auto value : bools)

		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str());

}

void CConfig::Load()
{
	static TCHAR path[MAX_PATH];

	std::string folder, file;



	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path)))

	{

		folder = std::string(path) + ("\\wanheda sdk\\");

		switch (settings::options.newconfigs)

		{

		case 0: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Legit") + XorStr(".wanheda"); break;
		case 1: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Legit2") + XorStr(".wanheda"); break;
		case 2: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Rage") + XorStr(".wanheda"); break;
		case 3: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Rage2") + XorStr(".wanheda"); break;
		case 4: file = std::string(path) + ("\\wanheda sdk\\") + std::string("HvH") + XorStr(".wanheda"); break;
		case 5: file = std::string(path) + ("\\wanheda sdk\\") + std::string("HvH2") + XorStr(".wanheda"); break;
		case 6: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Scout") + XorStr(".wanheda"); break;
		case 7: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Scout2") + XorStr(".wanheda"); break;
		case 8: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Auto") + XorStr(".wanheda"); break;
		case 9: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Auto2") + XorStr(".wanheda"); break;
		case 10: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Pistol") + XorStr(".wanheda"); break;
		case 11: file = std::string(path) + ("\\wanheda sdk\\") + std::string("Pistol2") + XorStr(".wanheda"); break;

		}

	}



	CreateDirectory(folder.c_str(), NULL);



	char value_l[32] = { '\0' };



	for (auto value : ints)

	{

		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());

		*value->value = atoi(value_l);

	}



	for (auto value : floats)

	{

		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());

		*value->value = (float)atof(value_l);

	}



	for (auto value : bools)

	{

		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());

		*value->value = !strcmp(value_l, "true");

	}
}

CConfig* Config = new CConfig();