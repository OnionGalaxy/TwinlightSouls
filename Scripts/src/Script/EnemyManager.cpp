#include "EnemyManager.h"

#include "Component/ComponentBillboard.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Log/EngineLog.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleScriptManager.h"


#include "imgui.h"
#include <stdlib.h> 
#include <time.h>

#include "BattleEvent.h"
#include "Biter.h"
#include "EnemyController.h"
#include "EventManager.h"
#include "Mushdoom.h"
#include "Necroplant.h"
#include "MushdoomBall.h"
#include "WorldManager.h"

EnemyManager* EnemyManagerDLL()
{
	EnemyManager* instance = new EnemyManager();
	return instance;
}

EnemyManager::EnemyManager()
{
	
}

EnemyManager::~EnemyManager()
{
	for (unsigned int i = 0; i < enemies.size(); ++i)
	{
		delete enemies[i];
	}

}

// Use this for initialization before Start()
void EnemyManager::Awake()
{
	event_manager = static_cast<EventManager*>(App->scene->GetGameObjectByName("EventManager")->GetComponentScript("EventManager")->script);

	GameObject* world_manager_go = App->scene->GetGameObjectByName("World Manager");
	ComponentScript* world_manager_component = world_manager_go->GetComponentScript("WorldManager");
	world_manager = static_cast<WorldManager*>(world_manager_component->script);

}

// Use this for initialization
void EnemyManager::Start()
{
	current_attackers_for_player.push_back(std::pair<unsigned int, std::vector<EnemyController*>>(0, {}));
	current_attackers_for_player.push_back(std::pair<unsigned int, std::vector<EnemyController*>>(1, {}));
	current_shooters_for_player.push_back(std::pair<unsigned int, std::vector<EnemyController*>>(0, {}));
	current_shooters_for_player.push_back(std::pair<unsigned int, std::vector<EnemyController*>>(1, {}));
	current_seekers_for_player.push_back(std::pair<unsigned int, std::vector<EnemyController*>>(0, {}));
	current_seekers_for_player.push_back(std::pair<unsigned int, std::vector<EnemyController*>>(1, {}));
	InitEnemies();
	SpawnInitialEnemies();
}

// Update is called once per frame
void EnemyManager::Update()
{
	if (world_manager->on_pause)
	{
		return;
	}

	for (auto it = enemies_spawning_queue.begin(); it != enemies_spawning_queue.end(); ++it)
	{
		float3 real_spawn_position(it->first.x, it->first.y, it->first.z);

		if (CheckSpawnAvailability(real_spawn_position) || it->second == EnemyType::NECROPLANT)
		{
			SpawnEnemy(it->second, real_spawn_position, true);
			enemies_spawning_queue.erase(it);
			break;
		}
	}
	
}

void EnemyManager::AddEnemy(EnemyController* enemy)
{
	enemies.emplace_back(enemy);
}

void EnemyManager::KillEnemy(EnemyController* enemy)
{
	//This method is called once the enemy animation ended
	enemy->Kill();
	enemy->owner->transform.SetGlobalMatrixTranslation(graveyard_position);

	++total_enemies_killed;

	if (event_manager->battle_event_triggered && enemy->is_wave_enemy)
	{
		++event_manager->enemies_killed_on_wave;
	}
}

void EnemyManager::SpawnEnemy(EnemyType type, const float3& spawn_position, bool is_wave_enemy, bool invincible)
{
	for (size_t i = 0; i < enemies.size(); ++i)
	{
		if (!enemies[i]->is_alive && enemies[i]->enemy_type == type)
		{
			EnemyController* enemy = nullptr;
			enemy = enemies[i];

			assert(enemy != nullptr);

			if (enemy->collider)
			{
				enemy->collider->UpdateDimensions();
			}
			if (type == EnemyType::NECROPLANT) 
			{
				Necroplant* spawned_necroplant = static_cast<Necroplant*>(enemy);
				spawned_necroplant->is_static = !is_wave_enemy;
			}
			enemy->owner->transform.SetGlobalMatrixTranslation(spawn_position);
			enemy->ResetEnemy();
			enemy->owner->transform.SetGlobalMatrixTranslation(spawn_position);
			
			switch (type)
			{
				case EnemyType::MUSHDOOM:
				{
					Mushdoom* spawned_mushdoom = static_cast<Mushdoom*>(enemy);
					if (!is_wave_enemy)
					{
						spawned_mushdoom->detect_player_distance = 10.f;
					}
					break;
				}
				case EnemyType::BITER:
				{
					Biter* spawned_biter = static_cast<Biter*>(enemy);
					if (!is_wave_enemy)
					{
						spawned_biter->current_position = spawn_position;
					}
					break;
				}
				//Maybe Biter also needs the same code
			}

			enemy->invincible = invincible;
			enemy->is_wave_enemy = is_wave_enemy;
				
			break;
		}
	}
}

