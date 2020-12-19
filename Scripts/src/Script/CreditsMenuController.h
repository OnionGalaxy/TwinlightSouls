#ifndef  __CREDITSMENUCONTROLLER_H__
#define  __CREDITSMENUCONTROLLER_H__

#include "MainMenuSubMenuController.h"

class CreditsManager;
class MainMenuCamera;
class InputManager;

class CreditsMenuController : public MainMenuSubMenuController
{
public:

	CreditsMenuController();
	~CreditsMenuController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Show() override;
	void FastHide() override;

	void OnInspector(ImGuiContext*) override;

private:
	CreditsManager* credits_manager = nullptr;
	MainMenuCamera* main_menu_camera = nullptr;
	InputManager* input_manager = nullptr;
	
	float total_camera_time = 500.f;
	float current_camera_time = 0.f;
	bool triggered = false;

};
extern "C" SCRIPT_API CreditsMenuController* CreditsMenuControllerDLL(); //This is how we are going to load the script
#endif