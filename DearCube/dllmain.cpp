// dllmain.cpp : Définit le point d'entrée de l'application DLL.
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>

#include "hook32.h"
#include "Menu.h"
#include "glDraw.h"
#include "glText.h"
//#include "gameVariables.h"
#include "gameStructures.h"
#include "hacks.h"

// Globals
// Version
const char DEARCUBE_VERSION[] = "1.0";
// The menu
Menu* menu;


// OpenGL wglSwapBuffers hooking
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
    // Key inputs
    // Check if user want to shutdown menu
    if (GetAsyncKeyState(VK_END) & 1)
    {
        menu->shutdown();
    }

    // Retrieve in-game objects
    GameObjects* gameObjects = Hacks::getGameObjectsPtr();
    PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
    EntityVector& playerEntityVector = gameObjects->playerEntityVector;
    // Declaration with & operator so we do not copy the content, 
    // instead we get a reference witouh having to declare a pointer.

    // Draw menu
    menu->render(gameObjects);

    // Maybe call hacks that are external to the menu like aimbot if enbale
    if (menu->isAimbotEnabled())
    {
        Hacks::aimbot(gameObjects, menu->getFovValue(), menu->getAimSmoothValue());
    }

    //draw();

    // Update screen by calling the original opengl wglSwapBuffers function
    return gateway_wglSwapBuffers(hDc);
}

DWORD WINAPI injectedThread(HMODULE hModule)
{
    // Menu setup
    char windowTitle[] = "AssaultCube";
    HWND hwnd = FindWindowA(NULL, windowTitle);
    if (!hwnd)
    {
        char message[256];
        sprintf_s(message, 256, 
            "DearCube was not able to retrieve the window handle for \"%s\". Ensure you injected the DLL into the right process.\nIf this error persists, please contact the developer on github.", 
            windowTitle);
        MessageBoxA(NULL, message, "Unable to retrieve the window handle", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(hModule, EXIT_FAILURE);
    }
    menu = new Menu(hwnd);

    // Hooking
    // the OpenGL SwapBuffers function is called at every screen update
    Hook32 swapBuffersHook32("opengl32.dll", "wglSwapBuffers", hooked_wglSwapBuffers, &gateway_wglSwapBuffers, 5);
    swapBuffersHook32.enable();
    
    // If menu isn't running, then we can exit.
    while (menu->isRunning())
        Sleep(5);

    // Cleanup & exit (WHEN UNHOOKING IMPLEMENTED)
    // Unhook
    swapBuffersHook32.disable();
    // Exit
    // SOMETIMES THE WHOLE PROCCESS CRACHES WHEN CALLING THIS
    FreeLibraryAndExitThread(hModule, EXIT_SUCCESS);
    // Just to debug when we have a crash on exit.
    // If we don't have the crash without freeing the library,
    // then it probably means that the crash was caused by a hooking.
    //ExitThread(EXIT_SUCCESS);

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

