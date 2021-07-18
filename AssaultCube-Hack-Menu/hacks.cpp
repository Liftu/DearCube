#include "PCH.h" // WHY ???
#include "hacks.h"

#include "gameStructures.h"
#include "gameVariables.h"

// Entities stuff

PlayerEntity* Hacks::getMyPlayerEntity(DWORD moduleBaseAddr)
{
	// Still don't know how to do it with a reinterpret_cast
	//PlayerEntityClass* myPlayerEntity = reinterpret_cast<PlayerEntityClass*>(moduleBaseAddr + o_myPlayerEntityPtr);
	return *(PlayerEntity**)(moduleBaseAddr + o_myPlayerEntityPtr);
}

PlayerEntity** Hacks::getPlayerEntityList(DWORD moduleBaseAddr)
{
	return *(PlayerEntity***)(moduleBaseAddr + o_entityListPtr);
}

int Hacks::getNumberOfPlayer(DWORD moduleBaseAddr)
{
	return (*(int*)(moduleBaseAddr + o_entityListSize));
}

std::vector<PlayerEntity*> Hacks::getValidEntityList(PlayerEntity** entityList, int entityNumber)
{
	std::vector<PlayerEntity*> validEntityList;
	if (entityList)
	{
		int entitiesFound = 0;
		for (int i = 1; entitiesFound < entityNumber && i < (MAX_NUMBER_OF_PLAYER - 1); i++)  // i = 1 because first 4 bytes are null; MAX_NUMBER_OF_PLAYER -1 because our own playerEntity isn't in the array
		//for (int i = 1; i < (MAX_NUMBER_OF_PLAYER - 1); i++)  // i = 1 because first 4 bytes are null; MAX_NUMBER_OF_PLAYER -1 because our own playerEntity isn't in the array
		{
			///**/std::cout << "[DEBUG] Looking at index " << i << std::endl;
			if (isValidEntity(entityList[i]))
			{
				entitiesFound++;
				validEntityList.push_back(entityList[i]);
				/**/std::cout << "Found \"" << validEntityList.back()->name << "\"" << std::endl;
			}
		}
		/**/std::cout << std::dec << "Found " << validEntityList.size() << " entities." << std::endl;
	}
	return validEntityList;
}

bool Hacks::isValidEntity(PlayerEntity* playerEntity)
{
	if (playerEntity)
	{
		try
		{
			if (playerEntity->vTable == c_playerEntityType || playerEntity->vTable == c_botEntityType)
				return true;
		}
		catch (...)
		{
			return false;
		}
	}
	return false;
}

// Weapons stuff

Weapon* Hacks::getWeaponPtr(PlayerEntity* playerEntity, WeaponTypes weaponType)
{
	// Get weapon ptr
	Weapon* weapon = nullptr;
	switch (weaponType)
	{
	case WeaponTypes::Knife:
		weapon = playerEntity->weaponKnifePtr;
		break;
	case WeaponTypes::Pistol:
		weapon = playerEntity->weaponPistolPtr;
		break;
	case WeaponTypes::Carabine:
		weapon = playerEntity->weaponCarabinePtr;
		break;
	case WeaponTypes::Shotgun:
		weapon = playerEntity->weaponShotgunPtr;
		break;
	case WeaponTypes::Subgun:
		weapon = playerEntity->weaponSubgunPtr;
		break;
	case WeaponTypes::Sniper:
		weapon = playerEntity->weaponSniperPtr;
		break;
	case WeaponTypes::Assault:
		weapon = playerEntity->weaponAssaultPtr;
		break;
	case WeaponTypes::Cpistol:
		weapon = playerEntity->weaponCpistolPtr;
		break;
	case WeaponTypes::Grenade:
		weapon = playerEntity->weaponGrenadePtr;
		break;
	case WeaponTypes::Akimbo:
		weapon = playerEntity->weaponAkimboPtr;
		break;
	}
	return weapon;
}

bool Hacks::toggleWeaponNoRecoil(PlayerEntity* playerEntity, WeaponTypes weaponType)
{
	Weapon* weapon = getWeaponPtr(playerEntity, weaponType);

	if (weapon == nullptr)
		return false;

	// Was disabled
	if (weapon->weaponCharacsPtr->recoil == c_defaultWeaponsRecoil[(WORD)weaponType])
	{
		// Enable no recoil
		weapon->weaponCharacsPtr->recoil = 0;
		weapon->weaponCharacsPtr->recoilAnimation = 0;
	}
	// Was enabled
	else
	{
		// Disable no recoil
		weapon->weaponCharacsPtr->recoil = c_defaultWeaponsRecoil[(WORD)weaponType];
		weapon->weaponCharacsPtr->recoilAnimation = c_defaultWeaponsRecoilAnimation[(WORD)weaponType];
	}

	return true;
}