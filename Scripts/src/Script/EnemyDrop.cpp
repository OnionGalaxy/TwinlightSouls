#include "EnemyDrop.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentMeshRenderer.h"

#include "Main/Application.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "PlayerController.h"
#include "WorldManager.h"

EnemyDrop::EnemyDrop()
{

}

// Use this for initialization before Start()
void EnemyDrop::Awake()
{
	GameObject* world_manager_game_object = App->scene->GetGameObjectByName("World Manager");
	world = static_cast<WorldManager*>(world_manager_game_object->GetComponentScript("WorldManager")->script);
	audio_source = static_cast<ComponentAudioSource*>(owner->GetComponent(Component::ComponentType::AUDIO_SOURCE));
}

// Use this for initialization
void EnemyDrop::Start()
{
	player_1 = world->GetPlayer1();
	player_2 = world->GetPlayer2();
}

// Update is called once per frame
void EnemyDrop::Update()
{
	if (world->on_pause)
	{
		return;
	}

	if (life_state == EnemyDropLifeState::DEAD)
	{
		return;
	}

	float delta_time = App->time->delta_time;
	current_time += delta_time;

	switch (life_state)
	{
	case EnemyDropLifeState::SPAWNING:
	{
		float3 next_position = spawning_position + initial_speed * current_time;
		next_position.y = Clamp(spawning_position.y + initial_speed.y * current_time - GRAVITY * 0.5f * current_time * current_time, waiting_position.y, 100.f);
		owner->transform.SetGlobalMatrixTranslation(next_position);

		if (next_position.y <= waiting_position.y && current_time > spawning_time)
		{
			current_time = 0;
			changed_state = true;
			life_state = EnemyDropLifeState::WAITING;
		}
		break;
	}

	case EnemyDropLifeState::WAITING:
	{
		player_to_deliver = GetClosestAlivePlayer();
		if (player_to_deliver != nullptr)
		{
			current_time = 0;
			life_state = EnemyDropLifeState::TRAVELLING;
			changed_state = true;
			waiting_position = owner->transform.GetGlobalTranslation();
			break;
		}

		float new_height = (math::Sin(current_time * oscillation_speed) + 1) * 0.5f * max_oscillation_height;
		float3 oscillation_position = owner->transform.GetGlobalTranslation();
		oscillation_position.y = waiting_position.y + new_height;
		owner->transform.SetGlobalMatrixTranslation(oscillation_position);

		if (current_time >= stil_time)
		{
			current_time = 0;
			changed_state = true;
			life_state = EnemyDropLifeState::DEAD;
		}
		break;
	}
	case EnemyDropLifeState::TRAVELLING:
		if (!player_to_deliver->player_controller->is_alive)
		{
			player_to_deliver = GetClosestAlivePlayer();
			current_time = 0;
			waiting_position = owner->transform.GetGlobalTranslation();

			if (player_to_deliver != nullptr)
			{
				life_state = EnemyDropLifeState::TRAVELLING;
			}
			else
			{
				life_state = EnemyDropLifeState::WAITING;
			}
			break;
		}
		if (current_time < travel_time)
		{
			float progress = current_time / travel_time;
			float3 new_position = float3::Lerp(waiting_position, player_to_deliver->player_go->transform.GetGlobalTranslation() + float3::unitY, progress);
			owner->transform.SetGlobalMatrixTranslation(new_position);
		}
		else
		{
			current_time = 0;
			picked = true;
			changed_state = true;
			life_state = EnemyDropLifeState::DEAD;
			PlayerController* player_to_deliver_controller = static_cast<PlayerController*>(player_to_deliver->player_go->GetComponentScript("PlayerController")->script);
		}
		break;
	}
}

void EnemyDrop::Spawn(const float3& spawn_position)
{
	life_state = EnemyDropLifeState::SPAWNING;
	current_time = 0.f;

	spawning_position = spawn_position;
	float3 offset = float3(rand() % 100 - 50, 5, rand() % 100 - 50);
	offset.ScaleToLength(spread);
	waiting_position = GetClosestTerrainPosition(spawning_position + offset);

	owner->transform.SetGlobalMatrixTranslation(spawning_position);

	initial_speed = offset / spawning_time;
	initial_speed.y = GRAVITY * 0.5f * spawning_time;
	
	picked = false;
}

