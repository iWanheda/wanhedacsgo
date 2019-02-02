

#include "D9Visuals.h"

#include "../SDK/CInput.h"
#include "../SDK/IClient.h"
#include "../SDK/CPanel.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlowObjectManager.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/RecvData.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/RenderView.h"
#include "../SDK/CTrace.h"
#include "../SDK/CViewSetup.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CPrediction.h"
#include "../SDK/GameEvents.h"






float viewMatrix[4][4];

namespace Menuxd
{
	D3DVIEWPORT9 viewPort;
	LPD3DXFONT fntVerdana11 = nullptr;
	LPD3DXFONT fntVerdana10 = nullptr;
	LPD3DXFONT fntVerdana9 = nullptr;
	LPD3DXFONT fntWeaponIcon = nullptr;




	void CircleFilledRainbowColor(float x, float y, float rad, float rotate, int type, int resolution, IDirect3DDevice9* m_device)
	{
		LPDIRECT3DVERTEXBUFFER9 g_pVB2;

		std::vector<D3DTLVERTEX> circle(resolution + 2);

		float angle = rotate * D3DX_PI / 180, pi = D3DX_PI;

		if (type == 1)
			pi = D3DX_PI; // Full circle
		if (type == 2)
			pi = D3DX_PI / 2; // 1/2 circle
		if (type == 3)
			pi = D3DX_PI / 4; // 1/4 circle

		pi = D3DX_PI / type; // 1/4 circle

		circle[0].x = x;
		circle[0].y = y;
		circle[0].z = 0;
		circle[0].rhw = 1;
		circle[0].color = D3DCOLOR_RGBA(0, 0, 0, 0);

		float hue = 0.f;

		for (int i = 1; i < resolution + 2; i++)
		{
			circle[i].x = (float)(x - rad * cos(pi*((i - 1) / (resolution / 2.0f))));
			circle[i].y = (float)(y - rad * sin(pi*((i - 1) / (resolution / 2.0f))));
			circle[i].z = 0;
			circle[i].rhw = 1;

			auto clr = CColor::FromHSB(hue, 1.f, 1.f);
			circle[i].color = D3DCOLOR_RGBA(clr.RGBA[0], clr.RGBA[1], clr.RGBA[2], clr.RGBA[3]);
			hue += 0.02;
		}

		// Rotate matrix
		int _res = resolution + 2;
		for (int i = 0; i < _res; i++)
		{
			float Vx1 = x + (cosf(angle) * (circle[i].x - x) - sinf(angle) * (circle[i].y - y));
			float Vy1 = y + (sinf(angle) * (circle[i].x - x) + cosf(angle) * (circle[i].y - y));

			circle[i].x = Vx1;
			circle[i].y = Vy1;
		}

		m_device->CreateVertexBuffer((resolution + 2) * sizeof(D3DTLVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, NULL);

		VOID* pVertices;
		g_pVB2->Lock(0, (resolution + 2) * sizeof(D3DTLVERTEX), (void**)&pVertices, 0);
		memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(D3DTLVERTEX));
		g_pVB2->Unlock();

		m_device->SetTexture(0, NULL);
		m_device->SetPixelShader(NULL);
		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_device->SetStreamSource(0, g_pVB2, 0, sizeof(D3DTLVERTEX));
		m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
		if (g_pVB2 != NULL)
			g_pVB2->Release();
	}

