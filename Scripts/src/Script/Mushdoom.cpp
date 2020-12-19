#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentParticleSystem.h"

#include "Log/EngineLog.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleAI.h"

#include <imgui.h>
#include <stdlib.h> 

#include "Attacks.h"
#include "DebugModeScript.h"
#include "EnemyManager.h"
#include "EnemyDropManager.h"
#include "HitBlinker.h"
#include "MushdoomBall.h"
#include "MushdoomStateAttack.h"
#include "MushdoomStateDie.h"
#include "MushdoomStateHit.h"
#include "MushdoomStateIdle.h"
#include "MushdoomStatePursue.h"
#include "MushdoomStateScream.h"
#include "MushdoomStateThrow.h"
#include "PlayerController.h"
#include "UIManager.h"
#include "WorldManager.h"

Mushdoom* MushdoomDLL()
{
	Mushdoom* instance = new Mushdoom();
	return instance;
}

Mushdoom::Mushdoom()
{
	current_state = new MushdoomState();
	previous_state = new MushdoomState();
	idle_state = new MushdoomStateIdle(this);
	scream_state = new MushdoomStateScream(this);
	pursue_state = new MushdoomStatePursue(this);
	attack_state = new MushdoomStateAttack(this);
	die_state = new MushdoomStateDie(this);
	hit_state = new MushdoomStateHit(this);
	throw_state = new MushdoomStateThrow(this);
}

Mushdoom::~Mushdoom()
{
	delete horizontal_ray_detector_middle;
	delete horizontal_ray_detector_knee;

	delete main_vertical_ray_detector;
	delete border_vertical_ray_detector;
	delete main_diagonal_ray_detector;
}