Player* EnemyDrop::GetClosestAlivePlayer() const
{
	float distance_closest_player = 1000.0F;
	Player* closest_player = nullptr;

	if (player_1->player_controller->is_alive)
	{
		distance_closest_player = player_1->player_go->transform.GetGlobalTranslation().Distance(owner->transform.GetGlobalTranslation());
		closest_player = player_1;
	}

	if (world->multiplayer && player_2->player_controller->is_alive)
	{
		float distance_player_2 = player_2->player_go->transform.GetGlobalTranslation().Distance(owner->transform.GetGlobalTranslation());
		if (distance_player_2 < distance_closest_player)
		{
			closest_player = player_2;
			distance_closest_player = distance_player_2;
		}
	}

	if (distance_closest_player > attracting_radio)
	{
		return nullptr;
	}
	else
	{
		return closest_player;
	}
}

bool EnemyDrop::IsAlive() const
{
	return life_state != EnemyDropLifeState::DEAD;
}

void EnemyDrop::CloneProperties(EnemyDrop* original_orb)
{
	spread = original_orb->spread;
	spawning_time = original_orb->spawning_time;
	stil_time = original_orb->stil_time;
	travel_time = original_orb->travel_time;
	waiting_position = original_orb->waiting_position;
	spawning_position = original_orb->spawning_position;
	oscillation_speed = original_orb->oscillation_speed;
	max_oscillation_height = original_orb->max_oscillation_height;
	attracting_radio = original_orb->attracting_radio;
	mesh_environment = original_orb->GetMeshEnvironment();
}

// Use this for showing variables on inspector
void EnemyDrop::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Spawning");
	ImGui::Spacing();
	ImGui::DragFloat("Spread", &spread);
	ImGui::DragFloat("SpawningTime", &spawning_time);

	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Waiting");
	ImGui::Spacing();
	ImGui::DragFloat("StillTime", &stil_time);
	ImGui::DragFloat("OscillationSpeed", &oscillation_speed);
	ImGui::DragFloat("OscillationHeight", &max_oscillation_height);

	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Travelling");
	ImGui::Spacing();
	ImGui::DragFloat("TravelTime", &travel_time);
	ImGui::DragFloat("Attracting Radio", &attracting_radio);
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
void EnemyDrop::Save(Config& config) const
{
	config.AddFloat(spread, "Spread");
	config.AddFloat(spawning_time, "SpawningTime");
	config.AddFloat(stil_time, "StillTime");
	config.AddFloat(oscillation_speed, "OscillationSpeed");
	config.AddFloat(max_oscillation_height, "OscillationHeight");
	config.AddFloat(travel_time, "TravelTime");
	config.AddFloat(attracting_radio, "AttractingRadio");
}

// //Use this for linking GO AND VARIABLES automatically
void EnemyDrop::Load(const Config& config)
{
	spread = config.GetFloat("Spread", 3.f);
	spawning_time = config.GetFloat("SpawningTime", 1000.f);
	stil_time = config.GetFloat("StillTime", 5000.f);
	oscillation_speed = config.GetFloat("OscillationSpeed", 0.05f);
	max_oscillation_height = config.GetFloat("OscillationHeight", 0.5f);
	travel_time = config.GetFloat("TravelTime", 500.f);
	attracting_radio = config.GetFloat("AttractingRadio", 1.f);
}

float3 EnemyDrop::GetClosestTerrainPosition(const float3& current_next_position) const
{
	LineSegment ground_line(current_next_position, current_next_position - float3::unitY * ray_lenght);

	float min_distance = INFINITY;
	float3 intersection_point = float3::zero;

	for (const auto& mesh : mesh_environment)
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
				intersection_point = intersected_point;
			}
		}
	}

	intersection_point.x = current_next_position.x;
	intersection_point.y = ((intersection_point.z + vertical_offset) > current_next_position.y) ? current_next_position.y : intersection_point.z + vertical_offset;
	intersection_point.z = current_next_position.z;

	return intersection_point;
}

void EnemyDrop::SetMeshEnvironment(const std::vector<ComponentMeshRenderer*>& mesh_environment)
{
	this->mesh_environment = mesh_environment;
}

std::vector<ComponentMeshRenderer*> EnemyDrop::GetMeshEnvironment() const
{
	return mesh_environment;
}