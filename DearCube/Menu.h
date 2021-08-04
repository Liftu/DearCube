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
public:
	enum class ESPTools : int
	{
		ESP_TOOL_IMGUI = 0,
		ESP_TOOL_OPENGL
	};

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
	bool bRunning = false;
	bool bShow = false;
	bool bDebug = false;

	// Aimbot
	bool bAimbot = false;
	bool bTriggerbot = false;
	bool bShowFov = false;
	float fov = 10.0f;
	float aimSmooth = 15.0f;
	float triggerDistance = 0.5f;
	float fovThickness = 2.0f;
	ImVec4 fovColor = ImVec4(0.25f, 0.0f, 1.0f, 0.45f);

	// ESP
	ESPTools currentESPTool = ESPTools::ESP_TOOL_IMGUI;

	bool bESPBox = false;
	float espBoxThickness = 1.0f;
	ImVec4 espBoxColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

	bool bESPName = false;
	bool bESPHealthBar = false;
	bool bESPShieldBar = false;

	bool bESPHead = false;
	float espHeadThickness = 1.0f;
	ImVec4 espHeadColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

public:
	Menu(HWND hwnd);
	~Menu();

	void drawMenu(GameObjects* gameObjects);
	void render(Vector2 screenDimensions, GameObjects* gameObjects);
	void init();
	void shutdown();

	void helpMarker(const char* desc);

	void toggleMenu() { this->bShow = !this->bShow; }
	bool isShown() { return this->bShow; }
	bool isRunning() { return this->bRunning; }

	// Aimbot
	bool isAimbotEnabled() { return this->bAimbot; }
	bool isTriggerbotEnabled() { return this->bTriggerbot; }
	float getFovValue() { return this->fov; }
	float getAimSmoothValue() { return this->aimSmooth; }
	float getTriggerDistanceValue() { return this->triggerDistance; }


	// ESP
	ESPTools getCurrentESPTool() { return this->currentESPTool; }

	bool isESPBoxEnabled() { return this->bESPBox; }
	float getESPBoxThickness() { return this->espBoxThickness; }
	Vector4 getESPBoxColor() { return Vector4(this->espBoxColor.x, this->espBoxColor.y, this->espBoxColor.z, this->espBoxColor.w); }
	bool isESPNameEnabled() { return this->bESPName; }
	bool isESPHealthBarEnabled() { return this->bESPHealthBar; }
	bool isESPShieldBarEnabled() { return this->bESPShieldBar; }

	bool isESPHeadEnabled() { return this->bESPHead; }
	float getESPHeadThickness() { return this->espHeadThickness; }
	Vector4 getESPHeadColor() { return Vector4(this->espHeadColor.x, this->espHeadColor.y, this->espHeadColor.z, this->espHeadColor.w); }
};