	void drawfatalpricel(IDirect3DDevice9* pDevice)
	{
		if (g_csgo::Engine->IsConnected() && g_csgo::Engine->IsInGame()) {
			int w, h;
			static float rot = 0.f;
			SDK::CBaseEntity *pLocal = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());
			SDK::CBaseWeapon* pWeapon = NULL;
			g_csgo::Engine->GetScreenSize(w, h); w /= 2; h /= 2;
			auto weapons = pLocal->m_hMyWeapons();
			for (size_t i = 0; weapons[i] != 0xFFFFFFFF; i++)
			{
				SDK::CBaseEntity *pEntity = g_csgo::ClientEntityList->GetClientEntityFromHandle(weapons[i]);
				pWeapon = (SDK::CBaseWeapon*)pEntity;
			}
			if (pWeapon)
			{
				int Index = pWeapon->GetItemDefenitionIndex();
				if (Index != 42 && Index != 59 && Index != 500)
				{
					auto accuracy = pWeapon->GetInaccuracy() * 550.f;
					CircleFilledRainbowColor(w, h, accuracy, rot, 1, 50, pDevice);

					rot += 1.f;
					if (rot > 360.f) rot = 0.f;
				}
			}
		}
	}


	void DrawLineFast(float x1, float y1, float x2, float y2, D3DCOLOR COLOR, IDirect3DDevice9* pDevice)
	{
		D3DTLVERTEX qV[2] = {
			{ x1, y1, 0.0f, 1.0f, COLOR },
		{ x2, y2, 0.0f, 1.0f, COLOR },
		};

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); //pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA); // neon mode
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_FOGENABLE, false);

		DWORD oldFVF;

		pDevice->GetFVF(&oldFVF);
		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		pDevice->SetTexture(0, 0);
		pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, qV, sizeof(D3DTLVERTEX));
		pDevice->SetFVF(oldFVF);
	}

	float GetTextWitdh(char *text, LPD3DXFONT fnt)
	{
		RECT fontRect = { 0,0,0,0 };

		fnt->DrawText(NULL, text, strlen(text), &fontRect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));

		return fontRect.right - fontRect.left;
	}
	int GetTextWitdhW(wchar_t *text, LPD3DXFONT fnt)
	{
		RECT fontRect = { 0,0,0,0 };

		fnt->DrawTextW(NULL, text, -1, &fontRect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));

		return fontRect.right - fontRect.left;
	}
	bool WorldToScreen(Vector vOrigin, Vector &vScreen)
	{
		float w = viewMatrix[3][0] * vOrigin.x + viewMatrix[3][1] * vOrigin.y + viewMatrix[3][2] * vOrigin.z + viewMatrix[3][3];
		float ScreenWidth = (float)Menuxd::viewPort.Width;
		float ScreenHeight = (float)Menuxd::viewPort.Height;

		if (w > 0.01f)
		{
			float inverseWidth = 1.f / w;
			vScreen.x = (float)((ScreenWidth / 2) + (0.5 * ((viewMatrix[0][0] * vOrigin.x + viewMatrix[0][1] * vOrigin.y + viewMatrix[0][2] * vOrigin.z + viewMatrix[0][3]) * inverseWidth) * ScreenWidth + 0.5));
			vScreen.y = (float)((ScreenHeight / 2) - (0.5 * ((viewMatrix[1][0] * vOrigin.x + viewMatrix[1][1] * vOrigin.y + viewMatrix[1][2] * vOrigin.z + viewMatrix[1][3]) * inverseWidth) * ScreenHeight + 0.5));
			return true;
		}

		return false;
	}




	void DrawFilledRectangle(float x, float y, float height, float width, D3DCOLOR COLOR, IDirect3DDevice9* pDevice)
	{
		D3DXVECTOR2 points[5];

		/*
		x	y
		h	w
		*/

		D3DTLVERTEX t1[3] = {
			{ x, y, 0.0f, 1.0f, COLOR },
		{ x + width, y, 0.0f, 1.0f, COLOR },
		{ x + width, y + height, 0.0f, 1.0f, COLOR },
		};

		D3DTLVERTEX t2[3] = {
			{ x , y, 0.0f, 1.0f, COLOR },
		{ x + width , y + height, 0.0f, 1.0f, COLOR },
		{ x, y + height, 0.0f, 1.0f, COLOR },
		};

		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); //pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA); // neon mode
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_FOGENABLE, false);

		DWORD oldFVF;

		pDevice->GetFVF(&oldFVF);
		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		pDevice->SetTexture(0, 0);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, t1, sizeof(D3DTLVERTEX));
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, t2, sizeof(D3DTLVERTEX));
		pDevice->SetFVF(oldFVF);
	}

	int FrameRate() {
		static int iFps, iLastFps;
		static float flLastTickCount, flTickCount;
		flTickCount = clock() * 0.001f;
		iFps++;
		if ((flTickCount - flLastTickCount) >= 1.0f) {
			flLastTickCount = flTickCount;
			iLastFps = iFps;
			iFps = 0;
		}
		return iLastFps;
	}

	void DrawStringWithFont(LPD3DXFONT fnt, float x, float y, D3DCOLOR color, char *format, ...)
	{
		char buffer[256];
		RECT fontRect = { (int)x, (int)y, (int)x, (int)y };

		va_list va_argList;

		va_start(va_argList, format);
		wvsprintf(buffer, format, va_argList);
		va_end(va_argList);

		fnt->DrawText(NULL, buffer, strlen(buffer), &fontRect, DT_NOCLIP, color);
	}





	void DrawCornierBoxFastAlpha(float x, float y, float height, float width, D3DCOLOR COLOR, float alpha, IDirect3DDevice9* pDevice)
	{
		D3DXVECTOR2 points[4][3];



		switch (settings::options.Visuals.Visuals_BoxType)
		{
		case 0:
		{
			points[0][0] = D3DXVECTOR2(x, y);
			points[0][1] = D3DXVECTOR2(x + (width)* alpha, y);
			points[0][2] = D3DXVECTOR2(x, y + (height)* alpha);

			points[1][0] = D3DXVECTOR2(x + width, y);
			points[1][1] = D3DXVECTOR2(x + width - (width)* alpha, y);
			points[1][2] = D3DXVECTOR2(x + width, y + (height)* alpha);


			points[2][0] = D3DXVECTOR2(x, y + height);
			points[2][1] = D3DXVECTOR2(x + (width)* alpha, y + height);
			points[2][2] = D3DXVECTOR2(x, y + height - (height)* alpha);

			points[3][0] = D3DXVECTOR2(x + width, y + height);
			points[3][1] = D3DXVECTOR2(x + width - (width)* alpha, y + height);
			points[3][2] = D3DXVECTOR2(x + width, y + height - (height)* alpha);
		}break;
		case 1:
		{
			points[0][0] = D3DXVECTOR2(x, y);
			points[0][1] = D3DXVECTOR2(x + (width / 4.5f)* alpha, y);
			points[0][2] = D3DXVECTOR2(x, y + (height / 5.f)* alpha);

			points[1][0] = D3DXVECTOR2(x + width, y);
			points[1][1] = D3DXVECTOR2(x + width - (width / 4.5f)* alpha, y);
			points[1][2] = D3DXVECTOR2(x + width, y + (height / 5.f)* alpha);


			points[2][0] = D3DXVECTOR2(x, y + height);
			points[2][1] = D3DXVECTOR2(x + (width / 4.5f)* alpha, y + height);
			points[2][2] = D3DXVECTOR2(x, y + height - (height / 5.f)* alpha);

			points[3][0] = D3DXVECTOR2(x + width, y + height);
			points[3][1] = D3DXVECTOR2(x + width - (width / 4.5f)* alpha, y + height);
			points[3][2] = D3DXVECTOR2(x + width, y + height - (height / 5.f)* alpha);
		}break;
		default: break;
		}





		DrawLineFast(points[0][0].x, points[0][0].y, points[0][1].x, points[0][1].y, COLOR, pDevice);
		DrawLineFast(points[0][0].x, points[0][0].y, points[0][2].x, points[0][2].y, COLOR, pDevice);

		DrawLineFast(points[1][0].x, points[1][0].y, points[1][1].x, points[1][1].y, COLOR, pDevice);
		DrawLineFast(points[1][0].x, points[1][0].y, points[1][2].x, points[1][2].y, COLOR, pDevice);

		DrawLineFast(points[2][0].x, points[2][0].y, points[2][1].x, points[2][1].y, COLOR, pDevice);
		DrawLineFast(points[2][0].x, points[2][0].y, points[2][2].x, points[2][2].y, COLOR, pDevice);

		DrawLineFast(points[3][0].x, points[3][0].y, points[3][1].x, points[3][1].y, COLOR, pDevice);
		DrawLineFast(points[3][0].x, points[3][0].y, points[3][2].x, points[3][2].y, COLOR, pDevice);
	}


	void DrawCircleFast(float x, float y, int radius, int numSides, D3DCOLOR color, IDirect3DDevice9* pDevice)
	{
		float step = (float)(D3DX_PI * 2.0 / numSides);

		if (step <= 0)
			return;

		for (float a = 0; a < D3DX_PI*2.0; a += step)
		{
			float X1 = radius * cosf(a) + x;
			float Y1 = radius * sinf(a) + y;
			float X2 = radius * cosf(a + step) + x;
			float Y2 = radius * sinf(a + step) + y;

			DrawLineFast(X1, Y1, X2, Y2, color, pDevice);
		}
	}




}


