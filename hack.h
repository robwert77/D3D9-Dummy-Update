#pragma once

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define ABS(x) ((x < 0) ? (-x) : (x))

struct Vec2 {
	float x, y;
};
struct Vec3 {
	float x, y, z;
};
struct Vec4 {
	float x, y, z, w;
};

class Ent {
public:
	union
	{
		DEFINE_MEMBER_N(float, health, 0x1C8);

		DEFINE_MEMBER_N(int, healthInt, 0x1C8);

		DEFINE_MEMBER_N(Vec3, vecOrigin, 0x18);

		DEFINE_MEMBER_N(Vec3, headOrigin, 0x154);

		DEFINE_MEMBER_N(int, Test, 0x4);

		DEFINE_MEMBER_N(float, roundHealth, 0x1CC);

	};
};

class EntListObj {
public:
	struct Ent* ent;
	char pad_0000[0x88 - sizeof(ent)];
};

class EntList {
public:
	EntListObj ents[32];
};

class Hack {
public:
	uintptr_t dwEntityList = 0x18E73C0;
	uintptr_t dwViewMatrix = 0x0008E870C;

	uintptr_t client;

	ID3DXLine* LineL;
	ID3DXFont* Font;

	~Hack();

	Ent* localEnt;
	EntList* entList;
	float viewMatrix[16];

	void cheatEn();
	void CheckButtons();
	void Init();
	void Update();
	bool CheckEnt(Ent* ent);
	bool WorldToScreen(Vec3 pos, Vec2& screen);

	struct settings
	{
		bool bEspBox2D = false;
		bool bText = false;
		bool bLine = false;
		bool showMenu = true;
		bool showCross = false;
		bool bCheats = false;
		bool bSpeedHack = false;
	}settings;

	struct Cheats
	{
		int* pointsAdr = (int*)0x018EF124;
		DWORD weaponBase = 0x018ED068;
		DWORD speedAdr = 0x021D77B8;
	}Cheats;

	struct Buttons
	{
		DWORD bEspBox2D = VK_F2;
		DWORD bText = VK_F3;
		DWORD bLine = VK_F1;
		DWORD showMenu = VK_INSERT;
		DWORD showCross = VK_F4;
		DWORD bSpeedHack = VK_F6;
		DWORD bCheats = VK_F5;
	}Buttons;
};