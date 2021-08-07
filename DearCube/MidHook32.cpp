#include "MidHook32.h"

MidHook32::MidHook32(LPVOID srcAddr, LPVOID dstAddr, SIZE_T len)
{
	this->srcAddr = srcAddr;
	this->dstAddr = dstAddr;

	// len has to be the minimum size of the complete
	// intructions opcodes that will be override
	// it shouldn't take more than 16 bytes
	// Minimum jump size is 5 bytes in 32 bits
	if (len < MIN_LEN || len > MAX_LEN)
		throw(EXCEPTION_HOOK_INCORRECT_LEN);
	else
		this->len = len;

	memset(this->originalBytes, 0, MAX_LEN);
	this->bGatewayInstanciated = false;
	this->bStatus = false;
}

MidHook32::MidHook32(LPCSTR moduleName, LPCSTR functionName, DWORD offsetToFunctionBeginning, LPVOID dstAddr, SIZE_T len)
{
	LPVOID srcAddr = getFunctionAddr(moduleName, functionName);
	if (srcAddr)
		//MidHook32(srcAddr, dstAddr, ptrToGatewayFuncPtr, len);	// Cannot call other constructor...
	{
		this->srcAddr = (LPVOID)((DWORD)srcAddr + offsetToFunctionBeginning);
		this->dstAddr = dstAddr;

		// len has to be the minimum size of the complete
		// intructions opcodes that will be override
		// it shouldn't take more than 16 bytes
		// Minimum jump size is 5 bytes in 32 bits
		if (len < MIN_LEN || len > MAX_LEN)
			throw(EXCEPTION_HOOK_INCORRECT_LEN);
		else
			this->len = len;

		memset(this->originalBytes, 0, MAX_LEN);
		this->bStatus = false;
		this->bGatewayInstanciated = false;
	}
	else
		throw(EXCEPTION_HOOK_INCORRECT_MODULE_OR_FUNCTION_NAME);
}

MidHook32::~MidHook32()
{
	disable();
}

// /!\ A mid hook function has to be declared as __declspec(naked) and end with a ret inst /!\ //
// Because of the PUSHAD and the CALL instructions, values in stack will be shifted of 0x24 bytes
bool MidHook32::enable()
{
	if (this->bStatus)
		return false;

	// Save original bytes
	memcpy_s(this->originalBytes, MAX_LEN, this->srcAddr, this->len);

	if (!this->bGatewayInstanciated)
	{
		// Create trampoline gateway
		this->gatewayFuncAddr = this->midHookTrampoline(this->srcAddr, this->dstAddr, this->len);
		
		// If it failed (ptr points to nullptr addr)
		if (this->gatewayFuncAddr == nullptr)
			return false;

		this->bGatewayInstanciated = true;
	}

	// Apply detour
	this->bStatus = this->midHookDetour(this->srcAddr, this->gatewayFuncAddr, this->len);
	
	return this->bStatus;
}

void MidHook32::disable()
{
	if (!this->bStatus)
		return;

	// Save original page protection
	DWORD originalProtection;
	VirtualProtect(srcAddr, len, PAGE_EXECUTE_READWRITE, &originalProtection);
	// Restore original bytes to disable detour
	memcpy_s(this->srcAddr, MAX_LEN, this->originalBytes, this->len);
	// Restore original page protection
	VirtualProtect(srcAddr, len, originalProtection, &originalProtection);

	this->bStatus = false;
}

void MidHook32::toggle()
{
	if (this->bStatus)
		disable();
	else
		enable();
}

// Static functions 

LPVOID MidHook32::getFunctionAddr(LPCSTR moduleName, LPCSTR functionName)
{
	HMODULE hModule = GetModuleHandleA(moduleName);
	if (!hModule)
		return nullptr;
		
	return GetProcAddress(hModule, functionName);
}

LPVOID MidHook32::midHookTrampoline(LPVOID srcAddr, LPVOID dstAddr, const SIZE_T len)
{
	// len as to be the minimum size of the complete 
	// intructions opcodes that will be overriden
	// it shouldn't take more than 16 bytes
	// Minimum jump size is 5 bytes in 32 bits
	if (len < MIN_LEN || len > MAX_LEN)
		return nullptr;

	// Create the trampoline gateway
	// 1 byte for PUSHAD
	// 5 bytes for the CALL dstAddr
	// 1 byte for the POPAD
	// len bytes for the stolen bytes
	// 5 bytes for the JMP original
	DWORD gateway = (DWORD)VirtualAlloc(NULL, (1 + 5 + 1 + len + 5), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (gateway == NULL)
		return nullptr;

	// Write trampoline gateway shellcode

	// PUSHAD
	*(BYTE*)(gateway) = 0x60;

	// Get the gateway to destination relative address
	DWORD dstRelativeAddr = (DWORD)dstAddr - (gateway + 6);
	// CALL dstAddr
	*(BYTE*)(gateway + 1) = 0xE8;
	*(DWORD*)(gateway + 2) = dstRelativeAddr;

	// POPAD
	*(BYTE*)(gateway + 6) = 0x61;

	// Stolen bytes
	memcpy_s((LPVOID)(gateway + 7), len, srcAddr, len);

	// Get the gateway to source relative address
	DWORD srcRelativeAddr = ((DWORD)srcAddr + len) - (gateway + 7 + len + 5);
	// JMP srcAddr + len
	*(BYTE*)(gateway + 7 + len) = 0xE9;
	*(DWORD*)(gateway + 7 + len + 1) = srcRelativeAddr;

	return (LPVOID)gateway;
}

bool MidHook32::midHookDetour(LPVOID srcAddr, LPVOID dstAddr, SIZE_T len)
{
	// dstAddr represents the gateway in case of mid hook trampoline

	// len as to be the minimum size of the complete 
	// intructions opcodes that will be overriden
	// it shouldn't take more than 16 bytes
	// Minimum jump size is 5 bytes in 32 bits
	if (len < MIN_LEN || len > MAX_LEN)
		return false;
	
	// Write source hook shellcode

	// Save original srcAddr page protection
	DWORD originalProtection;
	if (VirtualProtect(srcAddr, len, PAGE_EXECUTE_READWRITE, &originalProtection) == 0)
		return false;

	// Just to make things nice (when overriding half of an inst, it won't break display in disasm).
	memset(srcAddr, 0x90, len);

	// Get the source to gateway relative address
	DWORD gatewayRelativeAddr = (DWORD)dstAddr - (DWORD)srcAddr - 5;

	// JMP gateway
	*(BYTE*)srcAddr = 0xE9;
	*(DWORD*)((DWORD)srcAddr + 1) = gatewayRelativeAddr;

	// Restore original page protection
	VirtualProtect(srcAddr, len, originalProtection, &originalProtection);

	return true;
}