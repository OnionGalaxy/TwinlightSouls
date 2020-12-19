#include "WorldManager.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentImage.h"

#include "Filesystem/PathAtlas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"


#include <imgui.h>

#include "BossController.h"
#include "CameraController.h"
#include "EventManager.h"
#include "InputManager.h"
#include "LevelSelectionMenuController.h"
#include "PlayerController.h"
#include "UIMainMenuInputController.h"
#include "UIManager.h"


bool WorldManager::multiplayer;
Level WorldManager::current_level;
bool WorldManager::player1_choice;
bool WorldManager::level_selection_activated;
bool WorldManager::is_game_finished;
bool WorldManager::show_boot_screens = true;
int WorldManager::difficulty;

WorldManager* WorldManagerDLL()
{
	WorldManager* instance = new WorldManager();
	return instance;
}

WorldManager::WorldManager()
{
	
}

WorldManager::~WorldManager()
{
	free(player_1);
	free(player_2);
	free(boss);
}

// Use this for initialization before Start()
void WorldManager::Awake()
{
	if(on_main_menu)
	{
		return;
	}
	GameObject* ui_manager_go = App->scene->GetGameObjectByName("UIManager");
	ComponentScript* ui_manager_component = ui_manager_go->GetComponentScript("UIManager");
	ui_manager = static_cast<UIManager*>(ui_manager_component->script);
	player1_HUD = ui_manager->GetPlayerHUD(1);
	player2_HUD = ui_manager->GetPlayerHUD(2);
	InitPlayers();
	InitializeHUD(true);
	GameObject* input_manager_go = App->scene->GetGameObjectByName("Input Manager");
	ComponentScript* input_manager_component = input_manager_go->GetComponentScript("InputManager");
	input_manager = static_cast<InputManager*>(input_manager_component->script);

	GameObject* boss_go = App->scene->GetGameObjectByName("Boss");
	if(boss_go != nullptr)
	{
		is_boss = true;
		BossController* boss_controller = static_cast<BossController*>(boss_go->GetComponentScript("BossController")->script);
		InitBoss(boss_go, boss_controller);
	}

	
}

// Use this for initialization
void WorldManager::Start()
{
}

// Update is called once per frame
void WorldManager::Update()
{
	if (!timer_started)
	{
		StartTimer();
		timer_started = true;
	}
	//Check if game is pause
	if(!on_main_menu && UIMainMenuInputController::StartButtonPressed(input_manager))
	{
		if(on_pause)
		{
			on_pause = false;
			App->time->time_scale = 1.f;
			ui_manager->SetPauseScreen(false);
		}
		else
		{
			on_pause = true;
			App->time->time_scale = 0.f;
			ui_manager->SetPauseScreen(true);
		}
	}

#if MULTITHREADING
	if(!on_pause && App->resources->loading_thread_communication.loading)
	{
		loading = true;
		on_pause = true;
	}

	if(loading && !App->resources->loading_thread_communication.loading)
	{
		on_pause = false;
	}
#endif
}

// Use this for showing variables on inspector
void WorldManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Checkbox("Multiplayer", &multiplayer);
	ImGui::Checkbox("Player 1 female", &player1_choice);
	ImGui::Checkbox("Main menu", &on_main_menu);
	ImGui::Checkbox("On Pause", &on_pause);
	ImGui::Checkbox("Boss", &is_boss);

	ImGui::Separator();
	
	ImGui::DragFloat("Enemy multiplier easy", &enemy_amount_multiplier_easy, 0.01f, 0.f, 1.f);
	ImGui::DragFloat("Enemy multiplier hard", &enemy_amount_multiplier_hard, 0.01f, 0.f, 1.f);

	ImGui::DragFloat("Enemy damage multiplier easy", &enemy_damage_multiplier_easy, 0.01f, 0.f, 1.f);
	ImGui::DragFloat("Enemy damage multiplier hard", &enemy_damage_multiplier_hard, 0.01f, 0.f, 1.f);

	ImGui::DragFloat("Health multiplier easy", &health_multiplier_easy, 0.01f, 0.f, 1.f);
	ImGui::DragFloat("Health multiplier hard", &health_multiplier_hard, 0.01f, 0.f, 1.f);
}

void WorldManager::Save(Config& config) const
{
	config.AddBool(on_main_menu, "Main Menu");
}

void WorldManager::Load(const Config& config)
{
	on_main_menu = config.GetBool("Main Menu", false);
}

void WorldManager::LoadLevel(Level scene_position) const
{
	current_level = scene_position;
	App->scene->LoadScene(static_cast<unsigned>(scene_position));
}

