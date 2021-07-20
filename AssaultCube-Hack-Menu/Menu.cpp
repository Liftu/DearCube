#include "Menu.h"

Menu::Menu(HWND hwnd)
{
	this->init(hwnd);

	this->_SDL_WM_GrabInput = (t_SDL_WM_GrabInput)Hook32::getFunctionAddr("SDL.dll", "SDL_WM_GrabInput");
	this->_SDL_ShowCursor	= (t_SDL_ShowCursor)Hook32::getFunctionAddr("SDL.dll", "SDL_WM_GrabInput");

	this->bRunning	= true;
	this->bShow		= false;//false;
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
		//_SDL_WM_GrabInput(SDL_GrabMode(2));
		//_SDL_ShowCursor(SDL_ENABLE);
		//std::cout << "Cursor : " << _SDL_ShowCursor(SDL_QUERY) << std::endl;

		ImGui::Begin("Hack menu", &this->bShow, ImGuiWindowFlags_NoSavedSettings);

		if (ImGui::Button("Click me !"))
			this->counter++;
		ImGui::Text("%i", this->counter);

		ImGui::End();
	}
	else
	{
		//_SDL_WM_GrabInput(SDL_GrabMode(1));
		//_SDL_ShowCursor(SDL_DISABLE);
		//std::cout << "Cursor : " << _SDL_ShowCursor(SDL_QUERY) << std::endl;
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

void Menu::init(HWND hwnd)
{
	if (this->bRunning)
		return;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplOpenGL2_Init();
}

void Menu::shutdown()
{
	if (!this->bRunning)
		return;

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	this->bRunning = false;
}