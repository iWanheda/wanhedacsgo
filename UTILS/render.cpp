#include "../includes.h"

#include "render.h"
#include "interfaces.h"

#include "../SDK/IMaterial.h"
#include "../SDK/IVDebugOverlay.h"


namespace RENDER
{
	unsigned int CreateF(std::string font_name, int size, int weight, int blur, int scanlines, int flags)
	{
		auto font = g_csgo::Surface->SCreateFont();
		g_csgo::Surface->SetFontGlyphSet(font, font_name.c_str(), size, weight, blur, scanlines, flags);

		return font;
	}
	void TexturedPolygon(int n, std::vector<SDK::Vertex_t> vertice, CColor color)
	{
		static int texture_id = g_csgo::Surface->CreateNewTextureID(true); // 
		static unsigned char buf[4] = { 255, 255, 255, 255 };
		g_csgo::Surface->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
		g_csgo::Surface->DrawSetColor(color); //
		g_csgo::Surface->DrawSetTexture(texture_id); //
		g_csgo::Surface->DrawTexturedPolygon(n, vertice.data()); //
	}
	void DrawF(int X, int Y, unsigned int Font, bool center_width, bool center_height, CColor Color, std::string Input, ...)
	{
		/* char -> wchar */
		size_t size = Input.size() + 1;
		auto wide_buffer = std::make_unique<wchar_t[]>(size);
		mbstowcs_s(0, wide_buffer.get(), size, Input.c_str(), size - 1);

		/* check center */
		int width = 0, height = 0;
		g_csgo::Surface->GetTextSize(Font, wide_buffer.get(), width, height);
		if (!center_width)
			width = 0;
		if (!center_height)
			height = 0;

		/* call and draw*/
		g_csgo::Surface->DrawSetTextColor(Color);
		g_csgo::Surface->DrawSetTextFont(Font);
		g_csgo::Surface->DrawSetTextPos(X - (width * .5), Y - (height * .5));
		g_csgo::Surface->DrawPrintText(wide_buffer.get(), wcslen(wide_buffer.get()), 0);
	}
	void DrawWF(int X, int Y, unsigned int Font, CColor Color, const wchar_t* Input) //std::string for the nn's
	{
		/* call and draw*/
		g_csgo::Surface->DrawSetTextColor(Color);
		g_csgo::Surface->DrawSetTextFont(Font);
		g_csgo::Surface->DrawSetTextPos(X, Y);
		g_csgo::Surface->DrawPrintText(Input, wcslen(Input), 0);
	}
	Vector2D GetTextSize(unsigned int Font, std::string Input)
	{
		/* char -> wchar */
		size_t size = Input.size() + 1;
		auto wide_buffer = std::make_unique<wchar_t[]>(size);
		mbstowcs_s(0, wide_buffer.get(), size, Input.c_str(), size - 1);

		int width, height;
		g_csgo::Surface->GetTextSize(Font, wide_buffer.get(), width, height);

		return Vector2D(width, height);
	}
	void FillRectangle(int x1, int y2, int width, int height, CColor color) {
		g_csgo::Surface->DrawSetTextColor(color);
		g_csgo::Surface->DrawFilledRect(x1, y2, x1 + width, y2 + height);
	}
	void DrawLine(int x1, int y1, int x2, int y2, CColor color)
	{
		g_csgo::Surface->DrawSetColor(color);
		g_csgo::Surface->DrawLine(x1, y1, x2, y2);
	}

