#pragma once

#include "pch.h"

#define MAX_NUMBER_OF_PLAYER 32

// o_ for offsets
// c_ for constants

const DWORD o_gameObjects			= 0x0010F4F4;
const DWORD o_playerEntityVectorPtr	= 0x0010F4F8;
//const DWORD o_entityListSize		= 0x0010F500;

const DWORD c_playerEntityType	= 0x004E4A98;
const DWORD c_botEntityType		= 0x004E4AC0;

// Weapon constants
const WORD c_defaultWeaponsSpread[(int)WeaponTypes::SIZE]				= { 0x01, 0x35, 0x0A, 0x01, 0x2D, 0x32, 0x12, 0x23, 0x01, 0x32 };
const WORD c_defaultWeaponsKickback[(int)WeaponTypes::SIZE]				= { 0x01, 0x0A, 0x3C, 0x23, 0x0F, 0x32, 0x1E, 0x0A, 0x01, 0x0A };
const WORD c_defaultWeaponsKickbackAnimation[(int)WeaponTypes::SIZE]	= { 0x00, 0x05, 0x04, 0x09, 0x02, 0x04, 0x02, 0x05, 0x01, 0x05 };
const WORD c_defaultWeaponsTargetKickback[(int)WeaponTypes::SIZE]		= { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01 };
const WORD c_defaultWeaponsRecoil[(int)WeaponTypes::SIZE]				= { 0x00, 0x3A, 0x3C, 0x8C, 0x32, 0x55, 0x32, 0x32, 0x00, 0x19 };
const WORD c_defaultWeaponsRecoilAnimation[(int)WeaponTypes::SIZE]		= { 0x00, 0x06, 0x04, 0x09, 0x01, 0x04, 0x00, 0x06, 0x03, 0x06 };
const WORD c_defaultWeaponsIsSemiAutomatic[(int)WeaponTypes::SIZE]		= { 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01 };
