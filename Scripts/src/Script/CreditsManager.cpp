#include "CreditsManager.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Log/EngineLog.h"
#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "EventManager.h"
#include "UIImageDissolver.h"
#include "UIManager.h"


CreditsManager* CreditsManagerDLL()
{
	CreditsManager* instance = new CreditsManager();
	return instance;
}


CreditsManager::CreditsManager()
{

}


// Use this for initialization before Start()
void CreditsManager::Awake()
{
	GameObject* ui_manager_go = App->scene->GetGameObjectByName("UIManager");
	if(ui_manager_go)
	{
		ui_manager = static_cast<UIManager*>(ui_manager_go->GetComponentScript("UIManager")->script);
		event_manager = static_cast<EventManager*>(App->scene->GetGameObjectByName("EventManager")->GetComponentScript("EventManager")->script);
	}

	GameObject* credits_go = App->scene->GetGameObjectByName("Credits Menu");
	for(size_t i = 0; i < credits_go->children.size(); ++i)
	{
		UIImageDissolver* dissolver = static_cast<UIImageDissolver*>(credits_go->children[i]->GetComponentScript("UIImageDissolver")->script);
		dissolver->SetTotalBlendingTime(800.f);
		teams.emplace_back(dissolver);

		if(i == 0)
		{
			dissolver->SetTotalBlendingTime(1300.f);
		}
	}
}


// Use this for initialization
void CreditsManager::Start()
{

}


// Update is called once per frame
void CreditsManager::Update()
{
	if(!are_credits_active)
	{
		return;
	}

	if(next_team && current_time_on_screen >= next_team_time)
	{
		teams[current_index]->Enable();
		current_time_on_screen = 0.f;
		next_team = false;
	}

	if(App->input->GetKeyDown(KeyCode::F))
	{
		pay_respect = true;
	}

	if(current_time_on_screen >= total_time_on_screen)
	{
		if(current_index >= teams.size() - 2 + pay_respect)
		{
			teams[current_index]->Disable();

			EndCredits();

			return;
		}

		if(current_index == 0)
		{
			total_time_on_screen = total_time_teams_on_screen;
		}

		teams[current_index]->Disable();
		++current_index;
		next_team = true;
		current_time_on_screen = 0.f;
	}
	
	current_time_on_screen += App->time->delta_time;
}


// Use this for showing variables on inspector
void CreditsManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
}


//Use this for linking JUST GO automatically 
void CreditsManager::InitPublicGameObjects()
{
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

void CreditsManager::Enable()
{
	if(ui_manager != nullptr)
	{
		ui_manager->EnableHUD(false, true);
		ui_manager->DisableComboCounter();
		ui_manager->HideSkipButton();
		ui_manager->ActiveCredits();	
	}
	are_credits_active = true;
	teams[current_index]->Enable();
	current_time_on_screen = 0.f;
}

void CreditsManager::Disable()
{
	are_credits_active = false;
}

void CreditsManager::EndCredits()
{
	Disable();

	if(event_manager != nullptr)
	{
		event_manager->last_cinematic_finish = true;
	}
}

void CreditsManager::FastHide()
{
	for(auto& team : teams)
	{
		team->FastHide();
	}
	are_credits_active = false;
	current_index = 0;
}

bool CreditsManager::AreCreditsActive() const
{
	return are_credits_active;
}
