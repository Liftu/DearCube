#include "hacks.h"

// Hooks related
std::set<MidHook32*> midHookList;

void Hacks::disableMidHooks()
{
	std::for_each(midHookList.begin(), midHookList.end(), [](MidHook32* midHook) {midHook->disable(); });
}

__declspec(naked) void hooked_glDrawElements()
{
	_asm
	{
		mov eax, dword ptr[esp + 0x24]
		cmp eax, 0x100
		jl enableDepth
	}
	//glDepthRange(0.0, 0.0);
	glDepthFunc(GL_ALWAYS);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	//glEnable(GL_COLOR_MATERIAL);
	//glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	goto exit;

enableDepth:
	//glDepthRange(0.0, 1.0);
	glDepthFunc(GL_LEQUAL);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	//glDisable(GL_COLOR_MATERIAL);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

exit:
	__asm
	{
		ret
	}
}


// Miscellaneous

DWORD Hacks::getModuleBaseAddr()
{
	return (DWORD)GetModuleHandle(L"ac_client.exe");
}


// Entities related

GameObjects* Hacks::getGameObjectsPtr()
{
	DWORD moduleBaseAddr = getModuleBaseAddr();
	if (moduleBaseAddr == NULL)
		return nullptr;

	return reinterpret_cast<GameObjects*>(moduleBaseAddr + o_gameObjects);
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

bool Hacks::isEnemyEntity(GameObjects* gameObjects, PlayerEntity* enemyEntityPtr)
{
	PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
	GameModes gameMode = gameObjects->gameMode;

	if (!isValidEntity(myPlayerEntityPtr) || !isValidEntity(enemyEntityPtr))
		return false;
	
	// If no team
	else if (myPlayerEntityPtr->team != Teams::Blue && myPlayerEntityPtr->team != Teams::Red)
		return false;
	
	// based on gamemode
	else if (gameMode == GameModes::TDM || gameMode == GameModes::TSURV || gameMode == GameModes::CTF || gameMode == GameModes::BTDM || gameMode == GameModes::TOSOK ||
		gameMode == GameModes::TKTF || gameMode == GameModes::TPF || gameMode == GameModes::TLSS || gameMode == GameModes::BTSURV || gameMode == GameModes::BTOSOK)
		if (myPlayerEntityPtr->team == enemyEntityPtr->team || (enemyEntityPtr->team != Teams::Blue && enemyEntityPtr->team != Teams::Red))
			return false;

	return true;
}

bool Hacks::isAliveEntity(PlayerEntity* playerEntityPtr)
{
	if (!isValidEntity(playerEntityPtr))
		return false;

	return playerEntityPtr->currentState == States::Alive;
}

std::vector<PlayerEntity*> Hacks::getValidEntityList(EntityVector* playerEntityVector)
{
	std::vector<PlayerEntity*> validEntityList;
	// enittyListPtr is a pointer on an array of pointers of PlayerEntity
	if (playerEntityVector && playerEntityVector->entityListPtr)
	{
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
				validEntityList.push_back((*playerEntityVector->entityListPtr)[i]);
		}
	}
	return validEntityList;
}

std::vector<PlayerEntity*> Hacks::getEnemyList(GameObjects* gameObjects)
{
	PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
	EntityVector* playerEntityVector = &gameObjects->playerEntityVector;

	std::vector<PlayerEntity*> enemyList = getValidEntityList(playerEntityVector);
	// Filter ennemies based on gameMode and teams
	GameModes gameMode = gameObjects->gameMode;
	// If we are a spectator, we have no ennemies
	if (myPlayerEntityPtr->team != Teams::Blue && myPlayerEntityPtr->team != Teams::Red)
	{
		enemyList.clear();
	}
	//// If team based game modes, then remove allies and spectators from the list
	//else if (gameMode == GameModes::TDM || gameMode == GameModes::TSURV || gameMode == GameModes::CTF || gameMode == GameModes::BTDM || gameMode == GameModes::TOSOK ||
	//	gameMode == GameModes::TKTF || gameMode == GameModes::TPF || gameMode == GameModes::TLSS || gameMode == GameModes::BTSURV || gameMode == GameModes::BTOSOK)
	//{
	//	// Remove allies and spectators (filter function is a lambda)
	//	enemyList.erase(
	//		std::remove_if(
	//			enemyList.begin(),
	//			enemyList.end(),
	//			[myPlayerEntityPtr](PlayerEntity* playerEntityPtr)
	//			{
	//				return (myPlayerEntityPtr->team == playerEntityPtr->team || (playerEntityPtr->team != Teams::Blue && playerEntityPtr->team != Teams::Red));
	//			}),
	//		enemyList.end());
	//}
	else
	{
		// Remove non enemies
		enemyList.erase(
			std::remove_if(
				enemyList.begin(),
				enemyList.end(),
				[gameObjects](PlayerEntity* enemyEntityPtr)
				{
					return !isEnemyEntity(gameObjects, enemyEntityPtr);
				}),
			enemyList.end());
	}

	return enemyList;
}