void WorldManager::LoadNextLevel()
{
	if (current_level == Level::FIRST)
	{
		LoadLevel(Level::SECOND);
	}
	else if (current_level == Level::SECOND)
	{
		LoadLevel(Level::BOSS);
	}
	else if (current_level == Level::BOSS)
	{
		is_game_finished = true;
		LoadLevel(Level::MAIN_MENU);
	}
	else 
	{
		App->scene->LoadScene(0);
	}
}

float WorldManager::GetEnemyAmountMultiplier()
{
	return difficulty == 0 ? enemy_amount_multiplier_easy : difficulty == 1 ? 1.0 : enemy_amount_multiplier_hard;
}

float WorldManager::GetHealthMultiplier()
{
	return difficulty == 0 ? health_multiplier_easy : difficulty == 1 ? 1.0 : health_multiplier_hard;
}

float WorldManager::GetEnemyDamageMultiplier()
{
	return difficulty == 0 ? enemy_damage_multiplier_easy : difficulty == 1 ? 1.0 : enemy_damage_multiplier_hard;
}

void WorldManager::InitPlayers()
{
	////Male model
	GameObject* player1_go = App->scene->GetGameObjectByName("Player1");
	ComponentScript* player1_controller_component = (ComponentScript*)player1_go->GetComponentScript("PlayerController");
	PlayerController* player1_controller = static_cast<PlayerController*>(player1_controller_component->script);

	////Female model
	GameObject* player2_go = App->scene->GetGameObjectByName("Player2");
	ComponentScript* player2_controller_component = (ComponentScript*)player2_go->GetComponentScript("PlayerController");
	PlayerController* player2_controller = static_cast<PlayerController*>(player2_controller_component->script);

	//Logic of choosing character and single/multi player
//Singleplayer
	if (!multiplayer)
	{
		//If player1_choice == 0 he is chosing male model
		if (!player1_choice)
		{
			player1_controller->player = PlayerController::WhichPlayer::PLAYER1;
			player2_controller->player = PlayerController::WhichPlayer::PLAYER2;
			InitPlayer1(player1_go, player1_controller_component, player1_controller);
			DisablePlayer(player2_go, player2_controller);
			InitPlayer2(player2_go, player2_controller_component, player2_controller);
		}
		//Chosing female model
		else
		{
			player2_controller->player = PlayerController::WhichPlayer::PLAYER1;
			player1_controller->player = PlayerController::WhichPlayer::PLAYER2;
			InitPlayer1(player2_go, player2_controller_component, player2_controller);
			DisablePlayer(player1_go, player1_controller);
			InitPlayer2(player1_go, player1_controller_component, player1_controller);
		}
	}
	//Multiplayer
	else
	{
		//If player1_choice == 0 he is chosing male model
		if (!player1_choice)
		{
			player1_controller->player = PlayerController::WhichPlayer::PLAYER1;
			player2_controller->player = PlayerController::WhichPlayer::PLAYER2;
			InitPlayer1(player1_go, player1_controller_component, player1_controller);
			InitPlayer2(player2_go, player2_controller_component, player2_controller);
		}
		//Chosing female model
		else
		{
			player2_controller->player = PlayerController::WhichPlayer::PLAYER1;
			player1_controller->player = PlayerController::WhichPlayer::PLAYER2;
			InitPlayer1(player2_go, player2_controller_component, player2_controller);
			InitPlayer2(player1_go, player1_controller_component, player1_controller);
		}
	}

}
void WorldManager::InitializeHUD(bool enable)
{
	if (enable)
	{
		if (!multiplayer)
		{
			//If player1_choice == 0 he is chosing male model
			if (!player1_choice)
			{
				player1_HUD->SetEnabled(true);
				player2_HUD->SetEnabled(false);
				for (const auto go : player1_HUD->children)
				{
					for (const auto goUI : go->children)
					{
						if (goUI->name == "Player 1 Icon 2")
						{
							goUI->SetEnabled(false);
						}
						if (goUI->name == "Player 1 Icon")
						{
							goUI->SetEnabled(true);
						}
						if (goUI->name == "Lightball Ready Image Player 1")
						{
							goUI->SetEnabled(false);
						}
					}
				}
			}
			//Chosing female model
			else
			{
				player1_HUD->SetEnabled(true);
				player2_HUD->SetEnabled(false);
				for (const auto go : player1_HUD->children)
				{
					for (const auto goUI : go->children)
					{
						if (goUI->name == "Player 1 Icon 2")
						{
							goUI->SetEnabled(true);
						}
						if (goUI->name == "Player 1 Icon")
						{
							goUI->SetEnabled(false);
						}
						if (goUI->name == "Lightball Ready Image Player 1")
						{
							goUI->SetEnabled(false);
						}
					}
				}
			}
		}
		//Multiplayer
		else
		{
			//If player1_choice == 0 he is chosing male model
			if (!player1_choice)
			{
				player1_HUD->SetEnabled(true);
				player2_HUD->SetEnabled(true);
				for (const auto go : player1_HUD->children)
				{
					for (const auto goUI : go->children)
					{
						if (goUI->name == "Player 1 Icon 2")
						{
							goUI->SetEnabled(false);
						}
						if (goUI->name == "Player 1 Icon")
						{
							goUI->SetEnabled(true);
						}
						if (goUI->name == "Lightball Ready Image Player 1")
						{
							goUI->SetEnabled(false);
						}
					}
				}
				for (const auto go : player2_HUD->children)
				{
					for (const auto goUI : go->children)
					{
						if (goUI->name == "Player 2 Icon 2")
						{
							goUI->SetEnabled(true);
						}
						if (goUI->name == "Player 2 Icon")
						{
							goUI->SetEnabled(false);
						}
						if (goUI->name == "Lightball Ready Image Player 2")
						{
							goUI->SetEnabled(false);
						}
					}
				}
			}
			//Chosing female model
			else
			{
				player1_HUD->SetEnabled(true);
				player2_HUD->SetEnabled(true);
				for (const auto go : player1_HUD->children)
				{
					for (const auto goUI : go->children)
					{
						if (goUI->name == "Player 1 Icon 2")
						{
							goUI->SetEnabled(true);
						}
						if (goUI->name == "Player 1 Icon")
						{
							goUI->SetEnabled(false);
						}
						if (goUI->name == "Lightball Ready Image Player 1")
						{
							goUI->SetEnabled(false);
						}
					}
				}
				for (const auto go : player2_HUD->children)
				{
					for (const auto goUI : go->children)
					{
						if (goUI->name == "Player 2 Icon 2")
						{
							goUI->SetEnabled(false);
						}
						if (goUI->name == "Player 2 Icon")
						{
							goUI->SetEnabled(true);
						}
						if (goUI->name == "Lightball Ready Image Player 2")
						{
							goUI->SetEnabled(false);
						}
					}
				}
			}
		}
	}
}

