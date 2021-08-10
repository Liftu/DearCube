#pragma once

#include <Windows.h>

#define EXCEPTION_HOOK_INCORRECT_LEN						0x0
#define EXCEPTION_HOOK_INCORRECT_MODULE_OR_FUNCTION_NAME	0x1

#define MIN_LEN 5
#define MAX_LEN 16

class MidHook32
{
private:
	LPVOID srcAddr;
	LPVOID dstAddr;
	LPVOID gatewayFuncAddr;
	size_t len;
	BYTE originalBytes[MAX_LEN];
	bool bStatus;
	bool bGatewayInstanciated;

public:
	// /!\ A mid hook function has to be declared as __cdecl calling convention /!\ //
	// midHook function declaration : void __cdecl midHook(DWORD edi, DWORD esi, DWORD ebp, DWORD esp, DWORD ebx, DWORD edx, DWORD ecx, DWORD eax)
	// Because because the gateway shellcode will send esp backup as argument to the stack and clean it.
	// /!\ Do not hook on a CALL or a JMP that is relative to the instruction address, else it will break /!\ //
	MidHook32(LPVOID srcAddr, LPVOID dstAddr, size_t len);
	// /!\ A mid hook function has to be declared as __cdecl calling convention /!\ //
	// midHook function declaration : void __cdecl midHook(DWORD edi, DWORD esi, DWORD ebp, DWORD esp, DWORD ebx, DWORD edx, DWORD ecx, DWORD eax)
	// Because because the gateway shellcode will send esp backup as argument to the stack and clean it.
	// /!\ Do not hook on a CALL or a JMP that is relative to the instruction address, else it will break /!\ //
	MidHook32(LPCSTR moduleName, LPCSTR functionName, DWORD offsetToFunctionBeginning, LPVOID dstAddr, size_t len);
	~MidHook32();

	bool enable();
	void disable();
	void toggle();

	static LPVOID getFunctionAddr(LPCSTR moduleName, LPCSTR functionName);

	static LPVOID midHookTrampoline(LPVOID srcAddr, LPVOID dstAddr, const size_t len);
	static bool midHookDetour(LPVOID srcAddr, LPVOID dstAddr, const size_t len);
};
