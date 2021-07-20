#pragma once

#include <Windows.h>
/**/#include <iostream>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl2.h"
#include "hook32.h"

#define SDL_QUERY   -1
#define SDL_IGNORE   0
#define SDL_DISABLE  0
#define SDL_ENABLE   1

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Window WndProc hooking
LRESULT __stdcall WndProc(const HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Menu;

extern Menu* menu;

class Menu
{
private:
	HWND hwnd;
	bool bRunning = false;
	bool bShow = false;
	int counter = 0;

	// SDL definitions for cursor display
	typedef enum {
		SDL_GRAB_QUERY,
		SDL_GRAB_OFF,
		SDL_GRAB_ON
	} SDL_GrabMode;
	typedef BOOL(__cdecl* t_SDL_WM_GrabInput)(SDL_GrabMode mode);
	typedef BOOL(__cdecl* t_SDL_ShowCursor)(int toggle);
	t_SDL_WM_GrabInput	_SDL_WM_GrabInput;
	t_SDL_ShowCursor	_SDL_ShowCursor;

public:
	Menu(HWND hwnd);
	~Menu();

	void drawMenu();
	void render();
	void init();
	void shutdown();

	bool isShown() { return this->bShow; }
	bool isRunning() { return this->bRunning; }
};

