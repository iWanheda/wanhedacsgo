#include "../includes.h"

#include "interfaces.h"

#include "../SDK/ISurface.h"
#include "render.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"

namespace global
{
	HWND csgo_hwnd = FindWindow(0, "Counter-Strike: Global Offensive");
	bool render_init;

	bool should_send_packet;
	bool m_round_changed;
	bool is_fakewalking;
	bool hasclantagged;
	bool Should_Desync;
	int packets_choked;
	int choke_amount;

	std::string reason;

	bool should_choke_packets;

	Vector real_angles;
	Vector fake_angles;
	Vector angles;
	Vector strafe_angle;

	int missed_shots[65];
	bool breaking_lby;

	int randomnumber;
	float	flHurtTime;
	bool DisableAA;
	bool Aimbotting;

	using msg_t = void(__cdecl*)(const char*, ...);
	msg_t Msg = reinterpret_cast< msg_t >(GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg"));

	Vector FakePosition;
	int ground_tickz;
	bool CircleStraferActive;
	SDK::CUserCmd originalCMD;
}
namespace FONTS
{
	unsigned int menu_tab_font;
	unsigned int menu_checkbox_font;
	unsigned int menu_slider_font;
	unsigned int menu_groupbox_font;
	unsigned int menu_combobox_font;
	unsigned int menu_window_font;
	unsigned int numpad_menu_font;
	unsigned int visuals_esp_font;
	unsigned int visuals_xhair_font;
	unsigned int visuals_side_font;
	unsigned int visuals_name_font;
	unsigned int visuals_lby_font;
	unsigned int visuals_grenade_pred_font;
	unsigned int  in_game_logging_font;
	bool ShouldReloadFonts()
	{
		static int old_width, old_height;
		int width, height;
		g_csgo::Engine->GetScreenSize(width, height);

		if (width != old_width || height != old_height)
		{
			old_width = width;
			old_height = height;
			return true;
		}
		return false;
	}
	void InitFonts()
	{
		menu_tab_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_checkbox_font = RENDER::CreateF("Verdana", 16, 550, 0, 0, NULL);
		menu_slider_font = RENDER::CreateF("Verdana", 14, 550, 0, 0, NULL);
		menu_groupbox_font = RENDER::CreateF("Verdana", 16, 550, 0, 0, NULL);
		menu_combobox_font = RENDER::CreateF("Verdana", 14, 550, 0, 0, NULL);
		menu_window_font = RENDER::CreateF("Verdana", 16, 550, 0, 0, NULL);
		numpad_menu_font = RENDER::CreateF("Tahoma", 13, 600, 0, 0, SDK::FONTFLAG_OUTLINE);
		visuals_esp_font = RENDER::CreateF("Smallest Pixel-7", 10, 100, 0, 0, SDK::FONTFLAG_OUTLINE);
		visuals_xhair_font = RENDER::CreateF("Tahoma", 18, 600, 0, 0, SDK::FONTFLAG_OUTLINE);
		visuals_side_font = RENDER::CreateF("Calibri", 46, 900, 0, 0, NULL);
		visuals_name_font = RENDER::CreateF("Tahoma", 12, 400, 0, 0, SDK::FONTFLAG_OUTLINE);
		visuals_lby_font = RENDER::CreateF("Verdana", 32, 650, 0, 0, SDK::FONTFLAG_ANTIALIAS | SDK::FONTFLAG_DROPSHADOW);
		visuals_grenade_pred_font = RENDER::CreateF("undefeated", 16, 400, 0, 0, SDK::FONTFLAG_DROPSHADOW | SDK::FONTFLAG_ANTIALIAS);
		in_game_logging_font = RENDER::CreateF("Verdana", 12, FW_BOLD, false, false, SDK::FONTFLAG_DROPSHADOW);
	}
}
namespace settings
{
	CSettings options; 

	bool CSettings::Save(std::string file_name)
	{
		std::string file_path = "" + file_name + ".cfg";

		std::fstream file(file_path, std::ios::out | std::ios::in | std::ios::trunc);
		file.close();

		file.open(file_path, std::ios::out | std::ios::in);
		if (!file.is_open())
		{
			file.close();
			return false;
		}

		const size_t settings_size = sizeof(CSettings);
		for (int i = 0; i < settings_size; i++)
		{
			byte current_byte = *reinterpret_cast<byte*>(uintptr_t(this) + i);
			for (int x = 0; x < 8; x++)
			{
				file << (int)((current_byte >> x) & 1);
			}
		}

		file.close();

		return true;
	}
	bool CSettings::Load(std::string file_name)
	{
		CreateDirectory("", NULL);

		std::string file_path = "" + file_name + ".cfg";

		std::fstream file;
		file.open(file_path, std::ios::out | std::ios::in);
		if (!file.is_open())
		{
			file.close();
			return false;
		}

		std::string line;
		while (file)
		{
			std::getline(file, line);

			const size_t settings_size = sizeof(CSettings);
			if (line.size() > settings_size * 8)
			{
				file.close();
				return false;
			}
			for (int i = 0; i < settings_size; i++)
			{
				byte current_byte = *reinterpret_cast<byte*>(uintptr_t(this) + i);
				for (int x = 0; x < 8; x++)
				{
					if (line[(i * 8) + x] == '1')
						current_byte |= 1 << x;
					else
						current_byte &= ~(1 << x);
				}
				*reinterpret_cast<byte*>(uintptr_t(this) + i) = current_byte;
			}
		}

		file.close();

		return true;
	}
	void CSettings::CreateConfig(std::string name)
	{
		CreateDirectory("", NULL); CreateDirectory("\\", NULL);
		std::ofstream ofs("" + name + ".cfg");
	}

	bool CSettings::Remove(std::string file_name)
	{
		CreateDirectory("", NULL);

		std::string file_path = "" + file_name + ".cfg";
		remove(file_path.c_str());

		return true;
	}

	std::vector<std::string> CSettings::GetConfigs()
	{
		std::vector<std::string> configs;

		WIN32_FIND_DATA ffd;
		auto directory = "*";
		auto hFind = FindFirstFile(directory, &ffd);

		while (FindNextFile(hFind, &ffd))
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string file_name = ffd.cFileName;
				if (file_name.size() < 4) // .cfg
					continue;

				std::string end = file_name;
				end.erase(end.begin(), end.end() - 4); // erase everything but the last 4 letters
				if (end != ".cfg")
					continue;

				file_name.erase(file_name.end() - 4, file_name.end()); // erase the .cfg part
				configs.push_back(file_name);
			}
		}

		return configs;
	}
}