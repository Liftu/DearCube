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

class Menu
{
private:
	bool bRunning = false;
	bool bShow = false;
	int counter = 0;

	// SDL definitions to manage the cursor
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
	void init(HWND hwnd);
	void shutdown();
};

