#include "PlayMenuController.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "LevelSelectionArrows.h"
#include "InputManager.h"
#include "MainMenuButton.h"
#include "UIMainMenuInputController.h"
#include "WorldManager.h"

PlayMenuController* PlayMenuControllerDLL()
{
	PlayMenuController* instance = new PlayMenuController();
	return instance;
}

PlayMenuController::PlayMenuController()
{
	hide_main_menu_background = true;
}

// Use this for initialization before Start()
void PlayMenuController::Awake()
{
	main_menu_camera = static_cast<MainMenuCamera*>(App->scene->GetGameObjectByName("Main Camera")->GetComponentScript("MainMenuCamera")->script);
	input_manager = static_cast<InputManager*>(App->scene->GetGameObjectByName("Input Manager")->GetComponentScript("InputManager")->script);
	world_manager = static_cast<WorldManager*>(App->scene->GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	menu_audio_source = static_cast<ComponentAudioSource*>(App->scene->GetGameObjectByName("Main Camera")->GetComponent(Component::ComponentType::AUDIO_SOURCE));

	/////////////////////////////////////////////////////////////

	GameObject* player_selection_arrow_button_controller_go = App->scene->GetGameObjectByName("Player Selection")->GetChildrenWithName("Arrow Button Controller");
	player_selection_arrow_button_controller = static_cast<MainMenuButton*>(player_selection_arrow_button_controller_go->GetComponentScript("MainMenuButton")->script);

	GameObject* player_selection_arrow_button_keyboard_go = App->scene->GetGameObjectByName("Player Selection")->GetChildrenWithName("Arrow Button Keyboard");
	player_selection_arrow_button_keyboard = static_cast<MainMenuButton*>(player_selection_arrow_button_keyboard_go->GetComponentScript("MainMenuButton")->script);

	player_selection_arrow_button = input_manager->total_game_controllers > 0 ? player_selection_arrow_button_controller : player_selection_arrow_button_keyboard;

	/////////////////////////////////////////////////////////////

	GameObject* player_selection_back_button_controller_go = App->scene->GetGameObjectByName("Player Selection")->GetChildrenWithName("Back Button Controller");
	player_selection_back_button_controller = static_cast<MainMenuButton*>(player_selection_back_button_controller_go->GetComponentScript("MainMenuButton")->script);
	
	GameObject* player_selection_back_button_keyboard_go = App->scene->GetGameObjectByName("Player Selection")->GetChildrenWithName("Back Button Keyboard");
	player_selection_back_button_keyboard = static_cast<MainMenuButton*>(player_selection_back_button_keyboard_go->GetComponentScript("MainMenuButton")->script);

	player_selection_back_button = input_manager->total_game_controllers > 0 ? player_selection_back_button_controller : player_selection_back_button_keyboard;
	
	/////////////////////////////////////////////////////////////

	GameObject* player_selection_confirm_button_controller_go = App->scene->GetGameObjectByName("Player Selection")->GetChildrenWithName("Confirm Button Controller");
	player_selection_confirm_button_controller = static_cast<MainMenuButton*>(player_selection_confirm_button_controller_go->GetComponentScript("MainMenuButton")->script);
	
	GameObject* player_selection_confirm_button_keyboard_go = App->scene->GetGameObjectByName("Player Selection")->GetChildrenWithName("Confirm Button Keyboard");
	player_selection_confirm_button_keyboard = static_cast<MainMenuButton*>(player_selection_confirm_button_keyboard_go->GetComponentScript("MainMenuButton")->script);

	player_selection_confirm_button = input_manager->total_game_controllers > 0 ? player_selection_confirm_button_controller : player_selection_confirm_button_keyboard;
	
	/////////////////////////////////////////////////////////////

	p1_selection_button = static_cast<MainMenuButton*>(App->scene->GetGameObjectByName("P1 Selection Button")->GetComponentScript("MainMenuButton")->script);
	
	p2_selection_button_aux = static_cast<MainMenuButton*>(App->scene->GetGameObjectByName("P2 Selection Button")->GetComponentScript("MainMenuButton")->script);
	p2_selection_button_start = static_cast<MainMenuButton*>(App->scene->GetGameObjectByName("P2 Start Button")->GetComponentScript("MainMenuButton")->script);
	p2_selection_button = p2_selection_button_start;

	background_vignette = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Background Vignette")->GetComponent(Component::ComponentType::UI_IMAGE));
	player_selection_title = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Player Selection Title")->GetComponent(Component::ComponentType::UI_IMAGE));

	GameObject* furro_go = App->scene->GetGameObjectByName("Furro");
	ComponentAnimation* furro_animator = static_cast<ComponentAnimation*>(furro_go->GetComponent(Component::ComponentType::ANIMATION));
	ComponentMeshRenderer* furro_mesh_renderer = static_cast<ComponentMeshRenderer*>(furro_go->children[0]->GetComponent(Component::ComponentType::MESH_RENDERER));
	furro_selector = { furro_animator, furro_mesh_renderer, float3(160.f, 240.f, 0.f) };

	GameObject* furra_go = App->scene->GetGameObjectByName("Furra");
	ComponentAnimation* furra_animator = static_cast<ComponentAnimation*>(furra_go->GetComponent(Component::ComponentType::ANIMATION));
	ComponentMeshRenderer* furra_mesh_renderer = static_cast<ComponentMeshRenderer*>(furra_go->children[0]->GetComponent(Component::ComponentType::MESH_RENDERER));
	furra_selector = { furra_animator,  furra_mesh_renderer, float3(-260.f, 240.f, 0.f) };

	////////////////////////////////////////////////////////////

	GameObject* level_selection_back_button_controller_go = App->scene->GetGameObjectByName("Level Selection")->GetChildrenWithName("Back Button Controller");
	level_selection_back_button_controller = static_cast<MainMenuButton*>(level_selection_back_button_controller_go->GetComponentScript("MainMenuButton")->script);

	GameObject* level_selection_back_button_keyboard_go = App->scene->GetGameObjectByName("Level Selection")->GetChildrenWithName("Back Button Keyboard");
	level_selection_back_button_keyboard = static_cast<MainMenuButton*>(level_selection_back_button_keyboard_go->GetComponentScript("MainMenuButton")->script);

	level_selection_back_button = input_manager->total_game_controllers > 0 ? level_selection_back_button_controller : level_selection_back_button_keyboard;

	/////////////////////////////////////////////////////////////

	GameObject* level_selection_confirm_button_controller_go = App->scene->GetGameObjectByName("Level Selection")->GetChildrenWithName("Confirm Button Controller");
	level_selection_confirm_button_controller = static_cast<MainMenuButton*>(level_selection_confirm_button_controller_go->GetComponentScript("MainMenuButton")->script);

	GameObject* level_selection_confirm_button_keyboard_go = App->scene->GetGameObjectByName("Level Selection")->GetChildrenWithName("Confirm Button Keyboard");
	level_selection_confirm_button_keyboard = static_cast<MainMenuButton*>(level_selection_confirm_button_keyboard_go->GetComponentScript("MainMenuButton")->script);

	level_selection_confirm_button = input_manager->total_game_controllers > 0 ? level_selection_confirm_button_controller : level_selection_confirm_button_keyboard;
	
	/////////////////////////////////////////////////////////////

	GameObject* level_selection_arrows_controller_go = App->scene->GetGameObjectByName("Level Selection")->GetChildrenWithName("Arrows Controller");

	GameObject* level_selection_right_arrow_controller_go = level_selection_arrows_controller_go->GetChildrenWithName("Right Arrow");
	level_selection_right_arrow_controller = static_cast<LevelSelectionArrows*>(level_selection_right_arrow_controller_go->GetComponentScript("LevelSelectionArrows")->script);

	GameObject* level_selection_left_arrow_controller_go = level_selection_arrows_controller_go->GetChildrenWithName("Left Arrow");
	level_selection_left_arrow_controller = static_cast<LevelSelectionArrows*>(level_selection_left_arrow_controller_go->GetComponentScript("LevelSelectionArrows")->script);

	/////////////////////////////////////////////////////////////

	GameObject* level_selection_arrows_keyboard_go = App->scene->GetGameObjectByName("Level Selection")->GetChildrenWithName("Arrows Keyboard");

	GameObject* level_selection_right_arrow_keyboard_go = level_selection_arrows_keyboard_go->GetChildrenWithName("Right Arrow");
	level_selection_right_arrow_keyboard = static_cast<LevelSelectionArrows*>(level_selection_right_arrow_keyboard_go->GetComponentScript("LevelSelectionArrows")->script);

	GameObject* level_selection_left_arrow_keyboard_go = level_selection_arrows_keyboard_go->GetChildrenWithName("Left Arrow");
	level_selection_left_arrow_keyboard = static_cast<LevelSelectionArrows*>(level_selection_left_arrow_keyboard_go->GetComponentScript("LevelSelectionArrows")->script);

	/////////////////////////////////////////////////////////////

	level_selection_left_arrow = input_manager->total_game_controllers > 0 ? level_selection_left_arrow_controller : level_selection_left_arrow_keyboard;
	level_selection_right_arrow = input_manager->total_game_controllers > 0 ? level_selection_right_arrow_controller : level_selection_right_arrow_keyboard;

	/////////////////////////////////////////////////////////////

	level_selection_title = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Level Selection Title")->GetComponent(Component::ComponentType::UI_IMAGE));
	
	GameObject* level_selectors_go = App->scene->GetGameObjectByName("Level Selection")->GetChildrenWithName("Level Selectors");

	for (size_t i = 0; i < 3; ++i)
	{
		auto& child = level_selectors_go->children[i];

		MainMenuButton* level_selector_title = static_cast<MainMenuButton*>(child->GetChildrenWithName("Title")->GetComponentScript("MainMenuButton")->script);
		MainMenuButton* level_selector_description = static_cast<MainMenuButton*>(child->GetChildrenWithName("Description")->GetComponentScript("MainMenuButton")->script);

		level_selectors.push_back({
			level_selector_title,
			level_selector_description,
			static_cast<MainMenuCamera::MainMenuCameraPresetName>(i + 2)
		});
	}
}

// Use this for initialization
void PlayMenuController::Start()
{
	if (WorldManager::level_selection_activated)
	{
		closed = false;

		current_state = PlayMenuController::PlayMenuControllerState::LEVEL_SELECTION;
		level_selection_title->color.w = 1.f;
		background_vignette->color.w = 1.f;
		level_selection_confirm_button->Select();
		level_selection_back_button->Select();

		level_selection_right_arrow->Select();
		selected_level = 0.f;
		level_selectors[selected_level].level_title->Select();
		level_selectors[selected_level].level_description->Show();
	}
}

// Update is called once per frame
void PlayMenuController::Update()
{
	MainMenuButton* player_selection_last_arrow_button = player_selection_arrow_button;
	player_selection_arrow_button = input_manager->total_game_controllers > 0 ? player_selection_arrow_button_controller : player_selection_arrow_button_keyboard;
	if (player_selection_arrow_button != player_selection_last_arrow_button)
	{
		player_selection_last_arrow_button->FastHide();
		if (current_state == PlayMenuControllerState::SHOWING || current_state == PlayMenuControllerState::PLAYER_SELECTION || current_state == PlayMenuControllerState::LEVEL_SELECTION_TO_PLAYER_SELECTION)
		{
			player_selection_arrow_button->Select();
		}
	}

	MainMenuButton* player_selection_last_confirm_button = player_selection_confirm_button;
	player_selection_confirm_button = input_manager->total_game_controllers > 0 ? player_selection_confirm_button_controller : player_selection_confirm_button_keyboard;
	if (player_selection_confirm_button != player_selection_last_confirm_button)
	{
		player_selection_last_confirm_button->FastHide();
		if (current_state == PlayMenuControllerState::SHOWING || current_state == PlayMenuControllerState::PLAYER_SELECTION || current_state == PlayMenuControllerState::LEVEL_SELECTION_TO_PLAYER_SELECTION)
		{
			player_selection_confirm_button->Select();
		}
	}

	MainMenuButton* player_selection_last_back_button = player_selection_back_button;
	player_selection_back_button = input_manager->total_game_controllers > 0 ? player_selection_back_button_controller : player_selection_back_button_keyboard;
	if (player_selection_back_button != player_selection_last_back_button)
	{
		player_selection_last_back_button->FastHide();
		if (current_state == PlayMenuControllerState::SHOWING || current_state == PlayMenuControllerState::PLAYER_SELECTION || current_state == PlayMenuControllerState::LEVEL_SELECTION_TO_PLAYER_SELECTION)
		{
			player_selection_back_button->Select();
		}
	}

	////////////////////////////////////////////////////////////

	MainMenuButton* level_selection_last_confirm_button = level_selection_confirm_button;
	level_selection_confirm_button = input_manager->total_game_controllers > 0 ? level_selection_confirm_button_controller : level_selection_confirm_button_keyboard;
	if (level_selection_confirm_button != level_selection_last_confirm_button)
	{
		level_selection_last_confirm_button->FastHide();
		if (current_state == PlayMenuControllerState::PLAYER_SELECTION_TO_LEVEL_SELECTION || current_state == PlayMenuControllerState::LEVEL_SELECTION)
		{
			level_selection_confirm_button->Select();
		}
	}

	MainMenuButton* level_selection_last_back_button = level_selection_back_button;
	level_selection_back_button = input_manager->total_game_controllers > 0 ? level_selection_back_button_controller : level_selection_back_button_keyboard;
	if (level_selection_back_button != level_selection_last_back_button)
	{
		level_selection_last_back_button->FastHide();
		if (current_state == PlayMenuControllerState::PLAYER_SELECTION_TO_LEVEL_SELECTION || current_state == PlayMenuControllerState::LEVEL_SELECTION)
		{
			level_selection_back_button->Select();
		}
	}

	LevelSelectionArrows* level_selection_last_right_arrow = level_selection_right_arrow;
	level_selection_right_arrow = input_manager->total_game_controllers > 0 ? level_selection_right_arrow_controller : level_selection_right_arrow_keyboard;
	if (level_selection_last_right_arrow != level_selection_right_arrow)
	{
		level_selection_last_right_arrow->FastHide();
		if (current_state == PlayMenuControllerState::PLAYER_SELECTION_TO_LEVEL_SELECTION || current_state == PlayMenuControllerState::LEVEL_SELECTION)
		{
			if (selected_level != 2)
			{
				level_selection_right_arrow->Show();
			}
		}
	}

	LevelSelectionArrows* level_selection_last_left_arrow = level_selection_left_arrow;
	level_selection_left_arrow = input_manager->total_game_controllers > 0 ? level_selection_left_arrow_controller : level_selection_left_arrow_keyboard;
	if (level_selection_last_left_arrow != level_selection_left_arrow)
	{
		level_selection_last_left_arrow->FastHide();
		if (current_state == PlayMenuControllerState::PLAYER_SELECTION_TO_LEVEL_SELECTION || current_state == PlayMenuControllerState::LEVEL_SELECTION)
		{
			if (selected_level != 0)
			{
				level_selection_left_arrow->Show();
			}
		}
	}

	////////////////////////////////////////////////////////////

	switch (current_state)
	{
	case PlayMenuController::PlayMenuControllerState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / menu_transition_time);
		background_vignette->color.w = progress;
		player_selection_title->color.w = progress;

		if (progress == 1.f)
		{
			current_state = PlayMenuController::PlayMenuControllerState::PLAYER_SELECTION;
			player_selection_confirm_button->Select();
			player_selection_back_button->Select();
			player_selection_arrow_button->Select();
			SelectPlayer();
		}
	}
	break;

	case PlayMenuController::PlayMenuControllerState::PLAYER_SELECTION:
	{
		if (UIMainMenuInputController::CancelButtonPressed(input_manager))
		{
			main_menu_camera->TransitionTo(MainMenuCamera::MainMenuCameraPresetName::MAIN_MENU);
			player_selection_confirm_button->Hide();
			player_selection_back_button->Hide();
			player_selection_arrow_button->Hide();

			p1_selection_button->Hide();
			p2_selection_button->Hide();

			menu_audio_source->PlayEvent("menu_select");

			current_state = PlayMenuController::PlayMenuControllerState::HIDING;
			current_time = 0.f;
		}
		else if (UIMainMenuInputController::ConfirmMovedLeft(input_manager) || UIMainMenuInputController::ConfirmMovedRight(input_manager))
		{
			selected_player = selected_player ? 0 : 1;
			menu_audio_source->PlayEvent("menu_hover");
			SelectPlayer();
		}
		else if (UIMainMenuInputController::StartButtonPressed(input_manager) && !multiplayer)
		{
			multiplayer = !multiplayer;

			p2_selection_button->Hide();
			p2_selection_button = multiplayer ? p2_selection_button_aux : p2_selection_button_start;
			p2_selection_button->Select();
			p2_selection_button->owner->transform_2d.SetTranslation(other_selected_player_selector->indicator_position);
			menu_audio_source->PlayEvent("menu_select");
		}
		else if (UIMainMenuInputController::ComfirmButtonPressed(input_manager))
		{
			world_manager->multiplayer = multiplayer;
			world_manager->player1_choice = selected_player;

			player_selection_confirm_button->Hide();
			player_selection_back_button->Hide();
			player_selection_arrow_button->Hide();

			p1_selection_button->Hide();
			p2_selection_button->Hide();

			level_selection_confirm_button->Show();
			level_selection_back_button->Show();

			level_selection_left_arrow->Show();
			level_selection_right_arrow->Show();
			menu_audio_source->PlayEvent("menu_select");

			SelectLevel(0);

			current_state = PlayMenuController::PlayMenuControllerState::PLAYER_SELECTION_TO_LEVEL_SELECTION;
			current_time = 0.f;
		}

		App->debug_draw->RenderOutline(selected_player_selector->player_mesh, float4(0.301f, 0.901f, 0.922f, 1.f));
		if (multiplayer)
		{
			App->debug_draw->RenderOutline(other_selected_player_selector->player_mesh, float4(0.969f, 0.322f, 0.15f, 1.f));
		}
	}
	break;

	case PlayMenuController::PlayMenuControllerState::PLAYER_SELECTION_TO_LEVEL_SELECTION:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / menu_transition_time);
		player_selection_title->color.w = 1 - progress;

		level_selection_title->color.w = progress;

		if (progress == 1.f)
		{
			current_state = PlayMenuController::PlayMenuControllerState::LEVEL_SELECTION;
			level_selection_confirm_button->Select();
			level_selection_back_button->Select();
		}
	}
	break;

	case PlayMenuController::PlayMenuControllerState::LEVEL_SELECTION:
	{
		if (WorldManager::level_selection_activated)
		{
			WorldManager::level_selection_activated = false;
		}

		if (UIMainMenuInputController::CancelButtonPressed(input_manager))
		{
			main_menu_camera->TransitionTo(MainMenuCamera::MainMenuCameraPresetName::PLAYER_SELECTION);
			level_selection_confirm_button->Hide();
			level_selection_back_button->Hide();

			level_selectors[selected_level].level_title->Hide();
			level_selectors[selected_level].level_description->Hide();

			if (selected_level != 0)
			{
				level_selection_left_arrow->Hide();
			}
			if (selected_level != 2)
			{
				level_selection_right_arrow->Hide();
			}

			menu_audio_source->PlayEvent("menu_select");
			current_state = PlayMenuController::PlayMenuControllerState::LEVEL_SELECTION_TO_PLAYER_SELECTION;
			current_time = 0.f;
		}
		else if (UIMainMenuInputController::ConfirmMovedLeft(input_manager) && selected_level > 0)
		{
			level_selection_left_arrow->Select();
			menu_audio_source->PlayEvent("menu_hover");
			SelectLevel(selected_level - 1);
		}
		else if (UIMainMenuInputController::ConfirmMovedRight(input_manager) && selected_level < 2)
		{
			level_selection_right_arrow->Select();
			menu_audio_source->PlayEvent("menu_hover");
			SelectLevel(selected_level + 1);
		}
		else if (UIMainMenuInputController::ComfirmButtonPressed(input_manager))
		{
			menu_audio_source->PlayEvent("menu_select");
			world_manager->LoadLevel(static_cast<Level>(selected_level + 1));
		}
	}
	break;

	case PlayMenuController::PlayMenuControllerState::LEVEL_SELECTION_TO_PLAYER_SELECTION:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / menu_transition_time);
		player_selection_title->color.w = progress;

		level_selection_title->color.w = 1 - progress;

		if (progress == 1.f)
		{
			current_state = PlayMenuController::PlayMenuControllerState::PLAYER_SELECTION;
			player_selection_confirm_button->Select();
			player_selection_back_button->Select();
			player_selection_arrow_button->Select();
			SelectPlayer();
		}
	}
	break;

	case PlayMenuController::PlayMenuControllerState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / menu_transition_time);
		background_vignette->color.w = 1.f - progress;
		player_selection_title->color.w = 1.f - progress;

		if (progress == 1.f)
		{
			current_state = PlayMenuController::PlayMenuControllerState::HIDDEN;
			closed = true;
		}
	}
	break;

	case PlayMenuController::PlayMenuControllerState::HIDDEN:
		break;
	default:
		break;
	}
	
}

