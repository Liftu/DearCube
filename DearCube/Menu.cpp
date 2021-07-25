#include "Menu.h"

// Window WndProc hooking
WNDPROC	original_WndProc;
LRESULT __stdcall WndProc(const HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// If the menu is hidden, then pass the event to the original window WndProc.
	if (!menu->isShown())
	{
		// Weirdly it doesn't work if I don't do "this->bShow = !this->bShow;" in the render function ?????
		//switch (wParam)
		//{
		//case VK_INSERT:
		//	menu->toggleMenu();
		//	break;
		//}
		return CallWindowProc(original_WndProc, hWnd, msg, wParam, lParam);
	}
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

void Menu::drawMenu(GameObjects* gameObjects)
{
	// Main window
	ImGui::Begin("DearCube", &this->bShow, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);

	// Tabs
	if (ImGui::BeginTabBar("MainTabBar"))
	{
		PlayerEntity* myPlayerEntityPtr = gameObjects->myPlayerEntityPtr;
		//// If local player entity is found, then we can draw the different hack tabs
		if (Hacks::isValidEntity(myPlayerEntityPtr))
		{
			// HUD
			//if (ImGui::BeginTabItem("HUD"))
			//{
			//	ImGui::Text("This is the HUD tab!");
			//	// Custom cursor

			//	ImGui::EndTabItem();
			//}

			// ESP (may put in inside HUD tab)
			//if (ImGui::BeginTabItem("ESP"))
			//{
			//	ImGui::Text("This is the ESP tab!");
			// 
			//	ImGui::EndTabItem();
			//}

			// Aimbot
			if (ImGui::BeginTabItem("Aimbot"))
			{
				//static bool aimbot = false;
				ImGui::Checkbox("Aimbot", &this->bAimbot);

				ImGui::BeginChild("ChildPlayers", ImVec2(0, 0), true);
				//ImGui::Text("%s : %.2f", closestEnemyPtr->name, myPlayerEntityPtr->vec3HeadPos.getDistance(closestEnemyPtr->vec3HeadPos));
				//ImGui::Text("%s : %d : %s", playerEntity->name, playerEntity->health, (playerEntity->state == States::Alive ? "Alive" : "Dead"));


				Vector2 myViewAngles(myPlayerEntityPtr->vec3ViewAxis.x, myPlayerEntityPtr->vec3ViewAxis.y);
				ImGui::Text("Current pos        : %.3f, %.3f, %.3f", myPlayerEntityPtr->vec3HeadPos.x, myPlayerEntityPtr->vec3HeadPos.y, myPlayerEntityPtr->vec3HeadPos.z);
				ImGui::Text("Current viewAngles : %.3f, %.3f", myViewAngles.x, myViewAngles.y);
				ImGui::Separator();

				PlayerEntity* closestEnemyPtr = Hacks::getClosestEnemyToCrosshair(gameObjects);
				if (Hacks::isValidEntity(closestEnemyPtr))
				{
					ImGui::Text("%s : ", closestEnemyPtr->name);
					ImGui::Text("Pos        : % .3f, % .3f, % .3f", closestEnemyPtr->vec3HeadPos.x, closestEnemyPtr->vec3HeadPos.y, closestEnemyPtr->vec3HeadPos.z);
					ImGui::Text("Distance   : %.3f", myPlayerEntityPtr->vec3HeadPos.getDistance(closestEnemyPtr->vec3HeadPos));
					Vector2 viewAngleToEnemy = Geom::calcAngle(myPlayerEntityPtr->vec3HeadPos, closestEnemyPtr->vec3HeadPos);
					ImGui::Text("viewAngle  : %.3f, %.3f", viewAngleToEnemy.x, viewAngleToEnemy.y);
					Vector2 viewAngleToEnemyDelta = myViewAngles - viewAngleToEnemy;
					ImGui::Text("angleDelta : %.3f, %.3f", viewAngleToEnemyDelta.x, viewAngleToEnemyDelta.y);
				}

				std::vector<PlayerEntity*> enemyList = Hacks::getEnemyList(gameObjects);
				for (PlayerEntity* enemyEntityPtr : enemyList)
				{
					Vector2 viewAngleToEnemy = Geom::calcAngle(myPlayerEntityPtr->vec3HeadPos, enemyEntityPtr->vec3HeadPos);
					ImGui::Text("%s : distance to crosshair : %f", enemyEntityPtr->name, myViewAngles.getDistance(viewAngleToEnemy));
				}

				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			// Weapons
			if (ImGui::BeginTabItem("Weapons"))
			{
				// As "All weapons" is the 0th element in the array, all the
				// weapon index are shifted of 1 compared to the WeaponTypes enum.
				static const char* weaponNames[] = { "All weapons", "Knife", "Pistol", "Carabine", "Shotgun", "Subgun", "Sniper", "Assault", "Cpistol", "Grenade", "Akimbo" };
				static int currentWeaponIndex = (int)Hacks::getCurrentWeaponType(myPlayerEntityPtr) + 1;	// first time : display current weapon, then display last selected //0;
				ImGui::Combo("##ComboWeapons", &currentWeaponIndex, weaponNames, (int)WeaponTypes::SIZE + 1);

				// Size here is totally hardcoded and found by manual dcothomy.
				ImGui::BeginChild("ChildWeapons", ImVec2(0, 105.0f), true);

				// If "All weapons"
				if (currentWeaponIndex == 0)
				{
					static int noSpread = 100;
					static int noRecoil = 100;
					static int noKickback = 100;
					static bool fullAuto = false;
					 
					// No spread
					if (ImGui::SliderInt("No spread", &noSpread, 0, 100, "%d%%"))
					{
						Hacks::setAllWeaponsHackValue(myPlayerEntityPtr, Hacks::WeaponHackTypes::NoSpread, noSpread);
					}
					ImGui::SameLine(); helpMarker("The no spread feature will only apply to the 2/3 shots.\n The following shots will keep getting a bit of spreading.");

					// No recoil
					if (ImGui::SliderInt("No recoil", &noRecoil, 0, 100, "%d%%"))
					{
						Hacks::setAllWeaponsHackValue(myPlayerEntityPtr, Hacks::WeaponHackTypes::NoRecoil, noRecoil);
					}
					//ImGui::SameLine(); helpMarker("The no recoil feature will prevent your weapon from rising up when shooting.");

					// No kickback
					if (ImGui::SliderInt("No kickback", &noKickback, 0, 100, "%d%%"))
					{
						Hacks::setAllWeaponsHackValue(myPlayerEntityPtr, Hacks::WeaponHackTypes::NoKickback, noKickback);
					}
					ImGui::SameLine(); helpMarker("The no kickback feature will prevent yourself from moving when shooting.");

					// Full auto
					if (ImGui::Checkbox("Full-auto weapons", &fullAuto))
					{
						Hacks::toggleAllWeaponsHack(myPlayerEntityPtr, Hacks::WeaponHackTypes::FullAuto, fullAuto);
					}
					ImGui::SameLine(); helpMarker("The full-auto feature only applies to semi auto weapons");
				}
				// Else : specific weapon
				else
				{
					WeaponTypes currentWeaponType = WeaponTypes(currentWeaponIndex - 1);

					// No spread
					int spread = Hacks::getWeaponHackValue(myPlayerEntityPtr, currentWeaponType, Hacks::WeaponHackTypes::NoSpread);
					if (ImGui::SliderInt("No spread", &spread, 0, Hacks::c_defaultWeaponsSpread[(int)currentWeaponType]))
					{
						//ImGui::Text("%d", spread);
						Hacks::setWeaponHackValue(myPlayerEntityPtr, currentWeaponType, Hacks::WeaponHackTypes::NoSpread, spread);
					}
					ImGui::SameLine(); helpMarker("The no spread feature will only apply to the 2/3 shots.\n The following shots will keep getting a bit of spreading.");

					// No recoil
					int recoil = Hacks::getWeaponHackValue(myPlayerEntityPtr, currentWeaponType, Hacks::WeaponHackTypes::NoRecoil);
					if (ImGui::SliderInt("No recoil", &recoil, 0, Hacks::c_defaultWeaponsRecoil[(int)currentWeaponType]))
					{
						Hacks::setWeaponHackValue(myPlayerEntityPtr, currentWeaponType, Hacks::WeaponHackTypes::NoRecoil, recoil);
					}
					//ImGui::SameLine(); helpMarker("The no recoil feature will prevent your weapon from rising up when shooting.");

					// No kickback
					int kickback = Hacks::getWeaponHackValue(myPlayerEntityPtr, currentWeaponType, Hacks::WeaponHackTypes::NoKickback);
					if (ImGui::SliderInt("No kickback", &kickback, 0, Hacks::c_defaultWeaponsKickback[(int)currentWeaponType]))
					{
						Hacks::setWeaponHackValue(myPlayerEntityPtr, currentWeaponType, Hacks::WeaponHackTypes::NoKickback, kickback);
					}
					ImGui::SameLine(); helpMarker("The no kickback feature will prevent yourself from moving when shooting.");

					// Full auto
					// TODO : We may want to disable this feature for already full-auto weapons when the ImGui feature will be out (https://github.com/ocornut/imgui/issues/211)
					bool fullAuto = (bool)Hacks::getWeaponHackValue(myPlayerEntityPtr, currentWeaponType, Hacks::WeaponHackTypes::FullAuto);
					if (ImGui::Checkbox("Full-auto", &fullAuto))
					{
						Hacks::toggleWeaponHack(myPlayerEntityPtr, currentWeaponType, Hacks::WeaponHackTypes::FullAuto, fullAuto);
					}
					ImGui::SameLine(); helpMarker("The full-auto feature only applies to semi auto weapons");
				}

				ImGui::EndChild();

				// No self kickback
				static bool noSelfKickback = false;
				if (ImGui::Checkbox("No self kickback", &noSelfKickback))
				{
					Hacks::toggleAllWeaponsHack(myPlayerEntityPtr, Hacks::WeaponHackTypes::NoSelfKickback, noSelfKickback);
				}
				ImGui::SameLine(); helpMarker("The no self kickback feature will prevent yourself from moving when getting shot.");

				ImGui::EndTabItem();
			}

			// Debug
			if (this->bDebug)
			{
				static MemoryEditor memEdit;
				static bool show = false;
				static PlayerEntity* selectedPlayerEntityPtr = myPlayerEntityPtr;	// Default is myPlayerEntityPtr
				if (ImGui::BeginTabItem("Debug"))
				{
					ImGui::Text("Player selected : %s", Hacks::isValidEntity(selectedPlayerEntityPtr) ? selectedPlayerEntityPtr->name : "none");
					ImGui::Checkbox("Show PlayerEntity memory editor", &show);

					if (ImGui::Button(myPlayerEntityPtr->name))
						selectedPlayerEntityPtr = myPlayerEntityPtr;

					ImGui::BeginChild("ChildDebugPlayers", ImVec2(0, 0), true);
					std::vector<PlayerEntity*> entityList = Hacks::getValidEntityList(&gameObjects->playerEntityVector);
					for (PlayerEntity* enemyEntityPtr : entityList)
						if (ImGui::Button(enemyEntityPtr->name))
							selectedPlayerEntityPtr = enemyEntityPtr;
					ImGui::EndChild();

					ImGui::EndTabItem();
				}
				if (show && Hacks::isValidEntity(selectedPlayerEntityPtr))
					memEdit.DrawWindow(selectedPlayerEntityPtr->name, selectedPlayerEntityPtr, sizeof(PlayerEntity));
			}
		}
		// Else : local player entity no found
		else
		{
			if (ImGui::BeginTabItem("Warning"))
			{
				ImGui::TextWrapped("The local player entity coud not be found.");
				ImGui::TextWrapped("Therefore, the hacks cannot be run.");

				ImGui::EndTabItem();
			}
		}

		//// Presets
		//if (ImGui::BeginTabItem("Presets"))
		//{
		//	ImGui::Text("Maybe if I manage to do all the remaining features fisrt.");

		//	ImGui::EndTabItem();
		//}

		// About
		if (ImGui::BeginTabItem("About"))
		{
			ImGui::TextWrapped("DearCube %s", DEARCUBE_VERSION);
			ImGui::Separator();

			ImGui::TextWrapped("By Liftu and a lot of help from all the Guided Hacking community.");
			ImGui::TextWrapped("Special thanks to Rake and all the effort he has put into it.");
			ImGui::TextWrapped("DearCube is licensed under the MIT License, see LICENSE for more information.");
			ImGui::Separator();

			ImGui::TextWrapped("DearCube uses Dear ImGui %s to draw this menu.", ImGui::GetVersion());
			ImGui::Separator();

			ImGui::Checkbox("Enable debug mode", &this->bDebug);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Menu::render(GameObjects* gameObjects)
{
	if (!this->bRunning)
		return;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (GetAsyncKeyState(VK_INSERT) & 1)
		this->bShow = !this->bShow;

	if (this->bShow)
	{
		// Intercept cursor
		_SDL_WM_GrabInput(SDL_GrabMode(2));
		// Thought it would be needed to show the cursor, but it juste made the game laggy.
		//_SDL_ShowCursor(SDL_ENABLE);

		// Actual menu code
		//this->drawMenu(gameObjects);
	}
	else
	{
		// Restore cursor
		_SDL_WM_GrabInput(SDL_GrabMode(1));
		//_SDL_ShowCursor(SDL_DISABLE);
	}
	this->drawMenu(gameObjects);

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
	ImGuiStyle &style = ImGui::GetStyle();
	style.WindowMinSize = ImVec2(400, 250);
	style.WindowRounding = 3.0f;
	style.ChildRounding = 3.0f;
	style.FrameRounding = 3.0f;
	style.GrabRounding = 3.0f;

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

// Helper to display a little (?) mark which shows a tooltip when hovered.
// This is directly copied from ImGui's imgui_demo.cpp
void Menu::helpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}