void EnemyManager::SpawnWave(std::vector<float3>& spawns, unsigned enemies_per_wave, EnemyType enemy_type)
{
	float multiplier = world_manager->GetEnemyAmountMultiplier();
	for (size_t i = 0; i < enemies_per_wave; ++i)
	{
		int random_spawn = rand() % spawns.size();
		enemies_spawning_queue.emplace_back(std::make_pair(spawns[random_spawn], enemy_type));
	}
}

void EnemyManager::InitEnemies()
{
	GameObject* mushdooms = App->scene->GetGameObjectByName("Mushdooms");
	if (mushdooms != nullptr)
	{
		for (int i = 0; i < mushdooms->children.size(); ++i)
		{
			Mushdoom* mushdoom = static_cast<Mushdoom*>(mushdooms->children[i]->GetComponentScript("Mushdoom")->script);

			mushdoom->collider->active_physics = false;
			mushdoom->owner->SetEnabled(false);
			mushdoom->owner->transform.SetGlobalMatrixTranslation(graveyard_position);
			
			enemies.emplace_back(mushdoom);
		}
	}

	GameObject* biters = App->scene->GetGameObjectByName("Biters");
	if (biters != nullptr)
	{
		for (int i = 0; i < biters->children.size(); ++i)
		{
			Biter* biter = static_cast<Biter*>(biters->children[i]->GetComponentScript("Biter")->script);

			biter->collider->active_physics = false;
			biter->owner->SetEnabled(false);
			biter->owner->transform.SetGlobalMatrixTranslation(graveyard_position);

			enemies.emplace_back(biter);
		}
	}

	GameObject* necroplants = App->scene->GetGameObjectByName("Necroplants");
	if (necroplants != nullptr)
	{
		for (int i = 0; i < necroplants->children.size(); ++i)
		{
			Necroplant* necroplant = static_cast<Necroplant*>(necroplants->children[i]->GetComponentScript("Necroplant")->script);
			
			necroplant->owner->SetEnabled(false);
			necroplant->owner->transform.SetGlobalMatrixTranslation(graveyard_position);
		
			enemies.emplace_back(necroplant);
		}
	}
}

