// dllmain.cpp : Définit le point d'entrée de l'application DLL.
#include <windows.h>

#include "detours.h"
#include "Menu.h"
#include "gameStructures.h"
#include "hacks.h"

// Globals
// Version
const char DEARCUBE_VERSION[] = "1.0";
// The menu
Menu* menu;
// Screen dimensions
Vector2 screenDimensions;


// OpenGL wglSwapBuffers hooking
// Define a function pointer
typedef BOOL(__stdcall* t_wglSwapBuffers)(HDC hDc);
// Declare the function pointer variable
t_wglSwapBuffers gateway_wglSwapBuffers;
// Define the hoooked function (this will be our mainloop)


Vector2 getScreenDimensions()
{
    GLint viewport[4] = { 0 };	// window : 0 = x, 1 = y, 2 = width, 3 = heigth
    glGetIntegerv(GL_VIEWPORT, viewport);
    return Vector2((float)viewport[2], (float)viewport[3]);
}


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

    // Get screen dimensions
    screenDimensions = getScreenDimensions();

    // Draw menu
    menu->render(screenDimensions, gameObjects);

    // Some hacks are directly called by the menu (to improve performances).
    // Call hacks that are external to the menu.
    if (menu->isAimbotEnabled())
    {
        Hacks::aimbot(gameObjects, menu->getFovValue(), menu->getAimSmoothValue());
    }
    if (menu->isTriggerbotEnabled())
    {
        Hacks::triggerbot(gameObjects);
    }
    //Hacks::wallhack(menu->isWallhackEnabled());
    // Drawing functions
    if (menu->getCurrentDrawingTool() == Menu::DrawingTools::DRAWING_TOOL_OPENGL)
    {
        if (menu->isFovCircleEnabled())
            Hacks::drawFov(screenDimensions, menu->getFovValue(), menu->getFovThickness(), menu->getFovColor());

        Hacks::drawEsp(gameObjects, screenDimensions, menu->isEspBoxEnabled(), menu->getEspBoxThickness(), menu->getEspBoxColor(), menu->isEspNameEnabled(),
            menu->isEspHealthBarEnabled(), menu->isEspShieldBarEnabled(), menu->isEspHeadEnabled(), menu->getEspHeadThickness(), menu->getEspHeadColor());
    }

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
            "DearCube was not able to retrieve the window handle for \"%s\". Ensure you injected the DLL into the right process.\nIf this error persists, please contact the developer on Github.",
            windowTitle);
        MessageBoxA(NULL, message, "Unable to retrieve the window handle", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(hModule, EXIT_FAILURE);
    }
    menu = new Menu(hwnd);

    // Hooking
    // the OpenGL SwapBuffers function is called at every screen update
    gateway_wglSwapBuffers = (t_wglSwapBuffers)DetourFindFunction("opengl32.dll", "wglSwapBuffers");
    if (gateway_wglSwapBuffers == NULL)
    {
        MessageBoxA(NULL, "DearCube was not able to hook the function \"wglSwapBuffers\".\nPlease contact the developer on Github.",
            "Unable to hook \"wglSwapBuffers\"", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(hModule, EXIT_FAILURE);
    }
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)gateway_wglSwapBuffers, hooked_wglSwapBuffers);
    DetourTransactionCommit();


    // Idle mode, if menu isn't running anymore we can exit.
    while (menu->isRunning())
        Sleep(5);


    // Cleanup & exit (WHEN UNHOOKING IMPLEMENTED)
    // Unhooking
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)gateway_wglSwapBuffers, hooked_wglSwapBuffers);
    DetourTransactionCommit();
    // Disable all hacks midHooks
    Hacks::disableMidHooks();

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

