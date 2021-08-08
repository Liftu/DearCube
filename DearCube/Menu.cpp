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
	this->_SDL_WM_GrabInput = (t_SDL_WM_GrabInput)DetourFindFunction("SDL.dll", "SDL_WM_GrabInput");

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
		// If local player entity is found, then we can draw the different hack tabs
		if (Hacks::isValidEntity(myPlayerEntityPtr))
		{
			// ESP/Wallhack
			if (ImGui::BeginTabItem("ESP"))
			{
				// ESP box
				ImGui::Checkbox("ESP box", &this->bEspBox);
				ImGui::SameLine(); helpMarker("Display a box around enemies to detect them through walls.");
				if (this->bEspBox)
				{
					ImGui::BeginGroup();
					ImGui::Indent(ImGui::GetStyle().IndentSpacing);

					// ESP box health and shield bar
					ImGui::Checkbox("Display health bar", &this->bEspHealthBar);
					ImGui::Checkbox("Display shield bar", &this->bEspShieldBar);
					ImGui::Checkbox("Display name", &this->bEspName);

					ImGui::EndGroup();
				}

				// ESP head
				ImGui::Checkbox("ESP head circle", &this->bEspHead);
				ImGui::SameLine(); helpMarker("Display a circle around enemies head to detect them through walls.");

				// Wallhack
				if (ImGui::Checkbox("Wallhack (experimental)", &this->bWallhack))
				{
					Hacks::wallhack(this->bWallhack);
				}
				ImGui::SameLine(); helpMarker("Draw player models through walls.");

				ImGui::EndTabItem();
			}

			// Aimbot
			if (ImGui::BeginTabItem("Aimbot"))
			{
				ImGui::Checkbox("Aimbot", &this->bAimbot);

				// Smoothness
				ImGui::SliderFloat("Smoothness", &this->aimSmooth, 1.0f, 100.0f, "%.2f");
				ImGui::SameLine(); helpMarker("The aim smoothness represents the spead at which the aimbot will aim at the target.");

				// FOV
				ImGui::SliderFloat("FOV size", &this->fov, 0.0f, 200.0f, "%.2f");
				ImGui::SameLine(); helpMarker("The FOV represents the maximum distance in degrees from your current view to the enemy at which the aimbot will start aiming.");
				// FOV circle
				ImGui::Checkbox("Show FOV circle", &this->bFovCircle);

				// Triggerbot
				ImGui::Checkbox("Triggerbot", &this->bTriggerbot);

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

				// Depends on weapon choose
				ImGui::BeginGroup();
				ImGui::Indent(ImGui::GetStyle().IndentSpacing / 2.0f);

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

				ImGui::EndGroup();

				// No self kickback
				static bool noSelfKickback = false;
				if (ImGui::Checkbox("No self kickback", &noSelfKickback))
				{
					Hacks::toggleAllWeaponsHack(myPlayerEntityPtr, Hacks::WeaponHackTypes::NoSelfKickback, noSelfKickback);
				}
				ImGui::SameLine(); helpMarker("The no self kickback feature will prevent yourself from moving when getting shot.");

				ImGui::EndTabItem();
			}

			// Misc
			if (ImGui::BeginTabItem("Misc"))
			{
				// Custom crosshair
				ImGui::Checkbox("Custom crosshair", &this->bCrosshair);
				if (this->bCrosshair)
				{
					ImGui::BeginGroup();
					ImGui::Indent(ImGui::GetStyle().IndentSpacing);

					if (ImGui::Checkbox("Hide default crosshair", &this->bHideDefaultCrosshair))
					{
						Hacks::hideDefaultCrosshair(this->bHideDefaultCrosshair);
					}

					ImGui::SliderFloat("Crosshair size", &this->crosshairSize, 1.0f, 50.0f);
					ImGui::SliderFloat("Crosshair thickness", &this->crosshairThickness, 1.0f, 10.0f);
					ImGui::ColorEdit4("Crosshair color", &this->crosshairColor.x);

					ImGui::EndGroup();
				}


				ImGui::EndTabItem();
			}

			// Settings
			if (ImGui::BeginTabItem("Settings"))
			{
				static const char* espToolNames[] = { "ImGui", "OpenGL" };
				ImGui::Combo("Drawing tool", (int*)&this->currentDrawingTool, espToolNames, 2);
				ImGui::SameLine(); helpMarker("This is the library that will be used to draw on the screen.\nChoose the one that works best for you or just your favorite.");

				if (ImGui::CollapsingHeader("Colors/thickness"))
				{
					if (ImGui::TreeNode("ESP box"))
					{
						ImGui::SliderFloat("Thickness", &this->espBoxThickness, 1.0, 5.0, "%.2f px");
						ImGui::ColorEdit4("Color", &this->espBoxColor.x);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("ESP head"))
					{
						ImGui::SliderFloat("Thickness", &this->espHeadThickness, 1.0, 5.0, "%.2f px");
						ImGui::ColorEdit4("Color", &this->espHeadColor.x);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("FOV circle"))
					{
						ImGui::SliderFloat("Thickness", &this->fovThickness, 1.0, 5.0, "%.2f px");
						ImGui::ColorEdit4("Color", &this->fovColor.x);
						ImGui::TreePop();
					}
				}

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

void Menu::drawHud(Vector2 screenDimensions, GameObjects* gameObjects)
{
	if (this->currentDrawingTool == DrawingTools::DRAWING_TOOL_IMGUI)
	{
		// Draw the fov circle
		if (this->bFovCircle)
		{
			ImGui::Begin("##FOVCIRCLE", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
			ImDrawList* drawList = ImGui::GetBackgroundDrawList();
			// This calculation is approximate
			float fovCircleRadius = this->fov / 50 * (screenDimensions.x / 2.0f);	// 50 approximately represents the angle degree to the side of the screen
			drawList->AddCircle(ImVec2(screenDimensions.x / 2.0f, screenDimensions.y / 2.0f), fovCircleRadius, ImColor(this->fovColor), 0, this->fovThickness);

			ImGui::End();
		}


		// Draw the ESPs
		ImGui::Begin("##ESPDRAWS", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		for (PlayerEntity* enemyPtr : Hacks::getAliveEnemyList(gameObjects))
		{
			// Draw body box
			if (this->bEspBox)
			{
				Vector2 enemyLowerBoxPos;
				Vector2 enemyUpperBoxPos;
				bool LowerVisibility = Hacks::worldToScreen(Hacks::getEnemyLowerBoxPos(enemyPtr), screenDimensions, enemyLowerBoxPos);
				bool upperVisility = Hacks::worldToScreen(Hacks::getEnemyUpperBoxPos(enemyPtr), screenDimensions, enemyUpperBoxPos);
				if (LowerVisibility || upperVisility)
				{
					float distance = gameObjects->myPlayerEntityPtr->headPos.getDistance(enemyPtr->headPos);
					// Calculate enemy width on screen
					float enemyScreenWidth = 1200.0f / distance;	// arbitrary value

					ImVec2 boxUpperLeftCoords = ImVec2((enemyUpperBoxPos.x + enemyLowerBoxPos.x) / 2.0f - (enemyScreenWidth / 2.0f), enemyUpperBoxPos.y);
					ImVec2 boxLowerRightCoords = ImVec2((enemyUpperBoxPos.x + enemyLowerBoxPos.x) / 2.0f + (enemyScreenWidth / 2.0f), enemyLowerBoxPos.y);

					// Draw box
					drawList->AddRect(boxUpperLeftCoords, boxLowerRightCoords, ImColor(this->espBoxColor), 0.0f, 0, this->espBoxThickness);

					// Draw name
					if (this->bEspName)
					{
						ImVec2 textSize = ImGui::CalcTextSize(enemyPtr->name);
						ImVec2 textCoords = ImVec2((boxUpperLeftCoords.x + boxLowerRightCoords.x) / 2.0f - (textSize.x / 2.0f), boxLowerRightCoords.y + textSize.y / 2.0f);
						drawList->AddText(textCoords, ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), enemyPtr->name);
					}

					if (this->bEspHealthBar)
					{
						// Draw the backgound red bar
						ImVec2 healthBarUpperLeftCoords = ImVec2(boxLowerRightCoords.x + 3.0f, boxUpperLeftCoords.y + 1);
						ImVec2 healthBarLowerRightCoords = ImVec2(boxLowerRightCoords.x + 6.0f, boxLowerRightCoords.y - 1);
						drawList->AddRectFilled(healthBarUpperLeftCoords, healthBarLowerRightCoords, ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
						// Draw the frontground green bar (proportional to health value)
						float barSize = healthBarLowerRightCoords.y - healthBarUpperLeftCoords.y;
						healthBarUpperLeftCoords.y += barSize - barSize * (enemyPtr->health / 100.0f);
						drawList->AddRectFilled(healthBarUpperLeftCoords, healthBarLowerRightCoords, ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)));
					}

					if (this->bEspShieldBar)
					{
						// Do not display backgorund for the shield bar
						ImVec2 shieldhBarUpperLeftCoords = ImVec2(boxLowerRightCoords.x + 3.0f, boxUpperLeftCoords.y + 1);
						ImVec2 shieldBarLowerRightCoords = ImVec2(boxLowerRightCoords.x + 6.0f, boxLowerRightCoords.y - 1);
						// Do not overlap health bar if displayed
						if (this->bEspHealthBar)
						{
							shieldhBarUpperLeftCoords.x += 6.0f;
							shieldBarLowerRightCoords.x += 6.0f;
						}
						// Draw the blue bar (proportional to shield value)
						float barSize = shieldBarLowerRightCoords.y - shieldhBarUpperLeftCoords.y;
						shieldhBarUpperLeftCoords.y += barSize - barSize * (enemyPtr->shield / 100.0f);
						drawList->AddRectFilled(shieldhBarUpperLeftCoords, shieldBarLowerRightCoords, ImColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f)));
					}
				}
			}
			// Draw head circle
			if (this->bEspHead)
			{
				Vector2 enemyHeadScreenPos;
				if (Hacks::worldToScreen(Hacks::getEnemyHeadPos(enemyPtr), screenDimensions, enemyHeadScreenPos))
				{
					// Calculate head circle radius
					float distance = gameObjects->myPlayerEntityPtr->headPos.getDistance(enemyPtr->headPos);
					float headCircleRadius = 200 / distance;	// arbitrary value
					drawList->AddCircle(ImVec2(enemyHeadScreenPos.x, enemyHeadScreenPos.y), headCircleRadius, ImColor(this->espHeadColor), 0, this->espHeadThickness);
				}
			}
		}

		ImGui::End();


		// Draw the custom crosshair
		if (this->bCrosshair)
		{
			ImGui::Begin("##CUSTOMCROSSHAIR", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
			ImDrawList* drawList = ImGui::GetBackgroundDrawList();

			ImVec2 verticalCoords = ImVec2(screenDimensions.x / 2, screenDimensions.y / 2 - this->crosshairSize / 2);
			drawList->AddLine(verticalCoords, ImVec2(verticalCoords.x, verticalCoords.y + this->crosshairSize), ImColor(this->crosshairColor), this->crosshairThickness);

			ImVec2 horizontalCoords = ImVec2(screenDimensions.x / 2 - this->crosshairSize / 2, screenDimensions.y / 2);
			drawList->AddLine(horizontalCoords, ImVec2(horizontalCoords.x + this->crosshairSize, horizontalCoords.y), ImColor(this->crosshairColor), this->crosshairThickness);

			ImGui::End();
		}
	}
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

	this->drawHud(screenDimensions, gameObjects);


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
	style.WindowMinSize = ImVec2(400, 275);
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