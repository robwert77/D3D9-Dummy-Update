#include "includes.h"

extern Hack* hack;

// filled rectangle
void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x,y,x + w,y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color) {
	ID3DXLine* LineL;
	D3DXCreateLine(pDevice, &LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	LineL->SetWidth(thickness);
	LineL->Draw(Line, 2, color);
	LineL->Release();
}

void DrawLine(Vec2 src, Vec2 dst, int thickness, D3DCOLOR color) {
	DrawLine(src.x, src.y, dst.x, dst.y, thickness, color);
}

void DrawEspBox2D(Vec2 top, Vec2 bot, int thickness, D3DCOLOR colorL) {
	int height = ABS(top.y - bot.y);
	int cornerLength = height / 4;  // Set corner length

	// Calculate corner points
	Vec2 tl, tr, bl, br;
	tl.x = top.x - cornerLength;
	tr.x = top.x + cornerLength;
	tl.y = tr.y = top.y;
	bl.x = bot.x - cornerLength;
	br.x = bot.x + cornerLength;
	bl.y = br.y = bot.y;

	// Top left corner
	DrawLine(tl, Vec2{ tl.x, tl.y - cornerLength }, thickness, colorL);
	DrawLine(tl, Vec2{ tl.x + cornerLength, tl.y }, thickness, colorL);
	// Top right corner
	DrawLine(tr, Vec2{ tr.x, tr.y - cornerLength }, thickness, colorL);
	DrawLine(tr, Vec2{ tr.x - cornerLength, tr.y }, thickness, colorL);
	// Bottom left corner
	DrawLine(bl, Vec2{ bl.x, bl.y + cornerLength  }, thickness, colorL);
	DrawLine(bl, Vec2{ bl.x + cornerLength, bl.y }, thickness, colorL);
	// Bottom right corner
	DrawLine(br, Vec2{ br.x, br.y + cornerLength }, thickness, colorL);
	DrawLine(br, Vec2{ br.x - cornerLength, br.y}, thickness, colorL);
}

// drawTExt
void DrawText(const char* text, float x, float y, D3DCOLOR color) {
	RECT rect;

	if (!hack->Font)
		D3DXCreateFont(pDevice, 14, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &hack->Font);

	SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	hack->Font->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));

	SetRect(&rect, x, y, x, y);
	hack->Font->DrawTextA(NULL, text, -1, &rect, DT_CENTER | DT_NOCLIP, color);
}