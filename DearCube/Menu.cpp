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
			if (ImGui::BeginTabItem("ESP"))
			{
				// ESP box
				ImGui::Checkbox("ESP box", &this->bESPBox);
				if (this->bESPBox)
				{
					ImGui::SliderFloat("ESP box thickness", &this->espBoxThickness, 1.0, 5.0, "%.2f px");
					ImGui::ColorEdit4("ESP box color", &this->espBoxColor.x);
				}

				// ESP head
				ImGui::Checkbox("ESP head circle", &this->bESPHead);
				if (this->bESPHead)
				{
					ImGui::SliderFloat("ESP head thickness", &this->espHeadThickness, 1.0, 5.0, "%.2f px");
					ImGui::ColorEdit4("ESP head color", &this->espHeadColor.x);
				}

				ImGui::EndTabItem();
			}

			// Aimbot
			if (ImGui::BeginTabItem("Aimbot"))
			{
				ImGui::Checkbox("Aimbot", &this->bAimbot);
				
				// FOV
				ImGui::SliderFloat("FOV size", &this->fov, 0.0f, 200.0f, "%.2f");
				ImGui::SameLine(); helpMarker("The FOV represents the maximum distance in degrees from your current view to the enemy at which the aimbot will start aiming.");
				// FOV circle
				ImGui::Checkbox("Show FOV circle", &this->bShowFov);
				if (this->bShowFov)
				{
					ImGui::SliderFloat("FOV circle thickness", &this->fovThickness, 1.0, 5.0, "%.2f px");
					ImGui::ColorEdit4("FOV circle color", &this->fovColor.x);
				}

				// Smoothness
				ImGui::SliderFloat("Smoothness", &this->aimSmooth, 1.0f, 100.0f, "%.2f°");
				ImGui::SameLine(); helpMarker("The aim smoothness represents the spead at which the aimbot will aim at the target.");

				// Triggerbot
				ImGui::Checkbox("Triggerbot", &this->bTriggerbot);
				ImGui::SliderFloat("Trigger distance", &this->triggerDistance, 0.1f, 10.0f, "%.2f°");
				ImGui::SameLine(); helpMarker("The triggerbot distance represents the distance in degrees from the target at which the triggerbot will start shooting.");

				//// Debugging
				//ImGui::Separator();
				//ImGui::Separator();
				//ImGui::BeginChild("ChildPlayers", ImVec2(0, 0), true);
				////ImGui::Text("%s : %.2f", closestEnemyPtr->name, myPlayerEntityPtr->headPos.getDistance(closestEnemyPtr->headPos));
				////ImGui::Text("%s : %d : %s", playerEntity->name, playerEntity->health, (playerEntity->state == States::Alive ? "Alive" : "Dead"));

				//Vector2 myViewAngles(myPlayerEntityPtr->viewAngles.x, myPlayerEntityPtr->viewAngles.y);
				//ImGui::Text("Current pos        : %.3f, %.3f, %.3f", myPlayerEntityPtr->headPos.x, myPlayerEntityPtr->headPos.y, myPlayerEntityPtr->headPos.z);
				//ImGui::Text("Current viewAngles : %.3f, %.3f", myViewAngles.x, myViewAngles.y);
				//ImGui::Separator();

				//PlayerEntity* closestEnemyPtr = Hacks::getClosestEnemyToCrosshair(gameObjects, this->fov);
				//if (Hacks::isValidEntity(closestEnemyPtr))
				//{
				//	ImGui::Text("%s : ", closestEnemyPtr->name);
				//	ImGui::Text("Pos        : % .3f, % .3f, % .3f", closestEnemyPtr->headPos.x, closestEnemyPtr->headPos.y, closestEnemyPtr->headPos.z);
				//	ImGui::Text("Distance   : %.3f", myPlayerEntityPtr->headPos.getDistance(closestEnemyPtr->headPos));
				//	Vector2 viewAnglesToEnemy = Geom::calcAngle(myPlayerEntityPtr->headPos, closestEnemyPtr->headPos);
				//	ImGui::Text("viewAngle  : %.3f, %.3f", viewAnglesToEnemy.x, viewAnglesToEnemy.y);
				//	Vector2 viewAnglesToEnemyDelta = viewAnglesToEnemy - myViewAngles;
				//	ImGui::Text("angleDelta : %.3f, %.3f", viewAnglesToEnemyDelta.x, viewAnglesToEnemyDelta.y);
				//}

				//std::vector<PlayerEntity*> enemyList = Hacks::getEnemyList(gameObjects);
				//for (PlayerEntity* enemyEntityPtr : enemyList)
				//{
				//	Vector2 viewAnglesToEnemy = Geom::calcAngle(myPlayerEntityPtr->headPos, enemyEntityPtr->headPos);
				//	ImGui::Text("%s : distance to crosshair : %f", enemyEntityPtr->name, myViewAngles.getDistance(viewAnglesToEnemy));
				//}

				//ImGui::EndChild();

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
				// Show the memory editor for the selected player entity
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
		//
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

void Menu::render(Vector2 screenDimensions, GameObjects* gameObjects)
{
	if (!this->bRunning)
		return;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	// Hide/show the menu with INSERT
	if (GetAsyncKeyState(VK_INSERT) & 1)
		this->bShow = !this->bShow;


	if (this->bShow)
	{
		// Intercept cursor
		_SDL_WM_GrabInput(SDL_GrabMode(2));

		// Actual menu code
		this->drawMenu(gameObjects);
	}
	else
	{
		// Restore cursor
		_SDL_WM_GrabInput(SDL_GrabMode(1));
	}
	//this->drawMenu(gameObjects);

	// Draw the fov circle
	if (this->bShowFov)
	{
		ImGui::Begin("##FOVCIRCLE", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | 
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		// This calculation is approximate
		float fovCircleRadius = this->fov / 50 * (screenDimensions.x / 2.0f);	// 50 approximately represents the angle degree to the side of the screen
		drawList->AddCircle(ImVec2(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f), fovCircleRadius, ImColor(this->fovColor), 0, this->fovThickness);
		ImGui::End();
	}

	// Draw ESP
	ImGui::Begin("##ESPDRAWS", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	for (PlayerEntity* enemyPtr : Hacks::getAliveEnemyList(gameObjects))
	{
		// Draw body box
		if (this->bESPBox)
		{
			Vector2 enemyLowerBoxPos;
			Vector2 enemyUpperBoxPos;
			bool LowerVisibility = Hacks::worldToScreen(Hacks::getEnemyLowerBoxPos(enemyPtr), Vector2(1024, 768), enemyLowerBoxPos);
			bool upperVisility = Hacks::worldToScreen(Hacks::getEnemyUpperBoxPos(enemyPtr), Vector2(1024, 768), enemyUpperBoxPos);
			if (LowerVisibility || upperVisility)
			{
				float distance = gameObjects->myPlayerEntityPtr->headPos.getDistance(enemyPtr->headPos);
				// Calculate enemy width on screen
				float enemyScreenWidth = 1200.0f / distance;	// arbitrary value

				ImVec2 boxUpperLeftCoords = ImVec2((enemyUpperBoxPos.x + enemyLowerBoxPos.x) / 2 - (enemyScreenWidth / 2), enemyUpperBoxPos.y);
				ImVec2 boxLowerRightCoords = ImVec2((enemyUpperBoxPos.x + enemyLowerBoxPos.x) / 2 + (enemyScreenWidth / 2), enemyLowerBoxPos.y);

				// Draw box
				drawList->AddRect(boxUpperLeftCoords, boxLowerRightCoords, ImColor(this->espBoxColor), 0.0f, 0, this->espBoxThickness);

			}
		}
		// Draw head circle
		if (this->bESPHead)
		{
			Vector2 enemyHeadScreenPos;
			if (Hacks::worldToScreen(Hacks::getEnemyHeadPos(enemyPtr), Vector2(1024, 768), enemyHeadScreenPos))
			{
				// Calculate head circle radius
				float distance = gameObjects->myPlayerEntityPtr->headPos.getDistance(enemyPtr->headPos);
				float headCircleRadius = 200 / distance;
				drawList->AddCircle(ImVec2(enemyHeadScreenPos.x, enemyHeadScreenPos.y), headCircleRadius, ImColor(this->espHeadColor), 0, this->espHeadThickness);
			}
		}
	}
	ImGui::End();


	// ImGui rendering
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