bool D9Visuals::canUseSetupBones = false;
int D9Visuals::PlayerAlpha[65];
int D9Visuals::PlayerNickVal[65];
float D9Visuals::PlayerBoxAlpha[65];
#define PlayerReadyAlpha(x) PlayerAlpha[x->GetIndex()]

void DrawStringWithFontW(LPD3DXFONT fnt, float x, float y, D3DCOLOR color, wchar_t *format, ...)
{
	wchar_t buffer[256];
	RECT fontRect = { (int)x, (int)y, (int)x, (int)y };

	va_list va_argList;

	va_start(va_argList, format);
	wvsprintfW(buffer, format, va_argList);
	va_end(va_argList);

	fnt->DrawTextW(NULL, buffer, -1, &fontRect, DT_NOCLIP, color);
}





void DrawString(float x, float y, D3DCOLOR color, char *format, ...)
{
	char buffer[256];
	RECT fontRect = { (int)x, (int)y, (int)x, (int)y };

	va_list va_argList;

	va_start(va_argList, format);
	wvsprintf(buffer, format, va_argList);
	va_end(va_argList);

	Menuxd::fntVerdana10->DrawText(NULL, buffer, strlen(buffer), &fontRect, DT_NOCLIP, color);
}
FORCEINLINE float DotProductESP(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

bool SDK::CBaseEntity::IsVisibleVector(Vector bone)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!this || !local_player)
		return false;


	Ray_t ray;
	trace_t tr;
	ray.Init(local_player->GetEyePosition(), bone);
	CTraceFilter traceFilter;
	traceFilter.pSkip1 = local_player;
	g_csgo::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

	return (tr.m_pEnt == this || tr.IsVisible());
}

bool SDK::CBaseEntity::IsVisible(int bone)
{
	if (!this)
		return false;
	Ray_t ray;
	trace_t tr;
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	ray.Init(local_player->GetEyePosition(), this->GetBonePosition(bone));
	CTraceFilter traceFilter;
	traceFilter.pSkip1 = local_player;
	g_csgo::Trace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);
	return (tr.m_pEnt == this || tr.IsVisible());
}


bool D9Visuals::Filter(SDK::CBaseEntity *entity, bool& IsVis)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (!entity || !local_player) return true;
	if (!entity->IsAlive()) return true;
	if (entity->GetTeam() == local_player->GetTeam() && !settings::options.Visuals.Visuals_EspTeam) return true;
	if (entity->GetTeam() != 2 && entity->GetTeam() != 3) return true;

	IsVis = (entity->IsVisible(8) && !entity->GetIsDormant());
	if (settings::options.Visuals.Visuals_VisableOnly && !IsVis)return true;
	return false;
}



