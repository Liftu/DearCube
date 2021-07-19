#include "pch.h" // WHY ???
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
		// Entities are not next to each others in the array
		// So we have to check every entry of the array
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
	if (playerEntity != nullptr)
	{
		// The try catch is to prevent try to dereference a not a pointer variable
		// which would lead to an EXCEPTION_ACCESS_VIOLATION
		__try
		{
			if (playerEntity->vTable == c_playerEntityType || playerEntity->vTable == c_botEntityType)
				return true;
		}
		__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_EXECUTION)
		{
			return false;
		}
	}
	return false;
}


// Weapons stuff

//Weapon* Hacks::getWeaponPtr(PlayerEntity* playerEntity, WeaponTypes weaponType)
//{
//	// Get weapon ptr
//	Weapon* weapon = nullptr;
//	switch (weaponType)
//	{
//	case WeaponTypes::Knife:
//		weapon = playerEntity->weaponKnifePtr;
//		break;
//	case WeaponTypes::Pistol:
//		weapon = playerEntity->weaponPistolPtr;
//		break;
//	case WeaponTypes::Carabine:
//		weapon = playerEntity->weaponCarabinePtr;
//		break;
//	case WeaponTypes::Shotgun:
//		weapon = playerEntity->weaponShotgunPtr;
//		break;
//	case WeaponTypes::Subgun:
//		weapon = playerEntity->weaponSubgunPtr;
//		break;
//	case WeaponTypes::Sniper:
//		weapon = playerEntity->weaponSniperPtr;
//		break;
//	case WeaponTypes::Assault:
//		weapon = playerEntity->weaponAssaultPtr;
//		break;
//	case WeaponTypes::Cpistol:
//		weapon = playerEntity->weaponCpistolPtr;
//		break;
//	case WeaponTypes::Grenade:
//		weapon = playerEntity->weaponGrenadePtr;
//		break;
//	case WeaponTypes::Akimbo:
//		weapon = playerEntity->weaponAkimboPtr;
//		break;
//	}
//	return weapon;
//}

WeaponTypes Hacks::getCurrentWeaponType(PlayerEntity* playerEntity)
{
	if (isValidEntity(playerEntity))
		return playerEntity->currentWeaponPtr->weaponType;
	else
		// Default, didn't know what else to return.
		return WeaponTypes::Knife;
}