	void DrawEmptyRect(int x1, int y1, int x2, int y2, CColor color, unsigned char ignore_flags)
	{
		g_csgo::Surface->DrawSetColor(color);
		if (!(ignore_flags & 0b1))
			g_csgo::Surface->DrawLine(x1, y1, x2, y1);
		if (!(ignore_flags & 0b10))
			g_csgo::Surface->DrawLine(x2, y1, x2, y2);
		if (!(ignore_flags & 0b100))
			g_csgo::Surface->DrawLine(x2, y2, x1, y2);
		if (!(ignore_flags & 0b1000))
			g_csgo::Surface->DrawLine(x1, y2, x1, y1);
	}
	void DrawCornerRect(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const bool outlined, const CColor& color, const CColor& outlined_color)
	{
		auto corner = [&](const int32_t _x, const int32_t _y, const int32_t width, const int32_t height, const bool right_side, const bool down, const CColor& _color, const bool _outlined, const CColor& _outlined_color) -> void
		{
			const auto corner_x = right_side ? _x - width : _x;
			const auto corner_y = down ? _y - height : _y;
			const auto corner_w = down && right_side ? width + 1 : width;

			DrawEmptyRect(corner_x, _y, corner_w, 1, _color);
			DrawEmptyRect(_x, corner_y, 1, height, _color);

			if (_outlined) {
				DrawEmptyRect(corner_x, down ? _y + 1 : _y - 1, !down && right_side ? corner_w + 1 : corner_w, 1, _outlined_color);
				DrawEmptyRect(right_side ? _x + 1 : _x - 1, down ? corner_y : corner_y - 1, 1, down ? height + 2 : height + 1, _outlined_color);
			}
		};

		corner(x - w, y, w / 2, w / 2, false, false, color, outlined, outlined_color);
		corner(x - w, y + h, w / 2, w / 2, false, true, color, outlined, outlined_color);
		corner(x + w, y, w / 2, w / 2, true, false, color, outlined, outlined_color);
		corner(x + w, y + h, w / 2, w / 2, true, true, color, outlined, outlined_color);
	}
	void DrawEdges(float topX, float topY, float bottomX, float bottomY, float length, CColor color)
	{
		float scale = (bottomY - topY) / 5.0f;
		DrawLine(topX - scale, topX - scale + length, topY, topY, color); //  --- Top left
		DrawLine(topX - scale, topX - scale, topY, topY + length, color); // | Top left
		DrawLine(topX + scale, topX + scale + length, topY, topY, color); // --- Top right
		DrawLine(topX + scale + length, topX + scale + length, topY, topY + length, color); // | Top right
		DrawLine(bottomX - scale, topX - scale + length, bottomY, bottomY, color); // --- Bottom left
		DrawLine(bottomX - scale, topX - scale, bottomY, bottomY - length, color); // | Bottom left
		DrawLine(bottomX + scale, topX + scale + length, bottomY, bottomY, color); // --- Bottom right
		DrawLine(bottomX + scale + length, topX + scale + length, bottomY, bottomY - length, color); // | Bottom right
	}
	void DrawFilledRect(int x1, int y1, int x2, int y2, CColor color)
	{
		g_csgo::Surface->DrawSetColor(color);
		g_csgo::Surface->DrawFilledRect(x1, y1, x2, y2);
	}
	void DrawFilledRectOutline(int x1, int y1, int x2, int y2, CColor color)
	{
		g_csgo::Surface->DrawSetColor(color);
		g_csgo::Surface->DrawFilledRect(x1, y1, x2, y2);
		DrawEmptyRect(x1 - 1, y1 - 1, x2, y2, CColor(0, 0, 0, 100));
	}
	void DrawFilledRectArray(SDK::IntRect* rects, int rect_amount, CColor color)
	{
		g_csgo::Surface->DrawSetColor(color);
		g_csgo::Surface->DrawFilledRectArray(rects, rect_amount);
	}

	void DrawCircle(int x, int y, int radius, int segments, CColor color)
	{
		g_csgo::Surface->DrawSetColor(color);
		g_csgo::Surface->DrawOutlinedCircle(x, y, radius, segments);
	}



