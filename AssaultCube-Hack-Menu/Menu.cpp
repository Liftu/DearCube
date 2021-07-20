#include "Menu.h"

// Window WndProc hooking
WNDPROC	original_WndProc;
LRESULT __stdcall WndProc(const HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// If the menu is hidden, then pass the event to the original window WndProc.
	if (!menu->isShown())
		return CallWindowProc(original_WndProc, hWnd, msg, wParam, lParam);
	// If the menu is displayed on screen, interpret cursor event.
	else
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return true;
		//if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		//	return true;

		// This part doen't seem necessary because ImGui_ImplWin32_WndProcHandler do it anyway.
		//ImGuiIO& io = ImGui::GetIO();

		//switch (wParam)
		//{
		//case WM_LBUTTONDOWN:
		//	io.MouseDown[ImGuiMouseButton_Left] = true;
		//	return 0;	// return so we don't pass the cursor event to the game.
		//case WM_LBUTTONUP:
		//	io.MouseDown[ImGuiMouseButton_Left] = false;
		//	io.MouseReleased[ImGuiMouseButton_Left] = true;
		//	return 0;
		//case WM_RBUTTONDOWN:
		//	io.MouseDown[ImGuiMouseButton_Right] = true;
		//	return 0;
		//case WM_RBUTTONUP:
		//	io.MouseDown[ImGuiMouseButton_Right] = false;
		//	io.MouseReleased[ImGuiMouseButton_Right] = true;
		//	return 0;
		//	//case WM_MOUSEMOVE:
		//	//	io.MousePos.x = (int16_t)(lParam);
		//	//	io.MousePos.y = (int16_t)(lParam >> 16);
		//	//	return 0;
		//}
	}
	//return CallWindowProc(original_WndProc, hWnd, msg, wParam, lParam);
}

Menu::Menu(HWND hwnd)
{
	this->hwnd = hwnd;

	// Hook the window procedure to get the cursor inputs
	//original_WndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
	original_WndProc = (WNDPROC)SetWindowLongPtr(this->hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

	// Init ImGui
	this->init();

	// Get some SDL functions
	this->_SDL_WM_GrabInput = (t_SDL_WM_GrabInput)Hook32::getFunctionAddr("SDL.dll", "SDL_WM_GrabInput");
	this->_SDL_ShowCursor	= (t_SDL_ShowCursor)Hook32::getFunctionAddr("SDL.dll", "SDL_WM_GrabInput");

	// Set default values
	this->bRunning	= true;
	this->bShow		= true;//false;
}

Menu::~Menu()
{
	if (bRunning)
	{
		this->shutdown();
	}
}

void Menu::drawMenu()
{
	if (this->bShow)
	{
		// Intercept cursor
		_SDL_WM_GrabInput(SDL_GrabMode(2));
		// Thought it would be needed to show the cursor, but it juste made the game laggy.
		//_SDL_ShowCursor(SDL_ENABLE);

		ImGui::Begin("Hack menu", &this->bShow, ImGuiWindowFlags_NoSavedSettings);

		if (ImGui::Button("Click me !"))
			this->counter++;
		ImGui::Text("%i", this->counter);

		ImGui::End();
	}
	else
	{
		// Restore cursor
		_SDL_WM_GrabInput(SDL_GrabMode(1));
		//_SDL_ShowCursor(SDL_DISABLE);
	}
}

void Menu::render()
{
	if (!this->bRunning)
		return;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Actual menu code
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		this->bShow = !this->bShow;
	}

	this->drawMenu();

	// Rendering
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void Menu::init()
{
	if (this->bRunning)
		return;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.MouseDrawCursor = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(this->hwnd);
	ImGui_ImplOpenGL2_Init();
}

void Menu::shutdown()
{
	if (!this->bRunning)
		return;

	(WNDPROC)SetWindowLongPtr(this->hwnd, GWLP_WNDPROC, (LONG_PTR)original_WndProc);

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	this->bRunning = false;
}