#pragma once

#include <Windows.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl2.h"
#include "detours.h"
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
	enum class DrawingTools : int
	{
		DRAWING_TOOL_IMGUI = 0,
		DRAWING_TOOL_OPENGL
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
	bool bFovCircle = false;
	float fov = 10.0f;
	float aimSmooth = 15.0f;

	// ESP
	bool bEspBox = false;
	bool bEspName = false;
	bool bEspHealthBar = false;
	bool bEspShieldBar = false;
	bool bEspHead = false;
	bool bWallhack = false;

	// Misc
	bool bCrosshair = false;
	bool bHideDefaultCrosshair = false;
	float crosshairSize = 10.0f;
	float crosshairThickness = 1.0f;
	ImVec4 crosshairColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

	// Settings
	DrawingTools currentDrawingTool = DrawingTools::DRAWING_TOOL_IMGUI;
	float espBoxThickness = 1.0f;
	ImVec4 espBoxColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	float espHeadThickness = 1.0f;
	ImVec4 espHeadColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	float fovThickness = 1.0f;
	ImVec4 fovColor = ImVec4(0.25f, 0.0f, 1.0f, 1.0f);


	void drawMenu(GameObjects* gameObjects);
	void drawHud(Vector2 screenDimensions, GameObjects* gameObjects);
	void init();
	void helpMarker(const char* desc);

public:
	Menu(HWND hwnd);
	~Menu();

	void render(Vector2 screenDimensions, GameObjects* gameObjects);
	void shutdown();

	void toggleMenu() { this->bShow = !this->bShow; }
	bool isShown() { return this->bShow; }
	bool isRunning() { return this->bRunning; }

	// Aimbot
	bool isAimbotEnabled() { return this->bAimbot; }
	bool isFovCircleEnabled() { return this->bFovCircle; }
	bool isTriggerbotEnabled() { return this->bTriggerbot; }
	float getFovValue() { return this->fov; }
	float getAimSmoothValue() { return this->aimSmooth; }


	// ESP
	bool isEspBoxEnabled() { return this->bEspBox; }
	bool isEspNameEnabled() { return this->bEspName; }
	bool isEspHealthBarEnabled() { return this->bEspHealthBar; }
	bool isEspShieldBarEnabled() { return this->bEspShieldBar; }
	bool isEspHeadEnabled() { return this->bEspHead; }
	bool isWallhackEnabled() { return this->bWallhack; }

	// Misc
	bool isCustomCrosshairEnabled() { return this->bCrosshair; }
	bool isHideDefaultCrosshairEnabled() { return this->bHideDefaultCrosshair; }
	float getCustomCrosshairSize() { return this->crosshairSize; }
	float getCustomCrosshairThickness() { return this->crosshairThickness; }
	Vector4 getCustomCrosshairColor() { return Vector4(this->crosshairColor.x, this->crosshairColor.y, this->crosshairColor.z, this->crosshairColor.w); }


	// Settings
	DrawingTools getCurrentDrawingTool() { return this->currentDrawingTool; }
	float getEspBoxThickness() { return this->espBoxThickness; }
	Vector4 getEspBoxColor() { return Vector4(this->espBoxColor.x, this->espBoxColor.y, this->espBoxColor.z, this->espBoxColor.w); }
	float getEspHeadThickness() { return this->espHeadThickness; }
	Vector4 getEspHeadColor() { return Vector4(this->espHeadColor.x, this->espHeadColor.y, this->espHeadColor.z, this->espHeadColor.w); }
	float getFovThickness() { return this->fovThickness; }
	Vector4 getFovColor() { return Vector4(this->fovColor.x, this->fovColor.y, this->fovColor.z, this->fovColor.w); }
};
