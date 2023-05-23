#include "includes.h"

Hack::~Hack() {
	this->Font->Release();
	this->LineL->Release();
}

void Hack::Init() {
	entList = (EntList*)(dwEntityList);	
	localEnt = entList->ents[0].ent;
}

void Hack::CheckButtons() {
	if (GetAsyncKeyState(Buttons.showMenu) & 1) {
		settings.showMenu = !settings.showMenu;
	}
	if (GetAsyncKeyState(Buttons.bText) & 1) {
		settings.bText = !settings.bText;
	}
	if (GetAsyncKeyState(Buttons.bLine) & 1) {
		settings.bLine = !settings.bLine;
	}
	if (GetAsyncKeyState(Buttons.bEspBox2D) & 1) {
		settings.bEspBox2D = !settings.bEspBox2D;
	}
	if (GetAsyncKeyState(Buttons.showCross) & 1) {
		settings.showCross = !settings.showCross;
	}
	if (GetAsyncKeyState(Buttons.bCheats) & 1) {
		settings.bCheats = !settings.bCheats;
	}
	if (GetAsyncKeyState(Buttons.bSpeedHack) & 1) {
		settings.bSpeedHack = !settings.bSpeedHack;
	}
}

void Hack::cheatEn() {
	int* points = Cheats.pointsAdr;
	*points = 999999;

	for (DWORD i = 0; i < 31; i++) {
		Cheats.weaponBase + 4 * i + 0x5FC;
		*(int*)(Cheats.weaponBase + 4 * i + 0x5FC) = 1024;
	}
}


void Hack::Update() {
	memcpy(&viewMatrix, (PBYTE*)(client + dwViewMatrix), sizeof(viewMatrix)); // updating the view matrix
	this->CheckButtons();
}

bool Hack::CheckEnt(Ent* ent) {
	if(ent == nullptr)
		return false;
	if(ent->health <= 0)
		return false;
	if((isnan(ent->health)))
		return false;
	if(ent->Test < 2)
		return false;
	return true;
}
bool Hack::WorldToScreen(Vec3 pos, Vec2& screen) {
	Vec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	Vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