// Use this for showing variables on inspector
void EnemyManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Text("enemies_spawning_queue size:  %d", enemies_spawning_queue.size());
	ImGui::Text("total_enemies_killed: %d", total_enemies_killed);
	ImGui::Text("enemies: %d", enemies.size());
	ImGui::DragInt("MAX_DOUBLE_Z_RANGE:", &max_double_z_spawn);

	ImGui::Separator();
	for (const auto& enemy : enemies)
	{
		std::string enemy_type;
		switch (enemy->enemy_type)
		{
		case EnemyType::MUSHDOOM:
			enemy_type = "Mushdoom";
			break;
		case EnemyType::BITER:
			enemy_type = "Biter";
			break;
		case EnemyType::NECROPLANT:
			enemy_type = "Necroplant";
			break;
		default:
			break;
		}
		ImGui::Text("%s | Is Alive: %d", enemy_type.c_str(), enemy->is_alive);
	}

	int aux = current_ranged_mushdoom_attackers;
	ImGui::DragInt("CURRENT RANGED MUSHDOOMS", &aux);

	aux = current_melee_mushdoom_attackers;
	ImGui::DragInt("CURRENT MELEE MUSHDOOMS", &aux);


	for (size_t i = 0; i < initial_enemy_spawns.size(); ++i)
	{
		ImGui::PushID(i);
		ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Initial Enemy Spawn %u", i);
		ImGui::SameLine();
		if (ImGui::Button("-"))
		{
			initial_enemy_spawns.erase(initial_enemy_spawns.begin() + i);
			break;
		}

		ImGui::DragFloat3("Spawn Position", initial_enemy_spawns[i].position.ptr(), 0.1f);

		int enemy_type_int = static_cast<int>(initial_enemy_spawns[i].enemy_type);
		if (ImGui::Combo("Enemy Type", &enemy_type_int, "Mushdoom\0Biter\0Necroplant"))
		{
			switch (enemy_type_int)
			{
			case 0:
				initial_enemy_spawns[i].enemy_type = EnemyType::MUSHDOOM;
				break;
			case 1:
				initial_enemy_spawns[i].enemy_type = EnemyType::BITER;
				break;
			case 2:
				initial_enemy_spawns[i].enemy_type = EnemyType::NECROPLANT;
				break;
			default:
				break;
			}
		}
		ImGui::PopID();
	}

	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Add Initial Enemy Spawn"))
	{
		initial_enemy_spawns.push_back(InitialEnemySpawn());
	}

	ImGui::Separator();
	ImGui::Text("New Mushdoom");
	ImGui::Spacing();
	ImGui::DragFloat("Attack distance", &attack_distance, 0.1f, 1.0f, 2.5f);
	ImGui::DragFloat("Shoot distance", &shoot_distance, 0.1f, 2.5f, 4.0f);
	ImGui::DragFloat("Seek distance", &seek_distance, 0.1f, 4.0f, 5.5f);
	ImGui::DragFloat("Spawn distance", &spawn_distance, 0.1f, 5.5f, 8.0f);
	if (ImGui::DragFloat("Angle offset", &angle, 0.1f, 1.0f, 45.0f))
	{
		angle_rad = DegToRad(angle);
	}

	ImGui::DragInt("Max Attackers", &max_attackers, 1.0f, 2);
	ImGui::DragInt("Max Shooters", &max_shooters, 1.0f, 2);
}