void PlayMenuController::Show()
{
	main_menu_camera->TransitionTo(MainMenuCamera::MainMenuCameraPresetName::PLAYER_SELECTION);

	player_selection_confirm_button->Show();
	player_selection_back_button->Show();
	player_selection_arrow_button->Show();

	current_state = PlayMenuController::PlayMenuControllerState::SHOWING;
	current_time = 0.f;

	selected_player = 0;
	multiplayer = false;
	p2_selection_button = p2_selection_button_start;

	closed = false; 
}

void PlayMenuController::FastHide()
{
	background_vignette->color.w = 0.f;
	player_selection_title->color.w = 0.f;
	
	player_selection_confirm_button_controller->FastHide();
	player_selection_confirm_button_keyboard->FastHide();
	player_selection_confirm_button->FastHide();

	player_selection_back_button_controller->FastHide();
	player_selection_back_button_keyboard->FastHide();
	player_selection_back_button->FastHide();

	player_selection_arrow_button_controller->FastHide();
	player_selection_arrow_button_keyboard->FastHide();
	player_selection_arrow_button->FastHide();

	p1_selection_button->FastHide();
	p2_selection_button_start->FastHide();
	p2_selection_button_aux->FastHide();
	p2_selection_button->FastHide();

	////////////////////////////////////////////
	
	level_selection_title->color.w = 0.f;

	level_selection_confirm_button_controller->FastHide();
	level_selection_confirm_button_keyboard->FastHide();
	level_selection_confirm_button->FastHide();

	level_selection_back_button_controller->FastHide();
	level_selection_back_button_keyboard->FastHide();
	level_selection_back_button->FastHide();

	level_selection_left_arrow_controller->FastHide();
	level_selection_left_arrow_keyboard->FastHide();
	level_selection_left_arrow->FastHide();

	level_selection_right_arrow_controller->FastHide();
	level_selection_right_arrow_keyboard->FastHide();
	level_selection_right_arrow->FastHide();

	for (auto& level_selector : level_selectors)
	{
		level_selector.level_title->FastHide();
		level_selector.level_description->FastHide();
	}
}

