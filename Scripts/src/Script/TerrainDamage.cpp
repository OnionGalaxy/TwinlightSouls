#include "TerrainDamage.h"

#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EnemyController.h"
#include "EventManager.h"
#include "PlayerController.h"
#include "PlayerMovement.h"
#include "WorldManager.h"

#include <imgui.h>


namespace
{
	const float distance_difference = 0.3f;
}


TerrainDamage* TerrainDamageDLL()
{
	TerrainDamage* instance = new TerrainDamage();
	return instance;

}


TerrainDamage::TerrainDamage()
{

}


// Use this for initialization before Start()
void TerrainDamage::Awake()
{
	GameObject* world_manager_go = App->scene->GetGameObjectByName("World Manager");
	world = static_cast<WorldManager*>(world_manager_go->GetComponentScript("WorldManager")->script);
	event_manager = static_cast<EventManager*>(App->scene->GetGameObjectByName("EventManager")->GetComponentScript("EventManager")->script);
	GameObject* go_spawn = owner->GetChildrenWithName("Spawn");
	if (go_spawn != nullptr)
	{
		spawn_position = go_spawn->transform.GetGlobalTranslation();
	}
}


// Use this for initialization
void TerrainDamage::Start()
{
	player_1 = world->GetPlayer1();
	player_2 = world->GetPlayer2();
}



// Update is called once per frame
void TerrainDamage::Update()
{
	if (world->on_pause)
	{
		return;
	}

	bool player1_in_range = IsPlayerInRange(player_1->player_go);
	bool player1_collides = player_1->player_controller->is_alive && IsPlayerColliding(player_1->player_go) && player1_in_range;

	if (player1_collides)
	{
		ComputeDamage(0);
	}

	if(terrain_type == TerrainType::SLOWING_WATER)
	{
		SlowPlayer(0, player1_collides, player1_in_range);
	}


	if(world->multiplayer)
	{
		bool player2_in_range = IsPlayerInRange(player_2->player_go);
		bool player2_collides = player_2->player_controller->is_alive && IsPlayerColliding(player_2->player_go) && player2_in_range;

		if (player2_collides)
		{
			ComputeDamage(1);
		}

		if (terrain_type == TerrainType::SLOWING_WATER)
		{
			SlowPlayer(1, player2_collides, player2_in_range);
		}
	}
}

// Use this for showing variables on inspector
void TerrainDamage::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

	if (ImGui::BeginCombo("Terrain Type", terrain_name.c_str()))
	{
		
		if (ImGui::Selectable("Water"))
		{
			terrain_type = TerrainType::CORRUPTED_WATER;
			terrain_name = "Water";
		}

		if (ImGui::Selectable("Poison"))
		{
			terrain_type = TerrainType::CORRUPTED_POISON;
			terrain_name = "Poison";
		}

		if (ImGui::Selectable("Magma"))
		{
			terrain_type = TerrainType::MAGMA;
			terrain_name = "Magma";
		}

		if (ImGui::Selectable("Falling"))
		{
			terrain_type = TerrainType::FALLING;
			terrain_name = "Falling";
		}

		if (ImGui::Selectable("Slowing Water"))
		{
			terrain_type = TerrainType::SLOWING_WATER;
			terrain_name = "Slowing Water";
		}
		
		ImGui::EndCombo();
	}
	ImGui::DragInt("Zone", &zone);

	ShowDraggedObjects();
}


//Use this for linking JUST GO automatically 
void TerrainDamage::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}

}

void TerrainDamage::Save(Config & config) const
{
	config.AddInt((int) terrain_type, "TerrainType");
	config.AddInt(zone, "TerrainZone");
	Script::Save(config);
}

void TerrainDamage::Load(const Config & config)
{
	terrain_type = static_cast<TerrainType>(config.GetInt("TerrainType", 0));

	if(terrain_type == TerrainType::CORRUPTED_WATER)
	{
		terrain_name = "Water";
	}
	else if(terrain_type == TerrainType::MAGMA)
	{
		terrain_name = "Magma";
	}
	else if (terrain_type == TerrainType::FALLING)
	{
		terrain_name = "Falling";
	}
	else if (terrain_type == TerrainType::SLOWING_WATER)
	{
		terrain_name = "Slowing Water";
	}

	zone = config.GetInt("TerrainZone", 0);
	Script::Load(config);
}

void TerrainDamage::ComputeDamage(unsigned player)
{
	float damage_dealt = 0.f;
	
	float time_multiplier = App->time->delta_time / 1000.f;

	switch (terrain_type)
	{
		case TerrainType::CORRUPTED_WATER:
			damage_dealt = water_damage;
			break;

		case TerrainType::CORRUPTED_POISON:
			damage_dealt = poison_damage * time_multiplier;
			break;

		case TerrainType::MAGMA:
			damage_dealt = magma_damage * time_multiplier;
			break;

		case TerrainType::SLOWING_WATER:
			return;

		case TerrainType::FALLING:
			if (world->multiplayer)
			{
				if (player == 0)
				{
					player_1->player_controller->has_fallen = true;
					player_1->player_controller->falling_spawn_point = spawn_position;
				}
				else
				{
					player_2->player_controller->has_fallen = true;
					player_2->player_controller->falling_spawn_point = spawn_position;
				}
			}
			damage_dealt = water_damage;
			break;

		default:
			break;
	}

	(player == 0) ? player_1->player_controller->TakeDamage(damage_dealt, player_1->player_controller->hit_effect) : player_2->player_controller->TakeDamage(damage_dealt, player_2->player_controller->hit_effect);

}

void TerrainDamage::SlowPlayer(unsigned player, bool colliding, bool in_range)
{
	if(!in_range)
	{
		return;
	}

	float slow;
	(colliding) ? slow = 80.f : slow = 0.f;
	if(player == 0)
	{
		player_1->player_controller->player_movement->SlowPlayer(slow);
	}
	else
	{
		player_2->player_controller->player_movement->SlowPlayer(slow);
	}
}

bool TerrainDamage::IsPlayerColliding(GameObject* player) const
{
	return owner->transform.GetGlobalTranslation().y - player->transform.GetGlobalTranslation().y > distance_difference;
}

bool TerrainDamage::IsPlayerInRange(GameObject* player) const
{
	float3 position = player->transform.GetGlobalTranslation();

	return owner->aabb.global_bounding_box.maxPoint.x > position.x &&
		owner->aabb.global_bounding_box.maxPoint.z > position.z &&
		owner->aabb.global_bounding_box.minPoint.x < position.x &&
		owner->aabb.global_bounding_box.minPoint.z < position.z;
}
