#pragma once

#include <iostream>

#include "geom.h"

using Geom::Vector3;

// Created with ReClass.NET 1.2 by KN4CK3R

enum class WeaponTypes : int32_t
{
	Knife = 0,
	Pistol = 1,
	Carabine = 2,
	Shotgun = 3,
	Subgun = 4,
	Sniper = 5,
	Assault = 6,
	Cpistol = 7,
	Grenade = 8,
	Akimbo = 9,
	SIZE = 10
};

enum class Sounds : int16_t
{
	Spawn = 0,
	Walk = 1,
	Fall = 2,
	Bullet1 = 3,
	Bullet2 = 4,
	Unknown1 = 5,
	Water = 6,
	Knife = 7,
	PistolShot = 8,
	PistolReload = 9,
	CarabineShot = 10,
	CarabineReload = 11,
	ShotgunShot = 12,
	ShotgunReload = 13,
	SubgunShot = 14,
	SubgunReload = 15,
	SniperShot = 16,
	SniperReload = 17,
	AssaultShot = 18,
	AssaultReload = 19,
	Unknown2 = 20
};

enum class Teams : int32_t
{
	Red = 0,
	Blue = 1,
	Unk1 = 2,
	Unk2 = 3,
	Spectator = 4
};

enum class SpectatingModes : int32_t
{
	NoSpectate = 0,
	DeathCam = 1,
	FirstPersonView = 2,
	ThirdPersonView = 3,
	ThirdPersonViewTransparency = 4,
	FlyCam = 5,
	Overview = 6,
	SM_NUM = 7
};

enum class EntityTypes : int8_t
{
	Player = 0,
	Bot = 1,
	Camera = 2,
	Bounce = 3
};

enum class States : int8_t
{
	Alive = 0,
	Dead = 1,
	Spawning = 2,
	Lagged = 3,
	Editing = 4,
	Spectate = 5
};

enum class GameModes : int32_t
{
	TDM = 0,
	coop = 1,
	DM = 2,
	SURV = 3,
	TSURV = 4,
	CTF = 5,
	PF = 6,
	BTDM = 7,
	BDM = 8,
	LSS = 9,
	OSOK = 10,
	TOSOK = 11,
	BOSOK = 12,
	HTF = 13,
	TKTF = 14,
	KTF = 15,
	TPF = 16,
	TLSS = 17,
	BPF = 18,
	BLSS = 19,
	BTSURV = 20,
	BTOSOK = 21
};

