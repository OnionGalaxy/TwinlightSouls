#include "PauseMenuController.h"

#include "Component/ComponentAudioSource.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleScene.h"

#include "AudioManager.h"
#include "InputManager.h"
#include "UIMainMenuInputController.h"
#include "WorldManager.h"

#include "imgui.h"

PauseMenuController* PauseMenuControllerDLL()
{
	PauseMenuController* instance = new PauseMenuController();
	return instance;
}

PauseMenuController::PauseMenuController()
{
	
}

void PauseMenuController::Awake()
{
	buttons.push_back(resume_button);
	buttons.push_back(main_menu_button);
	buttons.push_back(level_selection_button);
	buttons.push_back(help_button);

	pause_menu_buttons.push_back(static_cast<PauseMenuButton*>(resume_button->GetComponentScript("PauseMenuButton")->script));
	pause_menu_buttons.push_back(static_cast<PauseMenuButton*>(main_menu_button->GetComponentScript("PauseMenuButton")->script));
	pause_menu_buttons.push_back(static_cast<PauseMenuButton*>(level_selection_button->GetComponentScript("PauseMenuButton")->script));
	pause_menu_buttons.push_back(static_cast<PauseMenuButton*>(help_button->GetComponentScript("PauseMenuButton")->script));

	back_key = static_cast<PauseMenuButton*>(back_keyboard->GetComponentScript("PauseMenuButton")->script);
	back_gpad = static_cast<PauseMenuButton*>(back_gamepad->GetComponentScript("PauseMenuButton")->script);
	arrows_key = static_cast<PauseMenuButton*>(arrows_keyboard->GetComponentScript("PauseMenuButton")->script);
	arrows_gpad = static_cast<PauseMenuButton*>(arrows_gamepad->GetComponentScript("PauseMenuButton")->script);
	title_key = static_cast<PauseMenuButton*>(keyboard_title->GetComponentScript("PauseMenuButton")->script);
	title_gpad = static_cast<PauseMenuButton*>(gamepad_title->GetComponentScript("PauseMenuButton")->script);

	pause_menu_anim = static_cast<PauseMenuButton*>(pause_menu_decoration->GetComponentScript("PauseMenuButton")->script);

	GameObject* audio_go = App->scene->GetGameObjectByName("AudioManager");
	const ComponentScript* audio_component = audio_go->GetComponentScript("AudioManager");
	audio_manager = (AudioManager*)audio_component->script;

	current = buttons.size() - 1;
	awaked = true;

	GameObject* world_go = App->scene->GetGameObjectByName("World Manager");
	const ComponentScript* world_component = world_go->GetComponentScript("WorldManager");
	world_manager = (WorldManager*)world_component->script;
	
	GameObject* input_go = App->scene->GetGameObjectByName("Input Manager");
	const ComponentScript* input_component = input_go->GetComponentScript("InputManager");
	input_manager = (InputManager*)input_component->script;
}