std::string SanitizeNameFST(char *name, int pEntity)
{
	name[127] = '\0';

	std::string tmp(name);

	for (size_t i = 0; i < tmp.length(); i++)
	{
		if ((
			tmp[i] >= 'a' && tmp[i] <= 'z' ||
			tmp[i] >= 'A' && tmp[i] <= 'Z' ||
			tmp[i] >= '0' && tmp[i] <= '9' ||
			tmp[i] == ' ' || tmp[i] == '.' || tmp[i] == '/' || tmp[i] == ':' ||
			tmp[i] == ',' || tmp[i] == '_' || tmp[i] == '#' || tmp[i] == '$' ||
			tmp[i] == '<' || tmp[i] == '>' || tmp[i] == '-' || tmp[i] == '+' ||
			tmp[i] == '*' || tmp[i] == '%' || tmp[i] == '@' || tmp[i] == '(' ||
			tmp[i] == ')' || tmp[i] == '{' || tmp[i] == '}' || tmp[i] == '[' || tmp[i] == ']' ||
			tmp[i] == '!' || tmp[i] == '&' || tmp[i] == '~' || tmp[i] == '^'
			) == false)
		{
			tmp[i] = '_';
		}
	}
	if (pEntity)
	{
		if (tmp.length() > (size_t)(20 * D9Visuals::PlayerNickVal[pEntity] / 100))
		{
			tmp.erase((20 * D9Visuals::PlayerNickVal[pEntity] / 100), (tmp.length() - (20 * D9Visuals::PlayerNickVal[pEntity] / 100)));
			tmp.append("...");
		}
	}
	else
	{
		if (tmp.length() > (20))
		{
			tmp.erase(20, (tmp.length() - 20));
			tmp.append("...");
		}
	}
	return tmp;
}

void D9Visuals::DrawName(SDK::CBaseEntity *entity)
{
	//	if (entity->IsDormant() && !CanShowDormant(entity) || entity == G::LocalPlayer)
	//		return;

	SDK::player_info_t info;
	g_csgo::Engine->GetPlayerInfo(entity->GetIndex(), &info);


	float height, width, x, y;

	if (GetPlayerBox(entity, x, y, width, height))
	{
		//	int da = GetDormantAlpha(entity) + 55;
		float _x = x;
		float _y = y;

		auto outlineColor = D3D_COLOR_BLACK(PlayerReadyAlpha(entity));
		auto textColor = D3D_COLOR_WHITE(PlayerReadyAlpha(entity));

		std::string name = SanitizeNameFST(info.szName, entity->GetIndex());

		_x += (width / 2.f) - Menuxd::GetTextWitdh((char*)name.c_str(), Menuxd::fntVerdana10) / 2.f;
		_y -= 8.f;
		DrawString(_x, _y - 3.f, outlineColor, "%s", (char*)name.c_str());
		DrawString(_x, _y - 2.f, textColor, "%s", (char*)name.c_str());

	}
}





bool D9Visuals::GetPlayerBox(SDK::CBaseEntity *entity, float &x, float &y, float &width, float &height, Vector offset)
{
	Vector top, down, s[2];

	Vector adjust = Vector(0, 0, -16) * entity->GetEyeAngles();

	down = entity->GetAbsOrigin()/*G::absOriginCache[entity->GetIndex()]*/ - Vector(0, 0, 1);//entity->GetRenderOrigin(); //entity->GetAbsOrigin();//entity->GetNetworkOrigin();
	top = down + Vector(0, 0, 72) + offset + adjust;

	if (Menuxd::WorldToScreen(top, s[1]) && Menuxd::WorldToScreen(down, s[0]))
	{
		Vector delta = s[1] - s[0];

		height = fabsf(delta.y);
		width = height / 2.0f;

		x = s[1].x - (width / 2);
		y = s[1].y;

		return true;
	}

	return false;
}


