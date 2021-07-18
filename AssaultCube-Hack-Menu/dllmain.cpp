// dllmain.cpp : Définit le point d'entrée de l'application DLL.
#include "pch.h"


DWORD WINAPI injectedThread(HMODULE hModule)
{
    // Create consloe for debug purpose
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "TOTO" << std::endl;

    // Get module base
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    // Hack loop
    bool running = true;
    while (running)
    {
        running = false;
    }

    // Key input

    // Continue write/freeze

    // Cleanup % eject

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)injectedThread, hModule, 0, NULL));
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