class PlayerEntity
{
public:
	uint32_t vTable; //0x0000
	Vector3 headPos; //0x0004
	Vector3 spead; //0x0010
	int32_t xSpeedInt; //0x001C
	int32_t ySpeedInt; //0x0020
	int32_t zSpeedInt; //0x0024
	Vector3 velocity; //0x0028
	Vector3 pos; //0x0034
	Vector3 viewAngles; //0x0040
	float pitchVel; //0x004C
	float maxSpeed; //0x0050
	int32_t timeInAir; //0x0054
	float radius; //0x0058
	float eyeHeight; //0x005C
	float maxEyeHeigth; //0x0060
	float aboveEye; //0x0064
	bool isInWater; //0x0068
	bool isOnFloor; //0x0069
	bool isOnLadder; //0x006A
	bool jumpNext; //0x006B
	bool isCrouch; //0x006C
	bool isCrounchInAir; //0x006D
	bool isTryingToCrouch; //0x006E
	bool isCancollide; //0x006F
	bool isStuck; //0x0070
	bool isScoping; //0x0071
	bool isShooting; //0x0072
	char pad_0073[1]; //0x0073
	int32_t lastJump; //0x0074
	float lastJumpHeight; //0x0078
	int32_t lastWaterSplash; //0x007C
	int8_t moveForwardBackward; //0x0080
	int8_t moveRightLeft; //0x0081
	States currentState; //0x0082
	EntityTypes type; //0x0083
	float eyeHeigthVel; //0x0084
	int32_t lastPos; //0x0088
	bool isPressingLeft; //0x008C
	bool isPressingRight; //0x008D
	bool isPressingUp; //0x008E
	bool isPressingDown; //0x008F
	char pad_0090[104]; //0x0090
	int32_t health; //0x00F8
	int32_t shield; //0x00FC
	int32_t primary; //0x0100
	int32_t nextPrimary; //0x0104
	int32_t gunselect; //0x0108
	bool akimbo; //0x010C
	char pad_010D[3]; //0x010D
	int32_t weaponsTotalAmmo[10]; //0x0110
	int32_t weaponsAmmo[10]; //0x0138
	int32_t weaponsTimer[10]; //0x0160
	int32_t weaponsShotsNumber[10]; //0x0188
	int32_t weaponsDamageDone[10]; //0x01B0
	int32_t currentSkin; //0x01D8
	int32_t nextSkinRedTeam; //0x01DC
	int32_t nextSkinBlueTeam; //0x01E0
	int32_t clientNum; //0x01E4
	int32_t lastUpdate; //0x01E8
	int32_t plag; //0x01EC
	int32_t ping; //0x01F0
	int32_t enet32Address; //0x01F4
	int32_t lifeSequence; //0x01F8
	int32_t frags; //0x01FC
	int32_t flagScore; //0x0200
	int32_t deaths; //0x0204
	int32_t points; //0x0208
	int32_t teamKills; //0x020C
	int32_t lastAction; //0x0210
	int32_t lastMove; //0x0214
	int32_t lastPain; //0x0218
	int32_t lastVoiceCom; //0x021C
	int32_t lastDeath; //0x0220
	bool attacking; //0x0224
	char name[15]; //0x0225
	char pad_0234[248]; //0x0234
	Teams team; //0x032C
	int32_t weaponChanging; //0x0330
	int32_t nextWeapon; //0x0334
	SpectatingModes spectateMode; //0x0338
	int32_t followPlayerCn; //0x033C
	int32_t earDamageMillis; //0x0340
	char pad_0344[4]; //0x0344
	class Weapon* weaponsPtr[10]; //0x0348
	class Weapon* lastWeaponUsedPtr; //0x0370
	class Weapon* currentWeaponPtr; //0x0374
	class Weapon* weaponToBeUsedPtr; //0x0378
	class Weapon* primaryWeaponPtr; //0x037C
	class Weapon* nextPrimaryWeaponPtr; //0x0380
	class Weapon* lastWeaponShotPtr; //0x0384
	int32_t gameTime; //0x0388
	char pad_038C[20]; //0x038C
}; //Size: 0x03A0
static_assert(sizeof(PlayerEntity) == 0x3A0);

class Weapon
{
public:
	char pad_0000[4]; //0x0000
	WeaponTypes weaponType; //0x0004
	class PlayerEntity* playerOwnerPtr; //0x0008
	class WeaponCharacs* weaponCharacsPtr; //0x000C
	int32_t* magazineAmmoPtr; //0x0010
	int32_t* weaponAmmoPtr; //0x0014
	int32_t* weaponTimer; //0x0018
	uint32_t shotsFiredInARow; //0x001C
	uint32_t reloadingStartTime; //0x0020
	char pad_0024[12]; //0x0024
}; //Size: 0x0030
static_assert(sizeof(Weapon) == 0x30);

class WeaponCharacs
{
public:
	char weaponName[16]; //0x0000
	char pad_0010[244]; //0x0010
	Sounds soundShot; //0x0104
	Sounds soundReload; //0x0106
	int16_t reloadTimerMs; //0x0108
	int16_t shotTimerMs; //0x010A
	int16_t closeRangeDamage; //0x010C
	int16_t N00000CA9; //0x010E
	int16_t N00000AB0; //0x0110
	int16_t N00000CAC; //0x0112
	int16_t firstShotsSpread; //0x0114
	int16_t kickback; //0x0116
	int16_t MaxAmmoPerMagazine; //0x0118
	int16_t recoilAnimation; //0x011A
	int16_t kickbackAnimation; //0x011C
	int16_t N00000CB5; //0x011E
	int16_t unknown; //0x0120
	int16_t recoil; //0x0122
	int16_t N00000AB5; //0x0124
	int16_t targetKickback; //0x0126
	int16_t isFullAuto; //0x0128
}; //Size: 0x012A
static_assert(sizeof(WeaponCharacs) == 0x12A);

class EntityVector
{
public:
	class PlayerEntity* (*entityListPtr)[32]; //0x0000
	int32_t capacity; //0x0004
	int32_t length; //0x0008
}; //Size: 0x000C
static_assert(sizeof(EntityVector) == 0xC);

class GameObjects
{
public:
	GameModes gameMode; //0x0000
	char pad_0004[84]; //0x0004
	class PlayerEntity* myPlayerEntityPtr; //0x0058
	class EntityVector playerEntityVector; //0x005C
}; //Size: 0x0068
static_assert(sizeof(GameObjects) == 0x68);