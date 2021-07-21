#include "hacks.h"

// Entities stuff

GameObjects* Hacks::getGameObjectsPtr(DWORD moduleBaseAddr)
{
	return reinterpret_cast<GameObjects*>(moduleBaseAddr + o_gameObjects);
}

PlayerEntity* Hacks::getMyPlayerEntityPtr(DWORD moduleBaseAddr)
{
	return getGameObjectsPtr(moduleBaseAddr)->myPlayerEntityPtr;
}

EntityVector* Hacks::getPlayerEntityVectorPtr(DWORD moduleBaseAddr)
{
	return &getGameObjectsPtr(moduleBaseAddr)->playerEntityVector;
}

int Hacks::getNumberOfPlayer(DWORD moduleBaseAddr)
{
	return getGameObjectsPtr(moduleBaseAddr)->playerEntityVector.length;
}

std::vector<PlayerEntity*> Hacks::getValidEntityList(EntityVector* playerEntityVector)
{
	std::vector<PlayerEntity*> validEntityList;
	// enittyListPtr is a pointer on an array of pointers of PlayerEntity
	if (playerEntityVector && playerEntityVector->entityListPtr)
	{
		int entitiesFound = 0;
		// Entities are not next to each others in the array
		// So we have to check every entry of the array
		// capacity is the size of the allocated array
		for (int i = 0; i < playerEntityVector->length; i++)
		//for (int i = 0; entitiesFound < playerEntityVector->length && i < playerEntityVector->capacity; i++)
		//for (int i = 1; i < (MAX_NUMBER_OF_PLAYER - 1); i++)  // i = 1 because first 4 bytes are null; MAX_NUMBER_OF_PLAYER -1 because our own playerEntity isn't in the array
		{
			///**/std::cout << "[DEBUG] Looking at index " << i << std::endl;
			// Pointer arithmetic is messing up how we access our entityList array
			// Probably the problem comes from how we declared our enityListPtr array.
			if (isValidEntity((*playerEntityVector->entityListPtr)[i]))
			{
				entitiesFound++;
				validEntityList.push_back((*playerEntityVector->entityListPtr)[i]);
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
		// May want to use IsBadReadPtr() :
		//if (pPointer && HIWORD(pPointer))
		//{
		//	if (!IsBadReadPtr(pPointer, sizeof(DWORD_PTR)))
		//		return true;
		//}
		//return false;
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
		// No self was enabled
		else
		{
			// Disable no self kickback
			weaponPtr->weaponCharacsPtr->targetKickback = c_defaultWeaponsTargetKickback[(WORD)weaponType];
		}
		break;

	case WeaponHackTypes::FullAuto:
		// Full auto was disabled
		if (weaponPtr->weaponCharacsPtr->isFullAuto == c_defaultWeaponsIsFullAuto[(WORD)weaponType])
		{
			// Enable full auto
			weaponPtr->weaponCharacsPtr->isFullAuto = 1;
		}
		// Full auto was enabled
		else
		{
			// Disable full auto
			weaponPtr->weaponCharacsPtr->isFullAuto = c_defaultWeaponsIsFullAuto[(WORD)weaponType];
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

	case WeaponHackTypes::FullAuto:
		if (enable)
		{
			// Enable full auto
			weaponPtr->weaponCharacsPtr->isFullAuto = 1;
		}
		else
		{
			// Disable full auto
			weaponPtr->weaponCharacsPtr->isFullAuto = c_defaultWeaponsIsFullAuto[(WORD)weaponType];
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

	for (int weaponType = 0; weaponType < (int)WeaponTypes::SIZE; weaponType++)
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

		case WeaponHackTypes::FullAuto:
			if (enable)
			{
				// Enable full auto
				weaponPtr->weaponCharacsPtr->isFullAuto = 1;
			}
			else
			{
				// Disable full auto
				weaponPtr->weaponCharacsPtr->isFullAuto = c_defaultWeaponsIsFullAuto[weaponType];
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

	case WeaponHackTypes::FullAuto:
		// Full auto is enabled
		if (weaponPtr->weaponCharacsPtr->isFullAuto != 0)	//c_defaultWeaponsIsFullAuto[(WORD)weaponType])
			state = true;
		break;

	default:
		return false;
	}

	return state;
}

int16_t Hacks::getWeaponHackValue(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType)
{
	if (!isValidEntity(playerEntity))
		return false;

	Weapon* weaponPtr = playerEntity->weaponsPtr[(int)weaponType];// getWeaponPtr(playerEntity, weaponType);

	if (weaponPtr == nullptr)
		return false;

	int16_t value = -1;

	switch (weaponHackType)
	{
	case WeaponHackTypes::NoSpread:
		value = weaponPtr->weaponCharacsPtr->firstShotsSpread;
		break;

	case WeaponHackTypes::NoRecoil:
		value = weaponPtr->weaponCharacsPtr->recoil;
		break;

	case WeaponHackTypes::NoKickback:
		value = weaponPtr->weaponCharacsPtr->kickback;
		break;

	case WeaponHackTypes::NoSelfKickback:
		value = weaponPtr->weaponCharacsPtr->targetKickback;
		break;

	case WeaponHackTypes::FullAuto:
		value = weaponPtr->weaponCharacsPtr->isFullAuto;
		break;

	default:
		return -1;
	}

	return value;
}

bool Hacks::setWeaponHackValue(PlayerEntity* playerEntity, WeaponTypes weaponType, WeaponHackTypes weaponHackType, int16_t value)
{
	if (!isValidEntity(playerEntity))
		return false;

	Weapon* weaponPtr = playerEntity->weaponsPtr[(int)weaponType];// getWeaponPtr(playerEntity, weaponType);

	if (weaponPtr == nullptr)
		return false;

	bool result = false;

	switch (weaponHackType)
	{
	case WeaponHackTypes::NoSpread:
		weaponPtr->weaponCharacsPtr->firstShotsSpread = value;
		result = true;
		break;

	case WeaponHackTypes::NoRecoil:
		weaponPtr->weaponCharacsPtr->recoil = value;
		// Set the recoil animation value relative to the recoil value
		weaponPtr->weaponCharacsPtr->recoilAnimation = (value / (float)c_defaultWeaponsRecoil[(int)weaponType]) * c_defaultWeaponsRecoilAnimation[(int)weaponType];
		result = true;
		break;

	case WeaponHackTypes::NoKickback:
		weaponPtr->weaponCharacsPtr->kickback = value;
		// Set the kickback animation value relative to the kickback value
		weaponPtr->weaponCharacsPtr->kickbackAnimation = (value / (float)c_defaultWeaponsKickback[(int)weaponType]) * c_defaultWeaponsKickbackAnimation[(int)weaponType];
		result = true;
		break;

	case WeaponHackTypes::NoSelfKickback:
		weaponPtr->weaponCharacsPtr->targetKickback = value;
		result = true;
		break;

	case WeaponHackTypes::FullAuto:
		weaponPtr->weaponCharacsPtr->isFullAuto = value;
		result = true;
		break;

	default:
		return false;
	}

	return value;
}

bool Hacks::setAllWeaponsHackValue(PlayerEntity* playerEntity, WeaponHackTypes weaponHackType, int16_t value)
{
	if (!isValidEntity(playerEntity))
		return false;

	for (int weaponType = 0; weaponType < (int)WeaponTypes::SIZE; weaponType++)
	{
		Weapon* weaponPtr = playerEntity->weaponsPtr[weaponType];// getWeaponPtr(playerEntity, (WeaponTypes)weaponType);

		if (weaponPtr == nullptr)
			return false;

		switch (weaponHackType)
		{
		case WeaponHackTypes::NoSpread:
			weaponPtr->weaponCharacsPtr->firstShotsSpread = (value / 100.0f) * c_defaultWeaponsSpread[(int)weaponType];
			break;

		case WeaponHackTypes::NoRecoil:
			weaponPtr->weaponCharacsPtr->recoil = (value / 100.0f) * c_defaultWeaponsRecoil[(int)weaponType];
			// Set the recoil animation value relative to the recoil value
			weaponPtr->weaponCharacsPtr->recoilAnimation = (value / 100.0f) * c_defaultWeaponsRecoilAnimation[(int)weaponType];
			break;

		case WeaponHackTypes::NoKickback:
			weaponPtr->weaponCharacsPtr->kickback = (value / 100.0f) * c_defaultWeaponsKickback[(int)weaponType];
			// Set the kickback animation value relative to the kickback value
			weaponPtr->weaponCharacsPtr->kickbackAnimation = (value / 100.0f) * c_defaultWeaponsKickbackAnimation[(int)weaponType];
			break;

		case WeaponHackTypes::NoSelfKickback:
			weaponPtr->weaponCharacsPtr->targetKickback = (value / 100.0f) * c_defaultWeaponsTargetKickback[(int)weaponType];
			break;

		case WeaponHackTypes::FullAuto:
			weaponPtr->weaponCharacsPtr->isFullAuto = (value / 100.0f) * c_defaultWeaponsIsFullAuto[(int)weaponType];
			break;
		}
	}
	return value;
}

int16_t Hacks::getDefaultWeaponHackValue(WeaponTypes weaponType, WeaponHackTypes weaponHackType)
{
	int16_t value = -1;
	switch (weaponHackType)
	{
	case WeaponHackTypes::NoSpread:
		value = c_defaultWeaponsSpread[(int)weaponType];
		break;

	case WeaponHackTypes::NoRecoil:
		value = c_defaultWeaponsRecoil[(int)weaponType];
		break;

	case WeaponHackTypes::NoKickback:
		value = c_defaultWeaponsKickback[(int)weaponType];
		break;

	case WeaponHackTypes::NoSelfKickback:
		value = c_defaultWeaponsTargetKickback[(int)weaponType];
		break;

	case WeaponHackTypes::FullAuto:
		value = c_defaultWeaponsIsFullAuto[(int)weaponType];
		break;
	}
	return value;
}
