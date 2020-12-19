#include "MainMenuController.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform2D.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleWindow.h"

#include "BootScreensController.h"
#include "CreditsMenuController.h"
#include "InputManager.h"
#include "MainMenuButton.h"
#include "MainMenuSubMenuController.h"
#include "PlayMenuController.h"
#include "SystemMenuController.h"
#include "UIMainMenuInputController.h"
#include "WorldManager.h"

#include <imgui.h>

MainMenuController* MainMenuControllerDLL()
{
	MainMenuController* instance = new MainMenuController();
	return instance;
}

MainMenuController::MainMenuController()
{	
}

// Use this for initialization before Start()
void MainMenuController::Awake()
{
	input_manager = static_cast<InputManager*>(App->scene->GetGameObjectByName("Input Manager")->GetComponentScript("InputManager")->script);
	world_manager = static_cast<WorldManager*>(App->scene->GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	menu_audio_source = static_cast<ComponentAudioSource*>(App->scene->GetGameObjectByName("Main Camera")->GetComponent(Component::ComponentType::AUDIO_SOURCE));

	background = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Background")->GetComponent(Component::ComponentType::UI_IMAGE));
	boot_screens_controller = static_cast<BootScreensController*>(App->scene->GetGameObjectByName("Boot Screens")->GetComponentScript("BootScreensController")->script);

	main_title_image = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Main Title")->GetComponent(Component::ComponentType::UI_IMAGE));
	start_button = static_cast<MainMenuButton*>(App->scene->GetGameObjectByName("Start Button")->GetComponentScript("MainMenuButton")->script);
	
	GameObject* main_menu_buttons_holder = App->scene->GetGameObjectByName("Main Menu")->GetChildrenWithName("Buttons");
	for (auto& main_menu_button : main_menu_buttons_holder->children)
	{
		main_menu_buttons.push_back(static_cast<MainMenuButton*>(main_menu_button->GetComponentScript("MainMenuButton")->script));
	}

	GameObject* main_menu_submenus_holder = App->scene->GetGameObjectByName("Main Menu Submenus");
	main_menu_submenus.push_back(static_cast<PlayMenuController*>(main_menu_submenus_holder->children[0]->GetComponentScript("PlayMenuController")->script));
	main_menu_submenus.push_back(static_cast<SystemMenuController*>(main_menu_submenus_holder->children[1]->GetComponentScript("SystemMenuController")->script));
	main_menu_submenus.push_back(static_cast<CreditsMenuController*>(main_menu_submenus_holder->children[2]->GetComponentScript("CreditsMenuController")->script));
}

void MainMenuController::Start()
{
	selected_button = 0;
	start_button->FastHide();
	start_button->fade_time = 1000.f;
	for (auto& main_menu_button : main_menu_buttons)
	{
		main_menu_button->FastHide();
	}
	for (auto& main_menu_submenu : main_menu_submenus)
	{
		main_menu_submenu->FastHide();
	}

	background->color.w = 1.f;
	main_title_image->color.w = 0.f;

	if (WorldManager::level_selection_activated)
	{
		background->color.w = 0.f;
		hide_background = true;
		current_state = MainMenuState::SUB_MENU;
		menu_audio_source->PlayEvent("Play_Music_Menu");
	}
	else if (WorldManager::show_boot_screens)
	{
		boot_screens_controller->Init();
		current_state = MainMenuState::BOOT_SCREEN;
	}
	else
	{
		menu_audio_source->PlayEvent("Play_Music_Menu");
		start_button->Show();
		current_time = 0.f;
		current_state = MainMenuController::MainMenuState::SHOWING_TITLE_SCREEN;
	}
}

// Update is called once per frame
void MainMenuController::Update()
{
	switch (current_state)
	{
	case MainMenuController::MainMenuState::BOOT_SCREEN:
		if (boot_screens_controller->HasFinished())
		{
			menu_audio_source->PlayEvent("Play_Music_Menu");
			start_button->Show();
			WorldManager::show_boot_screens = false;
			current_time = 0.f;
			current_state = MainMenuController::MainMenuState::SHOWING_TITLE_SCREEN;
		}
		break;

	case MainMenuController::MainMenuState::SHOWING_TITLE_SCREEN:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transition_time);
		background->color.w = math::Lerp(1.f, 0.25f, progress);
		main_title_image->color.w = progress;

		if (progress == 1.f)
		{
			start_button->Select();
			current_state = MainMenuController::MainMenuState::PRESS_START;
		}
	}	
	break;

	case MainMenuController::MainMenuState::PRESS_START:
	{
		if (App->input->GetAnyKeyPressedDown())
		{
			start_button->fade_time = 100.f;
			start_button->Hide();
			menu_audio_source->PlayEvent("start_game");
			ShowMainMenu();
			current_state = MainMenuController::MainMenuState::MAIN_MENU;
		}
	}
	break;

	case MainMenuController::MainMenuState::MAIN_MENU:
	{
		if (UIMainMenuInputController::ConfirmMovedUp(input_manager))
		{
			menu_audio_source->PlayEvent("menu_hover");
			main_menu_buttons[selected_button]->Unselect();
			if (selected_button == 0)
			{
				selected_button = main_menu_buttons.size() - 1;
			}
			else
			{
				--selected_button;
			}
			main_menu_buttons[selected_button]->Select();
		}
		else if (UIMainMenuInputController::ConfirmMovedDown(input_manager))
		{
			menu_audio_source->PlayEvent("menu_hover");
			main_menu_buttons[selected_button]->Unselect();
			selected_button = (selected_button + 1) % main_menu_buttons.size();
			main_menu_buttons[selected_button]->Select();
		}
		else if (UIMainMenuInputController::ComfirmButtonPressed(input_manager))
		{
			menu_audio_source->PlayEvent("menu_select");
			HideMainMenu();
			if (selected_button == main_menu_buttons.size() - 1)
			{
				App->window->CloseWindow();
			}
			else
			{
				hide_background = main_menu_submenus[selected_button]->hide_main_menu_background;
				main_menu_submenus[selected_button]->Show();
				current_time = 0.f;
				current_state = MainMenuController::MainMenuState::SHOWING_SUB_MENU;
			}
		}
	}
	break;

	case MainMenuController::MainMenuState::SHOWING_SUB_MENU:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / 150.f);
		main_title_image->color.w = 1 - progress;

		if (hide_background)
		{
			background->color.w = math::Lerp(0.25f, 0.f, progress);
		}

		if (progress == 1.f)
		{
			current_state = MainMenuController::MainMenuState::SUB_MENU;
		}
	}
	break;

	case MainMenuController::MainMenuState::SUB_MENU:
	{
		if (main_menu_submenus[selected_button]->Closed())
		{
			ShowMainMenu();
			current_time = 0.f;
			current_state = MainMenuController::MainMenuState::HIDING_SUB_MENU;
		}
	}
	break;

	case MainMenuController::MainMenuState::HIDING_SUB_MENU:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / 150.f);
		main_title_image->color.w = progress;

		if (hide_background)
		{
			background->color.w = math::Lerp(0.f, 0.25f, progress);
		}

		if (progress == 1.f)
		{
			current_state = MainMenuController::MainMenuState::MAIN_MENU;
		}
	}
	break;
	default:
		break;
	}
}

// Use this for showing variables on inspector
void MainMenuController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

}

void MainMenuController::ShowMainMenu()
{
	for (auto& main_menu_button : main_menu_buttons)
	{
		main_menu_button->Show();
	}
	main_menu_buttons[selected_button]->Select();
}

void MainMenuController::HideMainMenu()
{
	for (auto& main_menu_button : main_menu_buttons)
	{
		main_menu_button->Hide();
	}
}