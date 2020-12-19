#include "Statistics.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "InputManager.h"
#include "EnemyDropManager.h"
#include "PlayerController.h"
#include "WorldManager.h"


Statistics* StatisticsDLL()
{
	Statistics* instance = new Statistics();
	return instance;
}


Statistics::Statistics()
{

}

// Use this for initialization before Start()
void Statistics::Awake()
{
	world_manager = static_cast<WorldManager*>(App->scene->GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	input_manager = static_cast<InputManager*>(App->scene->GetGameObjectByName("Input Manager")->GetComponentScript("InputManager")->script);
	light_orbs_manager = static_cast<EnemyDropManager*>(App->scene->GetGameObjectByName("Enemy Drops")->GetComponentScript("EnemyDropManager")->script);
	InitPanel();
}

// Use this for initialization
void Statistics::Start()
{
	player_1 = world_manager->GetPlayer1();
	player_2 = world_manager->GetPlayer2();
	ui_manager = world_manager->GetUIManager();
	DisablePanel();
}

// Update is called once per frame
void Statistics::Update()
{
	if (!showing_statistics) 
	{
		return;
	}
	if (current_time < timer_to_show && statistics_panel->IsEnabled()) 
	{
		UpdatePanel();
	}
	else
	{
		if (!press_next->IsEnabled()) 
		{
			press_next->owner->SetEnabled(true);
		}
		if (App->input->GetAnyKeyPressedDown())
		{
			world_manager->LoadNextLevel();
		}
	}
}

// Use this for showing variables on inspector
void Statistics::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
}

void Statistics::ShowStatistics()
{
	showing_statistics = true;
	statistics_panel->SetEnabled(true);
	UpdatePanel();
	SetTime();
}

void Statistics::SetTime()
{
	int time_in_seconds = world_manager->GetTotalTime() * 0.001;
	int minutes = time_in_seconds / 60;
	time_in_seconds = time_in_seconds % 60;
	total_time->SetText(std::to_string(minutes) + ":" + std::to_string(time_in_seconds));
}

void Statistics::InitPanel()
{
	statistics_panel = owner;

	enemies_killed_player1 = static_cast<ComponentText*>(owner->GetChildrenWithName("Enemies Killed P1")->GetComponent(Component::ComponentType::UI_TEXT));
	total_damage_player1 = static_cast<ComponentText*>(owner->GetChildrenWithName("Damage P1")->GetComponent(Component::ComponentType::UI_TEXT));
	orbs_statistics_player1 = static_cast<ComponentText*>(owner->GetChildrenWithName("Orbs P1")->GetComponent(Component::ComponentType::UI_TEXT));

	enemies_killed_player2 = static_cast<ComponentText*>(owner->GetChildrenWithName("Enemies Killed P2")->GetComponent(Component::ComponentType::UI_TEXT));
	total_damage_player2 = static_cast<ComponentText*>(owner->GetChildrenWithName("Damage P2")->GetComponent(Component::ComponentType::UI_TEXT));
	orbs_statistics_player2 = static_cast<ComponentText*>(owner->GetChildrenWithName("Orbs P2")->GetComponent(Component::ComponentType::UI_TEXT));

	player_2_stats = static_cast<ComponentText*>(owner->GetChildrenWithName("Player 2 Stats")->GetComponent(Component::ComponentType::UI_TEXT));

	total_time = static_cast<ComponentText*>(owner->GetChildrenWithName("Total Time")->GetComponent(Component::ComponentType::UI_TEXT));
	press_next = static_cast<ComponentText*>(owner->GetChildrenWithName("Press Next")->GetComponent(Component::ComponentType::UI_TEXT));

}

void Statistics::UpdatePanel()
{
	current_time += App->time->delta_time;

	if (current_time/timer_to_show < 0.33f) 
	{
		float aux = min(1, float(3 * float(current_time/timer_to_show)));
		player1_kills = int(aux * player_1->player_controller->GetEnemiesKilled());
	}
	else if(current_time/timer_to_show >= 0.33f && current_time/timer_to_show < 0.67f)
	{
		player1_kills = player_1->player_controller->GetEnemiesKilled();
		float aux = min(1, float(3 * ((current_time - 0.33f*timer_to_show)/ timer_to_show)));
		player1_orbs = aux * player_1->player_controller->GetOrbsRecollected();
	}
	else if (current_time/timer_to_show >= 0.67f)
	{
		player1_kills = player_1->player_controller->GetEnemiesKilled();
		player1_orbs = player_1->player_controller->GetOrbsRecollected();
		float aux = min(1, float(3 * ((current_time - 0.67f*timer_to_show) / timer_to_show)));
		player1_damage = aux * player_1->player_controller->GetDamageReceived();
	}

	enemies_killed_player1->SetText(std::to_string(player1_kills));
	total_damage_player1->SetText(std::to_string(static_cast<int>(player1_damage)));
	orbs_statistics_player1->SetText(std::to_string(player1_orbs) + "/" + 
		std::to_string(light_orbs_manager->GetTotalOrbsSpawned()));

	if (world_manager->multiplayer) 
	{
		if (current_time / timer_to_show < 0.33f)
		{
			float aux = min(1, float(3 * float(current_time / timer_to_show)));
			player2_kills = int(aux * player_2->player_controller->GetEnemiesKilled());
		}
		else if (current_time / timer_to_show >= 0.33f && current_time / timer_to_show < 0.67f)
		{
			player2_kills = player_2->player_controller->GetEnemiesKilled();
			float aux = min(1, float(3 * ((current_time - 0.33f*timer_to_show) / timer_to_show)));
			player2_orbs = aux * player_2->player_controller->GetOrbsRecollected();	
		}
		else if (current_time / timer_to_show >= 0.67f)
		{
			player2_kills = player_2->player_controller->GetEnemiesKilled();
			player2_orbs = player_2->player_controller->GetOrbsRecollected();
			float aux = min(1, float(3 * ((current_time - 0.67f*timer_to_show) / timer_to_show)));
			player2_damage = aux * player_2->player_controller->GetDamageReceived();
		}

		enemies_killed_player2->SetText(std::to_string(player2_kills));
		total_damage_player2->SetText(std::to_string(static_cast<int>(player2_damage)));
		orbs_statistics_player2->SetText(std::to_string(player2_orbs) + "/" +
			std::to_string(light_orbs_manager->GetTotalOrbsSpawned()));
	}
	else 
	{
		enemies_killed_player2->owner->SetEnabled(false);
		total_damage_player2->owner->SetEnabled(false);
		orbs_statistics_player2->owner->SetEnabled(false);
		player_2_stats->owner->SetEnabled(false);
	}

	press_next->owner->SetEnabled(false);


	
}

void Statistics::DisablePanel() const
{
	for (auto& go : owner->children) 
	{
		go->SetEnabled(false);
	}
}