void PlayMenuController::SelectPlayer()
{
	selected_player_selector = selected_player ? &furra_selector : &furro_selector;
	p1_selection_button->FastHide();
	p1_selection_button->Select();
	p1_selection_button->owner->transform_2d.SetTranslation(selected_player_selector->indicator_position);

	other_selected_player_selector = selected_player ? &furro_selector : &furra_selector;
	p2_selection_button->FastHide();
	p2_selection_button->Select();
	p2_selection_button->owner->transform_2d.SetTranslation(other_selected_player_selector->indicator_position);
}

void PlayMenuController::SelectLevel(int new_level)
{
	if (selected_level == 0 && new_level != 0)
	{
		level_selection_left_arrow->Show();
	}
	if (selected_level == 2 && new_level != 2)
	{
		level_selection_right_arrow->Show();
	}
	
	level_selectors[selected_level].level_title->Hide();
	level_selectors[selected_level].level_description->Hide();
	
	selected_level = new_level;
	
	level_selectors[selected_level].level_title->Select();
	level_selectors[selected_level].level_description->Show();
	
	if (selected_level == 0)
	{
		level_selection_left_arrow->Hide();
	}

	else if (selected_level == 2)
	{
		level_selection_right_arrow->Hide();
	}
	main_menu_camera->TransitionTo(level_selectors[selected_level].camera_preset);
}
