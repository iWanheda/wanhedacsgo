#pragma once

namespace SDK
{
	class CRecvProxyData;
	struct RecvTable;
	struct RecvProp;
}
namespace UTILS
{
	typedef void(*NetvarHookCallback)(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut);

	class CNetvarHookManager
	{
	public:
		void Hook(std::string table, std::string prop, NetvarHookCallback callback);
		void Initialize();
		int GetOffset(const char* tableName, const char* propName);
	private:
		int Get_Prop(const char* tableName, const char* propName, SDK::RecvProp** prop = 0);
		int Get_Prop(SDK::RecvTable* recvTable, const char* propName, SDK::RecvProp** prop = 0);
		SDK::RecvTable* GetTable(const char* tableName);
		std::vector< SDK::RecvTable* > m_tables;




	private:
	}; extern CNetvarHookManager netvar_hook_manager;
}