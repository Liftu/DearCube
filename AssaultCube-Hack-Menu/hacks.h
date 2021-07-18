#pragma once

#include "pch.h"
#include "gameStructures.h"
#include "gameVariables.h"

namespace Hacks
{
	// Entities stuff
	PlayerEntity* getMyPlayerEntity(DWORD moduleBaseAddr);
	PlayerEntity** getPlayerEntityList(DWORD moduleBaseAddr);
	int getNumberOfPlayer(DWORD moduleBaseAddr);
	std::vector<PlayerEntity*> getValidEntityList(PlayerEntity** entityList, int entityNumber);
	bool isValidEntity(PlayerEntity* playerEntity);

	// Weapons stuff
	Weapon* getWeaponPtr(PlayerEntity* playerEntity, WeaponTypes weaponType);
	bool toggleWeaponNoRecoil(PlayerEntity* playerEntity, WeaponTypes weaponType);

}
