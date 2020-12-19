#include "TimelineManager.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include <imgui.h>

#include "BossIntroCinematic.h"
#include "BossOutroCinematic.h"
#include "Lvl1IntroCinematic.h"
#include "Lvl1OutroCinematic.h"
#include "Lvl2IntroCinematic.h"
#include "Lvl2OutroCinematic.h"

#include "Log/EngineLog.h"

#include "InputManager.h"
#include "PlayerAttack.h"
#include "PlayerController.h"
#include "UIManager.h"
#include "WorldManager.h"

namespace
{
	float TIME_TO_SHOW_SKIP = 2000;
}
TimelineManager* TimelineManagerDLL()
{
	TimelineManager* instance = new TimelineManager();
	return instance;
}

void TimelineManager::Awake()
{
	world_manager = static_cast<WorldManager*>(App->scene->GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	input_manager = static_cast<InputManager*>(App->scene->GetGameObjectByName("Input Manager")->GetComponentScript("InputManager")->script);
	ui_manager = static_cast<UIManager*>(App->scene->GetGameObjectByName("UIManager")->GetComponentScript("UIManager")->script);
}

// Use this for initialization
void TimelineManager::Start()
{
	cinematics.clear();
	world_manager = static_cast<WorldManager*>(App->scene->GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	player_1 = world_manager->GetPlayer1();
	ui_manager->ShowSkipButton();
	player_1->player_controller->player_attack->is_attack_blocked = true;
	switch (world_manager->current_level)
	{

		case Level::FIRST:
		{
			cinematics[LVL1_INTRO] = std::make_unique<Lvl1IntroCinematic>(*App->scene, *App->time);
			cinematics[LVL1_OUTRO] = std::make_unique<Lvl1OutroCinematic>(*App->scene, *App->time);
			cinematics[LVL1_INTRO]->Play();
			break;

		}
		case Level::SECOND:
		{
			cinematics[LVL2_INTRO] = std::make_unique<Lvl2IntroCinematic>(*App->scene, *App->time);
			//cinematics[LVL2_OUTRO] = std::make_unique<Lvl1OutroCinematic>(*App->scene, *App->time);
			cinematics[LVL2_INTRO]->Play();
			break;
		}
		case Level::BOSS:
		{
			cinematics[BOSS_OUTRO] = std::make_unique<BossOutroCinematic>(*App->scene, *App->time);
			cinematics[BOSS_INTRO] = std::make_unique<BossIntroCinematic>(*App->scene, *App->time);
			cinematics[BOSS_INTRO]->Play();
			break;

		}
	}

}


// Update is called once per frame
void TimelineManager::Update()
{
	bool skip_cinematic = false;
	if (ui_manager->IsSkipButtonActive() && input_manager->GetGameInputDown("Punch", static_cast<PlayerID>(0)))
	{
		skip_cinematic = true;
	}
	int current_playing = 0;
	for (auto& cinematic : cinematics)
	{
		if (cinematic.second->IsPlaying() && skip_cinematic)
		{
			cinematic.second->Stop();
			ui_manager->HideSkipButton();
			player_1->player_controller->player_attack->is_attack_blocked = false;
			break;
		}
		cinematic.second->Update();
		if (cinematic.second->IsPlaying())
		{
			++current_playing;
			current_playing_time += App->time->delta_time;
			if (current_playing_time >= TIME_TO_SHOW_SKIP && !ui_manager->IsSkipButtonActive())
			{
				ui_manager->ShowSkipButton();
			}
		}
	}
	if (current_playing == 0 && ui_manager->IsSkipButtonActive())
	{
		ui_manager->HideSkipButton();
		player_1->player_controller->player_attack->is_attack_blocked = false;
	}
}

void TimelineManager::PlayTimeline(CinematicID cinematic_id)
{
	App->engine_log->Log("%d", cinematics.size());
	if (cinematics.find(cinematic_id) != cinematics.end())
	{
		current_playing_time = 0;
		player_1->player_controller->player_attack->is_attack_blocked = true;
		cinematics[cinematic_id]->Play();
	}
}



// Use this for showing variables on inspector
void TimelineManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}



//Use this for linking JUST GO automatically 
void TimelineManager::InitPublicGameObjects()
{
	for (size_t i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