std::vector<PlayerEntity*> Hacks::getAliveEnemyList(GameObjects* gameObjects)
{
	std::vector<PlayerEntity*> aliveEnemyList = getEnemyList(gameObjects);
	// Remove all non alive enemies (filter function is a lambda)
	aliveEnemyList.erase(
		std::remove_if(
			aliveEnemyList.begin(),
			aliveEnemyList.end(),
			[](PlayerEntity* playerEntityPtr)
			{
				return !isAliveEntity(playerEntityPtr);
			}),
		aliveEnemyList.end());

	return aliveEnemyList;
}


// Weapons related

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

#pragma warning( push )
#pragma warning( disable : 4244 )

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

#pragma warning( pop )

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


// Aimbot related

bool Hacks::aimbot(GameObjects* gameObjects, float fov, float smoothness)
{
	PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
	//PlayerEntity* enemyEntityPtr = Hacks::getClosestEnemyToCrosshair(gameObjects, fov);	// Could use some close to crosshair / close in position ratio.
	PlayerEntity* enemyEntityPtr = getBestTarget(gameObjects, fov, 0.7f);

	if (!isValidEntity(myPlayerEntityPtr) || !isValidEntity(enemyEntityPtr))
		return false;

	Vector2 myViewAngles(myPlayerEntityPtr->viewAngles.x, myPlayerEntityPtr->viewAngles.y);
	Vector2 viewAnglseToEnemy = Geom::calcAngle(myPlayerEntityPtr->headPos, enemyEntityPtr->headPos);

	return smoothSetViewAngles(myPlayerEntityPtr, viewAnglseToEnemy, smoothness);
}

PlayerEntity* Hacks::getClosestEnemy(GameObjects* gameObjects)
{
	std::vector<PlayerEntity*> enemyList = getAliveEnemyList(gameObjects);
	PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
	PlayerEntity* closestEnemyPtr = nullptr;

	// Filter by distance
	for (PlayerEntity* enemyEntityPtr : enemyList)
		if (closestEnemyPtr == nullptr ||
			myPlayerEntityPtr->headPos.getDistance(enemyEntityPtr->headPos) <
			myPlayerEntityPtr->headPos.getDistance(closestEnemyPtr->headPos)
			)
			closestEnemyPtr = enemyEntityPtr;

	return closestEnemyPtr;
}

PlayerEntity* Hacks::getClosestEnemyToCrosshair(GameObjects* gameObjects, float fov)
{
	std::vector<PlayerEntity*> enemyList = getAliveEnemyList(gameObjects);
	PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
	PlayerEntity* closestEnemyPtr = nullptr;
	Vector2 myViewAngles(myPlayerEntityPtr->viewAngles.x, myPlayerEntityPtr->viewAngles.y);

	// Filter by distance to crosshair
	for (PlayerEntity* enemyEntityPtr : enemyList)
	{
		Vector2 viewAnglesToEnemy = Geom::calcAngle(myPlayerEntityPtr->headPos, enemyEntityPtr->headPos);
		
		if (myViewAngles.getDistance(viewAnglesToEnemy) > fov)
			continue;

		if (closestEnemyPtr == nullptr)
		{
			closestEnemyPtr = enemyEntityPtr;
			continue;
		}

		Vector2 viewAnglesToClosestEnemy = Geom::calcAngle(myPlayerEntityPtr->headPos, closestEnemyPtr->headPos);

		if (myViewAngles.getDistance(viewAnglesToEnemy) < myViewAngles.getDistance(viewAnglesToClosestEnemy))
			closestEnemyPtr = enemyEntityPtr;
	}

	return closestEnemyPtr;
}

PlayerEntity* Hacks::getBestTarget(GameObjects* gameObjects, float fov, float angleToDistanceRatio)
{
	// ratio has to be between [0, 1]
	if (angleToDistanceRatio < 0 || angleToDistanceRatio > 1)
		return nullptr;

	std::vector<PlayerEntity*> enemyList = getAliveEnemyList(gameObjects);
	PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;

	PlayerEntity* bestTargetPtr = nullptr;
	float bestTargetCoefficient = 0.0f;

	Vector2 myViewAngles(myPlayerEntityPtr->viewAngles.x, myPlayerEntityPtr->viewAngles.y);

	// Filter by distance to crosshair and distance to player
	for (PlayerEntity* enemyEntityPtr : enemyList)
	{
		// If player not visible : skip
		if (!isTargetVisible(myPlayerEntityPtr, enemyEntityPtr->headPos))
			continue;

		Vector2 viewAnglesToEnemy = Geom::calcAngle(myPlayerEntityPtr->headPos, enemyEntityPtr->headPos);

		// If player outside FOV : skip
		if (myViewAngles.getDistance(viewAnglesToEnemy) > fov)
			continue;


		// Coefficient = angle * angleRatio + distance * distanceRatio;
		float currentTargetCoefficient = myViewAngles.getDistance(viewAnglesToEnemy) * angleToDistanceRatio +
			myPlayerEntityPtr->headPos.getDistance(enemyEntityPtr->headPos) * (1 - angleToDistanceRatio);

		if (bestTargetPtr == nullptr)
		{
			bestTargetCoefficient = currentTargetCoefficient;
			bestTargetPtr = enemyEntityPtr;
			continue;
		}

		//Vector2 viewAnglesToClosestEnemy = Geom::calcAngle(myPlayerEntityPtr->headPos, bestTargetPtr->headPos);

		if (currentTargetCoefficient < bestTargetCoefficient)
		{
			bestTargetCoefficient = currentTargetCoefficient;
			bestTargetPtr = enemyEntityPtr;
			continue;
		}
	}

	return bestTargetPtr;
}

bool Hacks::smoothSetViewAngles(PlayerEntity* myPlayerEntityPtr, Vector2 viewAnglesToEnemy, float smoothness)
{
	if (smoothness < 1.0f)
		return false;

	if (!isValidEntity(myPlayerEntityPtr))
		return false;

	Vector2 myViewAngles(myPlayerEntityPtr->viewAngles.x, myPlayerEntityPtr->viewAngles.y);

	myPlayerEntityPtr->viewAngles = Vector3(myViewAngles + (viewAnglesToEnemy - myViewAngles) / smoothness);
	return true;
}

bool Hacks::isTargetVisible(PlayerEntity* myPlayerEntityPtr, Vector3 targetPos)
{
	DWORD moduleBaseAddr = getModuleBaseAddr();
	if (moduleBaseAddr == NULL)
		return false;

	// Traceline function addr
	DWORD traceLineFunctionAddr = moduleBaseAddr + o_traceLineFunction;
	Vector3 myPos = myPlayerEntityPtr->headPos;

	struct TraceLineResult { Vector3 pos; int8_t collided; };
	TraceLineResult traceLineResult = { Vector3(), false };

	// This traceLine function has an undefined calling convention (first arg in eax)
	// So we have to call it direclty from asm.
	__asm
	{
		push 0
		push 0;						Check entities
		push myPlayerEntityPtr;		Player trace owner (to avoid colliding with ourself)
		push targetPos.z
		push targetPos.y
		push targetPos.x;			Taget position
		push myPos.z
		push myPos.y
		push myPos.x;				Source position
		lea eax, [traceLineResult];	The result ptr
		call traceLineFunctionAddr;	Call to the traceLine game function
		add esp, 0x24;				Clean the stack (9 * 4 = 36 = 0x24)
	}

	return !(bool)traceLineResult.collided;
}

Vector3 Hacks::getEnemyTorsoPos(PlayerEntity* enemyPtr)
{
	if (!isValidEntity(enemyPtr))
		return Vector3();

	Vector3 enemyHeadPos = enemyPtr->headPos;
	enemyHeadPos.z -= 1.0f; // arbitrary value

	return enemyHeadPos;
}

void Hacks::drawFov(Vector2 screenDimensions, float fovValue, float fovThickness, Vector4 fovColor)
{
	GL::setupOrtho();

	// 50 approximately represents the angle degree to the side of the screen 
	// (it's actually 45, but it works best with 50).
	float fovCircleRadius = fovValue / 50 * (screenDimensions.x / 2.0f);
	GL::drawOutlineCircle(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f, fovCircleRadius, 50, fovThickness, &fovColor.x);

	GL::restoreGL();
}


// Triggerbot related

Vector3* Hacks::getAimAtPos()
{
	DWORD moduleBaseAddr = getModuleBaseAddr();
	if (moduleBaseAddr == NULL)
		return nullptr;

	return reinterpret_cast<Vector3*>(moduleBaseAddr + o_aimAtPos);
}

PlayerEntity* Hacks::getEnemyOnAim(PlayerEntity* myPlayerEntityPtr)
{
	Vector3* aimAtPosPtr = getAimAtPos();
	if (aimAtPosPtr == nullptr)
		return nullptr;

	DWORD intersectClosestAddr = getModuleBaseAddr() + o_traceLineFunction_enemyOnCrosshair;

	Vector3* myHeadPos = &myPlayerEntityPtr->headPos;
	PlayerEntity* result = nullptr;

	// The intersectClosest function call is really weird...
	// it pass the 4th argument in EDI and the rest to the stack.
	float dist;
	int hitZone;	// 1 = body, 2 = head
	__asm
	{
		push 0;						bool aiming
		lea edx, hitZone;
		push edx;					int hitZone
		push myPlayerEntityPtr;		PlayerEntity*
		push aimAtPosPtr;			Vector3*
		push myHeadPos;				Vector3*
		lea edi, dist;				float dist
		call intersectClosestAddr;	call to the traceLine function
		add esp, 0x14;				clean 5 values on stack
		mov result, eax;			ret
	}
	return result;
}

bool Hacks::triggerbot(GameObjects* gameObjects)
{
	PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
	myPlayerEntityPtr->isShooting = false;	// Stop shooting if shooting
	PlayerEntity* enemyOnAimPtr = getEnemyOnAim(myPlayerEntityPtr);
	if (!isValidEntity(enemyOnAimPtr))
		return false;

	if (isEnemyEntity(gameObjects, enemyOnAimPtr) && isAliveEntity(enemyOnAimPtr))
	{
		myPlayerEntityPtr->isShooting = true;
		return true;
	}

	return false;
}


// ESP related

float* Hacks::getviewMatrixPtr()
{
	DWORD moduleBaseAddr = getModuleBaseAddr();
	if (moduleBaseAddr == NULL)
		return nullptr;

	return reinterpret_cast<float*>(moduleBaseAddr + o_viewMatix);
}

bool Hacks::worldToScreen(Vector3 position, Vector2 screenDimensions, Vector2& screenPosition)
{
	// Resources :
	// https://guidedhacking.com/threads/so-what-is-a-viewmatrix-anyway-and-how-does-a-w2s-work.10964/
	// http://songho.ca/opengl/gl_transform.html

	float* viewMatrix = getviewMatrixPtr();
	Vector4 clipCoords;

	// Multiply positionMatrix with viewMatrix to get cameraMatrix (eye coordinates)
	clipCoords.x = position.x * viewMatrix[0] + position.y * viewMatrix[4] + position.z * viewMatrix[8]  + 1 * viewMatrix[12];	// position.w is 1.
	clipCoords.y = position.x * viewMatrix[1] + position.y * viewMatrix[5] + position.z * viewMatrix[9]  + 1 * viewMatrix[13];
	clipCoords.z = position.x * viewMatrix[2] + position.y * viewMatrix[6] + position.z * viewMatrix[10] + 1 * viewMatrix[14];	// This is useless btw.
	clipCoords.w = position.x * viewMatrix[3] + position.y * viewMatrix[7] + position.z * viewMatrix[11] + 1 * viewMatrix[15];

	// So, how do we know they're behind us then? Its just some weird f*ckin number.
	// Well you know how it was 1 before? I like to think of it as "1" simply means theyre in view. As a world position, they're always "in view", so it's always 1.
	// So all we have to do is check if its not 1 ( but theyre floats, so use < > ! ). This explanation isnt rooted in math, its anecdotal, i havent bothered much to check tbh.

	// I don't know why, bu this doesn't equal whet it should...
	if (clipCoords.w < 1.0) // maybe fiddle with the value for your specific game, but this should work for most id imagine
		return false; // not on our screen (behind us)

	// do some funky stuff here to scale the screenPosition to your screen dimensions
	// This is called converting from Clip-Space to Window-Space ( some call it NDC -- Normalized Device Coordinates )
	// First step is this thing called Perspective divide, simply divide your values by W.
	clipCoords.x /= clipCoords.w;
	clipCoords.y /= clipCoords.w;
	clipCoords.y *= -1; // This is done because of how y works ingame vs in our mind. When we say: 100 Y vs 0 Y, we generally think of 100 being taller/higher on the screen, but it would be the inverse in actuality. Likewise in the W2S, this ensures our projections move the right way, i.e. going down when we look up and vice versa.

	// These are now NDC values, lets save them.
	Vector2 NDC(clipCoords.x, clipCoords.y);	// stored for later calculations

	// Next, you half those values and add 0.5.
	clipCoords.x /= 2;
	clipCoords.y /= 2;

	// Next, you multiply by your screen dimensions, how else are you gonna scale it!?!?
	// Dont be a square. Get your screen dimensions programmatically. If you're on dx9, pDevice->GetViewPort() and the view port will spit out width+height.
	// DX11 is similar except you get it from the swapchain.
	clipCoords.x *= screenDimensions.x;
	clipCoords.y *= screenDimensions.y;

	// Finally, because NDC generally range from -1 to 1, 0 is actually the center of our screen. So following that logic, if we had an NDC of 0, which means our x_value * width == 0, we'd be drawing at X=0 instead of X=middle of our screen.
	// So we rectify that by adding half the width & height to our final values.
	clipCoords.x += (NDC.x + screenDimensions.x / 2);
	clipCoords.y += (NDC.y + screenDimensions.y / 2);

	// So, thanks to matrix multiplication, we've multiplied a 1x4 matrix ( position ) and a 4x4 matrix ( viewprojection ) and got a resultant 1x4 matrix!
	// And since we only need 2 dimensions, we discard one of them (z), and use the other (w) to determine whether we even need to draw

	screenPosition.x = clipCoords.x;
	screenPosition.y = clipCoords.y;

	// Last check to see if target is outside of screen
	if (screenPosition.x < 0.0f || screenPosition.x > screenDimensions.x || screenPosition.y < 0.0f || screenPosition.y > screenDimensions.y)
		return false;

	return true;
}

Vector3 Hacks::getEnemyHeadPos(PlayerEntity* enemyPtr)
{
	if (!isValidEntity(enemyPtr))
		return Vector3();

	Vector3 enemyHeadPos = enemyPtr->headPos;
	enemyHeadPos.z += 0.30f; // arbitrary value

	return enemyHeadPos;
}

Vector3 Hacks::getEnemyUpperBoxPos(PlayerEntity* enemyPtr)
{
	if (!isValidEntity(enemyPtr))
		return Vector3();

	Vector3 enemyHeadPos = enemyPtr->headPos;
	enemyHeadPos.z += 0.8f;	// arbitrary value

	return enemyHeadPos;
}

Vector3 Hacks::getEnemyLowerBoxPos(PlayerEntity* enemyPtr)
{
	if (!isValidEntity(enemyPtr))
		return Vector3();

	Vector3 enemyHeadPos = enemyPtr->pos;
	enemyHeadPos.z -= 0.5f;	// arbitrary value

	return enemyHeadPos;
}

void Hacks::drawEsp(GameObjects* gameObjects, Vector2 screenDimensions, bool drawEspBox, float espBoxThickness, Vector4 espBoxColor, 
	bool displayName, bool displayHealth, bool displayShield, bool drawEspHead, float espHeadThickness, Vector4 espHeadColor)
{
	static GL::Font glFont;
	const static int FONT_HEIGHT = 15;
	const static int FONT_WIDTH = 8;
	HDC currentHDC = wglGetCurrentDC();

	if (!glFont.bBuilt || currentHDC != glFont.hDc)
	{
		glFont.build(FONT_HEIGHT);
	}

	GL::setupOrtho();

	for (PlayerEntity* enemyPtr : getAliveEnemyList(gameObjects))
	{
		// Draw body box
		if (drawEspBox)
		{
			Vector2 enemyLowerBoxPos;
			Vector2 enemyUpperBoxPos;
			bool LowerVisibility = worldToScreen(getEnemyLowerBoxPos(enemyPtr), screenDimensions, enemyLowerBoxPos);
			bool upperVisility = worldToScreen(getEnemyUpperBoxPos(enemyPtr), screenDimensions, enemyUpperBoxPos);
			if (LowerVisibility || upperVisility)
			{
				float distance = gameObjects->myPlayerEntityPtr->headPos.getDistance(enemyPtr->headPos);
				// Calculate enemy width on screen
				float enemyScreenWidth = 1200.0f / distance;	// arbitrary value

				Vector2 boxUpperLeftCoords = Vector2((enemyUpperBoxPos.x + enemyLowerBoxPos.x) / 2.0f - (enemyScreenWidth / 2.0f), enemyUpperBoxPos.y);
				Vector2 boxLowerRightCoords = Vector2((enemyUpperBoxPos.x + enemyLowerBoxPos.x) / 2.0f + (enemyScreenWidth / 2.0f), enemyLowerBoxPos.y);

				// Draw box
				GL::drawOutlineRect(boxUpperLeftCoords.x, boxUpperLeftCoords.y, boxLowerRightCoords.x, boxLowerRightCoords.y, espBoxThickness, &espBoxColor.x);

				// Draw name
				if (displayName)
				{
					Vector2 textCoords = Vector2(glFont.centerText(boxUpperLeftCoords.x, boxLowerRightCoords.x, (float)strlen(enemyPtr->name) * FONT_WIDTH),
												boxLowerRightCoords.y + FONT_HEIGHT);
					glFont.print(textCoords.x, textCoords.y, rgb::white, "%s", enemyPtr->name);
				}

				if (displayHealth)
				{
					// Draw the backgound red bar
					Vector2 healthBarUpperLeftCoords = Vector2(boxLowerRightCoords.x + 5.0f, boxUpperLeftCoords.y + 1);
					Vector2 healthBarLowerRightCoords = Vector2(boxLowerRightCoords.x + 8.0f, boxLowerRightCoords.y - 1);
					GL::drawFilledRect(healthBarUpperLeftCoords.x, healthBarUpperLeftCoords.y, healthBarLowerRightCoords.x, healthBarLowerRightCoords.y, rgb::red);
					// Draw the frontground green bar (proportional to health value)
					float barSize = healthBarLowerRightCoords.y - healthBarUpperLeftCoords.y;
					healthBarUpperLeftCoords.y += barSize - barSize * (enemyPtr->health / 100.0f);
					GL::drawFilledRect(healthBarUpperLeftCoords.x, healthBarUpperLeftCoords.y, healthBarLowerRightCoords.x, healthBarLowerRightCoords.y, rgb::green);
				}

				if (displayShield)
				{
					// Do not display backgorund for the shield bar
					Vector2 shieldhBarUpperLeftCoords = Vector2(boxLowerRightCoords.x + 5.0f, boxUpperLeftCoords.y + 1);
					Vector2 shieldBarLowerRightCoords = Vector2(boxLowerRightCoords.x + 8.0f, boxLowerRightCoords.y - 1);
					// Do not overlap health bar if displayed
					if (displayHealth)
					{
						shieldhBarUpperLeftCoords.x += 6.0f;
						shieldBarLowerRightCoords.x += 6.0f;
					}
					// Draw the blue bar (proportional to shield value)
					float barSize = shieldBarLowerRightCoords.y - shieldhBarUpperLeftCoords.y;
					shieldhBarUpperLeftCoords.y += barSize - barSize * (enemyPtr->shield / 100.0f);
					GL::drawFilledRect(shieldhBarUpperLeftCoords.x, shieldhBarUpperLeftCoords.y, shieldBarLowerRightCoords.x, shieldBarLowerRightCoords.y, rgb::blue);
				}
			}
		}
		// Draw head circle
		if (drawEspHead)
		{
			Vector2 enemyHeadScreenPos;
			if (worldToScreen(getEnemyHeadPos(enemyPtr), screenDimensions, enemyHeadScreenPos))
			{
				// Calculate head circle radius
				float distance = gameObjects->myPlayerEntityPtr->headPos.getDistance(enemyPtr->headPos);
				float headCircleRadius = 200 / distance;	// arbitrary value
				GL::drawOutlineCircle(enemyHeadScreenPos.x, enemyHeadScreenPos.y, headCircleRadius, 50, espHeadThickness, &espHeadColor.x);
			}
		}
	}

	GL::restoreGL();

	return;
}

bool Hacks::wallhack(bool enable)
{
	// Attach a hook to the from the 16th to the 22th byte of the glDrawElements function
	static MidHook32 midHook32glDrawElements((LPVOID)((DWORD)original_glDrawElements + 0x16), hooked_glDrawElements, 6);
	midHookList.insert(&midHook32glDrawElements);

	if (enable)
	{
		return midHook32glDrawElements.enable();
	}
	else
		midHook32glDrawElements.disable();

	return true;
}
