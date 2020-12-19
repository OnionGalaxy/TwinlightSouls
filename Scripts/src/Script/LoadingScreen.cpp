#include "LoadingScreen.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleResourceManager.h"

#include "Log/EngineLog.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "InputManager.h"
#include "PlayerAttack.h"
#include "PlayerController.h"
#include "ProgressBar.h"
#include "UIMainMenuInputController.h"

LoadingScreen* LoadingScreenDLL()
{
	LoadingScreen* instance = new LoadingScreen();
	return instance;
}

LoadingScreen::LoadingScreen()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void LoadingScreen::Start()
{
	// Tips
	tips.push_back(App->scene->GetGameObjectByName("tip_01"));
	tips.push_back(App->scene->GetGameObjectByName("tip_02"));
	tips.push_back(App->scene->GetGameObjectByName("tip_03"));
	tips.push_back(App->scene->GetGameObjectByName("tip_04"));
	current_tip = tips[0];

	// Units
	units.push_back(App->scene->GetGameObjectByName("unit_0"));
	units.push_back(App->scene->GetGameObjectByName("unit_1"));
	units.push_back(App->scene->GetGameObjectByName("unit_2"));
	units.push_back(App->scene->GetGameObjectByName("unit_3"));
	units.push_back(App->scene->GetGameObjectByName("unit_4"));
	units.push_back(App->scene->GetGameObjectByName("unit_5"));
	units.push_back(App->scene->GetGameObjectByName("unit_6"));
	units.push_back(App->scene->GetGameObjectByName("unit_7"));
	units.push_back(App->scene->GetGameObjectByName("unit_8"));
	units.push_back(App->scene->GetGameObjectByName("unit_9"));
	current_unit_number = units[0];

	// Tens
	tens.push_back(App->scene->GetGameObjectByName("ten_0"));
	tens.push_back(App->scene->GetGameObjectByName("ten_1"));
	tens.push_back(App->scene->GetGameObjectByName("ten_2"));
	tens.push_back(App->scene->GetGameObjectByName("ten_3"));
	tens.push_back(App->scene->GetGameObjectByName("ten_4"));
	tens.push_back(App->scene->GetGameObjectByName("ten_5"));
	tens.push_back(App->scene->GetGameObjectByName("ten_6"));
	tens.push_back(App->scene->GetGameObjectByName("ten_7"));
	tens.push_back(App->scene->GetGameObjectByName("ten_8"));
	tens.push_back(App->scene->GetGameObjectByName("ten_9"));
	current_ten_number = tens[0];

	// Cents
	cents.push_back(App->scene->GetGameObjectByName("cent_0"));
	cents.push_back(App->scene->GetGameObjectByName("cent_1"));
	current_cent_number = cents[0];

	// ally

	/*ComponentTransform2D* ally = &App->scene->GetGameObjectByName("ally")->transform_2d;
	float2 ally_init_pos = ally->anchored_position;
	sequence = App->animations->CreateTweenSequence();
	sequence->Append(Tween::LOTranslate(ally, float2(ally_init_pos.x, ally_init_pos.y + 50.f), 0.75f)->SetLoops(-1, Tween::TweenLoop::YOYO)->SetEase(Tween::EaseType::EASE_OUT_SINE));
	sequence->Play();*/

	// progress bar
	progress_bar = static_cast<ProgressBar*>(App->scene->GetGameObjectByName("bar")->GetComponentScript("ProgressBar")->script);

}

// Update is called once per frame
void LoadingScreen::Update()
{
	if(input_manager == nullptr)
	{
		input_manager = static_cast<InputManager*>(App->scene->GetGameObjectByName("Input Manager")->GetComponentScript("InputManager")->script);
	}

	float current_loaded_resources = App->resources->loading_thread_communication.current_number_of_resources_loaded;
	float total_loaded_resources = App->resources->loading_thread_communication.total_number_of_resources_to_load;
	float percentage = (current_loaded_resources / total_loaded_resources) * 100.f;
	progress_bar->SetProgress(percentage / 100.0f);
	UpdateNumbers(percentage);
	UpdateTips();
}

// Use this for showing variables on inspector
void LoadingScreen::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void LoadingScreen::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

void LoadingScreen::InitHints()
{
	hints.push_back("Use attack buttons on the air to make an aerial attack!");
	hints.push_back("Dashing through an attack makes you invulnerable!");
	hints.push_back("There are two different combos a light combo presing (B) and a heavy combo pressing(X)!");
	hints.push_back("Bring nibery's heart where it belongs! The sacred tree!");
}

void LoadingScreen::UpdateHints()
{
	hints_buffer += App->time->real_time_delta_time / 1000.f;

	if (hints_buffer >= hints_change_time)
	{
		hints_buffer = 0;
		
		current_hint_index += 1;
		if (current_hint_index >= hints.size())
		{
			current_hint_index = 0;
		}

		hints_text->text = hints[current_hint_index];
	}
}

void LoadingScreen::UpdateNumbers(int percentage)
{
	current_unit_number->SetEnabled(false);
	current_unit_number = units[percentage % 10];
	current_unit_number->SetEnabled(true);

	percentage = percentage / 10;
	current_ten_number->SetEnabled(false);
	current_ten_number = tens[percentage % 10];
	current_ten_number->SetEnabled(true);

	percentage = percentage / 10;

	current_cent_number->SetEnabled(false);
	current_cent_number = cents[percentage % 10];
	current_cent_number->SetEnabled(true);
}

void LoadingScreen::UpdateTips()
{
	tips_buffer += App->time->real_time_delta_time / 1000.f;


	if(input_manager->GetKeyDown(KeyCode::LeftArrow) || input_manager->GetKeyDown(KeyCode::A) || input_manager->GetControllerButtonDown(ControllerCode::LeftDpad, ControllerID::ONE))
	{
		tips_buffer = 0;
		current_tip_index--;
		GetNextTip();
	}

	if (input_manager->GetKeyDown(KeyCode::RightArrow) || input_manager->GetKeyDown(KeyCode::D) || input_manager->GetControllerButtonDown(ControllerCode::RightDpad, ControllerID::ONE))
	{
		tips_buffer = 0;
		current_tip_index++;
		GetNextTip();
	}

	if (tips_buffer >= tips_change_time)
	{
		tips_buffer = 0;
		current_tip_index++;
		GetNextTip();
	}
}

void LoadingScreen::GetNextTip()
{
	if (current_tip_index < 0)
	{
		current_tip_index = tips.size() - 1;
	}
	if (current_tip_index >= tips.size())
	{
		current_tip_index = 0;
	}
	current_tip->SetEnabled(false);
	current_tip = tips[current_tip_index];
	current_tip->SetEnabled(true);
}