//Use this for linking JUST GO automatically 
void EnemyManager::InitPublicGameObjects()
{
	public_gameobjects.push_back(&mushdoom_go);
	variable_names.push_back(GET_VARIABLE_NAME(mushdoom_go));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

bool EnemyManager::ArePlayersInDanger() const
{
	for (size_t i = 0; i < enemies.size(); i++)
	{
		if (enemies[i]->is_alive && enemies[i]->PlayerInSight())
		{
			return true;
		}
	}
	return false;
}

void EnemyManager::CancelMeleeAttack(EnemyController* enemy)
{
	--current_melee_mushdoom_attackers;
}

void EnemyManager::CancelRangedAttack(EnemyController* enemy)
{
	--current_ranged_mushdoom_attackers;
}

bool EnemyManager::CheckSpawnAvailability(float3& spawn_position)
{
	for (const auto& enemy : enemies)
	{
		if (!enemy->is_alive)
		{
			continue;
		}

		if (enemy->owner->transform.GetGlobalTranslation().Distance(spawn_position) <= 6.f)
		{
			return false;
		}
	}

	return true;
}

void EnemyManager::KillAllTheEnemies()
{
	for(auto& enemy : enemies)
	{
		if(enemy->is_alive)
		{
			enemy->invincible = false;
			KillEnemy(enemy);
		}
	}
}

void EnemyManager::SpawnInitialEnemies()
{
	for (auto& initial_enemy_spawn : initial_enemy_spawns)
	{
		SpawnEnemy(initial_enemy_spawn.enemy_type, initial_enemy_spawn.position, false);
	}
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
void EnemyManager::Save(Config& config) const
{
	size_t num_initial_enemy_spawns = initial_enemy_spawns.size();
	config.AddUInt(num_initial_enemy_spawns, "Number of Initial Enemy Spawns");

	std::vector<float3> positions(num_initial_enemy_spawns);
	std::vector<int> enemy_types(num_initial_enemy_spawns);
	for (size_t i = 0; i < num_initial_enemy_spawns; ++i)
	{
		positions[i] = initial_enemy_spawns[i].position;
		enemy_types[i] = static_cast<int>(initial_enemy_spawns[i].enemy_type);
	}

	config.AddVector(positions, "Initial Enemy Spawns positions");
	config.AddVector(enemy_types, "Initial Enemy Spawns types");

	config.AddFloat(attack_distance, "Attack distance");
	config.AddFloat(shoot_distance, "Shoot distance");
	config.AddFloat(seek_distance, "Seek distance");
	config.AddFloat(spawn_distance, "Spawn distance");
	config.AddFloat(angle, "Angle offset");
	config.AddInt(max_attackers, "Max Attackers");
	config.AddInt(max_shooters, "Max Shooters");

	Script::Save(config);
}

//Use this for linking GO AND VARIABLES automatically
void EnemyManager::Load(const Config& config)
{
	size_t num_initial_enemy_spawns = config.GetUInt("Number of Initial Enemy Spawns", 0);
	initial_enemy_spawns = std::vector<InitialEnemySpawn>(num_initial_enemy_spawns);

	if (num_initial_enemy_spawns != 0)
	{
		std::vector<float3> positions(num_initial_enemy_spawns);
		std::vector<int> enemy_types(num_initial_enemy_spawns);
		config.GetVector("Initial Enemy Spawns positions", positions, std::vector<float3>());
		config.GetVector("Initial Enemy Spawns types", enemy_types, std::vector<int>());

		for (size_t i = 0; i < num_initial_enemy_spawns; ++i)
		{
			initial_enemy_spawns[i].position = positions[i];
			initial_enemy_spawns[i].enemy_type = static_cast<EnemyType>(enemy_types[i]);
		}
	}

	attack_distance = config.GetFloat("Attack distance", 2.0f);
	shoot_distance = config.GetFloat("Shoot distance", 3.5f);
	seek_distance = config.GetFloat("Seek distance", 5.0f);
	spawn_distance = config.GetFloat("Spawn distance", 8.0f);
	angle = config.GetFloat("Angle offset", 45.0f);
	angle_rad = DegToRad(angle);

	max_attackers = config.GetInt("Max Attackers", 2);
	max_shooters = config.GetInt("Max Shooters", 2);

	Script::Load(config);
}

int EnemyManager::GetCurrentSlot(unsigned int player, std::vector<std::pair<unsigned int, int>> &slots)
{
	for (auto it = slots.begin(); it != slots.end(); ++it)
	{
		if (it->first == player)
		{
			return it->second;
		}
	}
}

int EnemyManager::UpdateCurrentSlot(unsigned int player, std::vector<std::pair<unsigned int, int>> &slots, int value)
{
	for (auto it = slots.begin(); it != slots.end(); ++it)
	{
		if (it->first == player)
		{
			it->second += value;
			return it->second;
		}
	}
}

int EnemyManager::CreateSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots, EnemyController* new_enemy)
{
	for (auto it = slots.begin(); it != slots.end(); ++it)
	{
		if (it->first == player)
		{
			it->second.push_back(new_enemy);
			return it->second.size();
		}
	}
}

int EnemyManager::GetCurrentSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots)
{
	for (auto it = slots.begin(); it != slots.end(); ++it)
	{
		if (it->first == player)
		{
			return it->second.size();
		}
	}
}

void EnemyManager::RemoveSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots, EnemyController* enemy_to_remove)
{
	for (auto it = slots.begin(); it != slots.end(); ++it)
	{
		if (it->first == player)
		{
			auto enemny_it = std::find(it->second.begin(), it->second.end(), enemy_to_remove);
			if (enemny_it != it->second.end())
			{
				it->second.erase(enemny_it);
			}
		}
	}
}

std::vector<EnemyController*>* EnemyManager::GetEnemiesOnSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots)
{
	for (auto it = slots.begin(); it != slots.end(); ++it)
	{
		if (it->first == player)
		{
			return &it->second;
		}
	}
}

void EnemyManager::ClearAllSlots(unsigned int player)
{
	ClearSlot(player, current_attackers_for_player);
	ClearSlot(player, current_shooters_for_player);
	ClearSlot(player, current_seekers_for_player);
}

void EnemyManager::ClearSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots)
{
	for (auto it = slots.begin(); it != slots.end(); ++it)
	{
		if (it->first == player)
		{
			it->second.clear();
		}
	}
}