void D9Visuals::DrawHealth(SDK::CBaseEntity *entity, IDirect3DDevice9* pDevice)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	bool enemy = entity->GetTeam() != local_player->GetTeam() || settings::options.Visuals.Visuals_EspTeam;

	if (!enemy && !settings::options.Visuals.Visuals_EspTeam)
		return;

	float height, width, x, y;

	if (GetPlayerBox(entity, x, y, width, height))
	{
		//int da = GetDormantAlpha(entity) + 55;
		float boxWidth = 5;
		float padd = 1;
		float _x = x;
		float _y = y;



		DWORD outlineColor = D3D_COLOR_BLACK(PlayerReadyAlpha(entity) / 2);
		DWORD textColor = D3D_COLOR_WHITE(PlayerReadyAlpha(entity));

		static int OldHealth[65];
		static bool Animate[65];
		static float Curtime[65];
		int ID = entity->GetIndex();
		_x -= padd;

		float healthBarHeight = (entity->GetHealth() * height * PlayerBoxAlpha[entity->GetIndex()] / 100.0f);
		float healthBarHeight2 = (OldHealth[entity->GetIndex()] * height * PlayerBoxAlpha[entity->GetIndex()] / 100.0f);

		if (OldHealth[entity->GetIndex()] > 100)
		{
			healthBarHeight2 = (OldHealth[entity->GetIndex()] * height * PlayerBoxAlpha[entity->GetIndex()] / OldHealth[entity->GetIndex()]);
		}
		DWORD lineColor = D3DCOLOR_ARGB(PlayerReadyAlpha(entity), 153, min(255, OldHealth[entity->GetIndex()] * 225 / 100), 0);


		Menuxd::DrawFilledRectangle(_x - boxWidth, _y, height * PlayerBoxAlpha[entity->GetIndex()], boxWidth, outlineColor, pDevice);
		//	render->DrawFilledRectangle(_x - boxWidth + 1, _y + (height - healthBarHeight) + 1, healthBarHeight - 2, boxWidth - 2, lineColor);
		Menuxd::DrawFilledRectangle(_x - boxWidth + 1, _y + (height * PlayerBoxAlpha[entity->GetIndex()] - healthBarHeight2) + 1, healthBarHeight2 - 2, boxWidth - 2, lineColor, pDevice);

		if (settings::options.Visuals.Visuals_HealthBarType == 1)
		{
			for (int i = 1; i < 4; i++)
				Menuxd::DrawLineFast(_x - 1, _y + i * (height * PlayerBoxAlpha[entity->GetIndex()] / 4.0f), _x - boxWidth + 1, _y + i * (height * PlayerBoxAlpha[entity->GetIndex()] / 4.0f), outlineColor, pDevice);
		}

		if (entity->GetHealth() < 100)
		{
			char health[6];
			sprintf_s(health, 6, "%d", OldHealth[entity->GetIndex()]);

			_x -= Menuxd::GetTextWitdh(health, Menuxd::fntVerdana9) - 4;
			_y = y + (height * PlayerBoxAlpha[entity->GetIndex()] - healthBarHeight2) + 1;

			Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y + 1, outlineColor, "%s", health);
			Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y, textColor, "%s", health);
		}


		if (OldHealth[ID] <= entity->GetHealth())
		{
			OldHealth[ID] = entity->GetHealth();
			Animate[ID] = false;
		}


		if (OldHealth[ID] > entity->GetHealth())
		{
			if (!Animate[ID])
			{
				Curtime[ID] = g_csgo::Globals->curtime;
				Animate[ID] = true;
			}
			do
			{
				OldHealth[ID] -= 0.07;

			} while (Curtime[ID] - g_csgo::Globals->curtime > 0.44f && Animate[ID]);
		}
	}
}






void D9Visuals::DrawBox(SDK::CBaseEntity *entity, int cType, IDirect3DDevice9* pDevice)
{
	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

	if (entity == local_player)
		return;



	float height, width, x, y;


	if (GetPlayerBox(entity, x, y, width, height))
	{
		x += 1;
		y += 1;
		width -= 2;
		height -= 2;


		DWORD boxColor = D3D_COLOR_WHITE(PlayerReadyAlpha(entity));
		DWORD outlineColor = D3D_COLOR_BLACK(PlayerReadyAlpha(entity));


		bool IsVisible = entity->IsVisible(6);

		if (IsVisible)
		{
			if (entity->GetTeam() == 3)
			{
				boxColor = D3DCOLOR_ARGB(PlayerReadyAlpha(entity), int(settings::options.Visuals.BoxColorPickCTVIS[0] * 255), int(settings::options.Visuals.BoxColorPickCTVIS[1] * 255), int(settings::options.Visuals.BoxColorPickCTVIS[2] * 255));
			}
			else
			{
				boxColor = D3DCOLOR_ARGB(PlayerReadyAlpha(entity), int(settings::options.Visuals.BoxColorPickTVIS[0] * 255), int(settings::options.Visuals.BoxColorPickTVIS[1] * 255), int(settings::options.Visuals.BoxColorPickTVIS[2] * 255));
			}
		}
		else
		{
			if (entity->GetTeam() == 3)
			{
				boxColor = D3DCOLOR_ARGB(PlayerReadyAlpha(entity), int(settings::options.Visuals.BoxColorPickCTINVIS[0] * 255), int(settings::options.Visuals.BoxColorPickCTINVIS[1] * 255), int(settings::options.Visuals.BoxColorPickCTINVIS[2] * 255));
			}
			else
			{
				boxColor = D3DCOLOR_ARGB(PlayerReadyAlpha(entity), int(settings::options.Visuals.BoxColorPickTINVIS[0] * 255), int(settings::options.Visuals.BoxColorPickTINVIS[1] * 255), int(settings::options.Visuals.BoxColorPickTINVIS[2] * 255));
			}
		}







		if (settings::options.Visuals.Visuals_BoxEnabled)
		{

			int i = entity->GetIndex();
			Menuxd::DrawCornierBoxFastAlpha(x - 1, y - 1, height + 2, width + 2, outlineColor, PlayerBoxAlpha[i], pDevice);/*DrawBoxFastAlpha*/
			Menuxd::DrawCornierBoxFastAlpha(x, y, height, width, boxColor, PlayerBoxAlpha[i], pDevice);/*DrawBoxFastAlpha*/
			Menuxd::DrawCornierBoxFastAlpha(x + 1, y + 1, height - 2, width - 2, outlineColor, PlayerBoxAlpha[i], pDevice);/*DrawBoxFastAlpha*/
		}
	}
}

