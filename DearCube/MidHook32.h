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
	SIZE_T len;
	BYTE originalBytes[MAX_LEN];
	bool bStatus;
	bool bGatewayInstanciated;

public:
	MidHook32(LPVOID srcAddr, LPVOID dstAddr, SIZE_T len);
	MidHook32(LPCSTR moduleName, LPCSTR functionName, DWORD offsetToFunctionBeginning, LPVOID dstAddr, SIZE_T len);
	~MidHook32();

	// /!\ A mid hook function has to be declared as __declspec(naked) and end with a ret inst /!\ //
	// Because of the PUSHAD and the CALL instructions, values in stack will be shifted of 0x24 bytes
	bool enable();
	void disable();
	void toggle();

	static LPVOID getFunctionAddr(LPCSTR moduleName, LPCSTR functionName);

	static LPVOID midHookTrampoline(LPVOID srcAddr, LPVOID dstAddr, const SIZE_T len);
	static bool midHookDetour(LPVOID srcAddr, LPVOID dstAddr, SIZE_T len);
};
