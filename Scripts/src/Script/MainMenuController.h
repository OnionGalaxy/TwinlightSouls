#ifndef  __MAINMENUCONTROLLER_H__
#define  __MAINMENUCONTROLLER_H__

#include "Script.h"

class ComponentAudioSource;
class ComponentImage;

class BootScreensController;
class InputManager;
class MainMenuButton;
class MainMenuSubMenuController;
class WorldManager;

class MainMenuController : public Script
{
public:
	enum class MainMenuState
	{
		BOOT_SCREEN,
		SHOWING_TITLE_SCREEN,
		PRESS_START,
		SHOWING_MAIN_MENU,
		MAIN_MENU,
		SHOWING_SUB_MENU,
		SUB_MENU,
		HIDING_SUB_MENU
	};

	MainMenuController();
	~MainMenuController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

private:
	void ShowMainMenu();
	void HideMainMenu();

private:
	InputManager* input_manager = nullptr;
	WorldManager* world_manager = nullptr;

	MainMenuState current_state = MainMenuState::BOOT_SCREEN;
	float current_time = 0.f;
	float transition_time = 2000.f;

	
	BootScreensController* boot_screens_controller = nullptr;

	bool hide_background = false;
	ComponentImage* background = nullptr;
	
	ComponentImage* main_title_image = nullptr;
	MainMenuButton* start_button = nullptr;

	ComponentAudioSource* menu_audio_source = nullptr;

	size_t selected_button = 0;
	std::vector<MainMenuButton*> main_menu_buttons;
	std::vector<MainMenuSubMenuController*> main_menu_submenus;

};
extern "C" SCRIPT_API MainMenuController* MainMenuControllerDLL(); //This is how we are going to load the script
#endif