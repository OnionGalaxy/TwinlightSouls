#include "CreditsMenuController.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "CreditsManager.h"
#include "InputManager.h"
#include "MainMenuButton.h"
#include "MainMenuCamera.h"
#include "UIMainMenuInputController.h"

CreditsMenuController* CreditsMenuControllerDLL()
{
	CreditsMenuController* instance = new CreditsMenuController();
	return instance;
}

CreditsMenuController::CreditsMenuController()
{
}

// Use this for initialization before Start()
void CreditsMenuController::Awake()
{
	credits_manager = static_cast<CreditsManager*>(owner->GetComponentScript("CreditsManager")->script);
	main_menu_camera = static_cast<MainMenuCamera*>(App->scene->GetGameObjectByName("Main Camera")->GetComponentScript("MainMenuCamera")->script);
	input_manager = static_cast<InputManager*>(App->scene->GetGameObjectByName("Input Manager")->GetComponentScript("InputManager")->script);
}

// Use this for initialization
void CreditsMenuController::Start()
{

}

// Update is called once per frame
void CreditsMenuController::Update()
{
	if(closed)
	{
		return;
	}

	current_camera_time += App->time->delta_time;

	if(!triggered && current_camera_time >= total_camera_time)
	{
		credits_manager->Enable();
		triggered = true;
		return;
	}


	if (UIMainMenuInputController::CancelButtonPressed(input_manager) || (triggered && !credits_manager->AreCreditsActive()))
	{
		FastHide();
	}
}

void CreditsMenuController::Show()
{
	closed = false;
	current_camera_time = 0.f;
	main_menu_camera->TransitionTo(MainMenuCamera::MainMenuCameraPresetName::CREDITS);
}

void CreditsMenuController::FastHide()
{
	closed = true;
	credits_manager->FastHide();
	main_menu_camera->TransitionTo(MainMenuCamera::MainMenuCameraPresetName::MAIN_MENU);
	triggered = false;
}


// Use this for showing variables on inspector
void CreditsMenuController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
}