#pragma once

#include "pch.h"
#include "gameStructures.h"
#include "gameVariables.h"

namespace Hacks
{
	// Entities stuff
	GameObjects* getGameObjectsPtr(DWORD moduleBaseAddr);
	PlayerEntity* getMyPlayerEntityPtr(DWORD moduleBaseAddr);
	EntityVector* getPlayerEntityVectorPtr(DWORD moduleBaseAddr);
	int getNumberOfPlayer(DWORD moduleBaseAddr);
	std::vector<PlayerEntity*> getValidEntityList(EntityVector* playerEntityVector);
	bool isValidEntity(PlayerEntity* playerEntity);

	// Weapons stuff
	enum class WeaponHackTypes : int32_t
	{
		NoSpread = 0,
		NoRecoil = 1,
		NoKickback = 2,
		NoSelfKickback = 3,
		SemiAuto = 4
	};

	Weapon* getWeaponPtr(PlayerEntity* playerEntity, WeaponTypes weaponType);
	WeaponTypes getCurrentWeaponType(PlayerEntity* playerEntity);
	bool toggleWeaponHack(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType);
	bool toggleWeaponHack(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType, bool enable);
	bool toggleAllWeaponsHack(PlayerEntity* playerEntity, WeaponHackTypes weaponHackType, bool enable);
	bool getWeaponHackState(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType);
}