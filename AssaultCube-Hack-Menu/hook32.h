#pragma once

#include "pch.h"

#define EXCEPTION_HOOK_INCORRECT_LEN						0x0
#define EXCEPTION_HOOK_INCORRECT_MODULE_OR_FUNCTION_NAME	0x1

#define MIN_LEN 5
#define MAX_LEN 16

class Hook32
{
private:
	LPVOID srcAddr;
	LPVOID dstAddr;
	LPVOID ptrToGatewayFuncPtr;
	SIZE_T len;
	BYTE originalBytes[MAX_LEN];
	bool bStatus;

public:
	Hook32(LPVOID srcAddr, LPVOID dstAddr, LPVOID ptrToGatewayFuncPtr, SIZE_T len);
	Hook32(LPCSTR moduleName, LPCSTR functionName, LPVOID dstAddr, LPVOID ptrToGatewayFuncPtr, SIZE_T len);
	~Hook32();

	void enable();
	void disable();
	void toggle();

	static LPVOID getFunctionAddr(LPCSTR moduleName, LPCSTR functionName);
	static bool detour32(LPVOID srcAddr, LPVOID dstAddr, const SIZE_T len);
	static LPVOID trampolineHook32(LPVOID srcAddr, LPVOID dstAddr, const SIZE_T len);
};
