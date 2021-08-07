#pragma once

#include <Windows.h>
#include <vector>
#include <algorithm>

#include "gameStructures.h"
#include "glText.h"
#include "glDraw.h"

#define MAX_NUMBER_OF_PLAYER 32

using Geom::Vector2;
using Geom::Vector3;
using Geom::Vector4;

namespace Hacks
{
	// Offsets
	const DWORD o_gameObjects = 0x0010F49C;
	const DWORD o_playerEntityVectorPtr = 0x0010F4F8;
	const DWORD o_viewMatix = 0x00101AE8;
	const DWORD o_aimAtPos = 0x0010A400;
	const DWORD o_traceLineFunction = 0x0008A310;
	const DWORD o_traceLineFunction_enemyOnCrosshair = 0x00060670;

	// Constants
	const DWORD c_playerEntityType = 0x004E4A98;
	const DWORD c_botEntityType = 0x004E4AC0;

	// Weapon constants
	const WORD c_defaultWeaponsSpread[(int)WeaponTypes::SIZE] = { 0x01, 0x35, 0x0A, 0x01, 0x2D, 0x32, 0x12, 0x23, 0x01, 0x32 };
	const WORD c_defaultWeaponsKickback[(int)WeaponTypes::SIZE] = { 0x01, 0x0A, 0x3C, 0x23, 0x0F, 0x32, 0x1E, 0x0A, 0x01, 0x0A };
	const WORD c_defaultWeaponsKickbackAnimation[(int)WeaponTypes::SIZE] = { 0x00, 0x05, 0x04, 0x09, 0x02, 0x04, 0x02, 0x05, 0x01, 0x05 };
	const WORD c_defaultWeaponsTargetKickback[(int)WeaponTypes::SIZE] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01 };
	const WORD c_defaultWeaponsRecoil[(int)WeaponTypes::SIZE] = { 0x00, 0x3A, 0x3C, 0x8C, 0x32, 0x55, 0x32, 0x32, 0x00, 0x19 };
	const WORD c_defaultWeaponsRecoilAnimation[(int)WeaponTypes::SIZE] = { 0x00, 0x06, 0x04, 0x09, 0x01, 0x04, 0x00, 0x06, 0x03, 0x06 };
	const WORD c_defaultWeaponsIsFullAuto[(int)WeaponTypes::SIZE] = { 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01 };


	// Miscellaneous
	DWORD getModuleBaseAddr();


	// Entities related
	GameObjects* getGameObjectsPtr();
	bool isValidEntity(PlayerEntity* playerEntity);
	bool isEnemyEntity(GameObjects* gameObjects, PlayerEntity* enemyEntityPtr);
	bool isAliveEntity(PlayerEntity* playerEntityPtr);
	std::vector<PlayerEntity*> getValidEntityList(EntityVector* playerEntityVector);
	std::vector<PlayerEntity*> getEnemyList(GameObjects* gameObjects);
	std::vector<PlayerEntity*> getAliveEnemyList(GameObjects* gameObjects);


	// Weapons related
	enum class WeaponHackTypes : int32_t
	{
		NoSpread = 0,
		NoRecoil,
		NoKickback,
		NoSelfKickback,
		FullAuto
	};

	//Weapon* getWeaponPtr(PlayerEntity* playerEntity, WeaponTypes weaponType);
	WeaponTypes getCurrentWeaponType(PlayerEntity* playerEntity);
	bool toggleWeaponHack(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType);
	bool toggleWeaponHack(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType, bool enable);
	bool toggleAllWeaponsHack(PlayerEntity* playerEntity, WeaponHackTypes weaponHackType, bool enable);
	bool getWeaponHackState(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType);
	int16_t getWeaponHackValue(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType);
	bool setWeaponHackValue(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType, int16_t value);
	bool setAllWeaponsHackValue(PlayerEntity* playerEntity, WeaponHackTypes weaponHackType, int16_t value);
	int16_t getDefaultWeaponHackValue(WeaponTypes weaponType, WeaponHackTypes weaponHackType);


	// Aimbot related
	struct TraceLineResult
	{
		Vector3 pos;
		int8_t collided;
	};

	bool aimbot(GameObjects* gameObjects, float fov, float smoothness);
	PlayerEntity* getClosestEnemy(GameObjects* gameObjects);
	PlayerEntity* getClosestEnemyToCrosshair(GameObjects* gameObjects, float fov);
	PlayerEntity* getBestTarget(GameObjects* gameObjects, float fov, float distanceRatio);
	bool smoothSetViewAngles(PlayerEntity* myPlayerEntityPtr, Vector2 viewAnglesToEnemy, float smoothness);
	bool isTargetVisible(PlayerEntity* myPlayerEntityPtr, Vector3 targetPos);
	Vector3 getEnemyTorsoPos(PlayerEntity* enemyPtr);
	void drawFov(Vector2 screenDimensions, float fovValue, float fovThickness, Vector4 fovColor);

	
	// Triggerbot related
	Vector3* getAimAtPos();
	PlayerEntity* getEnemyOnAim(PlayerEntity* myPlayerEntityPtr);
	bool triggerbot(GameObjects* gameObjects);


	// ESP related
	float* getviewMatrixPtr();
	bool worldToScreen(Vector3 position, Vector2 screenDimensions, Vector2& screenPosition);
	Vector3 getEnemyHeadPos(PlayerEntity* enemyPtr);
	Vector3 getEnemyUpperBoxPos(PlayerEntity* enemyPtr);
	Vector3 getEnemyLowerBoxPos(PlayerEntity* enemyPtr);
	void drawEsp(GameObjects* gameObjects, Vector2 screenDimensions, bool drawEspBox, float espBoxThickness, Vector4 espBoxColor, 
		bool displayName, bool displayHealth, bool displayShield, bool drawEspHead, float espHeadThickness, Vector4 espHeadColor);
}