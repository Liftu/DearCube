#include "MidHook32.h"

MidHook32::MidHook32(LPVOID srcAddr, LPVOID dstAddr, size_t len)
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

MidHook32::MidHook32(LPCSTR moduleName, LPCSTR functionName, DWORD offsetToFunctionBeginning, LPVOID dstAddr, size_t len)
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

LPVOID MidHook32::midHookTrampoline(LPVOID srcAddr, LPVOID dstAddr, const size_t len)
{
	// len as to be the minimum size of the complete 
	// intructions opcodes that will be overriden
	// it shouldn't take more than 16 bytes
	// Minimum jump size is 5 bytes in 32 bits
	if (len < MIN_LEN || len > MAX_LEN)
		return nullptr;

	// Create the trampoline gateway
	// 1 byte for PUSHAD						(this adds 0x20 bytes on the stack, so we have to pass esp to the hooking func)
	// 5 bytes for the CALL dstAddr
	// 1 byte for the POPAD
	// len bytes for the stolen bytes
	// 5 bytes for the JMP original
	int gatewayLen = 1 + 5  + 1 + len + 5;
	DWORD gateway = (DWORD)VirtualAlloc(NULL, gatewayLen, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (gateway == NULL)
		return nullptr;
	memset((LPVOID)gateway, 0x90, gatewayLen);

	// Write trampoline gateway shellcode
	int bytesWritten = 0;

	// PUSHAD
	*(BYTE*)(gateway) = 0x60;
	bytesWritten += 1;

	// Get the gateway to destination relative address
	DWORD dstRelativeAddr = (DWORD)dstAddr - (gateway + 5 + bytesWritten);
	// CALL dstAddr
	*(BYTE*)(gateway + bytesWritten) = 0xE8;
	bytesWritten += 1;
	*(DWORD*)(gateway + bytesWritten) = dstRelativeAddr;
	bytesWritten += 4;

	// POPAD
	*(BYTE*)(gateway + bytesWritten) = 0x61;
	bytesWritten += 1;

	// Stolen bytes
	memcpy_s((LPVOID)(gateway + bytesWritten), len, srcAddr, len);
	bytesWritten += len;

	// Get the gateway to source relative address
	DWORD srcRelativeAddr = ((DWORD)srcAddr + len) - (gateway + 5 + bytesWritten);
	// JMP srcAddr + len
	*(BYTE*)(gateway + bytesWritten) = 0xE9;
	bytesWritten += 1;
	*(DWORD*)(gateway + bytesWritten) = srcRelativeAddr;
	bytesWritten += 4;

	return (LPVOID)gateway;
}

bool MidHook32::midHookDetour(LPVOID srcAddr, LPVOID dstAddr, const size_t len)
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