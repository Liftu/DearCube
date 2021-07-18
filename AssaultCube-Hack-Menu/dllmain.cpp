// dllmain.cpp : Définit le point d'entrée de l'application DLL.
#include "pch.h"
#include "hacks.h"
#include "gameVariables.h"
#include "gameStructures.h"

#define loopv(v)    for(int i = 0; i<(v).length(); i++)

DWORD WINAPI injectedThread(HMODULE hModule)
{
    // Create consloe for debugging purpose
    /**/AllocConsole();
    /**/FILE* con;
    /**/freopen_s(&con, "CONOUT$", "w", stdout);
    

    // Get module base
    DWORD moduleBaseAddr = (DWORD)GetModuleHandle(L"ac_client.exe");
    /**/std::cout << "ac_client base address : 0x" << std::hex << moduleBaseAddr << std::endl;

    // Retrieve in-game objects
    
    // Get my player entity ptr
    PlayerEntity* myPlayerEntity = Hacks::getMyPlayerEntity(moduleBaseAddr);
    /**/std::cout << std::hex << "My player entity address : 0x" << (DWORD*)myPlayerEntity << std::endl;

    // Get entity list as a vector of PlayerEntity pointers
    PlayerEntity** entityList = Hacks::getPlayerEntityList(moduleBaseAddr);
    DWORD numberOfPlayer = Hacks::getNumberOfPlayer(moduleBaseAddr);
    /**/std::cout << std::dec << numberOfPlayer << " Players :" << std::endl;
    /**///std::cout << "Players :" << std::endl;
    std::vector<PlayerEntity*> validEntityList = Hacks::getValidEntityList(entityList, numberOfPlayer);

  
    // Hack loop
    /**/std::cout << "Hack loop : " << std::endl;
    bool running = true;
    bool enable = false;
    while (running)
    {
        // Key input
        if (GetAsyncKeyState(VK_END) & 1)
            running = false;
        else if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            /**/std::cout << "Showing/hide menu" << std::endl; 
            continue;
        }
        // This will be replaced by an ImGui menu
        else if (GetAsyncKeyState(VK_F1) & 1)
        {
            ///**/std::cout << "Showing/hide menu" << std::endl; 
            if (Hacks::toggleWeaponHack(myPlayerEntity, Hacks::getCurrentWeaponType(myPlayerEntity), Hacks::WeaponHackTypes::NoSpread))
                /**/std::cout << "Toggled no spread." << std::endl;
            /**/else
                /**/std::cout << "Unable to toggle no spread." << std::endl;

            continue;
        }
        else if (GetAsyncKeyState(VK_F2) & 1)
        {
            ///**/std::cout << "Showing/hide menu" << std::endl; 
            if (Hacks::toggleWeaponHack(myPlayerEntity, Hacks::getCurrentWeaponType(myPlayerEntity), Hacks::WeaponHackTypes::NoRecoil))
                /**/std::cout << "Toggled no recoil." << std::endl;
            /**/else
                /**/std::cout << "Unable to toggle no recoil." << std::endl;

            continue;
        }
        else if (GetAsyncKeyState(VK_F3) & 1)
        {
            ///**/std::cout << "Showing/hide menu" << std::endl; 
            if (Hacks::toggleWeaponHack(myPlayerEntity, Hacks::getCurrentWeaponType(myPlayerEntity), Hacks::WeaponHackTypes::NoKickback))
                /**/std::cout << "Toggled no kickback." << std::endl;
            /**/else
                /**/std::cout << "Unable to toggle kickback." << std::endl;

            continue;
        }
        else if (GetAsyncKeyState(VK_F4) & 1)
        {
            ///**/std::cout << "Showing/hide menu" << std::endl; 
            if (Hacks::toggleWeaponHack(myPlayerEntity, Hacks::getCurrentWeaponType(myPlayerEntity), Hacks::WeaponHackTypes::NoSelfKickback))
                /**/std::cout << "Toggled no self kickback." << std::endl;
            /**/else
                /**/std::cout << "Unable to toggle no self kickback." << std::endl;

            continue;
        }
        else if (GetAsyncKeyState(VK_F5) & 1)
        {
            ///**/std::cout << "Showing/hide menu" << std::endl; 
            if (Hacks::toggleWeaponHack(myPlayerEntity, Hacks::getCurrentWeaponType(myPlayerEntity), Hacks::WeaponHackTypes::SemiAuto))
                /**/std::cout << "Toggled semi auto." << std::endl;
            /**/else
                /**/std::cout << "Unable to toggle semi auto." << std::endl;

            continue;
        }
        else if (GetAsyncKeyState(VK_F6) & 1)
        {
            enable = !enable;
            ///**/std::cout << "Showing/hide menu" << std::endl; 
            if (Hacks::toggleAllWeaponsHack(myPlayerEntity, Hacks::WeaponHackTypes::NoRecoil, enable))
                /**/std::cout << "Toggled all hack." << std::endl;
            /**/else
                /**/std::cout << "Unable to toggle all hack." << std::endl;

            continue;
        }

        // Actual hacks
        ///**/std::cout << std::dec << "My health : " << myPlayerEntity->health << std::endl;
        ///**/std::cout << std::dec << "My health : " << myPlayerEntity->currentWeaponPtr->playerOwnerPtr->health << std::endl;
        Sleep(250);
    }


    // Cleanup & exit
    /**/std::cout << "Exiting..." << std::endl;
    /**/if (con)
    /**/    fclose(con);
    /**/FreeConsole();
    FreeLibraryAndExitThread(hModule, EXIT_SUCCESS);

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
            // Creates the thread that will do the work and return.
            HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)injectedThread, hModule, 0, NULL);
            if (hThread)
                CloseHandle(hThread);
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