void PauseMenuController::Update()
{

	if (!awaked)
	{
		Awake();
	}
	if (!pause_menu_panel->IsEnabled() && !on_help)
	{
		return;
	}

	if(on_help)
	{
		UpdateGamepad_Keyboard();
		if ((input_manager->GetControllerButtonDown(ControllerCode::LeftShoulder, ControllerID::ONE) && input_manager->total_game_controllers > 0) || (input_manager->total_game_controllers <= 0 && input_manager->GetKeyDown(KeyCode::Q)))
		{
			if (!help_panel_1->IsEnabled()) {
				title_key->Unselect();
				title_gpad->Select();
			}
			help_panel_1->SetEnabled(true);
			help_panel_2->SetEnabled(false);
			audio_manager->audio_source_ui->PlayEvent("menu_hover");
		}

		if ((input_manager->total_game_controllers > 0 && input_manager->GetControllerButtonDown(ControllerCode::RightShoulder, ControllerID::ONE)) || (input_manager->total_game_controllers <=0 && input_manager->GetKeyDown(KeyCode::E)))
		{
			if (!help_panel_2->IsEnabled()) {
				title_key->Select();
				title_gpad->Unselect();
			}
			help_panel_1->SetEnabled(false);
			help_panel_2->SetEnabled(true);
			
			audio_manager->audio_source_ui->PlayEvent("menu_hover");
		}

		if ((UIMainMenuInputController::CancelButtonPressed(input_manager)&& input_manager->total_game_controllers <= 0) || (input_manager->total_game_controllers > 0 && input_manager->GetControllerButtonDown(ControllerCode::B, ControllerID::ONE)))
		{
			on_help = false;
			background->SetEnabled(false);
			help_panel_1->SetEnabled(false);
			help_panel_2->SetEnabled(false);
			back_keyboard->SetEnabled(false);
			arrows_keyboard->SetEnabled(false);
			keyboard_title->SetEnabled(false);
			back_gamepad->SetEnabled(false);
			arrows_gamepad->SetEnabled(false);
			gamepad_title->SetEnabled(false);
			pause_menu_panel->SetEnabled(true);
			audio_manager->audio_source_ui->PlayEvent("menu_select");
		}
		return;
	}

	if (UIMainMenuInputController::ComfirmButtonPressed(input_manager))
	{
		audio_manager->audio_source_ui->PlayEvent("menu_select");
		//Change scene
		switch (current)
		{
			case 0:
				//Resume game
				Close();
				return;
			case 1:
				//Main menu
				App->time->time_scale = 1.f;
				App->scene->LoadScene(MAIN_MENU_SCENE);
				break;
			case 2:
				//Active level selection
				App->time->time_scale = 1.f;
				WorldManager::level_selection_activated = true;
				App->scene->LoadScene(MAIN_MENU_SCENE);
				break;
			case 3:
				//Active help
				help_panel_1->SetEnabled(true);
				pause_menu_panel->SetEnabled(false);
				background->SetEnabled(true);
				keyboard_title->SetEnabled(true);
				gamepad_title->SetEnabled(true);
				title_gpad->Select();
				title_key->Unselect();
				changed_to_gamepad = (input_manager->total_game_controllers <= 0);

				UpdateGamepad_Keyboard();
				on_help = true;
				return;
			default:
				break;
		}
	}
	if (UIMainMenuInputController::ConfirmMovedDown(input_manager))
	{
		pause_menu_buttons[current]->Unselect();
		current = (current - 1) == -1 ? buttons.size() -1: current-1;
		pause_menu_buttons[current]->Select();
		audio_manager->audio_source_ui->PlayEvent("menu_hover");
	}

	else if (UIMainMenuInputController::ConfirmMovedUp(input_manager))
	{
		pause_menu_buttons[current]->Unselect();
		current += 1;
		current = current % buttons.size();
		pause_menu_buttons[current]->Select();
		audio_manager->audio_source_ui->PlayEvent("menu_hover");
	}
}

void PauseMenuController::Open()
{
	current = 0;
	pause_menu_panel->SetEnabled(true);
	help_panel_1->SetEnabled(false);
	help_panel_2->SetEnabled(false);
	background->SetEnabled(false);
	back_keyboard->SetEnabled(false);
	arrows_keyboard->SetEnabled(false);
	keyboard_title->SetEnabled(false);
	back_gamepad->SetEnabled(false);
	arrows_gamepad->SetEnabled(false);
	gamepad_title->SetEnabled(false);
	patterns->SetEnabled(true);
	SetupPatterns();
	for (auto but : pause_menu_buttons) {
		but->Unselect();
	}
	pause_menu_buttons[0]->Select();
	pause_menu_anim->Select();
}

