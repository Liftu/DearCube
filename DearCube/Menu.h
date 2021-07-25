#pragma once

#include <Windows.h>
/**/#include <vector>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl2.h"
#include "hook32.h"
#include "hacks.h"
/**/#include "imgui_memory_editor.h"

using Geom::Vector2;
using Geom::Vector3;

// Version
extern const char DEARCUBE_VERSION[];

// For cursor display
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Menu;
extern Menu* menu;
// Window WndProc hooking
LRESULT __stdcall WndProc(const HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Menu
{
private:
	// SDL definitions for cursor display
	typedef enum {
		SDL_GRAB_QUERY,
		SDL_GRAB_OFF,
		SDL_GRAB_ON
	} SDL_GrabMode;
	typedef BOOL(__cdecl* t_SDL_WM_GrabInput)(SDL_GrabMode mode);
	t_SDL_WM_GrabInput	_SDL_WM_GrabInput;

	// Variables
	HWND hwnd;
	int gameWigth = 1024, gameHeight = 768;
	bool bRunning = false;
	bool bShow = false;
	bool bDebug = false;

	// Aimbot
	bool bAimbot = false;
	bool bShowFov = false;
	float fov = 20.0f;
	float aimSmooth = 20.0f;
	float fovThickness = 2.0f;
	ImVec4 fovColors = ImVec4(0.25f, 0.0f, 1.0f, 0.45f);

public:
	Menu(HWND hwnd);
	~Menu();

	void drawMenu(GameObjects* gameObjects);
	void render(GameObjects* gameObjects);
	void init();
	void shutdown();

	void helpMarker(const char* desc);

	void toggleMenu() { this->bShow = !this->bShow; }
	bool isShown() { return this->bShow; }
	bool isRunning() { return this->bRunning; }

	bool isAimbotEnabled() { return bAimbot; }
	float getFovValue() { return fov; }
	float getAimSmoothValue() { return aimSmooth; }
};
