#include "OptionsMenuController.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleAudio.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "AudioManager.h"
#include "InputManager.h"
#include "LevelSelectionMenuController.h"
#include "MainMenuController.h"
#include "UIMainMenuInputController.h"
#include "WorldManager.h"

#include "Log/EngineLog.h"

OptionsMenuController::OptionsMenuController()
{

}

void OptionsMenuController::Awake()
{
	GameObject* audio_go = App->scene->GetGameObjectByName("AudioManager");
	const ComponentScript* audio_component = audio_go->GetComponentScript("AudioManager");
	audio_manager = (AudioManager*)audio_component->script;

	main_menu_controller = static_cast<MainMenuController*>(main_menu_game_object->GetComponentScript("MainMenuController")->script);
	world_manager = static_cast<WorldManager*>(world_manager_game_object->GetComponentScript("WorldManager")->script);
	back_button = static_cast<ComponentButton*>(back_button_game_object->GetComponent(Component::ComponentType::UI_BUTTON));
	cursor_transform = &cursor->transform_2d;
	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
	input_manager = static_cast<InputManager*>(input_manager_game_object->GetComponentScript("InputManager")->script);

	less_volume_button = static_cast<ComponentButton*>(less_volume_go->GetComponent(Component::ComponentType::UI_BUTTON));
	more_volume_button = static_cast<ComponentButton*>(more_volume_go->GetComponent(Component::ComponentType::UI_BUTTON));

	less_res_button = static_cast<ComponentButton*>(less_res_go->GetComponent(Component::ComponentType::UI_BUTTON));
	more_res_button = static_cast<ComponentButton*>(more_res_go->GetComponent(Component::ComponentType::UI_BUTTON));
	res_text = static_cast<ComponentText*>(res_text_go->GetComponent(Component::ComponentType::UI_TEXT));

	difficulty = world_manager->difficulty;
	ManageDifficultyUI();

	cursor_positions.push_back(&volume_settings->transform_2d);
	cursor_positions.push_back(&difficulty_settings->transform_2d);
	//cursor_positions.push_back(&resolution_settings->transform_2d);
	cursor_positions.push_back(&back_button_game_object->transform_2d);

	cursor->SetEnabled(false);
	MoveCursor();
	
}

void OptionsMenuController::Update()
{
	if (!enabled)
	{
		return;
	}

	if (!set_once)
	{
		ManageVolumeUI();
		set_once = true;
	}

	if (UIMainMenuInputController::ConfirmMovedUp(input_manager))
	{
		if (!cursor->IsEnabled())
			cursor->SetEnabled(true);
		if (current_button > 0)
			current_button -= 1;
		MoveCursor();
		audio_source->PlayEvent("menu_hover");
	}

	if (UIMainMenuInputController::ConfirmMovedDown(input_manager))
	{
		if (!cursor->IsEnabled())
			cursor->SetEnabled(true);
		if (current_button < 2) 
			current_button += 1;
		MoveCursor();
		audio_source->PlayEvent("menu_hover");
	}

	App->engine_log->Log("%d\n", current_volume);

	switch (current_button)
	{
		case 0: //volume
			if (UIMainMenuInputController::ConfirmMovedRight(input_manager))
			{
				if (current_volume < 10)
					++current_volume;
				ManageVolumeUI();
			}
			if (UIMainMenuInputController::ConfirmMovedLeft(input_manager))
			{
				if (current_volume > 0)
					--current_volume;
				ManageVolumeUI();
			}
			audio_manager->master_volume = current_volume * 10;
			App->audio->SetRTPCValue("master_volume", audio_manager->master_volume);
			break;
		/*
		case 1: //resolution
			if (UIMainMenuInputController::ConfirmMovedRight(input_manager) || UIMainMenuInputController::ConfirmMovedLeft(input_manager))
			{
				if (res_text->text == "1920x1080")
					res_text->text = "1024x768";

				else
					res_text->text = "1920x1080";
			}*/

		case 1: //difficulty
			if (UIMainMenuInputController::ConfirmMovedRight(input_manager))
			{
				if (difficulty < 2)
				{
					difficulty = difficulty + 1;
					ManageDifficultyUI();
					switch(difficulty)
					{
					case 1:
						world_manager->difficulty = 1;
						break;

					case 2:
						world_manager->difficulty = 2;
						break;
					
					default:
						break;
					}
				}
			}
			if (UIMainMenuInputController::ConfirmMovedLeft(input_manager))
			{
				if (difficulty > 0)
				{
					difficulty = difficulty - 1;
					ManageDifficultyUI();
					switch (difficulty)
					{
					case 1:
						world_manager->difficulty = 1;
						break;

					case 0:
						world_manager->difficulty = 0;
						break;

					default:
						break;
					}
				}
			}
	}
	
	if (input_manager->total_game_controllers <= 0)
	{
		gamepad_icons->SetEnabled(false);
		pc_icons->SetEnabled(true);
	}
	else
	{
		gamepad_icons->SetEnabled(true);
		pc_icons->SetEnabled(false);
	}

	if (UIMainMenuInputController::ComfirmButtonPressed(input_manager) && current_button == 2)
	{
		TweenSequence* sequence = App->animations->CreateTweenSequence();
		sequence->Append(Tween::LOScale(&back_button->owner->transform_2d, float3::one * 1.1f, 0.25f)->SetEase(Tween::EaseType::SINE));
		sequence->OnCompleted([this](void) {
			Close();
		});
		sequence->Play();

		return;
	}

	if (back_button->IsClicked() )
	{
		TweenSequence* sequence = App->animations->CreateTweenSequence();
		sequence->Append(Tween::LOScale(&back_button->owner->transform_2d, float3::one * 1.1f, 0.25f)->SetEase(Tween::EaseType::SINE));
		sequence->OnCompleted([this](void) {
			Close();
		});
		sequence->Play();

		return;
	}

	if (more_volume_button->IsClicked())
	{
		TweenSequence* sequence = App->animations->CreateTweenSequence();
		sequence->Append(Tween::LOScale(&more_volume_button->owner->transform_2d, float3::one * 1.1f, 0.1f)->SetEase(Tween::EaseType::SINE));
		sequence->OnCompleted([this](void) {
			if (current_volume < 10)
				++current_volume;
			ManageVolumeUI();

		});
		sequence->Play();
	}

	if (less_volume_button->IsClicked())
	{
		TweenSequence* sequence = App->animations->CreateTweenSequence();
		sequence->Append(Tween::LOScale(&less_volume_button->owner->transform_2d, float3::one * 1.1f, 0.1f)->SetEase(Tween::EaseType::SINE));
		sequence->OnCompleted([this](void) {
			if (current_volume > 0)
				--current_volume;
			ManageVolumeUI();
		});
		sequence->Play();
	}

	if (less_res_button->IsClicked() || more_res_button->IsClicked())
	{
		TweenSequence* sequence = App->animations->CreateTweenSequence();
		if (less_res_button->IsClicked()) sequence->Append(Tween::LOScale(&less_res_button->owner->transform_2d, float3::one * 1.1f, 0.1f)->SetEase(Tween::EaseType::SINE));
		else sequence->Append(Tween::LOScale(&more_res_button->owner->transform_2d, float3::one * 1.1f, 0.1f)->SetEase(Tween::EaseType::SINE));

		sequence->OnCompleted([this](void) {
			if (res_text->text == "1920x1080")
				res_text->text = "1024x768";

			else
				res_text->text = "1920x1080";
		});
		sequence->Play();
	}
}

