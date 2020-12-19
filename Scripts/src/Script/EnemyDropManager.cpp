#include "EnemyDropManager.h"

#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "LightOrb.h"
#include "PowerUp.h"

#include "imgui.h"

EnemyDropManager* EnemyDropManagerDLL()
{
	EnemyDropManager* instance = new EnemyDropManager();
	return instance;
}

EnemyDropManager::EnemyDropManager()
{
	
}

// Use this for initialization before Start()
void EnemyDropManager::Awake()
{
	std::vector<ComponentMeshRenderer*> mesh_collider = GetMeshCollider();

	light_orb_game_object = owner->children[0];
	LightOrb* light_orb = static_cast<LightOrb*>(light_orb_game_object->GetComponentScript("LightOrb")->script);
	light_orb->SetMeshEnvironment(mesh_collider);

	for (unsigned int i = 0; i < light_orbs.size(); ++i)
	{
		GameObject* light_orb_clone = App->scene->DuplicateGameObject(light_orb_game_object, owner);
		light_orbs[i] = static_cast<LightOrb*>(light_orb_clone->GetComponentScript("LightOrb")->script);
		light_orbs[i]->CloneProperties(light_orb);
		light_orb_clone->children[0]->SetEnabled(false);
	}
	
	attack_powerup_game_object = owner->children[1];
	PowerUp* powerup = static_cast<PowerUp*>(attack_powerup_game_object->GetComponentScript("PowerUp")->script);
	powerup->SetMeshEnvironment(mesh_collider);
	for (unsigned int i = 0; i < powerups.size(); ++i)
	{
		GameObject* attack_powerup_clone = App->scene->DuplicateGameObject(attack_powerup_game_object, owner);
		powerups[i] = static_cast<PowerUp*>(attack_powerup_clone->GetComponentScript("PowerUp")->script);
		powerups[i]->CloneProperties(powerup);
		attack_powerup_clone->children[0]->SetEnabled(false);
	}

	deaths_last_powerup = 0;
}

// Use this for initialization
void EnemyDropManager::Start()
{

}

// Update is called once per frame
void EnemyDropManager::Update()
{

}

void EnemyDropManager::SpawnOrbs(const float3& enemy_position, int number_orbs)
{
	if (HasToSpawnPowerUp())
	{
		for (int i = 0; i < powerups.size(); i++)
		{
			if (!powerups[i]->IsAlive())
			{
				AssignPowerUpType(powerups[i]);
				powerups[i]->Spawn(enemy_position);
				deaths_last_powerup = 0;
				break;
			}
		}
	}

	deaths_last_powerup++;
	unsigned int current_spawned = 0;

	for (int i = 0; i < light_orbs.size(); i++)
	{
		if (!light_orbs[i]->IsAlive())
		{
			light_orbs[i]->Spawn(enemy_position);
			++current_spawned;
			++total_orbs_spawned;

			if (current_spawned == number_orbs)
			{
				return;
			}
		}
	}
}

void EnemyDropManager::SpawnPowerup(const float3& position)
{
	for (int i = 0; i < powerups.size(); i++)
	{
		if (!powerups[i]->IsAlive())
		{
			AssignPowerUpType(powerups[i]);
			powerups[i]->Spawn(position);
			return;
		}
	}
}

bool EnemyDropManager::HasToSpawnPowerUp() const
{
	/*
		To tune the function for spawning the function, go to https://www.desmos.com/calculator	and introduce
		the function 1 / (1 + e ^ (-ax + b) ), then change the values of a and b to match the desired shape.
		The value of x is the number of defeated enemies without dropping a powerup.

		This function (sigmoid function) is made so that the chances are never 100% nor 0%, but are very low
		when a powerup has just spawned, suddently increase after a few kills, and then are very high
		until a powerup spawns.
	*/

	float powerup_chance = 1 / (1 + math::Pow(math::e, -1.2 * deaths_last_powerup + 6));
	float random = (float)(rand() % 1000) / 1000;

	return powerup_chance >= random;
}

void EnemyDropManager::AssignPowerUpType(PowerUp * powerup) const
{
	float total_chances = powerup->attack_powerup_chance + powerup->invulnerable_powerup_chance;
	float random = (float)(rand() % 1001) / 1000 * total_chances;

	//float cumulative = 0.f;
	if (random < powerup->attack_powerup_chance)
	{
		powerup->powerup_type = PowerUp::PowerUpType::ATTACK;
		return;
	}

	//cumulative += powerup->attack_powerup_chance;
	powerup->powerup_type = PowerUp::PowerUpType::INVULNERABLE;
}

// Use this for showing variables on inspector
void EnemyDropManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	float chance = 1 / (1 + math::Pow(math::e, -1.2 * deaths_last_powerup + 6));
	ImGui::DragFloat("Powerup chance", &chance);
	ImGui::DragInt("Kills since last", &deaths_last_powerup);
}

std::vector<ComponentMeshRenderer*>  EnemyDropManager::GetMeshCollider()
{
	std::vector<GameObject*> meshes = App->scene->GetGameObjectsWithTag("EnvironmentCollider");
	std::vector<ComponentMeshRenderer*> mesh_collider;

	for (auto& mesh : meshes)
	{
		mesh_collider.emplace_back(static_cast<ComponentMeshRenderer*>(mesh->GetComponent(Component::ComponentType::MESH_RENDERER)));
	}

	return mesh_collider;
}