// Use this for initialization before Start()
void Mushdoom::Awake()
{
	enemy_type = EnemyType::MUSHDOOM;
	EnemyController::Awake();
	mesh_collider = App->scene->GetGameObjectsWithTag("EnvironmentCollider");
	enemy_manager = static_cast<EnemyManager*>(App->scene->GetGameObjectByName("EnemyManager")->GetComponentScript("EnemyManager")->script);
	world_manager = static_cast<WorldManager*>(App->scene->GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	for (const auto& ch : owner->children)
	{
		if (ch->tag == "Projectile")
		{
			mushdoom_ball = static_cast<MushdoomBall*>(ch->GetComponentScript("MushdoomBall")->script);
			continue;
		}
		if (ch->name == "PS_footsteps")
		{
			mushdoom_footsteps = static_cast<ComponentParticleSystem*>(ch->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
			mushdoom_footsteps->Stop();
			continue;
		}
		if (ch->tag == "ThrowEffect")
		{
			throw_effect = static_cast<ComponentParticleSystem*>(ch->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
			continue;
		}
		if (ch->tag == "DieMushdoomBall")
		{
			ball_die_effect = static_cast<ComponentParticleSystem*>(ch->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
			continue;
		}
	}

	if (mushdoom_ball)
	{
		mushdoom_ball->owner->SetEnabled(false);
	}

	horizontal_ray_detector_middle = new RayHit();
	horizontal_ray_detector_knee = new RayHit();

	main_vertical_ray_detector = new RayHit();
	border_vertical_ray_detector = new RayHit();
	main_diagonal_ray_detector = new RayHit();
}

// Use this for initialization
void Mushdoom::Start()
{
	EnemyController::Start();
	current_state = idle_state;
	current_state->OnStateEnter();
	ball_die_effect->Stop();
	throw_effect->Stop();
}

// Update is called once per frame
void Mushdoom::Update()
{
	if (world_manager->on_pause)
	{
		return;
	}

	if (is_alive)
	{
		// If some mushdoom get buged we kill them
		if (owner->transform.GetGlobalTranslation().y < -50.f)
		{
			TakeDamage(10000.f, owner->transform.GetGlobalTranslation());
		}

		if (GetClosestTarget() == nullptr && current_state != idle_state && current_state != die_state)
		{
			current_state->Exit(idle_state);
		}
		current_state->OnStateUpdate();
	}
}

//Use this for showing variables on inspector
void Mushdoom::OnInspector(ImGuiContext* context)
{
	EnemyController::OnInspector(context);
}

void Mushdoom::SetProperties(EnemyController* original_enemy)
{
	EnemyController::SetProperties(original_enemy);

	const Mushdoom* original_mushdoom = (Mushdoom*)original_enemy;
}

void Mushdoom::KnockBack()
{
	float3 new_position = owner->transform.GetGlobalTranslation().Lerp(knockback_target_position, animation->GetCurrentClipPercentatge());
	new_position = new_position - owner->transform.GetGlobalTranslation();
	ComputeHorizontalVelocity((new_position - position).Normalized(), new_position.Length());
	new_position = owner->transform.GetGlobalTranslation() + velocity * App->time->delta_time * 0.001f;
	SetNewPosition(new_position);
}


void Mushdoom::TakeDamage(float damage, const float3& damage_source_position, float knockback)
{
	if (!is_alive || animation->IsOnState("Die")) return;

	if (!invincible)
	{
		health -= damage;
		hit_blinker->Blink();
	}

	ui_manager->SpawnDamageIndicator(damage, owner->transform.GetGlobalTranslation(), damage_source_position);

	//Knockback computations
	this->knockback_power = knockback_power;
	float3 current_position = owner->transform.GetGlobalTranslation();
	knockback_target_position = (knockback_direction.Normalized() * this->knockback_power) + current_position;

	if (health <= 0)
	{
		is_dying = true;
		current_state->Exit(die_state);
	}
	else
	{
		current_state->Exit(hit_state);
	}
}

void Mushdoom::Kill()
{
	light_orbs_manager->SpawnOrbs(owner->transform.GetGlobalTranslation(), 2);
	is_alive = false;

	if (seek_slot > -1)
	{
		enemy_manager->RemoveSlot(current_player, enemy_manager->current_seekers_for_player, this);
		seek_slot = -1;
	}
	if (attack_slot > -1)
	{
		enemy_manager->RemoveSlot(current_player, enemy_manager->current_attackers_for_player, this);
		attack_slot = -1;
	}
	if (shoot_slot > -1)
	{
		enemy_manager->RemoveSlot(current_player, enemy_manager->current_shooters_for_player, this);
		shoot_slot = -1;
	}
	owner->SetEnabled(false);
	is_dying = false;
}

void Mushdoom::ResetEnemy()
{
	EnemyController::ResetEnemy();

	is_spawning = false;
	has_ended_spawn = false;
	grounded = false;
	direction = float3::zero;
	fixed_pos = float3::unitY;
	current_time_to_throw = 0;
	current_time_to_attack = 0;
	is_throwing = false;
	can_attack = false;
	can_throw = false;
	is_moving = false;

	if (seek_slot > -1)
	{
		enemy_manager->RemoveSlot(current_player, enemy_manager->current_seekers_for_player, this);
		seek_slot = -1;
	}
	if (attack_slot > -1)
	{
		enemy_manager->RemoveSlot(current_player, enemy_manager->current_attackers_for_player, this);
		attack_slot = -1;
	}
	if (shoot_slot > -1)
	{
		enemy_manager->RemoveSlot(current_player, enemy_manager->current_shooters_for_player, this);
		shoot_slot = -1;
	}

	detect_player_distance = 30.f;
	animation->Stop();

	throw_effect->Stop();
	owner->SetEnabled(true);
	if (mushdoom_ball)
	{
		mushdoom_ball->owner->SetEnabled(false);
		mushdoom_ball->owner->transform.SetGlobalMatrixTranslation(float3(0, 0, 35));
	}
	InitVFX();

	current_state = idle_state;
	current_state->OnStateEnter();
	animation->Play();
}

void Mushdoom::HitSFX(NodeID attack)
{
	switch (attack)
	{
	case NodeID::LIGHT_ONE:
		audio_source->PlayEvent("play_receive_hit");
		break;
	case NodeID::LIGHT_TWO:
		audio_source->PlayEvent("play_ground");
		break;
	case NodeID::LIGHT_THREE:
		audio_source->PlayEvent("play_receive_hit");
		break;
	case NodeID::HEAVY_ONE:
		audio_source->PlayEvent("play_receive_hit");
		break;
	case NodeID::HEAVY_TWO:
		audio_source->PlayEvent("play_ground");
		break;
	case NodeID::HEAVY_THREE:
		audio_source->PlayEvent("play_receive_hit");
		break;
	case NodeID::DOWN_SMASH:
		audio_source->PlayEvent("play_receive_hit");
		break;
	}
}

bool Mushdoom::DetectCollisions(int collision_direction)
{
	float3 current_position = owner->transform.GetGlobalTranslation();
	bool collision = false;
	if (collision_direction == 1) //Vertical
	{
		vertical_offset_end = math::Clamp(abs(velocity.y), vertical_offset_minimum, vertical_offset_maximum);
		LineSegment ground_line(current_position + float3::unitY * vertical_offset_start, current_position - float3::unitY * vertical_offset_end);

		LineSegment border_line_detector(current_position + float3::unitY * vertical_offset_start - (owner->transform.GetFrontVector() * border_ray_offset),
			current_position - float3::unitY * vertical_offset_end - (owner->transform.GetFrontVector() * border_ray_offset));

		LineSegment diagonal_line_detector(current_position + float3::unitY,
			current_position - float3::unitY * 2 + (owner->transform.GetFrontVector() * 2.f));

		if (IsColliding(main_vertical_ray_detector, ground_line, true)
			&& main_vertical_ray_detector->game_object != nullptr
			&& main_vertical_ray_detector->hit_distance < vertical_threshold)
		{
			collision = true;
			main_vertical_ray = true;
			border_vertical_ray = false;
		}
		else if (IsColliding(border_vertical_ray_detector, border_line_detector, true)
			&& border_vertical_ray_detector->game_object != nullptr
			&& border_vertical_ray_detector->hit_distance < vertical_threshold)
		{
			collision = true;
			border_vertical_ray = true;
			main_vertical_ray = false;
		}
		else if (IsColliding(main_diagonal_ray_detector, diagonal_line_detector, true)
			&& main_diagonal_ray_detector->game_object != nullptr
			&& main_diagonal_ray_detector->hit_distance < vertical_threshold)
		{
			collision = true;
		}
		else
		{
			border_vertical_ray = false;
			main_vertical_ray = false;

			if (main_vertical_ray_detector->hit_distance > vertical_threshold)
			{
				minimum_y = main_vertical_ray_detector->hit_point.z;
			}
		}

		if (debug->show_movement)
		{
			App->debug_draw->RenderLine(ground_line.a, ground_line.b, float3(0.f, 1.f, 1.f));
			App->debug_draw->RenderLine(border_line_detector.a, border_line_detector.b, float3(1.f, 0.f, 1.f));
			App->debug_draw->RenderLine(diagonal_line_detector.a, diagonal_line_detector.b, float3(1.f, 1.f, 0.f));
		}

	}
	else if (collision_direction == 2) //Rotation
	{
		float3 front = current_position + owner->transform.GetFrontVector();

		float angle = math::Acos(rotation_angle);

		float3 max_collision = math::Quat(owner->transform.GetUpVector(), angle) * owner->transform.GetFrontVector();
		float3 min_collision = math::Quat(owner->transform.GetUpVector(), -angle) * owner->transform.GetFrontVector();

		max_collision.ScaleToLength(rotation_ray_length);
		min_collision.ScaleToLength(rotation_ray_length);

		max_collision += current_position;
		min_collision += current_position;

		LineSegment rotation_ray_max_line(current_position + float3::unitY * 0.2f, max_collision + float3::unitY * 0.2f);
		LineSegment rotation_ray_min_line(current_position + float3::unitY * 0.2f, min_collision + float3::unitY * 0.2f);

		RayHit* rotation_ray = new RayHit();

		if (IsColliding(rotation_ray, rotation_ray_max_line, true)
			&& rotation_ray->hit_distance > rotation_threshold)
		{
			collision = true;
		}
		else if (IsColliding(rotation_ray, rotation_ray_min_line, true)
			&& rotation_ray->hit_distance > rotation_threshold)
		{
			collision = true;
		}
		if (debug->show_movement)
		{
			App->debug_draw->RenderLine(rotation_ray_max_line.a, rotation_ray_max_line.b);
			App->debug_draw->RenderLine(rotation_ray_min_line.a, rotation_ray_min_line.b);
		}
		delete rotation_ray;
	}
	else //Horizontal
	{

		float3 cast_ray_position_middle = current_position + float3::unitY * horizontal_ray_middle_position - (owner->transform.GetFrontVector() * 0.5f);
		LineSegment ray_detector_middle(cast_ray_position_middle, cast_ray_position_middle + owner->transform.GetFrontVector() * horizontal_ray_middle_length);

		float3 cast_ray_position_knee = current_position + float3::unitY * horizontal_ray_knee_position;
		LineSegment ray_detector_knee(cast_ray_position_knee, cast_ray_position_knee + owner->transform.GetFrontVector() * horizontal_ray_knee_length);

		if (IsColliding(horizontal_ray_detector_middle, ray_detector_middle)
			&& horizontal_ray_detector_middle->hit_distance > horizontal_threshold)
		{
			collision = true;
		}
		else if (IsColliding(horizontal_ray_detector_knee, ray_detector_knee, true)
			&& horizontal_ray_detector_knee->hit_distance > 0.0f)
		{
			collision = true;
		}

		if (debug->show_movement)
		{
			App->debug_draw->RenderLine(ray_detector_middle.a, ray_detector_middle.b);
			App->debug_draw->RenderLine(ray_detector_knee.a, ray_detector_knee.b);
		}
	}
	return collision;
}

bool Mushdoom::IsColliding(RayHit* collision, const LineSegment& ray, bool only_mesh)
{
	bool collided = false;

	std::vector<ComponentMeshRenderer*> intersected_meshes;
	for (const auto& go : mesh_collider)
	{
		if (go->IsEnabled())
		{
			intersected_meshes.push_back(static_cast<ComponentMeshRenderer*>(go->GetComponent(Component::ComponentType::MESH_RENDERER)));
		}
	}

	if (!only_mesh && IsOutsideOfRange())
	{
		if (world_manager->ThereIsBoss())
		{
			for (const auto& zone : boss_zones)
			{
				if (zone->owner->IsEnabled())
				{
					intersected_meshes.push_back(zone);
				}
			}
		}
		for (const auto& enemy : enemy_manager->enemies)
		{
			if ((enemy->is_alive && !enemy->animation->IsOnState("Die"))
				&& enemy->owner->UUID != owner->UUID
				&& EnemyInFront(enemy->owner->transform.GetGlobalTranslation()))
			{
				bool hitted = false;
				//This is done because global_bounding_boxes don't work as expected and biter have different structure than the others
				if (enemy->enemy_type == EnemyType::BITER)
				{
					hitted = enemy->owner->children[0]->aabb.bounding_box.Intersects(ray);
				}
				else
				{
					hitted = enemy->owner->children[0]->children[0]->aabb.bounding_box.Intersects(ray);
				}
				if (hitted)
				{
					collision->game_object = enemy->owner;
					collision->hit_distance = 2.5f;
					return true;
				}
			}
		}
		if (world_manager->multiplayer)
		{
			GameObject* second_player = world_manager->GetPlayer2()->player_go;
			if (second_player->children[0]->aabb.bounding_box.Intersects(ray))
			{
				collision->game_object = second_player;
				collision->hit_distance = 1.f;
				return true;
			}
		}
		GameObject* first_player = world_manager->GetPlayer1()->player_go;
		if (first_player->children[0]->aabb.bounding_box.Intersects(ray))
		{
			collision->game_object = first_player;
			collision->hit_distance = 1.f;
			return true;
		}
	}

	GameObject* selected = nullptr;
	float min_distance = INFINITY;

	for (const auto& mesh : intersected_meshes)
	{
		LineSegment transformed_ray = ray;
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
				collided = true;
			}
		}
	}
	return collided;
}

bool Mushdoom::EnemyInFront(const float3& enemy_position) const
{
	float3 direction = enemy_position - owner->transform.GetGlobalTranslation();
	float dot_product = math::Dot(direction.Normalized(), owner->transform.GetFrontVector());
	if (dot_product > 0 && direction.Length() < range_collision)
	{
		return true;
	}

	return false;
}

void Mushdoom::Move()
{
	ResetMovement();
	if (is_spawning)
	{
		SpawnJump();
	}
	else
	{
		RotateTowardsPlayer();

		ComputeHorizontalVelocity(direction, move_speed * 0.7f);
		if (has_ended_spawn)
		{
			ComputeVerticalVelocity(float3::zero, 0, true);
		}

		ApplyVelocity();
	}
}

void Mushdoom::ComputeHorizontalVelocity(float3 direction, float speed)
{
	bool there_is_frontal_collision = DetectCollisions(0) || DetectCollisions(2);
	if (!has_ended_spawn)
	{
		there_is_frontal_collision = false;
		is_moving = true;
	}

	if (direction.Length() > 0 && !there_is_frontal_collision)
	{
		float3 velocity_tmp = direction.ScaledToLength(speed);
		velocity.x = velocity_tmp.x;
		velocity.z = velocity_tmp.z;
		is_moving = true;

	}
	else
	{
		velocity.x = 0.0f;
		velocity.z = 0.0f;
	}
}

void Mushdoom::ComputeVerticalVelocity(float3 direction, float speed, bool apply_gravity)
{
	grounded = DetectCollisions(1);

	if (direction.Length() > 0.f)
	{
		velocity.y += speed;
	}

	if (grounded)
	{
		velocity.y = 0;
		minimum_y = -10.f;
	}
	else if (!grounded)
	{
		velocity.y += -gravity * App->time->delta_time * 0.001f;
	}
}

void Mushdoom::SetNewPosition(const float3 & new_position) const
{
	float3 next_position = float3(new_position);
	if (grounded)
	{
		if (main_vertical_ray)
		{
			next_position.y = main_vertical_ray_detector->hit_point.z;
		}
		else
		{
			next_position.y = border_vertical_ray_detector->hit_point.z;
		}
	}

	next_position.y = math::Clamp(next_position.y, minimum_y, 2000.f);
	owner->transform.SetGlobalMatrixTranslation(next_position);
}

void Mushdoom::ApplyVelocity()
{
	float3 next_position = owner->transform.GetGlobalTranslation() + velocity * App->time->delta_time * 0.001f;
	distance = abs(next_position.Distance(target));
	if (!has_ended_spawn)
	{
		distance = float3(next_position.x, 0.0f, next_position.z).Distance(float3(target.x, 0.0f, target.z));
		if (distance <= enemy_manager->spawn_distance)
		{
			initial_spawn_position = position;
			position.x = owner->transform.GetGlobalTranslation().x;
			position.z = owner->transform.GetGlobalTranslation().z;
			position.y += jump_distance;
			audio_source->PlayEvent("play_ground");
			animation->ActiveAnimation("ground");
			is_spawning = true;
			final_spawn_position = position;
			current_spawning_time = 0.0f;
		}
	}
	else
	{
		HandleAttackZones(next_position);
	}
	SetNewPosition(next_position);
}

void Mushdoom::RotateTowardsPlayer() const
{
	float3 desired_direction = current_target->owner->transform.GetGlobalTranslation() - owner->transform.GetGlobalTranslation();
	desired_direction.y = 0;
	Quat desired_rotation = Quat::LookAt(float3::unitZ, desired_direction.Normalized(), float3::unitY, float3::unitY);
	Quat new_rotation = Quat::Slerp(owner->transform.GetGlobalRotation(), desired_rotation, App->time->delta_time * smooth_rotation);
	owner->transform.SetGlobalMatrixRotation(new_rotation);
}

void Mushdoom::SpawnJump() const
{
	if (current_spawning_time >= MAX_SPAWN_TIME)
	{
		is_spawning = false;
		has_ended_spawn = true;
		current_spawning_time = 0;
		return;
	}
	else
	{
		float3 next_position = initial_spawn_position.Lerp(final_spawn_position, (float)(current_spawning_time / MAX_SPAWN_TIME));
		owner->transform.SetGlobalMatrixTranslation(next_position);
		collider->UpdateDimensions();
		current_spawning_time += App->time->delta_time;
		if (current_spawning_time > MAX_SPAWN_TIME)
		{
			is_spawning = false;
			has_ended_spawn = true;
			current_spawning_time = MAX_SPAWN_TIME;
		}
	}
}

void Mushdoom::ResetMovement()
{
	current_target = GetClosestTarget();
	if (current_target)
	{
		current_player = static_cast<unsigned int>(current_target->player);
		if (target.Equals(float3::zero) || App->time->time > time_count_delay)
		{
			target = current_target->owner->transform.GetGlobalTranslation();
			time_count_delay = App->time->time + rand() % 400 + 300;
		}
	}
	position = owner->transform.GetGlobalTranslation();
	desired_velocity = float3::zero;
	direction = (target - position).Normalized();
	if (!fixed_pos.Equals(float3::unitY))
	{
		direction = (fixed_pos - position).Normalized();
	}
	distance = abs(float3(position.x, 0.0f, position.z).Distance(float3(target.x, 0.0f, target.z)));
}

void Mushdoom::HandleAttackZones(float3& new_position)
{
	distance = float3(new_position.x, 0.f, new_position.z).Distance(float3(target.x, 0.f, target.z));

	if (distance <= enemy_manager->seek_distance && seek_slot == -1 && attack_slot == -1 && shoot_slot == -1)
	{
		if (enemy_manager->GetCurrentSlot(current_player, enemy_manager->current_attackers_for_player) < enemy_manager->max_attackers)
		{
			attack_slot = enemy_manager->CreateSlot(current_player, enemy_manager->current_attackers_for_player, this);
		}
		else if (enemy_manager->GetCurrentSlot(current_player, enemy_manager->current_shooters_for_player) < enemy_manager->max_shooters)
		{
			shoot_slot = enemy_manager->CreateSlot(current_player, enemy_manager->current_shooters_for_player, this);
		}
		else
		{
			seek_slot = enemy_manager->CreateSlot(current_player, enemy_manager->current_seekers_for_player, this);
		}
		return;
	}

	if (distance > enemy_manager->seek_distance)
	{
		fixed_pos = float3::unitY;
		if (attack_slot > -1)
		{
			is_attacking = false;
		}
		if (shoot_slot > -1)
		{
			is_throwing = false;
		}
		return;
	}

	if (attack_slot > -1)
	{
		if (distance <= enemy_manager->attack_distance)
		{
			if (enemy_manager->GetCurrentSlot(current_player, enemy_manager->current_attackers_for_player) > 1)
			{
				RotateAtDistance(new_position, enemy_manager->attack_distance, enemy_manager->angle_rad);
			}
			LimitMovementAtDistance(new_position, enemy_manager->attack_distance);
			AttackPlayer();
			if (can_attack)
			{
				is_attacking = true;
			}
		}
		else
		{
			fixed_pos = float3::unitY;
		}
	}
	else if (shoot_slot > -1)
	{
		if (distance <= enemy_manager->shoot_distance)
		{
			RotateAtDistance(new_position, enemy_manager->shoot_distance, -enemy_manager->angle_rad);
			LimitMovementAtDistance(new_position, enemy_manager->shoot_distance);
			NewThrow();
			if (can_throw)
			{
				is_throwing = true;
			}
		}
		else
		{
			fixed_pos = float3::unitY;
		}
	}
	else if (seek_slot > -1)
	{

		if (distance <= enemy_manager->seek_distance)
		{
			RotateAtDistance(new_position, enemy_manager->seek_distance, enemy_manager->angle_rad);
			LimitMovementAtDistance(new_position, enemy_manager->seek_distance);
			AttackPlayer();
			if (attack_slot == -1)
			{
				NewThrow();
			}
		}
		else
		{
			fixed_pos = float3::unitY;
		}
	}

	if (debug->show_movement)
	{
		App->debug_draw->RenderLine(owner->transform.GetGlobalTranslation(), float3(target.x, target.y * 2.0f, target.z));
		if (!fixed_pos.Equals(float3::unitY))
		{
			App->debug_draw->RenderLine(owner->transform.GetGlobalTranslation(), float3(fixed_pos.x, fixed_pos.y * 2.0f, fixed_pos.z));
		}
		App->debug_draw->RenderCircle(float3(target.x, target.y * 2.0f, target.z), enemy_manager->attack_distance);
		App->debug_draw->RenderCircle(float3(target.x, target.y * 2.0f, target.z), enemy_manager->spawn_distance);
		App->debug_draw->RenderCircle(float3(target.x, target.y * 2.0f, target.z), enemy_manager->shoot_distance);
		App->debug_draw->RenderCircle(float3(target.x, target.y * 2.0f, target.z), enemy_manager->seek_distance);

	}

	if (!fixed_pos.Equals(float3::unitY))
	{
		if (attack_slot > -1)
		{
			AvoidEnemies(new_position, enemy_manager->GetEnemiesOnSlot(current_player, enemy_manager->current_attackers_for_player));
			return;
		}
		if (shoot_slot > -1)
		{
			AvoidEnemies(new_position, enemy_manager->GetEnemiesOnSlot(current_player, enemy_manager->current_shooters_for_player));
			return;
		}
		if (seek_slot > -1)
		{
			AvoidEnemies(new_position, enemy_manager->GetEnemiesOnSlot(current_player, enemy_manager->current_seekers_for_player));
			return;
		}
	}

}

void Mushdoom::LimitMovementAtDistance(float3& new_position, float max_distance)
{
	float3 gronund_pos = float3(new_position.x - target.x, 0.0f, new_position.z - target.z);
	float3 gronund_pos_norm = gronund_pos.Normalized();
	new_position.x = target.x + max_distance * gronund_pos_norm.x;
	new_position.z = target.z + max_distance * gronund_pos_norm.z;
	if (!new_position.Equals(position))
	{
		ComputeHorizontalVelocity((new_position - position).Normalized(), move_speed * 0.5f);
		new_position = owner->transform.GetGlobalTranslation() + velocity * App->time->delta_time * 0.001f;
	}
}

void Mushdoom::RotateAtDistance(float3& new_position, float max_distance, float angle)
{
	float3 gronund_pos = float3(new_position.x - target.x, 0.0f, new_position.z - target.z);
	float3 gronund_pos_norm = gronund_pos.Normalized();
	fixed_pos.x = target.x + max_distance * (gronund_pos_norm.x * Cos(angle) - gronund_pos_norm.z * Sin(angle));
	fixed_pos.y = position.y;
	fixed_pos.z = target.z + max_distance * (gronund_pos_norm.x * Sin(angle) + gronund_pos_norm.z * Cos(angle));
}

void Mushdoom::NewThrow()
{
	current_time_to_throw += App->time->delta_time;
	can_throw = false;
	int current_slot = enemy_manager->GetCurrentSlot(current_player, enemy_manager->current_shooters_for_player);
	// Throw ball every 'throw_delay' seconds
	if (current_time_to_throw > (throw_delay * 1000) && (enemy_manager->max_shooters > current_slot || shoot_slot > -1))
	{
		if (shoot_slot == -1)
		{
			shoot_slot = enemy_manager->CreateSlot(current_player, enemy_manager->current_shooters_for_player, this);
			if (seek_slot > -1)
			{
				seek_slot = -1;
				enemy_manager->RemoveSlot(current_player, enemy_manager->current_seekers_for_player, this);
			}
		}
		can_throw = true;
		current_time_to_throw = 0;
	}
}

void Mushdoom::AttackPlayer()
{
	current_time_to_attack += App->time->delta_time;
	can_attack = false;
	int current_slot = enemy_manager->GetCurrentSlot(current_player, enemy_manager->current_attackers_for_player);
	// Throw ball every 'throw_delay' seconds
	if (current_time_to_attack > (hit_delay * 1000) && (enemy_manager->max_attackers > current_slot || attack_slot > -1))
	{
		if (attack_slot == -1)
		{
			attack_slot = enemy_manager->CreateSlot(current_player, enemy_manager->current_attackers_for_player, this);
			if (seek_slot > -1)
			{
				seek_slot = -1;
				enemy_manager->RemoveSlot(current_player, enemy_manager->current_seekers_for_player, this);
			}
		}
		can_attack = true;
		current_time_to_attack = 0;
	}
}

bool Mushdoom::IsOutsideOfRange() const
{
	if (attack_slot > -1 && distance < enemy_manager->shoot_distance)
	{
		return false;
	}
	if (shoot_slot > -1 && distance < enemy_manager->seek_distance)
	{
		return false;
	}
	if (seek_slot > -1 && distance < enemy_manager->spawn_distance)
	{
		return false;
	}
	return true;
}



void Mushdoom::AvoidEnemies(float3& new_position, std::vector<EnemyController*>* enemies)
{
	float min_space = 3.f;
	EnemyController* previous_enemy = nullptr;
	for (unsigned int i = 0; i < enemies->size(); i++)
	{
		if (enemies->at(i) == this)
		{
			if (i == 0 && enemies->size() > 1)
			{
				previous_enemy = enemies->at(enemies->size() - 1);
				float3 enemy_pos = previous_enemy->owner->transform.GetGlobalTranslation();
				if (!IsNextTo(new_position, enemy_pos))
				{
					previous_enemy = nullptr;
				}
			}
			else if (i > 0)
			{
				previous_enemy = enemies->at(i - 1);
			}
			break;
		}

	}

	if (previous_enemy)
	{
		float3 enemy_pos = previous_enemy->owner->transform.GetGlobalTranslation();
		float space_between = float3(new_position.x, 0.f, new_position.z).Distance(float3(enemy_pos.x, 0.f, enemy_pos.z));

		if (space_between < min_space)
		{
			float3 new_velocity = float3(enemy_pos.x - new_position.x, 0.f, enemy_pos.z - new_position.z).Normalized();
			new_velocity = new_velocity.ScaledToLength(-min_space * 1.3f);
			velocity.x = new_velocity.x;
			velocity.z = new_velocity.z;
			new_position = owner->transform.GetGlobalTranslation() + velocity * App->time->delta_time * 0.001f;
		}
	}
}

bool Mushdoom::IsNextTo(float3& initial, float3& goal, bool is_left)
{
	float3 inital_offset = float3(initial.x - target.x, 0.0f, initial.z - target.z);
	float3 final_offset = float3(goal.x - target.x, 0.0f, goal.z - target.z);
	if (is_left && ((goal.z < 0 && initial.z < 0 && goal.x < initial.x)
		|| (goal.z > 0 && initial.z > 0 && goal.x > initial.x)
		|| (goal.x < 0 && initial.x < 0 && goal.z > initial.z)
		|| (goal.x > 0 && initial.x > 0 && goal.z < initial.z)))
	{
		return true;
	}
	if ((goal.z < 0 && initial.z < 0 && goal.x > initial.x)
		|| (goal.z > 0 && initial.z > 0 && goal.x < initial.x)
		|| (goal.x < 0 && initial.x < 0 && goal.z < initial.z)
		|| (goal.x > 0 && initial.x > 0 && goal.z > initial.z))
	{
		return true;
	}
	return false;
}
