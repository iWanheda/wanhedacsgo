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
#include "SkinChanger.h"
#include "StickerChanger.h"



#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
std::unordered_map<char*, char*> killIcons = {};
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
HANDLE worldmodel_handle;
SDK::CBaseWeapon* worldmodel;
#define MakePtr(cast, ptr, addValue) (cast)( (DWORD)(ptr) + (DWORD)(addValue))

void xdSkinchanger()
{
	SDK::CBaseEntity *pLocal = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
	auto weapons = pLocal->m_hMyWeapons();
	for (size_t i = 0; weapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		SDK::CBaseEntity *pEntity = g_csgo::ClientEntityList->GetClientEntityFromHandle(weapons[i]);
		if (pEntity)
		{
			SDK::CBaseWeapon* pWeapon = (SDK::CBaseWeapon*)pEntity;

			if (settings::options.skinenabled)
			{
				int Model = settings::options.Knife;
				int weapon = pWeapon->GetItemDefenitionIndex();
				ApplyStickerHooks(pWeapon);
				switch (weapon)
				{
				case 7: // AK47 
				{
					switch (settings::options.AK47Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 180;//fire serpent
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 524;//Fuel Injector
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 639;//Bloodsport
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 302;//vulcan
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 44;//case hardened
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 456;//Hydroponic
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 474;//Aquamarine Revenge
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 490;//Frontside Misty
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 506;//Point Disarray
						break;
					case 10:
						*pWeapon->FallbackPaintKit() = 600;//Neon Revolution
						break;
					case 11:
						*pWeapon->FallbackPaintKit() = 14;//red laminate
						break;
					case 12:
						*pWeapon->FallbackPaintKit() = 282;//redline
						break;
					case 13:
						*pWeapon->FallbackPaintKit() = 316;//jaguar
						break;
					case 14:
						*pWeapon->FallbackPaintKit() = 340;//jetset
						break;
					case 15:
						*pWeapon->FallbackPaintKit() = 380;//wasteland rebel
						break;
					case 16:
						*pWeapon->FallbackPaintKit() = 675;//The Empress
						break;
					case 17:
						*pWeapon->FallbackPaintKit() = 422;//Elite Build
						break;
					default:
						break;
					}
				}
				break;
				case 16: // M4A4
				{
					switch (settings::options.M4A4Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 255;//Asiimov
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 309;//Howl
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 400;//Dragon King
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 449;//Poseidon
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 471;//Daybreak
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 512;//Royal Paladin
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 533;//BattleStar
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 588;//Desolate Space
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 632;//Buzz Kill
						break;
					case 10:
						*pWeapon->FallbackPaintKit() = 155;//Bullet Rain
						break;
					case 11:
						*pWeapon->FallbackPaintKit() = 664;//Hell Fire
						break;
					case 12:
						*pWeapon->FallbackPaintKit() = 480;//Evil Daimyo
						break;
					case 13:
						*pWeapon->FallbackPaintKit() = 384;//Griffin
						break;
					case 14:
						*pWeapon->FallbackPaintKit() = 187;//Zirka
						break;
					case 15:
						*pWeapon->FallbackPaintKit() = 167;//Radiation Harzard
						break;
					default:
						break;
					}
				}
				break;
				case 2: // dual
				{
					switch (settings::options.DualSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 276;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 491;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 658;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 625;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 447;
						break;
					default:
						break;
					}
				}
				break;
				case 60: // M4A1
				{
					switch (settings::options.M4A1SSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 644;//Decimator
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 326;//Knight
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 548;//Chantico's Fire
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 497;//Golden Coi
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 430;//Hyper Beast
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 321;//Master Piece
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 445;//Hot Rod
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 587;//Mecha Industries
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 360;//Cyrex
						break;
					case 10:
						*pWeapon->FallbackPaintKit() = 440;//Icarus Fell
						break;
					case 11:
						*pWeapon->FallbackPaintKit() = 631;//Flashback
						break;
					case 12:
						*pWeapon->FallbackPaintKit() = 681;//Flashback
						break;
					case 13:
						*pWeapon->FallbackPaintKit() = 430;//Hyper Beast
						break;
					case 14:
						*pWeapon->FallbackPaintKit() = 301;//Atomic Alloy
						break;
					case 15:
						*pWeapon->FallbackPaintKit() = 257;//Guardian
						break;
					case 16:
						*pWeapon->FallbackPaintKit() = 663;//Briefing
						break;
					default:
						break;
					}
				}
				break;
				case 9: // AWP
				{
					switch (settings::options.AWPSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 279;//asiimov
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 344;//dlore
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 640;//Fever Dream
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 446;//medusa
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 475;//hyperbeast
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 174;//boom
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 51;//lightning strike
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 84;//pink ddpat
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 181;//corticera
						break;
					case 10:
						*pWeapon->FallbackPaintKit() = 259;//redline
						break;
					case 11:
						*pWeapon->FallbackPaintKit() = 395;//manowar
						break;
					case 12:
						*pWeapon->FallbackPaintKit() = 212;//graphite
						break;
					case 13:
						*pWeapon->FallbackPaintKit() = 227;//electric hive
						break;
					case 14:
						*pWeapon->FallbackPaintKit() = 451;//Sun in Leo
						break;
					case 15:
						*pWeapon->FallbackPaintKit() = 475;//Hyper Beast
						break;
					case 16:
						*pWeapon->FallbackPaintKit() = 251;//Pit viper
						break;
					case 17:
						*pWeapon->FallbackPaintKit() = 584;//Phobos
						break;
					case 18:
						*pWeapon->FallbackPaintKit() = 525;//Elite Build
						break;
					case 19:
						*pWeapon->FallbackPaintKit() = 424;//Worm God
						break;
					case 20:
						*pWeapon->FallbackPaintKit() = 662;//Oni Taiji
						break;
					case 21:
						*pWeapon->FallbackPaintKit() = 640;//Fever Dream
						break;
					default:
						break;
					}
				}
				break;
				case 61: // USP
				{
					switch (settings::options.USPSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 653;//Neo-Noir
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 637;//Cyrex
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 313;//Orion
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 504;//Kill Confirmed
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 183;//Overgrowth
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 339;//Caiman
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 221;//Serum
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 290;//Guardian
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 318;//Road Rash
						break;
					default:
						break;
					}
				}
				break;
				case 4: // Glock
				{
					switch (settings::options.GlockSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 38;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 48;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 437;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 586;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 353;
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 680;
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 607;
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 532;
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 381;
						break;
					case 10:
						*pWeapon->FallbackPaintKit() = 230;
						break;
					case 11:
						*pWeapon->FallbackPaintKit() = 159;
						break;
					case 12:
						*pWeapon->FallbackPaintKit() = 623;
						break;
					case 13:
						*pWeapon->FallbackPaintKit() = 479;
						break;
					case 14:
						*pWeapon->FallbackPaintKit() = 680;
						break;
					case 15:
						*pWeapon->FallbackPaintKit() = 367;
						break;
					default:
						break;
					}
				}
				break;
				case 1: // Deagle
				{
					switch (settings::options.DeagleSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 37;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 527;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 645;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 185;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 37;
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 645;
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 231;
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 603;
						break;
					default:
						break;
					}
				}
				break;
				case 3: // Five Seven
				{
					switch (settings::options.FiveSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 427;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 660;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 352;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 530;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 510;
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 646;
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 585;
						break;
					default:
						break;
					}
				}
				break;
				case 8: // AUG
				{
					switch (settings::options.AUGSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 9;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 33;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 280;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 455;
						break;
					default:
						break;
					}
				}
				break;
				case 10: // Famas
				{
					switch (settings::options.FAMASSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 429;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 371;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 477;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 492;
						break;
					default:
						break;
					}
				}
				break;
				case 11: // G3SG1
				{
					switch (settings::options.G3sg1Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 677;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 511;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 463;
						break;
					default:
						break;
					}
				}
				break;
				case 13: // Galil
				{
					switch (settings::options.GalilSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 398;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 647;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 661;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 428;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 379;
						break;
					default:
						break;
					}
				}
				break;
				case 14: // M249
				{
					switch (settings::options.M249Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 496;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 401;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 266;
						break;
					default:
						break;
					}
				}
				break;
				case 17: // Mac 10
				{
					switch (settings::options.Mac10Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 433;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 651;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 310;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 498;
						break;
					default:
						break;
					}
				}
				break;
				case 19: // P90
				{
					*pWeapon->FallbackPaintKit() = 156;
				}
				break;
				case 24: // UMP-45
				{
					switch (settings::options.UMP45Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 37;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 441;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 448;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 556;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 688;
						break;
					default:
						break;
					}
				}
				break;
				case 25: // XM1014
				{
					switch (settings::options.XmSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 654;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 363;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 689;
						break;
					default:
						break;
					}
				}
				break;
				case 63: // CZ75-Auto
				{
					switch (settings::options.Cz75Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 543;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 435;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 270;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 643;
						break;
					default:
						break;
					}
				}
				break;
				case 26: // Bizon
				{
					switch (settings::options.BizonSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 676;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 542;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 508;
						break;
					default:
						break;
					}
				}
				break;
				case 27: // Mag 7
				{
					switch (settings::options.MagSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 39;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 431;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 608;
						break;
					default:
						break;
					}
				}
				break;
				case 28: // Negev
				{
					switch (settings::options.NegevSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 514;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 483;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 432;
						break;
					default:
						break;
					}
				}
				break;
				case 29: // Sawed Off
				{
					switch (settings::options.SawedSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 638;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 256;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 517;
						break;
					default:
						break;
					}
				}
				break;
				case 30: // Tec 9
				{
					switch (settings::options.tec9Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 179;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 248;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 216;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 272;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 289;
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 303;
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 374;
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 555;
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 614;
						break;
					default:
						break;
					}
				}
				break;
				case 32: // P2000
				{
					switch (settings::options.P2000Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 485;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 38;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 184;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 211;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 389;
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 442;
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 443;
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 515;
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 550;
						break;
					case 10:
						*pWeapon->FallbackPaintKit() = 591;
						break;
					default:
						break;
					}
				}
				break;
				case 33: // MP7
				{
					switch (settings::options.Mp9Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 481;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 536;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 500;
						break;
					default:
						break;
					}
				}
				break;
				case 34: // MP9
				{
					switch (settings::options.Mp9Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 262;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 482;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 609;
						break;
					default:
						break;
					}
				}
				break;
				case 35: // Nova
				{
					switch (settings::options.NovaSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 537;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 356;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 286;
						break;
					default:
						break;
					}
				}
				break;
				case 36: // P250
				{
					switch (settings::options.P250Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 102;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 466;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 467;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 501;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 551;
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 678;
						break;
					default:
						break;
					}
				}
				break;
				case 38: // Scar 20
				{
					switch (settings::options.SCAR20Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 642;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 298;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 406;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 453;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 502;
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 518;
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 597;
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 685;
						break;
					default:
						break;
					}
				}
				break;
				case 39: // SG553
				{
					switch (settings::options.Sg553Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 519;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 487;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 287;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 586;
						break;
					default:
						break;
					}
				}
				break;
				case 40: // SSG08
				{
					switch (settings::options.SSG08Skin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 26;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 60;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 96;
						break;
					case 4:
						*pWeapon->FallbackPaintKit() = 99;
						break;
					case 5:
						*pWeapon->FallbackPaintKit() = 157;
						break;
					case 6:
						*pWeapon->FallbackPaintKit() = 200;
						break;
					case 7:
						*pWeapon->FallbackPaintKit() = 222;
						break;
					case 8:
						*pWeapon->FallbackPaintKit() = 233;
						break;
					case 9:
						*pWeapon->FallbackPaintKit() = 253;
						break;
					case 10:
						*pWeapon->FallbackPaintKit() = 304;
						break;
					case 11:
						*pWeapon->FallbackPaintKit() = 319;
						break;
					case 12:
						*pWeapon->FallbackPaintKit() = 361;
						break;
					case 13:
						*pWeapon->FallbackPaintKit() = 503;
						break;
					case 14:
						*pWeapon->FallbackPaintKit() = 538;
						break;
					case 15:
						*pWeapon->FallbackPaintKit() = 554;
						break;
					case 16:
						*pWeapon->FallbackPaintKit() = 624;
						break;
					default:
						break;
					}
				}
				break;
				case 64: // Revolver
				{
					switch (settings::options.RevolverSkin)
					{
					case 0:
						*pWeapon->FallbackPaintKit() = 0;//none
						break;
					case 1:
						*pWeapon->FallbackPaintKit() = 683;
						break;
					case 2:
						*pWeapon->FallbackPaintKit() = 522;
						break;
					case 3:
						*pWeapon->FallbackPaintKit() = 12;
						break;
					default:
						break;
					}
				}
				break;
				default:
					break;
				}
				if (pEntity->GetClientClass()->m_ClassID == 105)
				{
					auto pCustomName1 = MakePtr(char*, pWeapon, 0x301C);
					worldmodel_handle = pWeapon->m_hWeaponWorldModel();
					if (worldmodel_handle) worldmodel = (SDK::CBaseWeapon*)g_csgo::ClientEntityList->GetClientEntityFromHandle(worldmodel_handle);
					if (Model == 0) // Bayonet
					{
						int iBayonet = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
						*pWeapon->ModelIndex() = iBayonet; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iBayonet;
						if (worldmodel) *pWeapon->ModelIndex() = iBayonet + 1;
						*pWeapon->fixskins() = 500;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "bayonet";
						killIcons["knife_t"] = "bayonet";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 558; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 563; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 573; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 9) // Bowie Knife
					{
						int iBowie = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
						*pWeapon->ModelIndex() = iBowie; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iBowie;
						if (worldmodel) *worldmodel->ModelIndex() = iBowie + 1;
						*pWeapon->fixskins() = 514;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_survival_bowie";
						killIcons["knife_t"] = "knife_survival_bowie";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 558; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}

					}
					else if (Model == 6) // Butterfly Knife
					{
						int iButterfly = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
						*pWeapon->ModelIndex() = iButterfly; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iButterfly;
						if (worldmodel) *worldmodel->ModelIndex() = iButterfly + 1;
						*pWeapon->fixskins() = 515;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_butterfly";
						killIcons["knife_t"] = "knife_butterfly";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 558; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 7) // Falchion Knife
					{
						int iFalchion = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
						*pWeapon->ModelIndex() = iFalchion; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iFalchion;
						if (worldmodel) *worldmodel->ModelIndex() = iFalchion + 1;
						*pWeapon->fixskins() = 512;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_falchion";
						killIcons["knife_t"] = "knife_falchion";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 558; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 1) // Flip Knife
					{
						int iFlip = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
						*pWeapon->ModelIndex() = iFlip; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iFlip;
						if (worldmodel) *worldmodel->ModelIndex() = iFlip + 1;
						*pWeapon->fixskins() = 505;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_flip";
						killIcons["knife_t"] = "knife_flip";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 559; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 564; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}

					}
					else if (Model == 2) // Gut Knife
					{
						int iGut = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
						*pWeapon->ModelIndex() = iGut; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iGut;
						if (worldmodel) *worldmodel->ModelIndex() = iGut + 1;
						*pWeapon->fixskins() = 506;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_gut";
						killIcons["knife_t"] = "knife_gut";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 560; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 565; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 575; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 5) // Huntsman Knife
					{
						int iHuntsman = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
						*pWeapon->ModelIndex() = iHuntsman; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iHuntsman;
						if (worldmodel) *worldmodel->ModelIndex() = iHuntsman + 1;
						*pWeapon->fixskins() = 509;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_tactical";
						killIcons["knife_t"] = "knife_tactical";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 559; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 3) // Karambit
					{
						int iKarambit = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
						*pWeapon->ModelIndex() = iKarambit; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iKarambit;
						if (worldmodel) *worldmodel->ModelIndex() = iKarambit + 1;
						*pWeapon->fixskins() = 507;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_karambit";
						killIcons["knife_t"] = "knife_karambit";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 570; // Doppler Phase 4
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 568; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 566; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 576; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 582; // Freehand
						}

					}
					else if (Model == 4) // M9 Bayonet
					{
						int iM9Bayonet = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
						*pWeapon->ModelIndex() = iM9Bayonet; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iM9Bayonet;
						if (worldmodel) *worldmodel->ModelIndex() = iM9Bayonet + 1;
						*pWeapon->fixskins() = 508;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_m9_bayonet";
						killIcons["knife_t"] = "knife_m9_bayonet";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 0; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 570; // Doppler Phase 4
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 568; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 562; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 577; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 581; // Freehand
						}

					}
					else if (Model == 8)
					{
						int iDagger = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
						*pWeapon->ModelIndex() = iDagger; // m_nModelIndex
						*pWeapon->ViewModelIndex() = iDagger;
						if (worldmodel) *worldmodel->ModelIndex() = iDagger + 1;
						*pWeapon->fixskins() = 516;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 10)
					{
						int Navaja = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
						*pWeapon->ModelIndex() = Navaja; // m_nModelIndex
						*pWeapon->ViewModelIndex() = Navaja;
						if (worldmodel) *worldmodel->ModelIndex() = Navaja + 1;
						*pWeapon->fixskins() = 520;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 11) // Shadow Daggers
					{
						int Stiletto = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
						*pWeapon->ModelIndex() = Stiletto; // m_nModelIndex
						*pWeapon->ViewModelIndex() = Stiletto;
						if (worldmodel) *worldmodel->ModelIndex() = Stiletto + 1;
						*pWeapon->fixskins() = 522;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 12)
					{
						int Ursus = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_ursus.mdl");
						*pWeapon->ModelIndex() = Ursus; // m_nModelIndex
						*pWeapon->ViewModelIndex() = Ursus;
						if (worldmodel) *worldmodel->ModelIndex() = Ursus + 1;
						*pWeapon->fixskins() = 519;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
					else if (Model == 13)
					{
						int Talon = g_csgo::ModelInfo->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
						*pWeapon->ModelIndex() = Talon; // m_nModelIndex
						*pWeapon->ViewModelIndex() = Talon;
						if (worldmodel) *worldmodel->ModelIndex() = Talon + 1;
						*pWeapon->fixskins() = 523;
						*pWeapon->GetEntityQuality() = 3;
						killIcons.clear();
						killIcons["knife_default_ct"] = "knife_push";
						killIcons["knife_t"] = "knife_push";
						int Skin = settings::options.KnifeSkin;
						if (Skin == 0)
						{
							*pWeapon->FallbackPaintKit() = 5; // Forest DDPAT
						}
						else if (Skin == 1)
						{
							*pWeapon->FallbackPaintKit() = 12; // Crimson Web
						}
						else if (Skin == 2)
						{
							*pWeapon->FallbackPaintKit() = 27; // Bone Mask
						}
						else if (Skin == 3)
						{
							*pWeapon->FallbackPaintKit() = 38; // Fade
						}
						else if (Skin == 4)
						{
							*pWeapon->FallbackPaintKit() = 40; // Night
						}
						else if (Skin == 5)
						{
							*pWeapon->FallbackPaintKit() = 42; // Blue Steel
						}
						else if (Skin == 6)
						{
							*pWeapon->FallbackPaintKit() = 43; // Stained
						}
						else if (Skin == 7)
						{
							*pWeapon->FallbackPaintKit() = 44; // Case Hardened
						}
						else if (Skin == 8)
						{
							*pWeapon->FallbackPaintKit() = 59; // Slaughter
						}
						else if (Skin == 9)
						{
							*pWeapon->FallbackPaintKit() = 72; // Safari Mesh
						}
						else if (Skin == 10)
						{
							*pWeapon->FallbackPaintKit() = 77; // Boreal Forest
						}
						else if (Skin == 11)
						{
							*pWeapon->FallbackPaintKit() = 98; // Ultraviolet
						}
						else if (Skin == 12)
						{
							*pWeapon->FallbackPaintKit() = 143; // Urban Masked
						}
						else if (Skin == 13)
						{
							*pWeapon->FallbackPaintKit() = 175; // Scorched
						}
						else if (Skin == 14)
						{
							*pWeapon->FallbackPaintKit() = 323; // Rust Coat
						}
						else if (Skin == 15)
						{
							*pWeapon->FallbackPaintKit() = 409; // Tiger Tooth
						}
						else if (Skin == 16)
						{
							*pWeapon->FallbackPaintKit() = 410; // Damascus Steel
						}
						else if (Skin == 17)
						{
							*pWeapon->FallbackPaintKit() = 411; // Damascus Steel
						}
						else if (Skin == 18)
						{
							*pWeapon->FallbackPaintKit() = 413; // Marble Fade
						}
						else if (Skin == 19)
						{
							*pWeapon->FallbackPaintKit() = 414; // Rust Coat
						}
						else if (Skin == 20)
						{
							*pWeapon->FallbackPaintKit() = 415; // Doppler Ruby
						}
						else if (Skin == 21)
						{
							*pWeapon->FallbackPaintKit() = 416; // Doppler Sapphire
						}
						else if (Skin == 22)
						{
							*pWeapon->FallbackPaintKit() = 417; // Doppler Blackpearl
						}
						else if (Skin == 23)
						{
							*pWeapon->FallbackPaintKit() = 418; // Doppler Phase 1
						}
						else if (Skin == 24)
						{
							*pWeapon->FallbackPaintKit() = 419; // Doppler Phase 2
						}
						else if (Skin == 25)
						{
							*pWeapon->FallbackPaintKit() = 420; // Doppler Phase 3
						}
						else if (Skin == 26)
						{
							*pWeapon->FallbackPaintKit() = 421; // Doppler Phase 4
						}
						else if (Skin == 27)
						{
							*pWeapon->FallbackPaintKit() = 569; // Gamma Doppler Phase1
						}
						else if (Skin == 28)
						{
							*pWeapon->FallbackPaintKit() = 570; // Gamma Doppler Phase2
						}
						else if (Skin == 29)
						{
							*pWeapon->FallbackPaintKit() = 571; // Gamma Doppler Phase3
						}
						else if (Skin == 30)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Phase4
						}
						else if (Skin == 31)
						{
							*pWeapon->FallbackPaintKit() = 568; // Gamma Doppler Emerald
						}
						else if (Skin == 32)
						{
							*pWeapon->FallbackPaintKit() = 561; // Lore
						}
						else if (Skin == 33)
						{
							*pWeapon->FallbackPaintKit() = 567; // Black Laminate
						}
						else if (Skin == 34)
						{
							*pWeapon->FallbackPaintKit() = 574; // Autotronic
						}
						else if (Skin == 35)
						{
							*pWeapon->FallbackPaintKit() = 580; // Freehand
						}
					}
				}
				*pWeapon->OwnerXuidLow() = 0;
				*pWeapon->OwnerXuidHigh() = 0;
				*pWeapon->FallbackWear() = 0.001f;
				*pWeapon->ItemIDHigh() = 1;
			}
		}
	}
}