bool Hacks::toggleWeaponHack(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType)
{
	if (!isValidEntity(playerEntity))
		return false;

	Weapon* weaponPtr = playerEntity->weaponsPtr[(int)weaponType];// getWeaponPtr(playerEntity, weaponType);

	if (weaponPtr == nullptr)
		return false;

	switch (weaponHackType)
	{
	case WeaponHackTypes::NoSpread:
		// No spread was disabled
		if (weaponPtr->weaponCharacsPtr->firstShotsSpread == c_defaultWeaponsSpread[(WORD)weaponType])
		{
			// Enable no spread
			weaponPtr->weaponCharacsPtr->firstShotsSpread = 0;
		}
		// No spread was enabled
		else
		{
			// Disable no spread
			weaponPtr->weaponCharacsPtr->firstShotsSpread = c_defaultWeaponsSpread[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::NoRecoil:
		// No recoil was disabled
		if (weaponPtr->weaponCharacsPtr->recoil == c_defaultWeaponsRecoil[(WORD)weaponType])
		{
			// Enable no recoil
			weaponPtr->weaponCharacsPtr->recoil = 0;
			weaponPtr->weaponCharacsPtr->recoilAnimation = 0;
		}
		// No recoil was enabled
		else
		{
			// Disable no recoil
			weaponPtr->weaponCharacsPtr->recoil = c_defaultWeaponsRecoil[(WORD)weaponType];
			weaponPtr->weaponCharacsPtr->recoilAnimation = c_defaultWeaponsRecoilAnimation[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::NoKickback:
		// No kickback was disabled
		if (weaponPtr->weaponCharacsPtr->kickback == c_defaultWeaponsKickback[(WORD)weaponType])
		{
			// Enable no kickback
			weaponPtr->weaponCharacsPtr->kickback = 0;
			weaponPtr->weaponCharacsPtr->kickbackAnimation = 0;
		}
		// No kickback was enabled
		else
		{
			// Disable no kickback
			weaponPtr->weaponCharacsPtr->kickback = c_defaultWeaponsKickback[(WORD)weaponType];
			weaponPtr->weaponCharacsPtr->kickbackAnimation = c_defaultWeaponsKickbackAnimation[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::NoSelfKickback:
		// No self kickback was disabled
		if (weaponPtr->weaponCharacsPtr->targetKickback == c_defaultWeaponsTargetKickback[(WORD)weaponType])
		{
			// Enable no self kickback
			weaponPtr->weaponCharacsPtr->targetKickback = 0;
		}
		// No self kickback was enabled
		else
		{
			// Disable no self kickback
			weaponPtr->weaponCharacsPtr->targetKickback = c_defaultWeaponsTargetKickback[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::SemiAuto:
		// Semi auto was disabled
		if (weaponPtr->weaponCharacsPtr->isSemiAutomatic == c_defaultWeaponsIsSemiAutomatic[(WORD)weaponType])
		{
			// Enable semi auto
			weaponPtr->weaponCharacsPtr->isSemiAutomatic = 1;
		}
		// Semi auto kickback was enabled
		else
		{
			// Disable semi auto
			weaponPtr->weaponCharacsPtr->isSemiAutomatic = c_defaultWeaponsIsSemiAutomatic[(WORD)weaponType];
		}
		break;

	default:
		return false;
	}

	return true;
}

bool Hacks::toggleWeaponHack(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType, bool enable)
{
	if (!isValidEntity(playerEntity))
		return false;

	Weapon* weaponPtr = playerEntity->weaponsPtr[(int)weaponType];// getWeaponPtr(playerEntity, weaponType);

	if (weaponPtr == nullptr)
		return false;

	switch (weaponHackType)
	{
	case WeaponHackTypes::NoSpread:
		if (enable)
		{
			// Enable no spread
			weaponPtr->weaponCharacsPtr->firstShotsSpread = 0;
		}
		else
		{
			// Disable no spread
			weaponPtr->weaponCharacsPtr->firstShotsSpread = c_defaultWeaponsSpread[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::NoRecoil:
		if (enable)
		{
			// Enable no recoil
			weaponPtr->weaponCharacsPtr->recoil = 0;
			weaponPtr->weaponCharacsPtr->recoilAnimation = 0;
		}
		else
		{
			// Disable no recoil
			weaponPtr->weaponCharacsPtr->recoil = c_defaultWeaponsRecoil[(WORD)weaponType];
			weaponPtr->weaponCharacsPtr->recoilAnimation = c_defaultWeaponsRecoilAnimation[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::NoKickback:
		if (enable)
		{
			// Enable no kickback
			weaponPtr->weaponCharacsPtr->kickback = 0;
			weaponPtr->weaponCharacsPtr->kickbackAnimation = 0;
		}
		else
		{
			// Disable no kickback
			weaponPtr->weaponCharacsPtr->kickback = c_defaultWeaponsKickback[(WORD)weaponType];
			weaponPtr->weaponCharacsPtr->kickbackAnimation = c_defaultWeaponsKickbackAnimation[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::NoSelfKickback:
		if (enable)
		{
			// Enable no self kickback
			weaponPtr->weaponCharacsPtr->targetKickback = 0;
		}
		else
		{
			// Disable no self kickback
			weaponPtr->weaponCharacsPtr->targetKickback = c_defaultWeaponsTargetKickback[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::SemiAuto:
		if (enable)
		{
			// Enable semi auto
			weaponPtr->weaponCharacsPtr->isSemiAutomatic = 1;
		}
		else
		{
			// Disable semi auto
			weaponPtr->weaponCharacsPtr->isSemiAutomatic = c_defaultWeaponsIsSemiAutomatic[(WORD)weaponType];
		}
		break;

	default:
		return false;
	}

	return true;
}

bool Hacks::toggleAllWeaponsHack(PlayerEntity* playerEntity, WeaponHackTypes weaponHackType, bool enable)
{
	if (!isValidEntity(playerEntity))
		return false;

	for (int weaponType = 0; weaponType < (int)WeaponTypes::Akimbo; weaponType++)
	{
		Weapon* weaponPtr = playerEntity->weaponsPtr[weaponType];// getWeaponPtr(playerEntity, (WeaponTypes)weaponType);

		if (weaponPtr == nullptr)
			return false;

		switch (weaponHackType)
		{
		case WeaponHackTypes::NoSpread:
			if (enable)
			{
				// Enable no spread
				weaponPtr->weaponCharacsPtr->firstShotsSpread = 0;
			}
			else
			{
				// Disable no spread
				weaponPtr->weaponCharacsPtr->firstShotsSpread = c_defaultWeaponsSpread[weaponType];
			}
			break;

		case WeaponHackTypes::NoRecoil:
			if (enable)
			{
				// Enable no recoil
				weaponPtr->weaponCharacsPtr->recoil = 0;
				weaponPtr->weaponCharacsPtr->recoilAnimation = 0;
			}
			else
			{
				// Disable no recoil
				weaponPtr->weaponCharacsPtr->recoil = c_defaultWeaponsRecoil[weaponType];
				weaponPtr->weaponCharacsPtr->recoilAnimation = c_defaultWeaponsRecoilAnimation[weaponType];
			}
			break;

		case WeaponHackTypes::NoKickback:
			if (enable)
			{
				// Enable no kickback
				weaponPtr->weaponCharacsPtr->kickback = 0;
				weaponPtr->weaponCharacsPtr->kickbackAnimation = 0;
			}
			else
			{
				// Disable no kickback
				weaponPtr->weaponCharacsPtr->kickback = c_defaultWeaponsKickback[weaponType];
				weaponPtr->weaponCharacsPtr->kickbackAnimation = c_defaultWeaponsKickbackAnimation[weaponType];
			}
			break;

		case WeaponHackTypes::NoSelfKickback:
			if (enable)
			{
				// Enable no self kickback
				weaponPtr->weaponCharacsPtr->targetKickback = 0;
			}
			else
			{
				// Disable no self kickback
				weaponPtr->weaponCharacsPtr->targetKickback = c_defaultWeaponsTargetKickback[weaponType];
			}
			break;

		case WeaponHackTypes::SemiAuto:
			if (enable)
			{
				// Enable semi auto
				weaponPtr->weaponCharacsPtr->isSemiAutomatic = 1;
			}
			else
			{
				// Disable semi auto
				weaponPtr->weaponCharacsPtr->isSemiAutomatic = c_defaultWeaponsIsSemiAutomatic[weaponType];
			}
			break;

		default:
			return false;
		}
	}

	return true;
}

bool Hacks::getWeaponHackState(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType)
{
	if (!isValidEntity(playerEntity))
		return false;

	Weapon* weaponPtr = playerEntity->weaponsPtr[(int)weaponType];// getWeaponPtr(playerEntity, weaponType);

	if (weaponPtr == nullptr)
		return false;

	bool state = false;
	switch (weaponHackType)
	{
	case WeaponHackTypes::NoSpread:
		// No spread is enabled
		if (weaponPtr->weaponCharacsPtr->firstShotsSpread != c_defaultWeaponsSpread[(WORD)weaponType])
			state = true;
		break;

	case WeaponHackTypes::NoRecoil:
		// No recoil is enabled
		if (weaponPtr->weaponCharacsPtr->recoil != c_defaultWeaponsRecoil[(WORD)weaponType])
			state = true;
		break;

	case WeaponHackTypes::NoKickback:
		// No kickback is enabled
		if (weaponPtr->weaponCharacsPtr->kickback != c_defaultWeaponsKickback[(WORD)weaponType])
			state = true;
		break;

	case WeaponHackTypes::NoSelfKickback:
		// No self kickback is enabled
		if (weaponPtr->weaponCharacsPtr->targetKickback != c_defaultWeaponsTargetKickback[(WORD)weaponType])
			state = true;
		break;

	case WeaponHackTypes::SemiAuto:
		// Semi auto is enabled
		if (weaponPtr->weaponCharacsPtr->isSemiAutomatic != 0)	//c_defaultWeaponsIsSemiAutomatic[(WORD)weaponType])
			state = true;

	default:
		return false;
	}

	return state;
}
