#pragma once

//#include <iostream>
#include "pch.h"

struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

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
	ThirdPeronViewTransparency = 4,
	FreeCam = 5,
	Overview = 6,
	SM_NUM = 7
};

class PlayerEntity
{
public:
	uint32_t vTable; //0x0000
	Vector3 xyzHeadPos; //0x0004
	Vector3 xyzSpead; //0x0010
	int32_t xSpeedInt; //0x001C
	int32_t ySpeedInt; //0x0020
	int32_t zSpeedInt; //0x0024
	Vector3 xyzVelocity; //0x0028
	Vector3 xyzPos; //0x0034
	Vector3 xyViewAxis; //0x0040
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
	uint8_t state; //0x0082
	uint8_t type; //0x0083
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
	int32_t magAmmoKnife; //0x0110
	int32_t magAmmoPistol; //0x0114
	int32_t magAmmoCarabine; //0x0118
	int32_t magAmmoShotgun; //0x011C
	int32_t magAmmoSubgun; //0x0120
	int32_t magAmmoSniper; //0x0124
	int32_t magAmmoAssault; //0x0128
	int32_t magAmmoCpistol; //0x012C
	int32_t magAmmoGrenade; //0x0130
	int32_t magAmmoAkimbo; //0x0134
	int32_t ammoKnife; //0x0138
	int32_t ammoPistol; //0x013C
	int32_t ammoCarabine; //0x0140
	int32_t ammoShotgun; //0x0144
	int32_t ammoSubgun; //0x0148
	int32_t ammoSniper; //0x014C
	int32_t ammoAssault; //0x0150
	int32_t ammoCpistol; //0x0154
	int32_t ammoGrenade; //0x0158
	int32_t ammoAkimbo; //0x015C
	int32_t timerKnife; //0x0160
	int32_t timerPistol; //0x0164
	int32_t timerCarabine; //0x0168
	int32_t timerShotgun; //0x016C
	int32_t timerSubgun; //0x0170
	int32_t timerSniper; //0x0174
	int32_t timerAssault; //0x0178
	int32_t timerCpistol; //0x017C
	int32_t timerGrenade; //0x0180
	int32_t timerAkimbo; //0x0184
	int32_t statShotKnife; //0x0188
	int32_t statShotPistol; //0x018C
	int32_t statShotsCarabine; //0x0190
	int32_t statShotsShotgun; //0x0194
	int32_t statShotsSubgun; //0x0198
	int32_t statShotsSniper; //0x019C
	int32_t statShotsAssault; //0x01A0
	int32_t statShotsCpistol; //0x01A4
	int32_t statShotsGrenade; //0x01A8
	int32_t statShotsAkimbo; //0x01AC
	int32_t statDamageKnife; //0x01B0
	int32_t statDamagePistol; //0x01B4
	int32_t statDamageCarabine; //0x01B8
	int32_t statDamageShotgun; //0x01BC
	int32_t statDamageSubgun; //0x01C0
	int32_t statDamageSniper; //0x01C4
	int32_t statDamageAssault; //0x01C8
	int32_t statDamageCpistol; //0x01CC
	int32_t statDamageGrenade; //0x01D0
	int32_t statDamageAkimbo; //0x01D4
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
	class Weapon* weaponKnifePtr; //0x0348
	class Weapon* weaponPistolPtr; //0x034C
	class Weapon* weaponCarabinePtr; //0x0350
	class Weapon* weaponShotgunPtr; //0x0354
	class Weapon* weaponSubgunPtr; //0x0358
	class Weapon* weaponSniperPtr; //0x035C
	class Weapon* weaponAssaultPtr; //0x0360
	class Weapon* weaponCpistolPtr; //0x0364
	class Weapon* weaponGrenadePtr; //0x0368
	class Weapon* weaponAkimboPtr; //0x036C
	class Weapon* lastWeaponUsedPtr; //0x0370
	class Weapon* currentWeaponPtr; //0x0374
	class Weapon* weaponToBeUsedPtr; //0x0378
	class Weapon* primaryWeaponPtr; //0x037C
	class Weapon* nextPrimaryWeaponPtr; //0x0380
	class Weapon* lastWeaponShotPtr; //0x0384
	char pad_0388[24]; //0x0388
}; //Size: 0x03A0
static_assert(sizeof(PlayerEntity) == 0x3A0);

class Weapon
{
public:
	char pad_0000[4]; //0x0000
	WeaponTypes weaponType; //0x0004
	class PlayerEntity* playerOwnerPtr; //0x0008
	class WeaponCharacs* weaponCharacsPtr; //0x000C
	class Uint32_ptr* magazineAmmoPtr; //0x0010
	class WeaponStats* weaponStatsClassPtr; //0x0014
	class Uint32_ptr* weaponTimer; //0x0018
	uint32_t shotsFiredInARow; //0x001C
	uint32_t _reloading_related; //0x0020
	char pad_0024[12]; //0x0024
}; //Size: 0x0030
static_assert(sizeof(Weapon) == 0x30);

class WeaponStats
{
public:
	int32_t weaponAmmo; //0x0000
	char pad_0004[36]; //0x0004
	int32_t weaponTimer; //0x0028
	char pad_002C[36]; //0x002C
	int32_t weaponStatShots; //0x0050
	char pad_0054[128]; //0x0054
}; //Size: 0x00D4
static_assert(sizeof(WeaponStats) == 0xD4);

class Uint32_ptr
{
public:
	uint32_t uint_32; //0x0000
}; //Size: 0x0004
static_assert(sizeof(Uint32_ptr) == 0x4);

class WeaponCharacs
{
public:
	char weaponName[16]; //0x0000
	char pad_0010[244]; //0x0010
	Sounds soundShot; //0x0104
	Sounds soundReload; //0x0106
	uint16_t reloadTimerMs; //0x0108
	uint16_t shotTimerMs; //0x010A
	uint16_t closeRangeDamage; //0x010C
	uint16_t N00000CA9; //0x010E
	uint16_t N00000AB0; //0x0110
	uint16_t N00000CAC; //0x0112
	uint16_t firstShotsSpread; //0x0114
	uint16_t kickBack; //0x0116
	uint16_t MaxAmmoPerMagazine; //0x0118
	uint16_t recoilAnimation; //0x011A
	uint16_t kickbackAnimation; //0x011C
	uint16_t N00000CB5; //0x011E
	uint16_t unknown; //0x0120
	uint16_t recoil; //0x0122
	uint16_t N00000AB5; //0x0124
	uint16_t ennemyKickback; //0x0126
	uint16_t isSemiAutomatic; //0x0128
}; //Size: 0x012A
static_assert(sizeof(WeaponCharacs) == 0x12A);

class EntityListPtr
{
public:
	class EntityList* entityListClassPtr; //0x0000
}; //Size: 0x0004
static_assert(sizeof(EntityListPtr) == 0x4);

class EntityList
{
public:
	char pad_0000[4]; //0x0000
	class PlayerEntity* entityList[32]; //0x0004
	char pad_0084[12]; //0x0084
}; //Size: 0x0090
static_assert(sizeof(EntityList) == 0x90);