void D9Visuals::DrawBone(SDK::CBaseEntity *entity, matrix3x4_t *pBoneToWorldOut, DWORD color, IDirect3DDevice9* pDevice)
{

	SDK::studiohdr_t* pStudioModel = g_csgo::ModelInfo->GetStudioModel((SDK::model_t*)entity->GetModel());

	if (pStudioModel)
	{
		int num = 0;

		for (int i = 0; i < pStudioModel->numbones; i++)
		{
			SDK::mstudiobone_t* pBone = pStudioModel->pBone(i);

			if (!pBone)
				continue;

			if (!(pBone->flags & 256) || pBone->parent == -1)
				continue;

			Vector vBonePos1;

			if (!Menuxd::WorldToScreen(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
				continue;

			Vector vBonePos2;
			if (!Menuxd::WorldToScreen(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
				continue;


			Menuxd::DrawLineFast(vBonePos1.x, vBonePos1.y, vBonePos2.x, vBonePos2.y, color, pDevice);
		}
	}
}




wchar_t GetWeaponIcon(SDK::CBaseWeapon *weapon)
{
	int code = 0xE000 + max(0, min(500, weapon->GetItemDefenitionIndex()));

	return (wchar_t)(code);
}
void DrawSnapLine(Vector to, D3DCOLOR clr, int width, int height, IDirect3DDevice9* pDevice)
{
	Vector From((width / 2), height - 1, 0);
	Menuxd::DrawLineFast(From.x, From.y, to.x, to.y, clr, pDevice);
}
/*




void D9Visuals::DrawWeapon(SDK::CBaseEntity *entity, IDirect3DDevice9* pDevice)
{
auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());


bool enemy = entity->GetTeam() != local_player->GetTeam() || settings::options.Visuals.Visuals_EspTeam;

if (!enemy && !settings::options.Visuals.Visuals_EspTeam)
return;


SDK::CBaseWeapon *w = GetActiveBaseCombatWeaponxd(entity);

if (!w)
return;



float height, width, x, y;

if (GetPlayerBox(entity, x, y, width, height))
{
//	int da = GetDormantAlpha(entity) + 55;
float boxHeight = 5;
float padd = 1;
float _x = x;
float _y = y;

DWORD outlineColor = D3D_COLOR_BLACK(PlayerReadyAlpha(entity));
DWORD textColor = D3D_COLOR_WHITE(PlayerReadyAlpha(entity));
DWORD textColorScoped = D3D_COLOR_ORANGE(PlayerReadyAlpha(entity));
DWORD textColorFaked = D3D_COLOR_GREEN(PlayerReadyAlpha(entity));
DWORD textColorPENT = D3D_COLOR_RED(PlayerReadyAlpha(entity));
DWORD textColorEmpty = D3D_COLOR_YELLOW(PlayerReadyAlpha(entity));
DWORD textColorDefuser = D3D_COLOR_BLUE(PlayerReadyAlpha(entity));


DWORD lineColor = D3D_COLOR_WHITE(PlayerReadyAlpha(entity));








char ammo[20];
sprintf_s(ammo, 20, "%d/%d", w->ammo(), w->ammo2());



_x = x + width + padd;
_y = y;
if (settings::options.Visuals.Visuals_AimLines)
{
DrawSnapLine(Vector(x + (width / 2), y + height, 0), D3D_COLOR_RED(PlayerReadyAlpha(entity)), Menu::viewPort.Width, Menu::viewPort.Height, pDevice);
}

if (settings::options.Visuals.Visuals_Weapons && settings::options.Visuals.Visuals_WeaponsType == 0)
{
Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y + 1, outlineColor, "%s", XorStr((char*)w->GetWeaponName().c_str()));
Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y, textColorEmpty, "%s", XorStr((char*)w->GetWeaponName().c_str()));
_y += 7.f;
}



if (settings::options.Visuals.Visuals_ArmorBar)
{
if (entity->GetArmor() > 0)
{
if (entity->HasHelmet())
{
Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y + 1, outlineColor, "%s", ("2 ARMOR"));
Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y, textColorFaked, "%s", ("2 ARMOR"));
}
else
{
Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y + 1, outlineColor, "%s", ("1 ARMOR"));
Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y, textColorFaked, "%s", ("1 ARMOR"));
}
}
else
{
Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y + 1, outlineColor, "%s", ("NO ARMOR"));
Menuxd::DrawStringWithFont(Menuxd::fntVerdana9, _x, _y, textColorFaked, "%s", ("NO ARMOR"));
}
_y += 7.f;
}





int NewX = x;
int NewY = y + height;

if (settings::options.Visuals.Visuals_AmmoESP && settings::options.Visuals.Visuals_AmmoESPType == 1)
{


Menuxd::DrawFilledRectangle(NewX, NewY, 6, (width) * (PlayerReadyAlpha(entity) / 255), D3DCOLOR_ARGB(PlayerReadyAlpha(entity), 5, 5, 5), pDevice);
if (w->ammo() >= 1 && !w->IsReloading())
{
Menuxd::DrawFilledRectangle(NewX + 1, NewY + 1, 4, ((width - 2.f) * w->ammo() / w->BulletSize()) * (PlayerReadyAlpha(entity) / 255), D3DCOLOR_ARGB(PlayerReadyAlpha(entity), 83, 140, 232), pDevice);
}
else
{
Menuxd::DrawFilledRectangle(NewX + 1, NewY + 1, 4, (width - 2.f) * (PlayerReadyAlpha(entity) / 255), D3DCOLOR_ARGB(PlayerReadyAlpha(entity), 255, 165, 0), pDevice);
}


NewY += 7;
}

if (settings::options.Visuals.Visuals_Weapons && settings::options.Visuals.Visuals_WeaponsType == 1)
{
wchar_t weapIcon[2] = { GetWeaponIcon(w), '\0' };

_x = x + (width - Menuxd::GetTextWitdhW(weapIcon, Menuxd::fntWeaponIcon)) / 2;
_y = y + height + 7;

DrawStringWithFontW(Menuxd::fntWeaponIcon, _x, NewY + 1, outlineColor, L"%s", weapIcon);
DrawStringWithFontW(Menuxd::fntWeaponIcon, _x, NewY, textColor, L"%s", weapIcon);
}
}
}
*/

void D9Visuals::Render(IDirect3DDevice9* pDevice)
{


	auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());



	if (!g_csgo::Engine->IsInGame() || !g_csgo::Engine->IsConnected() || !local_player)
		return;

	/*
	if (settings::options.Visuals.Visuals_DrawLinesAA)
	{
	DrawLinesAA();
	}
	*/




	DamageESP::Draw();

	for (int i = 1; i < g_csgo::Engine->GetMaxClients(); i++)
	{
		auto entity = g_csgo::ClientEntityList->GetClientEntity(i);

		bool Visable;

		//PlayerAlpha[i] = 255;
		//	PlayerBoxAlpha[i] = 1.0f;
		//	PlayerNickVal[i] = 100;

		if (!Filter(entity, Visable))
		{

			if (entity->GetIsDormant())
			{
				if (PlayerAlpha[i] != 0)
					PlayerAlpha[i] -= 1.5;

				if (PlayerBoxAlpha[i] != 0.f)
					PlayerBoxAlpha[i] -= 0.01f;

				if (PlayerNickVal[i] != 0)
					PlayerNickVal[i] -= 2;

			}
			else
			{
				if (PlayerAlpha[i] != 255)
					PlayerAlpha[i] += 5;

				if (PlayerBoxAlpha[i] != 1.f)
					PlayerBoxAlpha[i] += 0.05f;

				if (PlayerNickVal[i] != 100)
					PlayerNickVal[i] += 2;

			}
			if (PlayerBoxAlpha[i] < 0.f)
				PlayerBoxAlpha[i] = 0.f;
			if (PlayerBoxAlpha[i] > 1.f)
				PlayerBoxAlpha[i] = 1.f;
			if (PlayerAlpha[i] < 0)
				PlayerAlpha[i] = 0;
			if (PlayerAlpha[i] > 255)
				PlayerAlpha[i] = 255;
			if (PlayerNickVal[i] < 0)
				PlayerNickVal[i] = 0;
			if (PlayerNickVal[i] > 100)
				PlayerNickVal[i] = 100;

			int typecolor = entity->GetTeam() == 3 ? Visable ? 0 : 1 : Visable ? 2 : 3;


			if (settings::options.Visuals.Visuals_Name) DrawName(entity);

			if (settings::options.Visuals.Visuals_BoxEnabled) DrawBox(entity, typecolor, pDevice);
			if (settings::options.Visuals.Visuals_HealthBar) DrawHealth(entity, pDevice);
			

			//DrawWeapon(entity, pDevice);


		}
	}
	/*
	for (int i = 65; i < g_csgo::ClientEntityList->GetHighestEntityIndex(); i++)
	{
	auto pEntity = g_csgo::ClientEntityList->GetClientEntity(i);
	if (!pEntity)
	continue;
	if (!items::pLocal)
	continue;
	if (pEntity == items::pLocal)
	continue;
	if (pEntity->GetIsDormant())
	continue;

	if (settings::options.Visuals.Visuals_C4 && strstr(pEntity->GetClientClass()->m_pNetworkName, "CPlantedC4"))
	{
	if (!pEntity->BombDefused())
	{
	float flTime = pEntity->BombTimer() - g_csgo::Globals->curtime;
	if (pEntity->BombTimer() - g_csgo::Globals->curtime > 0.f)
	{
	Vector vPos;
	if (Menu::WorldToScreen(pEntity->GetAbsOrigin(), vPos))
	{
	bool bCanDefuse = (items::pLocal->HasKit() && flTime > 5.f) || flTime > 10.f;
	static char msg[32];
	sprintf_s(msg, 32, "C4 | %.2f", flTime);

	Menu::DrawStringWithFont(Menu::fntVerdana10, vPos.x, vPos.y, D3D_COLOR_BLACK(255), "%s", msg);
	Menu::DrawStringWithFont(Menu::fntVerdana10, vPos.x, vPos.y + 1, bCanDefuse ? D3D_COLOR_WHITE(255) : D3D_COLOR_ORANGE(255), "%s", msg);
	}
	}
	}
	else
	{
	Vector vPos;
	if (Menu::WorldToScreen(pEntity->GetAbsOrigin(), vPos))
	{
	static char msg[32];
	sprintf_s(msg, 32, "C4 | DEFUSED");
	Menu::DrawStringWithFont(Menu::fntVerdana10, vPos.x, vPos.y, D3D_COLOR_BLACK(255), "%s", msg);
	Menu::DrawStringWithFont(Menu::fntVerdana10, vPos.x, vPos.y + 1, D3D_COLOR_RED(255), "%s", msg);
	}
	}
	}




	if (settings::options.Visuals.Visuals_DroppedWeapons && pEntity->IsDefusing())
	{

	auto weapon = reinterpret_cast<Shonax_SDK::CBaseWeapon*>(pEntity);
	auto plr = g_csgo::ClientEntityList->GetClientEntityFromHandle(weapon->GetOwnerHandle());
	Vector vOrig3D = pEntity->GetAbsOrigin(), vOrig;

	if (Menu::WorldToScreen(vOrig3D, vOrig))
	{
	Shonax_SDK::player_info_t info;
	if (plr == nullptr || !plr->IsAlive() || !(plr->GetHealth() > 0) || !(g_csgo::Engine->GetPlayerInfo(plr->GetIndex(), &info)))
	{
	if (!(vOrig3D.y >= -5 && vOrig3D.y <= 5 && vOrig3D.x >= -5 && vOrig3D.x <= 5))
	{
	DWORD outlineColor = D3D_COLOR_BLACK(255);
	DWORD textColor = D3D_COLOR_WHITE(255);
	Menu::DrawStringWithFont(Menu::fntVerdana9, vOrig.x, vOrig.y + 1, outlineColor, "%s", (char*)weapon->GetWeaponName().c_str());
	Menu::DrawStringWithFont(Menu::fntVerdana9, vOrig.x, vOrig.y, textColor, "%s", (char*)weapon->GetWeaponName().c_str());
	}
	}
	}

	}

	}*/

}




namespace DamageESP
{
	std::array<FloatingText, MAX_FLOATING_TEXTS> floatingTexts;
	int floatingTextsIdx = 0;

	void HandleGameEvent(SDK::IGameEvent* pEvent)
	{
		auto local_player = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetLocalPlayer());

		if (!settings::options.Visuals.Visuals_DamageESP || !(g_csgo::Engine->IsInGame() && g_csgo::Engine->IsConnected() && local_player))
			return;

		const char *name = pEvent->GetName();

		static Vector lastImpactPos = Vector(0, 0, 0);

		if (!strcmp(name, ("player_hurt")))
		{
			float curTime = g_csgo::Globals->curtime;

			int userid = pEvent->GetInt(("userid"));
			int attackerid = pEvent->GetInt(("attacker"));
			int dmg_health = pEvent->GetInt(("dmg_health"));
			int hitgroup = pEvent->GetInt(("hitgroup"));

			SDK::CBaseEntity *entity = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(userid));
			SDK::CBaseEntity *attacker = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(attackerid));

			if (!entity || attacker != local_player)
				return;

			FloatingText txt;
			txt.startTime = curTime;
			txt.hitgroup = hitgroup;
			txt.hitPosition = lastImpactPos;
			txt.damage = dmg_health;
			txt.randomIdx = rand() % 5;
			txt.valid = true;

			floatingTexts[floatingTextsIdx++ % MAX_FLOATING_TEXTS] = txt;
		}
		else if (!strcmp(name, ("bullet_impact")))
		{
			int userid = pEvent->GetInt(("userid"));
			float x = pEvent->GetFloat(("x"));
			float y = pEvent->GetFloat(("y"));
			float z = pEvent->GetFloat(("z"));

			SDK::CBaseEntity *entity = g_csgo::ClientEntityList->GetClientEntity(g_csgo::Engine->GetPlayerForUserID(userid));

			if (!entity || entity != local_player)
				return;

			lastImpactPos = Vector(x, y, z);
		}
	}

	void Draw()
	{
		if (!settings::options.Visuals.Visuals_DamageESP)
			return;


		for (int i = 0; i < MAX_FLOATING_TEXTS; i++)
		{
			FloatingText *txt = &floatingTexts[i % MAX_FLOATING_TEXTS];

			if (!txt->valid)
				continue;

			float endTime = txt->startTime + 1.1f;

			if (endTime < g_csgo::Globals->curtime)
			{
				txt->valid = false;
				continue;
			}

			Vector screen;

			if (Menuxd::WorldToScreen(txt->hitPosition, screen))
			{
				float t = 1.0f - (endTime - g_csgo::Globals->curtime) / (endTime - txt->startTime);

				screen.y -= t * (35.0f);
				screen.x -= (float)txt->randomIdx * t * 3.0f;

				char msg[12];
				sprintf_s(msg, 12, "-%dHP", txt->damage);

				int width = Menuxd::GetTextWitdh(msg, Menuxd::fntVerdana10);

				Menuxd::DrawStringWithFont(Menuxd::fntVerdana10, screen.x - width / 2, screen.y - 11 + 1, D3D_COLOR_BLACK((int)((1.0f - t) * (float)255)), msg);

				Menuxd::DrawStringWithFont(Menuxd::fntVerdana10, screen.x - width / 2, screen.y - 11, D3DCOLOR_ARGB(int(settings::options.Visuals.DamageESPColor[3] * 255), int(settings::options.Visuals.DamageESPColor[0] * 255), int(settings::options.Visuals.DamageESPColor[1] * 255), int(settings::options.Visuals.DamageESPColor[2] * 255)), msg);
			}
		}
	}
};