void PauseMenuController::Close()
{
	world_manager->on_pause = false;
	App->time->time_scale = 1.f;
	pause_menu_panel->SetEnabled(false);
	help_panel_1->SetEnabled(false);
	help_panel_2->SetEnabled(false);
	background->SetEnabled(false);
	back_keyboard->SetEnabled(false);
	arrows_keyboard->SetEnabled(false);
	keyboard_title->SetEnabled(false);
	back_gamepad->SetEnabled(false);
	arrows_gamepad->SetEnabled(false);
	gamepad_title->SetEnabled(false);
	patterns->SetEnabled(false);
	on_help = false;
}

void PauseMenuController::SetupPatterns()
{
	pattern_b_pos = patterns->children[0]->transform_2d.anchored_position;

	ComponentImage* img_b = (ComponentImage*)patterns->children[0]->GetComponent(Component::ComponentType::UI_IMAGE);
	
	img_b->SetColor(float4(1, 1, 1, 0.5f));
	
	TweenSequence* sequence = App->animations->CreateTweenSequence();
	sequence->Append(Tween::LOColor(img_b, float4::zero, 2.5f)->SetLoops(-1, Tween::TweenLoop::YOYO));
	
	sequence->Play();
}

void PauseMenuController::UpdateGamepad_Keyboard()
{
	if (input_manager->total_game_controllers <= 0 && changed_to_gamepad)
	{
		back_keyboard->SetEnabled(true);
		arrows_keyboard->SetEnabled(true);
		back_gamepad->SetEnabled(false);
		arrows_gamepad->SetEnabled(false);
		back_key->Select();
		arrows_key->Select();
		changed_to_gamepad = false;

	}
	else
	{
		if (!changed_to_gamepad && input_manager->total_game_controllers > 0) {
			back_gamepad->SetEnabled(true);
			arrows_gamepad->SetEnabled(true);
			back_keyboard->SetEnabled(false);
			arrows_keyboard->SetEnabled(false);
			back_gpad->Select();
			arrows_gpad->Select();
			changed_to_gamepad = true;
		}
	}
}


void PauseMenuController::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Checkbox("On Help", &on_help);
}

void PauseMenuController::InitPublicGameObjects()
{
	public_gameobjects.push_back(&resume_button);
	variable_names.push_back(GET_VARIABLE_NAME(resume_button));

	public_gameobjects.push_back(&help_button);
	variable_names.push_back(GET_VARIABLE_NAME(help_button));

	public_gameobjects.push_back(&level_selection_button);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection_button));

	public_gameobjects.push_back(&main_menu_button);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_button));

	public_gameobjects.push_back(&help_panel_1);
	variable_names.push_back(GET_VARIABLE_NAME(help_panel_1));

	public_gameobjects.push_back(&help_panel_2);
	variable_names.push_back(GET_VARIABLE_NAME(help_panel_2));

	public_gameobjects.push_back(&pause_menu_panel);
	variable_names.push_back(GET_VARIABLE_NAME(pause_menu_panel));

	public_gameobjects.push_back(&patterns);
	variable_names.push_back(GET_VARIABLE_NAME(patterns));

	public_gameobjects.push_back(&pause_menu_decoration);
	variable_names.push_back(GET_VARIABLE_NAME(pause_menu_decoration));

	public_gameobjects.push_back(&background);
	variable_names.push_back(GET_VARIABLE_NAME(background));

	public_gameobjects.push_back(&back_keyboard);
	variable_names.push_back(GET_VARIABLE_NAME(back_keyboard));

	public_gameobjects.push_back(&back_gamepad);
	variable_names.push_back(GET_VARIABLE_NAME(back_gamepad));

	public_gameobjects.push_back(&arrows_keyboard);
	variable_names.push_back(GET_VARIABLE_NAME(arrows_keyboard));

	public_gameobjects.push_back(&arrows_gamepad);
	variable_names.push_back(GET_VARIABLE_NAME(arrows_gamepad));

	public_gameobjects.push_back(&keyboard_title);
	variable_names.push_back(GET_VARIABLE_NAME(keyboard_title));

	public_gameobjects.push_back(&gamepad_title);
	variable_names.push_back(GET_VARIABLE_NAME(gamepad_title));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

