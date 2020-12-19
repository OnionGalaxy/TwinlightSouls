#include "AvalancheZone.h"

#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "CameraShake.h"
#include "PlayerController.h"
#include "WorldManager.h"

#include "Log/EngineLog.h"

AvalancheZone* AvalancheZoneDLL()
{
	AvalancheZone* instance = new AvalancheZone();
	return instance;
}

AvalancheZone::AvalancheZone()
{

}

// Use this for initialization before Start()
void AvalancheZone::Awake()
{
	world = static_cast<WorldManager*>(App->scene->GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	camera_shake = static_cast<CameraShake*>(App->scene->GetGameObjectByName("Main Camera")->GetComponentScript("CameraShake")->script);

	mesh_collider = App->scene->GetGameObjectsWithTag("EnvironmentCollider");
	starting_point = owner->GetChildrenWithName("Start Point");
	end_point = owner->GetChildrenWithName("End Point");
	destruction_point = owner->GetChildrenWithName("Destruction Point");
	GetRocks();
}

// Use this for initialization
void AvalancheZone::Start()
{
	player_1 = world->GetPlayer1();
	player_2 = world->GetPlayer2();
	is_multiplayer = world->multiplayer;
}

// Update is called once per frame
void AvalancheZone::Update()
{
	if (world->on_pause || ended) 
	{
		return;
	}
	if (!started) 
	{
		CheckPlayersInTheZone();
		return;
	}
	if (activated) 
	{
		if (current_delay >= delay_between_rocks)
		{
			current_delay = 0.f;
			SpawnRock();
		}
		else
		{
			current_delay += App->time->delta_time;
		}
		CheckPlayersOutTheZone();
	}
	else 
	{
		CheckEndedRocksKillRocks();
	}

	UpdateRocks();

}

// Use this for showing variables on inspector
void AvalancheZone::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void AvalancheZone::CheckPlayersInTheZone()
{
	float3 line_direction = starting_point->transform.GetGlobalTranslation() - end_point->transform.GetGlobalTranslation();
	float3 current_position_direction = camera_shake->owner->parent->transform.GetGlobalTranslation() - end_point->transform.GetGlobalTranslation();
	float dot_product = line_direction.Dot(current_position_direction);

	if (dot_product > 0)
	{
		started = true;
		activated = true;
	}
}

void AvalancheZone::CheckPlayersOutTheZone()
{
	if (camera_shake->owner->parent->transform.GetGlobalTranslation().Distance(starting_point->transform.GetGlobalTranslation()) <= deactivation_distance) 
	{
		activated = false;
	}
}

void AvalancheZone::GetRocks()
{
	std::vector<GameObject*> rocks_manager = owner->GetChildrenWithName("Rocks")->children;

	for (auto& go : rocks_manager)
	{
		Rock* rock = new Rock(go);
		rock->rock_go->SetEnabled(false);
		rocks.emplace_back(rock);
	}
}

void AvalancheZone::UpdateRocks() const
{
	for (auto& rock : rocks)
	{
		switch (rock->status)
		{

		case RockStatus::SPAWNED:
			rock->rock_go->SetEnabled(true);
			rock->rock_go->transform.SetGlobalMatrixTranslation(starting_point->transform.GetGlobalTranslation());
			rock->remaining_time_up = 0.f;
			rock->total_time_up = 0.f;
			rock->rock_go->transform.SetRotation(float3::zero);
			rock->player_already_hitted = false;
			rock->status = RockStatus::GOING_DOWN;
			break;

		case RockStatus::GOING_DOWN:
			if (!IsRockInTheEnd(rock)) 
			{
				if (!ComputeDamageZone(rock))
				{
					float3 new_position = ComputeNextPosition(rock);
					rock->rock_go->transform.SetGlobalMatrixTranslation(new_position);
					rock->total_time_up = rock->remaining_time_up += App->time->delta_time;
					float3 rotation = rock->rock_go->transform.GetRotationRadiants();
					rotation.z -= 0.05f;
					rock->rock_go->transform.SetRotationRad(rotation);
				}
				else 
				{
					rock->status = RockStatus::DESTROYED;
				}
			}
			break;

		case RockStatus::DESTROYED:
			rock->rock_go->SetEnabled(false);
			rock->status = RockStatus::UNSPAWNED;
			break;

		default:
			break;
		}

	}
}

bool AvalancheZone::IsRockInTheEnd(Rock* rock) const
{
	if(rock->total_time_up >= maximum_time_up)
	{
		rock->status = RockStatus::DESTROYED;
		return true;
	}

	return false;
}

void AvalancheZone::SpawnRock() const
{
	for(auto& rock : rocks)
	{
		if(rock->status == RockStatus::UNSPAWNED)
		{
			rock->status = RockStatus::SPAWNED;
			break;
		}
	}
}

bool AvalancheZone::ComputeDamageZone(Rock* rock) const
{
	float3 rock_current_position = rock->rock_go->transform.GetGlobalTranslation();
		//Collides so deals dmg
	if (!rock->player_already_hitted) 
	{
		if (rock_current_position.Distance(player_1->player_go->transform.GetGlobalTranslation() + float3::unitY) <= radius)
		{
			//Deal damage and stun if needed
			player_1->player_controller->Stun(time_stun_players);
			player_1->player_controller->TakeDamage(damage);
			SetCameraTrauma(0.1f);
			rock->player_already_hitted = true;
			return true;
		}
		if (is_multiplayer)
		{
			//Collides so deals dmg
			if (rock_current_position.Distance(player_2->player_go->transform.GetGlobalTranslation() + float3::unitY) <= radius)
			{
				//Deal damage and stun if needed
				player_2->player_controller->Stun(time_stun_players);
				player_2->player_controller->TakeDamage(damage);
				SetCameraTrauma(0.1f);
				rock->player_already_hitted = true;
				return true;
			}
		}
	}
	return false;
}

float3 AvalancheZone::ComputeNextPosition(Rock* rock) const
{
	float3 current_position = rock->rock_go->transform.GetGlobalTranslation();
	float3 new_position = float3::Lerp(current_position, end_point->transform.GetGlobalTranslation(), rock->remaining_time_up / maximum_time_moving);
	ComputeVerticalPosition(new_position);
	return new_position;
}

void AvalancheZone::ComputeVerticalPosition(float3& rock_current_position) const
{
	std::vector<ComponentMeshRenderer*> intersected_meshes;
	for (const auto& go : mesh_collider)
	{
		if (go->IsEnabled())
		{
			intersected_meshes.push_back(static_cast<ComponentMeshRenderer*>(go->GetComponent(Component::ComponentType::MESH_RENDERER)));
		}
	}

	LineSegment ground_line(rock_current_position, rock_current_position - float3::unitY * 5.f);

	RayLine* collision = new RayLine();
	float min_distance = INFINITY;

	for (const auto& mesh : intersected_meshes)
	{
		LineSegment transformed_ray = ground_line;
		transformed_ray.Transform(mesh->owner->transform.GetGlobalModelMatrix().Inverted());
		std::vector<Mesh::Vertex> &vertices = mesh->mesh_to_render->vertices;
		std::vector<uint32_t> &indices = mesh->mesh_to_render->indices;
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			float3 first_point = vertices[indices[i]].position;
			float3 second_point = vertices[indices[i + 1]].position;
			float3 third_point = vertices[indices[i + 2]].position;
			Triangle triangle(first_point, second_point, third_point);

			float distance;
			float3 intersected_point;
			bool intersected = triangle.Intersects(transformed_ray, &distance, &intersected_point);
			if (intersected && distance < min_distance)
			{
				min_distance = distance;

				collision->game_object = mesh->owner;
				collision->hit_distance = distance;
				collision->hit_point = intersected_point;
				rock_current_position.y = collision->hit_point.z + 1.5f;
			}
		}
	}

}

void AvalancheZone::SetCameraTrauma(float trauma) const
{
	camera_shake->trauma = trauma;
}

void AvalancheZone::CheckEndedRocksKillRocks()
{
	for (auto& rock : rocks)
	{
		if (rock->status != RockStatus::UNSPAWNED) 
		{
			return;
		}
	}
	ended = true;
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void AvalancheZone::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void AvalancheZone::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }