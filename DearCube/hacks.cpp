#include "hacks.h"

DWORD Hacks::getModuleBaseAddr()
{
	return (DWORD)GetModuleHandle(L"ac_client.exe");
}


// Entities stuff

GameObjects* Hacks::getGameObjectsPtr()
{
	DWORD moduleBaseAddr = getModuleBaseAddr();
	if (moduleBaseAddr == NULL)
		return nullptr;

	return reinterpret_cast<GameObjects*>(moduleBaseAddr + o_gameObjects);
}

//PlayerEntity* Hacks::getMyPlayerEntityPtr()
//{
//	GameObjects* gameObjectsPtr = getGameObjectsPtr();
//	if (gameObjectsPtr == nullptr)
//		return nullptr;
//
//	return gameObjectsPtr->myPlayerEntityPtr;
//}

//EntityVector* Hacks::getPlayerEntityVectorPtr()
//{
//	GameObjects* gameObjectsPtr = getGameObjectsPtr();
//	if (gameObjectsPtr == nullptr)
//		return nullptr;
//
//	return &gameObjectsPtr->playerEntityVector;
//}

//int Hacks::getNumberOfPlayer()
//{
//	GameObjects* gameObjectsPtr = getGameObjectsPtr();
//	if (gameObjectsPtr == nullptr)
//		return -1;
//
//	return gameObjectsPtr->playerEntityVector.length;
//}

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
	// If team based game modes, then remove allies and spectators from the list
	else if (gameMode == GameModes::TDM || gameMode == GameModes::TSURV || gameMode == GameModes::CTF || gameMode == GameModes::BTDM || gameMode == GameModes::TOSOK ||
		gameMode == GameModes::TKTF || gameMode == GameModes::TPF || gameMode == GameModes::TLSS || gameMode == GameModes::BTSURV || gameMode == GameModes::BTOSOK)
	{
		// Remove allies and spectators (filter function is a lambda)
		enemyList.erase(
			std::remove_if(
				enemyList.begin(),
				enemyList.end(),
				[myPlayerEntityPtr](PlayerEntity* playerEntityPtr)
				{
					return (myPlayerEntityPtr->team == playerEntityPtr->team || (playerEntityPtr->team != Teams::Blue && playerEntityPtr->team != Teams::Red));
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
				return (playerEntityPtr->currentState != States::Alive);
			}),
		aliveEnemyList.end());

	return aliveEnemyList;
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
	DWORD traceLineFunctionAddr = moduleBaseAddr + o_TraceLineFunction;
	Vector3 myPos = myPlayerEntityPtr->headPos;
	TraceLineResult traceLineResult;

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
		call traceLineFunctionAddr;		Call to the traceLine game function
		add esp, 0x24;				Clean the stack (9 * 4 = 36 = 0x24)
	}

	return !(bool)traceLineResult.collided;
}

bool Hacks::triggerbot(GameObjects* gameObjects, float degreeDistanceToShoot)
{
	std::vector<PlayerEntity*> enemyList = getAliveEnemyList(gameObjects);
	PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
	Vector2 myViewAngles(myPlayerEntityPtr->viewAngles.x, myPlayerEntityPtr->viewAngles.y);

	myPlayerEntityPtr->isShooting = false;	// Stop shooting if shooting

	for (PlayerEntity* enemyEntityPtr : enemyList)
	{
		if (!isTargetVisible(myPlayerEntityPtr, enemyEntityPtr->headPos))
			continue;

		// Instead I could reverse the call to traceLine that updates the HUD and recreate it here.
		Vector2 viewAnglesToEnemy = Geom::calcAngle(myPlayerEntityPtr->headPos, enemyEntityPtr->headPos);
		if (myViewAngles.getDistance(viewAnglesToEnemy) <= degreeDistanceToShoot)
		{
			// Shoot
			myPlayerEntityPtr->isShooting = true;
			return true;
		}
	}

	return false;
}
