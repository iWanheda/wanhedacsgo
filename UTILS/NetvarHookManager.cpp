#include "../includes.h"

#include "interfaces.h"
#include "../SDK/IClient.h"
#include "../SDK/RecvData.h"

#include "NetvarHookManager.h"

namespace UTILS
{
	CNetvarHookManager netvar_hook_manager;
	void CNetvarHookManager::Hook(std::string table_to_hook, std::string prop_to_hook, NetvarHookCallback callback)
	{
		auto client_class = g_csgo::Client->GetAllClasses();

		// go through all classes
		while (client_class)
		{
			std::string table_name = client_class->m_pRecvTable->m_pNetTableName;

			if (table_name == table_to_hook)
			{
				for (int i = 0; i < client_class->m_pRecvTable->m_nProps; i++)
				{
					auto& prop = client_class->m_pRecvTable->m_pProps[i];
					std::string prop_name = prop.m_pVarName;

					if (prop_name == prop_to_hook)
					{
						prop.m_ProxyFn = callback;
					}
				}
			}

			client_class = client_class->m_pNext; // go to the next class
		}
	}
	typedef void(__cdecl *MsgFn)(char const* pMsg, va_list);
	MsgFn oMsg;
	void __cdecl Msg(char const* msg, ...)
	{
		if (!oMsg)
			oMsg = (MsgFn)GetProcAddress(GetModuleHandle(("tier0.dll")), ("Msg"));

		char buffer[989];
		va_list list;
		va_start(list, msg);
		vsprintf_s(buffer, msg, list);
		va_end(list);
		oMsg(buffer, list);
	}
	int CNetvarHookManager::GetOffset(const char* tableName, const char* propName)
	{
		int offset = Get_Prop(tableName, propName);
		if (!offset)
		{
			Msg("%s not found!\n", propName);
			return 0;
		}
		Msg("%s: 0x%02X\n", propName, offset);
		return offset;
	}
	int CNetvarHookManager::Get_Prop(const char* tableName, const char* propName, SDK::RecvProp** prop)
	{
		SDK::RecvTable* recvTable = GetTable(tableName);
		if (!recvTable)
			return 0;

		int offset = Get_Prop(recvTable, propName, prop);
		if (!offset)
			return 0;

		return offset;
	}
	void CNetvarHookManager::Initialize()
	{
		m_tables.clear();

		SDK::ClientClass* clientClass = g_csgo::Client->GetAllClasses();
		if (!clientClass)
			return;

		while (clientClass)
		{
			SDK::RecvTable* recvTable = clientClass->m_pRecvTable;
			m_tables.push_back(recvTable);

			clientClass = clientClass->m_pNext;
		}
	}
	SDK::RecvTable* CNetvarHookManager::GetTable(const char* tableName)
	{
		if (m_tables.empty())
			return 0;

		for each(SDK::RecvTable* table in m_tables)
		{
			if (!table)
				continue;

			if (_stricmp(table->m_pNetTableName, tableName) == 0)
				return table;
		}

		return 0;
	}
	int CNetvarHookManager::Get_Prop(SDK::RecvTable* recvTable, const char* propName, SDK::RecvProp** prop)
	{
		int extraOffset = 0;
		for (int i = 0; i < recvTable->m_nProps; ++i)
		{
			SDK::RecvProp* recvProp = &recvTable->m_pProps[i];
			SDK::RecvTable* child = recvProp->m_pDataTable;

			if (child && (child->m_nProps > 0))
			{
				int tmp = Get_Prop(child, propName, prop);
				if (tmp)
					extraOffset += (recvProp->m_Offset + tmp);
			}

			if (_stricmp((const char*)recvProp->m_pVarName, propName))
				continue;

			if (prop)
				*prop = recvProp;

			return (recvProp->m_Offset + extraOffset);
		}

		return extraOffset;
	}
}