void::WorldManager::DisableHUDReadyLightBall(int player)
{
	if (multiplayer)
	{
		if (player == 0)
		{
			for (const auto go : player1_HUD->children)
			{
				for (const auto goUI : go->children)
				{
					if (goUI->name == "Lightball Ready Image Player 1")
					{
						goUI->SetEnabled(false);
					}
				}
			}
		}
		else
		{
			for (const auto go : player2_HUD->children)
			{
				for (const auto goUI : go->children)
				{
					if (goUI->name == "Lightball Ready Image Player 2")
					{
						goUI->SetEnabled(false);
					}
				}
			}
		}
		
	}
}
void WorldManager::InitPlayer1(GameObject* player_go, ComponentScript* player_component, PlayerController* controller)
{
	player_1 = (Player*)malloc(sizeof(Player));
	player_1->player_go = player_go;
	player_1->player_component = player_component;
	player_1->player_controller = controller;
}

void WorldManager::InitPlayer2(GameObject* player_go, ComponentScript* player_component, PlayerController* controller)
{
	player_2 = (Player*)malloc(sizeof(Player));
	player_2->player_go = player_go;
	player_2->player_component = player_component;
	player_2->player_controller = controller;
}

void WorldManager::InitBoss(GameObject* boss_go, BossController* controller)
{
	boss = (Boss*)malloc(sizeof(Boss));
	boss->boss_go = boss_go;
	boss->boss_controller = controller;
}

Player* WorldManager::GetPlayer1() const
{
	return player_1;
}

Player* WorldManager::GetPlayer2() const
{
	return player_2;
}

UIManager * WorldManager::GetUIManager() const
{
	return ui_manager;
}

bool WorldManager::ThereIsBoss() const
{
	return is_boss;
}

Boss* WorldManager::GetBoss() const
{
	return boss;
}

void WorldManager::StartTimer()
{
	starting_time = App->time->time;
}

float WorldManager::GetTotalTime() const
{
	return App->time->time - starting_time;
}

void WorldManager::DisablePlayer(GameObject* player, PlayerController* player_controller)
{
	player_controller->is_alive = false;
	player->SetEnabled(false);
	player->transform.SetGlobalMatrixTranslation(float3(100.f, 100.f, 100.f));
}