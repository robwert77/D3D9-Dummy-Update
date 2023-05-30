#include "includes.h"
#include <sstream>

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
}

void Hack::cheatEn() {
	int* points = Cheats.pointsAdr;
	*points = 999999;

	for (DWORD i = 0; i < 31; i++) {
		Cheats.weaponBase + 4 * i + 0x5FC;
		*(int*)(Cheats.weaponBase + 4 * i + 0x5FC) = 1024;
	}
}


void Hack::PowerShoot() {
	if (settings.bPowerShoot) {
		this->Nop((BYTE*)(Cheats.rapidBaseAdr), 5);
	}
	else {
		Patch((BYTE*)(Cheats.rapidBaseAdr), (BYTE*)"\xE8\x10\xFa\xFF\xFF", 5);
	}
}

void Hack::DrawDistanceToEntity(int entityIndex)
{
	if (entityIndex != -1) {
		Ent* closestEnt = entList->ents[entityIndex].ent;
		Vec2 closestEntPos2D;
		if (WorldToScreen(closestEnt->vecOrigin, closestEntPos2D)) {
			Vec2 crosshairPos = { windowWidth / 2, windowHeight / 2 };
			float distance = GetDistance(closestEnt->vecOrigin, entList->ents[0].ent->vecOrigin);
			std::stringstream ss;
			ss << "Distance: " << distance;
			DrawText(ss.str().c_str(), windowWidth / 2, windowHeight / 2 - 20, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
}

float Hack::GetDistance(const Vec3& target, const Vec3& myPos) {
	Vec3 delta = { target.x - myPos.x, target.y - myPos.y, target.z - myPos.z };
	return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

void Hack::Nop(BYTE* dst, unsigned int sizes) {
	DWORD oldProtect, newProtect, bytesRet; // for the VirtualProtect function
	VirtualProtect(dst, sizes, PAGE_EXECUTE_READWRITE, &oldProtect); // changing the memory protection
	memset(dst, 0x90, sizes); // filling the memory with NOP instructions
	VirtualProtect(dst, sizes, oldProtect, &newProtect); // restoring the memory protection
}

void Hack::Recoil() {
	if (settings.bRecoil) {
		this->Nop((BYTE*)(Cheats.recoilAdr), 5);
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
	if(ent)
	return true;
}

int Hack::CalculateClosestEntity() {
	float closestDistance = FLT_MAX;
	int closestEntIndex = -1;

	for (int i = 0; i < 32; i++) {
		Ent* curEnt = entList->ents[i].ent;
		if (!CheckEnt(curEnt))
			continue;

		Vec2 entPos2D;
		if (WorldToScreen(curEnt->vecOrigin, entPos2D)) {
			Vec2 crosshairPos = { windowWidth / 2, windowHeight / 2 };
			float distance = GetDistance(curEnt->vecOrigin, entList->ents[0].ent->vecOrigin);
			if (distance < closestDistance) {
				closestDistance = distance;
				closestEntIndex = i;
			}
		}
	}

	return closestEntIndex;
}

void Hack::aimbot() {
	// make it so if my right mouse button is down, then aimbot
	float* aimied = (float*)(Cheats.aimedIn);

	if (*aimied > 0.3f) {
		int closestEntIndex = CalculateClosestEntity();
		DrawDistanceToEntity(closestEntIndex);

		// aimbot to the closet entity by moving the mouse 
		if (closestEntIndex != -1) {
			Ent* closestEnt = entList->ents[closestEntIndex].ent;
			Vec2 closestEntPos2D;

			Vec3 headPos3d = closestEnt->headOrigin;
			headPos3d.y -= 15.9;
			headPos3d.x -= 15.9;

			if (WorldToScreen(headPos3d, closestEntPos2D)) {
				Vec2 crosshairPos = { windowWidth / 2, windowHeight / 2 };
				float distance = GetDistance(closestEnt->vecOrigin, entList->ents[0].ent->vecOrigin);
				// Creates an FOV circle aimbot based on distance to target
				if (distance < 2500) {
					moveMouseToEntity(closestEntPos2D, crosshairPos, 20);
				}

				if (distance < 1000) {
					moveMouseToEntity(closestEntPos2D, crosshairPos, 50);
				}

				if (distance < 300) {
					moveMouseToEntity(closestEntPos2D, crosshairPos, 90);
				}
			}
		}
	}
}

void Hack::moveMouseToEntity(const Vec2& entityPos, const Vec2& crosshairPos, int yAdjustment) {
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.dx = (entityPos.x - crosshairPos.x - 10) / 2;
	input.mi.dy = (entityPos.y - crosshairPos.y + yAdjustment) / 5;
	SendInput(1, &input, sizeof(INPUT));
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