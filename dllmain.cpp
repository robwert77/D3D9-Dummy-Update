#include "includes.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <math.h>

#pragma warning(disable : 4996)
// data
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
extern LPDIRECT3DDEVICE9 pDevice = nullptr;
Hack* hack;
#include <windows.h>

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice) {

	if (!pDevice)
		pDevice = o_pDevice;

	static bool imguiInitialized = false;
	if (!imguiInitialized) {
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init(GetProcessWindow());
		ImGui_ImplDX9_Init(pDevice);
		ImGui::StyleColorsDark();
		imguiInitialized = true;
	}

	ImGuiIO& io = ImGui::GetIO();
	POINT mousePos;
	GetCursorPos(&mousePos);
	io.MousePos = ImVec2((float)mousePos.x, (float)mousePos.y);
	io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON);
	io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON);
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	int menuOffX = windowWidth / 2;
	int menuOffY = 50;
	D3DCOLOR enabled = D3DCOLOR_ARGB(255, 0, 255, 0);
	D3DCOLOR disabled = D3DCOLOR_ARGB(255, 255, 0, 0);

	if (hack->settings.showMenu) {
		// Set up ImGui menu with custom colors and no resize or collapse
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.9f)); // Dark grey background
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.8f, 0.2f, 0.2f, 0.3f)); // Red frames
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White text

		ImGui::Begin("Hack Menu", &hack->settings.showMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::SetWindowPos(ImVec2(menuOffX, menuOffY), ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(200, 300), ImGuiCond_Once);

		// ESP options
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "ESP Options"); // Green text
		ImGui::Checkbox("ESP Box", &hack->settings.bEspBox2D);
		ImGui::Checkbox("ESP Line", &hack->settings.bLine);
		ImGui::Checkbox("ESP Text", &hack->settings.bText);

		// Aim and Fire control options
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Aim & Fire Control Options"); // Green text
		ImGui::Checkbox("Aimbot", &hack->settings.bAimbot);
		ImGui::Checkbox("Recoil", &hack->settings.bRecoil);
		ImGui::Checkbox("PowerShoot", &hack->settings.bPowerShoot);

		// Other cheat options
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Other Cheat Options"); // Green text
		ImGui::Checkbox("Cheats", &hack->settings.bCheats);
		ImGui::Checkbox("SpeedHack", &hack->settings.bSpeedHack);

		ImGui::End();
		ImGui::PopStyleColor(3); // Remember to pop each style color you pushed to reset them!
	}


	if (hack->settings.bRecoil)
	{
		hack->Recoil();
	}

	if (hack->settings.bPowerShoot)
	{
		hack->PowerShoot();
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

	if (hack->settings.bAimbot) {
		hack->aimbot();
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
				// aimbot the closest entity to my crosshai
	
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

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

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

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// uninject
	FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpr) {
	if (reason == DLL_PROCESS_ATTACH)
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0));
	return TRUE;
}