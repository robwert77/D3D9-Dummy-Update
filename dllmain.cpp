#include "includes.h"
#include <iostream>
#include <sstream>
#include <string.h>

#pragma warning(disable : 4996)
// data
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
Hack* hack;

// hook function
void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {

	if (!pDevice)
		pDevice = o_pDevice;

	DrawText("WOW ESP", windowWidth / 2, windowHeight - 20, D3DCOLOR_ARGB(255, 255, 255, 255));

	int menuOffX = windowWidth / 2;
	int menuOffY = 50;
	D3DCOLOR enabled = D3DCOLOR_ARGB(255, 0, 255, 0);
	D3DCOLOR disabled = D3DCOLOR_ARGB(255, 255, 0, 0);

	if (!hack->settings.showMenu) {
		DrawText("Show Menu (INS)", menuOffX, menuOffY, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else {
		DrawText("Snaplines (F1)", menuOffX, menuOffY + 1 * 12, hack->settings.bLine ? enabled : disabled);
		DrawText("2D Box (F2)", menuOffX, menuOffY + 2 * 12, hack->settings.bEspBox2D ? enabled : disabled);
		DrawText("Health Text (F3)", menuOffX, menuOffY + 3 * 12, hack->settings.bText ? enabled : disabled);
		DrawText("Crosshair (F4)", menuOffX, menuOffY + 4 * 12, hack->settings.showCross ? enabled : disabled);
		DrawText("Cheats (F5)", menuOffX, menuOffY + 5 * 12, hack->settings.bCheats ? enabled : disabled);
		DrawText("Speed Hack (F6)", menuOffX, menuOffY + 6 * 12, hack->settings.bSpeedHack ? enabled : disabled);
		DrawText("Hide Menu (INS)", menuOffX, menuOffY + 7 * 12, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	if (hack->settings.bCheats)
	{
		hack->cheatEn();
	}

	if (hack->settings.bSpeedHack)
	{
		hack->Cheats.speedAdr = 0x021D77B8;
		int* speed = (int*)hack->Cheats.speedAdr;
		*speed = 500;
	}
	if (!hack->settings.bSpeedHack)
	{
		hack->Cheats.speedAdr = 0x021D77B8;
		int* speed = (int*)hack->Cheats.speedAdr;
		*speed = 190;
	}

	for (int i = 0; i < 32; i++) {

		Ent* curEnt = hack->entList->ents[i].ent;

		if (!hack->CheckEnt(curEnt))
			continue;

		D3DCOLOR color;
		color = D3DCOLOR_ARGB(255, 255, 0, 0);

		Vec2 entPos2D, entHead2D;

		Vec3 headPos3d = curEnt->headOrigin;

		
		headPos3d.y -= 15.9;
		headPos3d.x -= 15.9;
		
		if (hack->WorldToScreen(curEnt->vecOrigin, entPos2D)) {
			if(hack->settings.bLine)
				DrawLine(entPos2D.x, entPos2D.y + 15, windowWidth / 2, windowHeight, 2, color);
		}
		if (hack->WorldToScreen(headPos3d, entHead2D)) {
			if (hack->settings.bEspBox2D)
			{
				DrawEspBox2D(entPos2D, entHead2D, 2, color);

				int height = ABS(entPos2D.y - entHead2D.y);
				int dx = entPos2D.x - entHead2D.x;

				float healthPrecnt = curEnt->health / curEnt->roundHealth;

				Vec2 botHealth, topHealth;

				float healthHeight = height * healthPrecnt;

				botHealth.y = entPos2D.y;

				botHealth.x = entPos2D.x - (height / 4) - 6;

				topHealth.y = entHead2D.y + height - healthHeight;

				topHealth.x = entPos2D.x - (height / 4) - 6 - (dx * healthPrecnt);

				DrawLine(botHealth, topHealth, 1.7, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
		}
		if (hack->settings.bText) {
			std::stringstream s1;
			s1 << curEnt->healthInt;
			std::string t1 = "HP: " + s1.str();
			char* healthMSG = (char*)t1.c_str();

			DrawText(healthMSG, entPos2D.x, entPos2D.y , D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
	if (hack->settings.showCross)
	{
		DrawFilledRect(windowWidth / 2 - 2, windowHeight / 2 - 2, 4, 4, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	// call og function
	oEndScene(pDevice);
}

DWORD WINAPI HackThread(HMODULE hModule) {

	// hook
	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
	}

	hack = new Hack();
	hack->Init();

	// hack loop
	while (!GetAsyncKeyState(VK_END)) {
		hack->Update();
	}

	// unhook
	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);

	// uninject
	FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr) {
	if (reason == DLL_PROCESS_ATTACH)
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
	return TRUE;
}