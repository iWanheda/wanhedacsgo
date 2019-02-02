#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/ConVar.h"
/*
template<typename T>
inline void SDK::MinspecCvar::SetValue(T value)
{
	m_pConVar->SetValue(T);
}
SDK::MinspecCvar::MinspecCvar(const char* szCVar, char* newname, float newvalue) : m_pConVar(nullptr)
{
	m_pConVar = g_csgo::cvar->FindVar(szCVar);
	m_newvalue = newvalue;
	m_szReplacementName = newname;
	Spoof();
}

SDK::MinspecCvar::~MinspecCvar()
{
	if (ValidCvar())
	{
		g_csgo::cvar->UnregisterConCommand(m_pConVar);
		m_pConVar->pszName = m_szOriginalName;
		m_pConVar->SetValue(m_OriginalValue);
		g_csgo::cvar->RegisterConCommand(m_pConVar);
	}
}

bool SDK::MinspecCvar::ValidCvar()
{
	return m_pConVar != nullptr;
}
void SDK::MinspecCvar::Spoof()
{
	if (ValidCvar())
	{
		g_csgo::cvar->UnregisterConCommand(m_pConVar);
		m_szOriginalName = m_pConVar->pszName;
		m_OriginalValue = m_pConVar->GetFloat();

		m_pConVar->pszName = m_szReplacementName;
		g_csgo::cvar->RegisterConCommand(m_pConVar);
		m_pConVar->SetValue(m_newvalue);
	}
}

int SDK::MinspecCvar::GetInt()
{
	if (ValidCvar()) {
		return m_pConVar->GetInt();
	}
	return 0;
}

float SDK::MinspecCvar::GetFloat()
{
	if (ValidCvar()) {
		return m_pConVar->GetFloat();
	}
	return 0.0f;
}

const char* SDK::MinspecCvar::GetString()
{
	if (ValidCvar()) {
		return m_pConVar->GetString();
	}
	return nullptr;
}
SDK::SpoofedConvar::SpoofedConvar(const char* szCVar) {
	m_pOriginalCVar = g_csgo::cvar->FindVar(szCVar);
	Spoof();
}
SDK::SpoofedConvar::SpoofedConvar(ConVar* pCVar) {
	m_pOriginalCVar = pCVar;
	Spoof();
}
SDK::SpoofedConvar::~SpoofedConvar() {
	if (IsSpoofed()) {
		DWORD dwOld;

		SetFlags(m_iOriginalFlags);
		SetString(m_szOriginalValue);

		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, PAGE_READWRITE, &dwOld);
		strcpy((char*)m_pOriginalCVar->pszName, m_szOriginalName);
		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, dwOld, &dwOld);

		//Unregister dummy cvar
		g_csgo::cvar->UnregisterConCommand(m_pDummyCVar);
		free(m_pDummyCVar);
		m_pDummyCVar = NULL;
	}
}
bool SDK::SpoofedConvar::IsSpoofed() {
	return m_pDummyCVar != NULL;
}
void SDK::SpoofedConvar::Spoof() 
{
	if (!IsSpoofed() && m_pOriginalCVar) 
	{
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->pszDefaultValue);

		sprintf_s(m_szDummyName, 128, "d_%s", m_szOriginalName);

		//Create the dummy cvar
		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->pNext = NULL;
		//Register it
		g_csgo::cvar->RegisterConCommand(m_pDummyCVar);

		//Fix "write access violation" bullshit
		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, PAGE_READWRITE, &dwOld);

		//Rename the cvar
		strcpy((char*)m_pOriginalCVar->pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}
void SDK::SpoofedConvar::SetFlags(int flags) {
	if (IsSpoofed()) {
		m_pOriginalCVar->nFlags = flags;
	}
}
int SDK::SpoofedConvar::GetFlags() {
	return m_pOriginalCVar->nFlags;
}
void SDK::SpoofedConvar::SetInt(int iValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(iValue);
	}
}
void SDK::SpoofedConvar::SetBool(bool bValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(bValue);
	}
}
void SDK::SpoofedConvar::SetFloat(float flValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(flValue);
	}
}
void SDK::SpoofedConvar::SetString(const char* szValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(szValue);
	}
}*/