void OptionsMenuController::ManageVolumeUI()
{
	for (int i = 0; i < current_volume; ++i)
	{
		volume_settings->children[i]->children[0]->SetEnabled(true);
	}

	for (int i = current_volume; i < 10; ++i)
	{
		volume_settings->children[i]->children[0]->SetEnabled(false);
	}
}

void OptionsMenuController::ManageDifficultyUI()
{
	for (int i = 0; i < 3; ++i)
	{
		if (i == difficulty)
		{
			difficulty_settings->children[i]->children[0]->SetEnabled(true);
		}
		else
		{
			difficulty_settings->children[i]->children[0]->SetEnabled(false);
		}
	}
}

void OptionsMenuController::Open()
{
	enabled = true;
	just_opened = true;
	options_panel->SetEnabled(true);
	resolution_settings->SetEnabled(false);
	ManageDifficultyUI();
}

void OptionsMenuController::Close()
{
	enabled = false;
	set_once = false;
	options_panel->SetEnabled(false);
	//main_menu_controller->Open();
}

void OptionsMenuController::MoveCursor()
{
	cursor->transform_2d.SetTranslation(float3(cursor_positions[current_button]->GetGlobalTranslation().x - cursor_positions[current_button]->GetWidth() / 2 - 20.f,
		cursor_positions[current_button]->GetGlobalTranslation().y, 0.0f));
}

void OptionsMenuController::OnInspector(ImGuiContext * context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
}

void OptionsMenuController::InitPublicGameObjects()
{
	public_gameobjects.push_back(&options_panel);
	variable_names.push_back(GET_VARIABLE_NAME(options_panel));

	public_gameobjects.push_back(&main_menu_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_game_object));

	public_gameobjects.push_back(&back_button_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(back_button_game_object));

	public_gameobjects.push_back(&world_manager_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(world_manager_game_object));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	public_gameobjects.push_back(&cursor);
	variable_names.push_back(GET_VARIABLE_NAME(cursor));

	public_gameobjects.push_back(&gamepad_icons);
	variable_names.push_back(GET_VARIABLE_NAME(gamepad_icons));

	public_gameobjects.push_back(&pc_icons);
	variable_names.push_back(GET_VARIABLE_NAME(pc_icons));

	public_gameobjects.push_back(&volume_settings);
	variable_names.push_back(GET_VARIABLE_NAME(volume_settings));

	public_gameobjects.push_back(&resolution_settings);
	variable_names.push_back(GET_VARIABLE_NAME(resolution_settings));

	//Volume buttons
	public_gameobjects.push_back(&more_volume_go);
	variable_names.push_back(GET_VARIABLE_NAME(more_volume_go));

	public_gameobjects.push_back(&less_volume_go);
	variable_names.push_back(GET_VARIABLE_NAME(less_volume_go));

	//res buttons
	public_gameobjects.push_back(&more_res_go);
	variable_names.push_back(GET_VARIABLE_NAME(more_res_go));

	public_gameobjects.push_back(&less_res_go);
	variable_names.push_back(GET_VARIABLE_NAME(less_res_go));

	public_gameobjects.push_back(&res_text_go);
	variable_names.push_back(GET_VARIABLE_NAME(res_text_go));

	public_gameobjects.push_back(&input_manager_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(input_manager_game_object));

	public_gameobjects.push_back(&main_menu_panel);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_panel));

	public_gameobjects.push_back(&difficulty_settings);
	variable_names.push_back(GET_VARIABLE_NAME(difficulty_settings));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

OptionsMenuController * OptionsMenuControllerDLL()
{
	OptionsMenuController* instance = new OptionsMenuController();
	return instance;
}