	void RoundedBoxStatic(int x, int y, int w, int h, int radius, CColor col)
	{
		SDK::Vertex_t round[64];

		for (int i = 0; i < 4; i++)
		{
			int _x = x + ((i < 2) ? (w - radius) : radius);
			int _y = y + ((i % 3) ? (h - radius) : radius);

			float a = 90.f * i;

			for (int j = 0; j < 16; j++)
			{
				float _a = DEG2RAD(a + j * 6.f);

				round[(i * 16) + j] = SDK::Vertex_t(Vector2D(_x + radius * sin(_a), _y - radius * cos(_a)));
			}
		}

		g_csgo::Surface->DrawSetColor(col);
		g_csgo::Surface->DrawTexturedPolygon(64, round);
	}
	/*void DrawFilledCircle(Vector2D center, CColor color, CColor outline, float radius, float points)
	{
		static bool once = true;

		static std::vector<float> temppointsx;
		static std::vector<float> temppointsy;

		if (once)
		{
			float step = (float)M_PI * 2.0f / points;
			for (float a = 0; a < (M_PI * 2.0f); a += step)
			{
				float x = cosf(a);
				float y = sinf(a);
				temppointsx.push_back(x);
				temppointsy.push_back(y);
			}
			once = false;
		}

		std::vector<int> pointsx;
		std::vector<int> pointsy;
		std::vector<SDK::Vertex_t> vertices;

		for (int i = 0; i < temppointsx.size(); i++)
		{
			float x = radius * temppointsx[i] + center.x;
			float y = radius * temppointsy[i] + center.y;
			pointsx.push_back(x);
			pointsy.push_back(y);

			vertices.push_back(SDK::Vertex_t(Vector2D(x, y)));
		}

		TexturedPolygon(vertices.size(), vertices, color);
		INTERFACES::Surface->DrawSetColor(outline);
		INTERFACES::Surface->DrawPolyLine(pointsx.data(), pointsy.data(), points);
	}*/
	void DrawSomething()
	{

	}
	bool WorldToScreen(Vector world, Vector& screen)
	{
		return (g_csgo::DebugOverlay->ScreenPosition(world, screen) != 1);
	}
	void DrawFilled3DBox(Vector origin, int width, int height, CColor outline, CColor filling)
	{
		float difw = float(width / 2);
		float difh = float(height / 2);
		Vector boxVectors[8] =
		{
			Vector(origin.x - difw, origin.y - difh, origin.z - difw),
			Vector(origin.x - difw, origin.y - difh, origin.z + difw),
			Vector(origin.x + difw, origin.y - difh, origin.z + difw),
			Vector(origin.x + difw, origin.y - difh, origin.z - difw),
			Vector(origin.x - difw, origin.y + difh, origin.z - difw),
			Vector(origin.x - difw, origin.y + difh, origin.z + difw),
			Vector(origin.x + difw, origin.y + difh, origin.z + difw),
			Vector(origin.x + difw, origin.y + difh, origin.z - difw),
		};

		static Vector vec0, vec1, vec2, vec3,
			vec4, vec5, vec6, vec7;

		if (WorldToScreen(boxVectors[0], vec0) &&
			WorldToScreen(boxVectors[1], vec1) &&
			WorldToScreen(boxVectors[2], vec2) &&
			WorldToScreen(boxVectors[3], vec3) &&
			WorldToScreen(boxVectors[4], vec4) &&
			WorldToScreen(boxVectors[5], vec5) &&
			WorldToScreen(boxVectors[6], vec6) &&
			WorldToScreen(boxVectors[7], vec7))
		{
			Vector2D lines[12][2];
			lines[0][0] = vec0;
			lines[0][1] = vec1;
			lines[1][0] = vec1;
			lines[1][1] = vec2;
			lines[2][0] = vec2;
			lines[2][1] = vec3;
			lines[3][0] = vec3;
			lines[3][1] = vec0;

			// top of box
			lines[4][0] = vec4;
			lines[4][1] = vec5;
			lines[5][0] = vec5;
			lines[5][1] = vec6;
			lines[6][0] = vec6;
			lines[6][1] = vec7;
			lines[7][0] = vec7;
			lines[7][1] = vec4;

			lines[8][0] = vec0;
			lines[8][1] = vec4;

			lines[9][0] = vec1;
			lines[9][1] = vec5;

			lines[10][0] = vec2;
			lines[10][1] = vec6;

			lines[11][0] = vec3;
			lines[11][1] = vec7;

			for (int i = 0; i < 12; i++)
				DrawLine(lines[i][0].x, lines[i][0].y, lines[i][1].x, lines[i][1].y, outline);
		}
	}
}