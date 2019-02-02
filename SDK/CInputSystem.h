#pragma once

namespace SDK
{
	class CInputSystem
	{
	public:
		void EnableInput(bool enabled)
		{
			typedef void(__thiscall* Fn)(void*, bool);
			return ((Fn)VMT::VMTHookManager::GetFunction<Fn>(this, 11))(this, enabled);
		}
	};
}