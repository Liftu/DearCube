// dllmain.cpp : Définit le point d'entrée de l'application DLL.
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>

#include "hook32.h"
#include "Menu.h"
#include "glDraw.h"
#include "glText.h"
#include "gameVariables.h"
#include "gameStructures.h"
#include "hacks.h"

// Globals
// Get module base address
DWORD moduleBaseAddr = (DWORD)GetModuleHandle(L"ac_client.exe");
// The menu
Menu* menu;


// Hooking
// Define a function pointer
typedef BOOL(__stdcall* t_wglSwapBuffers)(HDC hDc);
// Declare the function pointer variable
t_wglSwapBuffers gateway_wglSwapBuffers;
// Define the hoooked function (this will be our mainloop)

//GL::Font glFont;
//const int FONT_HEIGHT = 15;
//const int FONT_WIDTH = 9;
//
//const char* example1 = "ESP Box";
//const char* example2 = "I'm inside fam";
//
//void draw()
//{
//    HDC currentHDC = wglGetCurrentDC();
//
//    if (!glFont.bBuilt || currentHDC != glFont.hDc)
//    {
//        glFont.build(FONT_HEIGHT);
//    }
//
//    GL::setupOrtho();
//
//    // Box
//    GL::drawOutline(300, 300, 200, 200, 1.0f, rgb::red);
//
//    // Example text 1
//    float textPointX = glFont.centerText(300, 200, strlen(example1) * FONT_WIDTH);
//    float textPointY = 300 - FONT_HEIGHT / 2;
//    glFont.print(textPointX, textPointY, rgb::green, "%s", example1);
//
//    // Example text 2
//    Vec3 insideTextPoint = glFont.centerText(300, 300 + 100, 200, 200, strlen(example2) * FONT_WIDTH, FONT_HEIGHT);
//    glFont.print(insideTextPoint.x, insideTextPoint.y, rgb::ligthGrey, "%s", example2);
//
//    GL::restoreGL();
//}

BOOL __stdcall hooked_wglSwapBuffers(HDC hDc)
{
    static bool enable = false;

    // Retrieve in-game objects
    GameObjects* gameObjects = Hacks::getGameObjectsPtr(moduleBaseAddr);
    PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
    EntityVector& playerEntityVector = gameObjects->playerEntityVector;
    // Declaration with & operator so we do not copy the content, 
    // instead we get a reference witouh having to declare a pointer.

    ///**/std::cout << std::hex << "My player entity address : 0x" << (DWORD*)myPlayerEntityPtr << std::endl;

    ///**/std::cout << std::dec << playerEntityVector.length << " Players :" << std::endl;
    ///**/std::cout << "Players :" << std::endl;
    std::vector<PlayerEntity*> validEntityList = Hacks::getValidEntityList(&playerEntityVector);

    myPlayerEntityPtr->health = 1337;

    // Menu
    menu->render();

    // Key inputs
    if (GetAsyncKeyState(VK_END) & 1)
    {
        menu->shutdown();
        // Unhooking
    }
    // This will be replaced by an ImGui menu
    //else if (GetAsyncKeyState(VK_F1) & 1)
    //{
    //    ///**/std::cout << "Showing/hide menu" << std::endl; 
    //    if (Hacks::toggleWeaponHack(myPlayerEntityPtr, Hacks::getCurrentWeaponType(myPlayerEntityPtr), Hacks::WeaponHackTypes::NoSpread))
    //        /**/std::cout << "Toggled no spread." << std::endl;
    //    /**/else
    //        /**/std::cout << "Unable to toggle no spread." << std::endl;
    //}
    //else if (GetAsyncKeyState(VK_F2) & 1)
    //{
    //    ///**/std::cout << "Showing/hide menu" << std::endl; 
    //    if (Hacks::toggleWeaponHack(myPlayerEntityPtr, Hacks::getCurrentWeaponType(myPlayerEntityPtr), Hacks::WeaponHackTypes::NoRecoil))
    //        /**/std::cout << "Toggled no recoil." << std::endl;
    //    /**/else
    //        /**/std::cout << "Unable to toggle no recoil." << std::endl;
    //}
    //else if (GetAsyncKeyState(VK_F3) & 1)
    //{
    //    ///**/std::cout << "Showing/hide menu" << std::endl; 
    //    if (Hacks::toggleWeaponHack(myPlayerEntityPtr, Hacks::getCurrentWeaponType(myPlayerEntityPtr), Hacks::WeaponHackTypes::NoKickback))
    //        /**/std::cout << "Toggled no kickback." << std::endl;
    //    /**/else
    //        /**/std::cout << "Unable to toggle kickback." << std::endl;
    //}
    //else if (GetAsyncKeyState(VK_F4) & 1)
    //{
    //    ///**/std::cout << "Showing/hide menu" << std::endl; 
    //    if (Hacks::toggleWeaponHack(myPlayerEntityPtr, Hacks::getCurrentWeaponType(myPlayerEntityPtr), Hacks::WeaponHackTypes::NoSelfKickback))
    //        /**/std::cout << "Toggled no self kickback." << std::endl;
    //    /**/else
    //        /**/std::cout << "Unable to toggle no self kickback." << std::endl;
    //}
    //else if (GetAsyncKeyState(VK_F5) & 1)
    //{
    //    ///**/std::cout << "Showing/hide menu" << std::endl; 
    //    if (Hacks::toggleWeaponHack(myPlayerEntityPtr, Hacks::getCurrentWeaponType(myPlayerEntityPtr), Hacks::WeaponHackTypes::SemiAuto))
    //        /**/std::cout << "Toggled semi auto." << std::endl;
    //    /**/else
    //        /**/std::cout << "Unable to toggle semi auto." << std::endl;
    //}
    //else if (GetAsyncKeyState(VK_F6) & 1)
    //{
    //    enable = !enable;
    //    ///**/std::cout << "Showing/hide menu" << std::endl; 
    //    if (Hacks::toggleAllWeaponsHack(myPlayerEntityPtr, Hacks::WeaponHackTypes::NoRecoil, enable))
    //        /**/std::cout << "Toggled all hack." << std::endl;
    //    /**/else
    //        /**/std::cout << "Unable to toggle all hack." << std::endl;
    //}

    //draw();

    // Update screen by calling the original wglSwapBuffers function
    return gateway_wglSwapBuffers(hDc);
}

DWORD WINAPI injectedThread(HMODULE hModule)
{
    // Create consloe for debugging purpose
    ///**/AllocConsole();
    ///**/FILE* con;
    ///**/freopen_s(&con, "CONOUT$", "w", stdout);
    //
    ///**/std::cout << "ac_client base address : 0x" << std::hex << moduleBaseAddr << std::endl;

    ///**/std::cout << "Hack loop : " << std::endl;

    // Menu setup
    menu = new Menu(FindWindowA(NULL, "AssaultCube"));

    // Hooking
    // the OpenGL SwapBuffers function is called at every screen update
    Hook32 swapBuffersHook32("opengl32.dll", "wglSwapBuffers", hooked_wglSwapBuffers, &gateway_wglSwapBuffers, 5);
    swapBuffersHook32.enable();
    
    // END key to terminate 
    while (!(GetAsyncKeyState(VK_END) & 1))
        Sleep(5);
    swapBuffersHook32.disable();

    // Cleanup & exit (WHEN UNHOOKING IMPLEMENTED)
    ///**/std::cout << "Exiting..." << std::endl;
    ///**/if (con)
    ///**/    fclose(con);
    ///**/FreeConsole();
    // SOMETIMES THE WHOLE PROCCESS CRACHES WHEN CALLING THIS
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

