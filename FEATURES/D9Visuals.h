#include "../includes.h"

#include <d3d9.h>
#include <d3dx9.h>




namespace Menuxd
{
	extern LPD3DXFONT fntVerdana11;
	extern LPD3DXFONT fntVerdana10;
	extern D3DVIEWPORT9 viewPort;
	extern LPD3DXFONT fntWeaponIcon;
	extern void DrawFilledRectangle(float x, float y, float height, float width, D3DCOLOR COLOR, IDirect3DDevice9* pDevice);
	extern void DrawStringWithFont(LPD3DXFONT fnt, float x, float y, D3DCOLOR color, char *format, ...);
	extern LPD3DXFONT fntVerdana9;
	extern void DrawLineFast(float x1, float y1, float x2, float y2, D3DCOLOR COLOR, IDirect3DDevice9* pDevice);
	extern void DrawCornierBoxFastAlpha(float x, float y, float height, float width, D3DCOLOR COLOR, float alpha, IDirect3DDevice9* pDevice);
	extern bool WorldToScreen(Vector vOrigin, Vector &vScreen);
	extern int GetTextWitdhW(wchar_t *text, LPD3DXFONT fnt);
	extern float GetTextWitdh(char *text, LPD3DXFONT fnt);
	extern void DrawCircleFast(float x, float y, int radius, int numSides, D3DCOLOR color, IDirect3DDevice9* pDevice);
	extern void drawfatalpricel(IDirect3DDevice9* pDevice);
	extern void CircleFilledRainbowColor(float x, float y, float rad, float rotate, int type, int resolution, IDirect3DDevice9* m_device);
}

#define D3D_COLOR_LIME(a)	D3DCOLOR_ARGB(a, 204, 255, 153)
#define D3D_COLOR_WHITE(a)	D3DCOLOR_ARGB(a, 255, 255, 255)
#define D3D_COLOR_BLACK(a)	D3DCOLOR_ARGB(a, 0, 0, 0)
#define D3D_COLOR_RED(a)	D3DCOLOR_ARGB(a, 255, 0, 0)
#define D3D_COLOR_DARKRED(a)D3DCOLOR_ARGB(a, 153, 0, 0)
#define D3D_COLOR_GREEN(a)	D3DCOLOR_ARGB(a, 0, 255, 0)
#define D3D_COLOR_DARKGREEN(a)	D3DCOLOR_ARGB(a, 0, 153, 0)
#define D3D_COLOR_BLUE(a)	D3DCOLOR_ARGB(a, 0, 0, 255)
#define D3D_COLOR_DARKBLUE(a)D3DCOLOR_ARGB(a, 0, 0, 153)
#define D3D_COLOR_YELLOW(a)	D3DCOLOR_ARGB(a, 255, 255, 0)
#define D3D_COLOR_PINK(a)	D3DCOLOR_ARGB(a, 255, 0, 255)
#define D3D_COLOR_ORANGE(a)	D3DCOLOR_ARGB(a, 255, 153, 0)
#define D3D_COLOR_LIGHTBLUE(a)D3DCOLOR_ARGB(a, 0, 255, 255) 
#define D3D_COLOR_BROWN(a)	D3DCOLOR_ARGB(a, 153, 102, 0)
#define D3D_COLOR_GRAY(a)	D3DCOLOR_ARGB(a, 153, 153, 153)
#define D3D_COLOR_DARKGRAY(a)	D3DCOLOR_ARGB(a, 13, 13, 13)


struct D3DTLVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};
extern float viewMatrix[4][4];


namespace SDK
{
	class IGameEvent;
	class CBaseEntity;
}


namespace D9Visuals
{
	void Render(IDirect3DDevice9* pDevice);
	void DrawCrossHair(float size, DWORD color);
	void DrawName(SDK::CBaseEntity *entity);
	void DrawWeapon(SDK::CBaseEntity *entity, IDirect3DDevice9* pDevice);
	void DrawHealth(SDK::CBaseEntity *entity, IDirect3DDevice9* pDevice);
	void DrawBox(SDK::CBaseEntity *entity, int cType, IDirect3DDevice9* pDevice);
	void DrawBone(SDK::CBaseEntity * entity, matrix3x4_t * pBoneToWorldOut, DWORD color, IDirect3DDevice9 * pDevice);



	bool GetPlayerBox(SDK::CBaseEntity *entity, float &x, float &y, float &width, float &height, Vector offset = Vector(0, 0, 0));
	bool Filter(SDK::CBaseEntity *entity, bool& IsVis);

	void PunchAnglesToScreen(int height, int width, int FOV, int *resultX, int *resultY);
	void FrameRate();
	extern bool canUseSetupBones;

	extern int PlayerNickVal[65];
	extern int PlayerAlpha[65];
	extern float PlayerBoxAlpha[65];

};
#include <array>

namespace DamageESP
{
	class FloatingText
	{
	public:
		FloatingText() :
			valid(false)
		{ }

		bool valid;
		float startTime;
		int damage;
		int hitgroup;
		Vector hitPosition;
		int randomIdx;
	};

	const int MAX_FLOATING_TEXTS = 50;
	extern std::array<FloatingText, MAX_FLOATING_TEXTS> floatingTexts;

	void HandleGameEvent(SDK::IGameEvent* pEvent);
	void Draw();
};



