#include "Hook32.h"

Hook32::Hook32(LPVOID srcAddr, LPVOID dstAddr, LPVOID ptrToGatewayFuncPtr, SIZE_T len)
{
	this->srcAddr = srcAddr;
	this->dstAddr = dstAddr;
	this->ptrToGatewayFuncPtr = ptrToGatewayFuncPtr;

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
}

Hook32::Hook32(LPCSTR moduleName, LPCSTR functionName, LPVOID dstAddr, LPVOID ptrToGatewayFuncPtr, SIZE_T len)
{
	LPVOID srcAddr = getFunctionAddr(moduleName, functionName);
	if (srcAddr)
		//Hook32(srcAddr, dstAddr, ptrToGatewayFuncPtr, len);	// Cannot do this...
	{
		this->srcAddr = srcAddr;
		this->dstAddr = dstAddr;
		this->ptrToGatewayFuncPtr = ptrToGatewayFuncPtr;

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
	}
	else
		throw(EXCEPTION_HOOK_INCORRECT_MODULE_OR_FUNCTION_NAME);
}

Hook32::~Hook32()
{
	disable();
}

void Hook32::enable()
{
	if (this->bStatus)
		return;

	// Save original bytes
	memcpy_s(this->originalBytes, MAX_LEN, this->srcAddr, this->len);
	// Apply trampoline
	*(DWORD*)(this->ptrToGatewayFuncPtr) = (DWORD)trampolineHook32(this->srcAddr, this->dstAddr, this->len);
	
	this->bStatus = true;
}

void Hook32::disable()
{
	if (!this->bStatus)
		return;

	// Save original page protection
	DWORD originalProtection;
	VirtualProtect(srcAddr, len, PAGE_EXECUTE_READWRITE, &originalProtection);
	// Restore original bytes
	memcpy_s(this->srcAddr, MAX_LEN, this->originalBytes, this->len);
	// Restore original page protection
	VirtualProtect(srcAddr, len, originalProtection, &originalProtection);

	this->bStatus = false;
}

void Hook32::toggle()
{
	if (this->bStatus)
		disable();
	else
		enable();
}

// Static functions 

LPVOID Hook32::getFunctionAddr(LPCSTR moduleName, LPCSTR functionName)
{
	HMODULE hModule = GetModuleHandleA(moduleName);
	if (!hModule)
		return nullptr;
		
	return GetProcAddress(hModule, functionName);
}

bool Hook32::detour32(LPVOID srcAddr, LPVOID dstAddr, const SIZE_T len)
{
	// len has to be the minimum size of the complete 
	// intructions opcodes that will be override
	// it shouldn't take more than 16 bytes
	// Minimum jump size is 5 bytes in 32 bits
	if (len < MIN_LEN || len > MAX_LEN)
		return false;

	// Save original page protection
	DWORD originalProtection;
	VirtualProtect(srcAddr, len, PAGE_EXECUTE_READWRITE, &originalProtection);

	// Casts allow us to perform adds and subs (and some like "(DWORD)srcAddr" avoid pointer arithmetic like +4)

	// Get the source to destination relative address
	DWORD dstRelativeAddr = (DWORD)dstAddr - (DWORD)srcAddr - 5;

	// Just to make things nice (when overriding half of an inst, it won't break display in disasm).
	memset(srcAddr, 0x90, len);

	// Write the JMP opcode to the src address (1 byte)
	*(BYTE*)srcAddr = 0xE9;

	// Write the dst relative address to the jump (4 bytes)
	*(DWORD*)((DWORD)srcAddr + 1) = dstRelativeAddr;

	// Restore original page protection
	VirtualProtect(srcAddr, len, originalProtection, &originalProtection);

	return true;
}

LPVOID Hook32::trampolineHook32(LPVOID srcAddr, LPVOID dstAddr, const SIZE_T len)
{
	// len as to be the minimum size of the complete 
	// intructions opcodes that will be override
	// it shouldn't take more than 16 bytes
	// Minimum jump size is 5 bytes in 32 bits
	if (len < MIN_LEN || len > MAX_LEN)
		return nullptr;

	// Create the gateway (5 bytes for the JMP relativeAddr)
	DWORD gateway = (DWORD)VirtualAlloc(NULL, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// Write the stolen bytes to the gateway
	memcpy_s((LPVOID)gateway, len, srcAddr, len);

	// Get the gateway to destination relative address
	// (destination here is the addr of the inst following the stolen bytes)
	DWORD gatewayRelativeAddr = (DWORD)srcAddr - gateway - 5;

	// Add the JMP opcode to the end of the gateway (1 byte)
	*(BYTE*)(gateway + len) = 0xE9;

	// Write the relative address of the gateway to the jump (4 bytes)
	*(DWORD*)(gateway + len + 1) = gatewayRelativeAddr;

	// perform the detour
	detour32(srcAddr, dstAddr, len);

	return (LPVOID)gateway;
}

// /!\ A mid hook function has to be declared as __declspec(naked) and end with a ret inst /!\ //
// Because of the PUSHAD and the CALL instructions, values in stack will be shifted of 0x24 bytes
bool Hook32::midHookTrampoline(LPVOID srcAddr, LPVOID dstAddr, const SIZE_T len)
{	
	// len as to be the minimum size of the complete 
	// intructions opcodes that will be overriden
	// it shouldn't take more than 16 bytes
	// Minimum jump size is 5 bytes in 32 bits
	if (len < MIN_LEN || len > MAX_LEN)
		return false;

	// Create the trampoline gateway
	// 1 byte for PUSHAD
	// 5 bytes for the CALL dstAddr
	// 1 byte for the POPAD
	// len bytes for the stolen bytes
	// 5 bytes for the JMP original
	DWORD gateway = (DWORD)VirtualAlloc(NULL, (1 + 5 + 1 + len + 5), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (gateway == NULL)
		return false;

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


	// Write source hook shellcode

	// Save original srcAddr page protection
	DWORD originalProtection;
	VirtualProtect(srcAddr, len, PAGE_EXECUTE_READWRITE, &originalProtection);

	// Just to make things nice (when overriding half of an inst, it won't break display in disasm).
	memset(srcAddr, 0x90, len);


	// Get the source to gateway relative address
	DWORD gatewayRelativeAddr = gateway - (DWORD)srcAddr - 5;

	// JMP gateway
	*(BYTE*)srcAddr = 0xE9;
	*(DWORD*)((DWORD)srcAddr + 1) = gatewayRelativeAddr;

	// Restore original page protection
	VirtualProtect(srcAddr, len, originalProtection, &originalProtection);

	return true;
}