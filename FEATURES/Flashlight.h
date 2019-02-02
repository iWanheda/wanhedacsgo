#pragma once
#include "../HOOKS/hooks.h"
class CFlashLightEffect
{
public:
	bool m_bIsOn; //0x0000 
	char pad_0x0001[0x3]; //0x0001
	int m_nEntIndex; //0x0004 
	WORD m_FlashLightHandle; //0x0008 
	char pad_0x000A[0x2]; //0x000A
	float m_flMuzzleFlashBrightness; //0x000C 
	float m_flFov; //0x0010 
	float m_flFarZ; //0x0014 
	float m_flLinearAtten; //0x0018 
	bool m_bCastsShadows; //0x001C 
	char pad_0x001D[0x3]; //0x001D
	float m_flCurrentPullBackDist; //0x0020 
	DWORD m_MuzzleFlashTexture; //0x0024 
	DWORD m_FlashLightTexture; //0x0028 
	char m_szTextureName[64]; //0x1559888 
}; //Size=0x006C

class CFlashLight
{
public:
	CFlashLightEffect* CreateFlashLight(int nEntIndex, const char *pszTextureName, float flFov, float flFarZ, float flLinearAtten);
	void RunFrame();
private:
	void DestroyFlashLight(CFlashLightEffect *pFlashLight);
	void UpdateFlashLight(CFlashLightEffect *pFlashLight, const Vector &vecPos, const Vector &vecForward, const Vector &vecRight, const Vector &vecUp);

};

extern CFlashLight Flashlight;