#pragma once

#include "../UTILS/vmt.h"
#include "CBaseEntity.h"

namespace SDK
{
	class CBaseEntity;

	class CClientEntityList
	{
	public:

		CBaseEntity* GetClientEntity(int index)
		{
			typedef CBaseEntity*(__thiscall* Fn)(void*, int);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 3)(this, index);
		}

		int GetHighestEntityIndex()
		{
			typedef int(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 8)(this);
		}
		CBaseEntity *GetClientEntityFromHandle(unsigned long hEnt)
		{
			typedef CBaseEntity* (__thiscall* OriginalFn)(PVOID, unsigned long);
			return VMT::VMTHookManager::GetFunction<OriginalFn>(this, 4)(this, hEnt);
		}
		CBaseEntity* GetClientEntityFromHandle(void* handle)
		{
			typedef CBaseEntity*(__thiscall* Fn)(void*, void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 4)(this, handle);
		}
	};
	class CClientEntityListTWO
	{
	public:
		CBaseEntity * GetClientEntity(int index)
		{
			typedef CBaseEntity*(__thiscall* Fn)(void*, int);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 3)(this, index);
		}

		int GetHighestEntityIndex()
		{
			typedef int(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 8)(this);
		}

		CBaseEntity* GetClientEntityFromHandle(DWORD handle)
		{
			typedef CBaseEntity*(__thiscall* Fn)(void*, DWORD);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 7)(this, handle);
		}
	};
}
