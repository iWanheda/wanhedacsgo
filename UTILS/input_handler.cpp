#include "../includes.h"

namespace UTILS
{
	namespace INPUT
	{
		long old_wnd_proc;
		void InputHandler::Init()
		{
			for (int &key : cached_pressed_keys)
				key = 0;

			old_wnd_proc = SetWindowLongPtr(global::csgo_hwnd, GWL_WNDPROC, (long)WndProcCallback);
		}
		void InputHandler::Update()
		{
			for (int &key : cached_pressed_keys)
				(key > 0) ? key-- : key = 0; // decrement key press count until reaches 0
		}
		int InputHandler::keyBindings(int setting)
		{
			int vk = 0;
			switch (setting)
			{
			case 0: break;
			case 1: vk = VK_LBUTTON; break;
			case 2: vk = VK_RBUTTON; break;
			case 3: vk = VK_MBUTTON; break;
			case 4: vk = VK_XBUTTON1; break;
			case 5: vk = VK_XBUTTON2; break;
			case 6: vk = 0x41; break;
			case 7: vk = 0x42; break;
			case 8: vk = 0x43; break;
			case 9: vk = 0x44; break;
			case 10: vk = 0x45; break;
			case 11: vk = 0x46; break;
			case 12: vk = 0x47; break;
			case 13: vk = 0x48; break;
			case 14: vk = 0x49; break;
			case 15: vk = 0x4A; break;
			case 16: vk = 0x4B; break;
			case 17: vk = 0x4C; break;
			case 18: vk = 0x4D; break;
			case 19: vk = 0x4E; break;
			case 20: vk = 0x4F; break;
			case 21: vk = 0x50; break;
			case 22: vk = 0x51; break;
			case 23: vk = 0x52; break;
			case 24: vk = 0x53; break;
			case 25: vk = 0x54; break;
			case 26: vk = 0x55; break;
			case 27: vk = 0x56; break;
			case 28: vk = 0x57; break;
			case 29: vk = 0x58; break;
			case 30: vk = 0x59; break;
			case 31: vk = 0x5A; break;
			case 32: vk = 0x31; break;
			case 33: vk = 0x32; break;
			case 34: vk = 0x33; break;
			case 35: vk = 0x34; break;
			case 36: vk = 0x35; break;
			case 37: vk = 0x36; break;
			case 38: vk = 0x37; break;
			case 39: vk = 0x38; break;
			case 40: vk = 0x39; break;
			case 41: vk = 0x30; break;
			case 42: vk = VK_F1; break;
			case 43: vk = VK_F2; break;
			case 44: vk = VK_F3; break;
			case 45: vk = VK_F4; break;
			case 46: vk = VK_F5; break;
			case 47: vk = VK_F6; break;
			case 48: vk = VK_F7; break;
			case 49: vk = VK_F8; break;
			case 50: vk = VK_F9; break;
			case 51: vk = VK_F10; break;
			case 52: vk = VK_F11; break;
			case 53: vk = VK_F12; break;
			default: break;
			}
			return vk;
		}
		MouseInfo InputHandler::GetMouseInfo()
		{
			cached_mouse_info.position = GetMousePosition();

			int right = GetAsyncKeyState(VK_RBUTTON);
			int left = GetAsyncKeyState(VK_LBUTTON);

			cached_mouse_info.left = left;
			cached_mouse_info.right = right;

			auto ret = cached_mouse_info;

			cached_mouse_info.scroll = 0;
			return ret;
		}
		void InputHandler::GetKeyboardState(int* keyboard) 
		{		
			memcpy(keyboard, cached_pressed_keys, sizeof(int) * 255);
		}
		int InputHandler::GetKeyState(int key) 
		{
			return cached_pressed_keys[key];
		}
		int InputHandler::GetKeyStateEx(int key)
		{
			int ret = cached_pressed_keys_ex[key];

			cached_pressed_keys_ex[key] = max(0, cached_pressed_keys_ex[key] - 1);

			return ret;
		}
		void InputHandler::Callback(UINT Message, WPARAM wParam, LPARAM lParam)
		{
			switch (Message)
			{
			case WM_KEYDOWN:
				if (wParam >= 0 && wParam < 255)
				{
					cached_pressed_keys_ex[wParam]++;
					cached_pressed_keys[wParam]++;
				}
				break;
			case WM_XBUTTONDOWN:
				
				if (GET_XBUTTON_WPARAM(wParam) & XBUTTON1)
				{
					cached_pressed_keys[VK_XBUTTON1]++;
					cached_pressed_keys_ex[VK_XBUTTON1]++;
				}
				else if (GET_XBUTTON_WPARAM(wParam) & XBUTTON2)
				{
					cached_pressed_keys[VK_XBUTTON2]++;
					cached_pressed_keys_ex[VK_XBUTTON2]++;
				}
				break;
			case WM_MOUSEWHEEL:
				cached_mouse_info.scroll = GET_WHEEL_DELTA_WPARAM(wParam);
				break;
			}
		}

		Vector2D InputHandler::GetMousePosition()
		{
			POINT mouse_position;
			GetCursorPos(&mouse_position);
			ScreenToClient(global::csgo_hwnd, &mouse_position);
			return { static_cast<float>(mouse_position.x), static_cast<float>(mouse_position.y) };
		}


		LRESULT CALLBACK WndProcCallback(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
		{
			input_handler.Callback(Message, wParam, lParam);
			return CallWindowProc((WNDPROC)old_wnd_proc, Hwnd, Message, wParam, lParam);
		}

		InputHandler input_handler;
	}
}