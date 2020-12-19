#include "Biter.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentAnimation.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "ResourceManagement/Resources/Material.h"

#include "imgui.h"

#include "Attacks.h"
#include "BiterStateAttack.h"
#include "BiterStateIdle.h"
#include "BiterStatePursue.h"
#include "BiterStateDie.h"
#include "BiterStateHit.h"
#include "BiterStateExplosion.h"
#include "BiterStateCharge.h"
#include "EnemyDropManager.h"
#include "HitBlinker.h"
#include "PlayerController.h"
#include "UIManager.h"
#include "WorldManager.h"

Biter* BiterDLL()
{
	Biter* instance = new Biter();
	return instance;
}

Biter::Biter()
{
	panel = new PanelComponent();
	move_with_physics = false;

	current_state = new BiterState();
	previous_state = new BiterState();
	idle_state = new BiterStateIdle(this);
	pursue_state = new BiterStatePursue(this);
	attack_state = new BiterStateAttack(this);
	charge_state = new BiterStateCharge(this);
	explosion_state = new BiterStateExplosion(this);
	hit_state = new BiterStateHit(this);
	die_state = new BiterStateDie(this);

	move_speed = 2.F;
	health = 75.F;
}

Biter::~Biter()
{
	delete current_state;
	delete previous_state;
	delete idle_state;
	delete charge_state;
	delete pursue_state;
	delete attack_state;
	delete die_state;
	delete hit_state;
	delete explosion_state;
}

void Biter::Awake()
{
	enemy_type = EnemyType::BITER;
	EnemyController::Awake();
}

void Biter::Start()
{
	EnemyController::Start();

	current_state = idle_state;
	current_state->OnStateEnter();
	is_dying = false;

	next_position = float3::zero;
	current_velocity = float3::zero;
	current_position = owner->transform.GetGlobalTranslation();

	//enemy_manager->AddEnemy(this);

	for (const auto& go : owner->children)
	{
		if (go->name == "Mesh_Character_Biter")
		{
			biter_material = static_cast<ComponentMeshRenderer*>(go->GetComponent(Component::ComponentType::MESH_RENDERER));

			for (const auto& jnt : go->children)
			{
				if (jnt->name == "Tail2_jnt")
				{
					passive_particles = static_cast<ComponentParticleSystem*>(jnt->children[0]->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
				}

				if (jnt->name == "Body_jnt")
				{
					passive_particles_attack = static_cast<ComponentParticleSystem*>(jnt->children[0]->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
					passive_particles_attack_2 = static_cast<ComponentParticleSystem*>(jnt->children[1]->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
					spark_particles = static_cast<ComponentParticleSystem*>(jnt->children[2]->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
				}
			}
		}
			
		if (go->tag == "Biter_explosion_particles")
		{
			explosion_particles = static_cast<ComponentParticleSystem*>(go->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
		}
		if (go->name == "Biter_explosion_2")
		{
			explosion_particles_2 = static_cast<ComponentParticleSystem*>(go->GetComponent(Component::ComponentType::PARTICLE_SYSTEM));
		}
		
		if (go->tag == "VFX")
		{
			for (const auto vfx : go->children)
			{
				if (vfx->name == "charge_explosion_purple_VFX")
				{
					charge_explosion_purple_vfx = static_cast<ComponentBillboard*>(vfx->GetComponent(Component::ComponentType::BILLBOARD));
					charge_explosion_purple_vfx->Disable();
				}
				if (vfx->name == "charge_explosion_black_VFX")
				{
					charge_explosion_black_vfx = static_cast<ComponentBillboard*>(vfx->GetComponent(Component::ComponentType::BILLBOARD));
					charge_explosion_black_vfx->Disable();
				}
				if (vfx->name == "charge_explosion_blue_VFX")
				{
					charge_explosion_blue_vfx = static_cast<ComponentBillboard*>(vfx->GetComponent(Component::ComponentType::BILLBOARD));
					charge_explosion_blue_vfx->Disable();
				}
				if (vfx->name == "inner_circle_explosion_VFX")
				{
					inner_circle_explosion_vfx = static_cast<ComponentBillboard*>(vfx->GetComponent(Component::ComponentType::BILLBOARD));
					inner_circle_explosion_vfx->Disable();
				}
				if (vfx->name == "warning_basic_attack_VFX")
				{
					warning_basic_attack_vfx = static_cast<ComponentBillboard*>(vfx->GetComponent(Component::ComponentType::BILLBOARD));
					warning_basic_attack_vfx->Disable();
					warning_basic_attack_vfx->loop = false;
					warning_basic_attack_vfx->playing_once = true;
				}
				if (vfx->name == "warning_basic_attack_orange_VFX")
				{
					warning_basic_attack_orange_vfx = static_cast<ComponentBillboard*>(vfx->GetComponent(Component::ComponentType::BILLBOARD));
					warning_basic_attack_orange_vfx->Disable();
					warning_basic_attack_orange_vfx->loop = false;
					warning_basic_attack_orange_vfx->playing_once = true;
				}
				if (vfx->name == "shockwave_VFX")
				{
					shockwave_vfx = static_cast<ComponentBillboard*>(vfx->GetComponent(Component::ComponentType::BILLBOARD));
					shockwave_vfx->Disable();
					shockwave_vfx->loop = false;
					shockwave_vfx->playing_once = true;
				}
			}
		}
	}

	passive_particles->active = true;
	passive_particles->Play();
	spark_particles->active = true;
	spark_particles->Play();
	passive_particles_attack->active = true;
	passive_particles_attack->Stop();
	passive_particles_attack_2->active = true;
	passive_particles_attack_2->Stop();
	explosion_particles->Disable();
	explosion_particles_2->Disable();

	current_color = basic_color;
	next_color = basic_color;
	ChangeAttackEffects(BiterAttackEffects::MOVING);
}

void Biter::Update()
{
	if (world_manager->on_pause)
	{
		return;
	}

	if (is_alive)
	{
		if (!is_dying)
		{
			UpdateOffset();
			UpdateSpecialAttackDelay();
			UpdateExplosionCooldown();
			UpdateChargeCooldown();
		}
		UpdateAttackEffectsColor();
		current_state->OnStateUpdate();

		if (GetClosestTarget() == nullptr && current_state != idle_state)
		{
			current_state->Exit(idle_state);
		}
	}
}

void Biter::OnInspector(ImGuiContext* context)
{
	EnemyController::OnInspector(context);

	ImGui::Text("Current State:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", current_state->state_name);
	ImGui::DragFloat3("Current Real Position", &current_position[0], 0.1F);
	ImGui::NewLine();
	
	ImGui::DragFloat("Bite Distance", &bite_distance, 0.01F);
	ImGui::DragFloat("Bite Radius", &bite_radius, 0.01F);
	ImGui::NewLine();

	ImGui::DragFloat("Special Attack Delay", &current_special_attack_delay, 1.0F);
	ImGui::NewLine();

	ImGui::DragFloat("Charge Damage", &charge_damage, 0.01F);
	ImGui::DragFloat("Charge Radius", &charge_radius, 0.01F);
	ImGui::DragFloat("Charge Cooldown", &charge_cooldown, 1.F);
	ImGui::DragFloat("Charge current Cooldown", &charge_current_cooldown, 1.F);
	ImGui::DragFloat("Charge Total Ability Time", &charge_total_ability_time, 1.F);
	ImGui::DragFloat("Charge Time Before Dash", &charge_before_dash_time, 1.F);
	ImGui::DragFloat("Charge Distance Detection", &charge_distance_detection, 0.1F);
	ImGui::DragFloat("Charge Speed", &charge_speed, 0.1F);
	ImGui::NewLine();

	ImGui::DragFloat("Explosion Damage", &explosion_damage, 0.01F);
	ImGui::DragFloat("Explosion Radius", &explosion_radius, 0.01F);
	ImGui::DragFloat("Explosion current Cooldown", &explosion_current_cooldown, 1.F);
	ImGui::DragFloat("Explosion Cooldown", &explosion_cooldown, 1.F);
	ImGui::DragFloat("Explosion Total Ability Time", &explosion_total_ability_time, 1.F);
	ImGui::DragFloat("Explosion Delay Time", &explosion_delay_time, 1.F);
}

void Biter::TakeDamage(float damage, const float3& damage_source_position, float knockback)
{
	if (!is_alive || animation->IsOnState("Die") || animation->IsOnState("Hit")) return;
	if (is_charging)
	{
		ui_manager->SpawnDamageIndicator(0, owner->transform.GetGlobalTranslation(), damage_source_position);
	}
	else
	{
		health -= damage;
		ui_manager->SpawnDamageIndicator(damage, owner->transform.GetGlobalTranslation(), damage_source_position);
		hit_blinker->Blink();
	}
		
	if (health <= 0)
	{
		current_state->Exit(die_state);
	}
	
	else if (!is_charging && !is_exploding)
	{
		current_state->Exit(hit_state);
	}
}

void Biter::Kill()
{
	is_alive = false;
	collider->active_physics = false;
	owner->SetEnabled(false);
	light_orbs_manager->SpawnOrbs(owner->transform.GetGlobalTranslation(), 3);
	//enemy->DeathVFX(); --> maybe different effects for biter
}

void Biter::ResetEnemy()
{
	EnemyController::ResetEnemy();
	current_position = owner->transform.GetGlobalTranslation();
	current_state = idle_state;
	current_state->OnStateEnter();
	owner->SetEnabled(true);
	animation->Stop();
	animation->Play();
	collider->detect_collision = true;
	collider->SetCollisionDetection();

	current_color = basic_color;
	next_color = basic_color;

	if(passive_particles)
	{
		passive_particles->Play();
	}
	if(spark_particles)
	{
		spark_particles->Play();
	}
	if(passive_particles_attack)
	{
		passive_particles_attack->Stop();
		passive_particles_attack_2->Stop();
	}
		
	if(explosion_particles != nullptr)
	{
		explosion_particles->Disable();
	}
	if(explosion_particles_2 != nullptr)
	{
		explosion_particles_2->Disable();
	}
	if(shockwave_vfx != nullptr)
	{
		shockwave_vfx->Disable();
	}
	if(charge_explosion_purple_vfx != nullptr)
	{
		charge_explosion_purple_vfx->Disable();
	}
	if(charge_explosion_black_vfx != nullptr)
	{
		charge_explosion_black_vfx->Disable();
	}
	if(charge_explosion_blue_vfx != nullptr)
	{
		charge_explosion_blue_vfx->Disable();
	}
	if(inner_circle_explosion_vfx != nullptr)
	{
		inner_circle_explosion_vfx->Disable();
	}
	if(warning_basic_attack_vfx != nullptr)
	{
		warning_basic_attack_vfx->Disable();
	}
	if(warning_basic_attack_orange_vfx != nullptr)
	{
		warning_basic_attack_orange_vfx->Disable();
	}

	InitVFX();
}

void Biter::AirMovementAI()
{
	//check if any player is alive	
	if (!GetClosestTarget()) 	
	{	
		owner->transform.SetGlobalMatrixTranslation(current_position + offset);	
		collider->UpdateDimensions();	
		return;	
	}	
	float3 target = GetClosestTarget()->owner->transform.GetGlobalTranslation();

	if (next_position.Equals(float3::zero) || App->time->time > time_count_delay)
	{
		next_position = target;
		time_count_delay = App->time->time + rand() % 500 + 300;
	}
	AirMoveTo(next_position);

	float distance = current_position.Distance(target);
	if (distance <= explosion_radius && abs(current_position.y - target.y) < explosion_cast_range && explosion_current_cooldown <= 0 && is_special_attack_ready)
	{
		is_exploding = true;
		explosion_current_time = 0.0F;
		ExplosionStatus();
	}
	else if (distance <= bite_distance + bite_radius * 2 / 3 && !is_exploding && !is_charging)
	{
		is_attacking = true;
		ChangeAttackEffects(BiterAttackEffects::BITE);
		if (!warning_basic_attack_orange_vfx->IsPlaying() && warning_basic_attack_orange_vfx->playing_once)
		{
			warning_basic_attack_orange_vfx->Play();
		}
	}
	else if (distance < charge_distance_detection && charge_current_cooldown <= 0 && is_special_attack_ready)
	{
		Charge();
	}
}

void Biter::Charge()
{
	is_charging = true;
	current_target = GetClosestTarget();
	
	charge_current_time = charge_total_ability_time;
	charge_current_time -= App->time->delta_time;

	ChangeAttackEffects(BiterAttackEffects::CHARGE);
}

void Biter::ChargeStatus()
{
	UpdateAttackEffectsColor();

	if (is_charging)
	{
		if (charge_current_time >= App->time->delta_time)
		{
			if (charge_current_time < charge_total_ability_time - charge_before_dash_time)
			{
				warning_basic_attack_vfx->playing_once = true;
				warning_basic_attack_orange_vfx->playing_once = true;
				MoveIntoDirection(owner->transform.GetFrontVector(), charge_speed);
			}
			else
			{
				if (!warning_basic_attack_vfx->IsPlaying() && warning_basic_attack_vfx->playing_once)
				{
					warning_basic_attack_vfx->playing_once = false;
					warning_basic_attack_orange_vfx->playing_once = false;
					warning_basic_attack_vfx->Play();
					//warning_basic_attack_orange_vfx->Play();
				}
				charge_target = current_target->owner->transform.GetGlobalTranslation();
				charge_target.y = owner->transform.GetGlobalTranslation().y;	//this avoids rotating vertically when looking at the player
				owner->transform.LookAt(charge_target);
			}
			charge_current_time -= App->time->delta_time;
		}
		else
		{
			is_charging = false;
		}
	}
	owner->transform.SetGlobalMatrixTranslation(current_position + offset);
	collider->UpdateDimensions();
}

void Biter::MoveIntoDirection(const float3 direction, float speed)
{
	float3 normalized_direction = direction.Normalized();
	ComputeHorizontalVelocity(float3(normalized_direction.x, 0.f, normalized_direction.z), speed);
	current_position = current_position + velocity * App->time->delta_time * 0.001f;
}

void Biter::AirMoveTo(const float3& destination)
{
	float3 move_direction = destination - current_position;

	current_velocity = current_velocity * (1 - acceleration_factor) + move_direction.Normalized() * acceleration_factor;
	current_position = current_position + ((current_velocity.Normalized() + CollisionAvoidance(destination))* init_move_speed * App->time->delta_time * 0.001f);

	float3 desired_direction = current_velocity;
	Quat desired_rotation = Quat::LookAt(float3::unitZ, desired_direction.Normalized(), float3::unitY, float3::unitY);
	Quat new_rotation = Quat::Slerp(owner->transform.GetRotation(), desired_rotation, App->time->delta_time * 0.01F);
	owner->transform.SetRotation(new_rotation);

	owner->transform.SetGlobalMatrixTranslation(current_position + offset);
	collider->UpdateDimensions();
}

void Biter::ComputeHorizontalVelocity(const float3 direction, float speed)
{
	float ray_height = collider->GetBoxSize().y / 4;
	float3 origin = collider->GetOrigin() + float3(0.0f, ray_height, 0.0f);
	float3 second_origin = collider->GetOrigin() - float3(0.0f, ray_height, 0.0f);

	float end_distance = speed != speed_horizontal ? speed * App->time->delta_time * 0.001f : frontal_collision_detection_distance;

	float3 direction_aux = direction.ScaledToLength(frontal_collision_detection_distance);
	direction_aux.y = 0;
	float3 end = collider->GetOrigin() + direction_aux;

	CollisionInformation frontal_collision = collider->RaycastClosestHit(origin, end);
	if (frontal_collision.collider == nullptr)
	{
		end = collider->GetOrigin() + float3(direction_aux.x, 0, direction_aux.z);
		frontal_collision = collider->RaycastClosestHit(second_origin, end);
	}

	bool there_is_frontal_collision =
		frontal_collision.collider != nullptr
		&& frontal_collision.collider->detect_collision
		&& frontal_collision.collider->owner->tag == "EnvironmentCollider";

	if (direction.Length() > 0 && !there_is_frontal_collision)
	{
		float3 velocity_tmp = direction.ScaledToLength(speed);
		velocity.x = velocity_tmp.x;
		velocity.z = velocity_tmp.z;
	}
	else
	{
		velocity.x = 0;
		velocity.z = 0;
	}
}

void Biter::UpdateChargeCooldown()
{
	if (charge_current_cooldown >= 0)
	{
		charge_current_cooldown -= App->time->delta_time;
	}
}

void Biter::ExplosionStatus()
{
	UpdateAttackEffectsColor();

	explosion_current_time += App->time->delta_time;
	owner->transform.SetGlobalMatrixTranslation(current_position + offset);
	collider->UpdateDimensions();
}

void Biter::AttackPosition()
{
	UpdateAttackEffectsColor();

	owner->transform.SetGlobalMatrixTranslation(current_position + offset);
	collider->UpdateDimensions();
}

void Biter::UpdateExplosionCooldown()
{
	if (explosion_current_cooldown >= 0)
	{
		explosion_current_cooldown -= App->time->delta_time;
	}
}

void Biter::UpdateOffset()
{
	offset = float3(0, 0.55 + math::Sin(App->time->time / 300) / 5 + math::Sin(App->time->time / 150) / 20, 0);
	float3 movement = current_velocity;
	movement = movement.Cross(float3::unitY);
	offset += movement * math::Sin(App->time->time / 350) / 3 + movement * math::Sin(App->time->time / 150) / 20;
}

void Biter::UpdateSpecialAttackDelay()
{
	if (!is_special_attack_ready)
	{
		if (current_special_attack_delay > 0)
		{
			current_special_attack_delay -= App->time->delta_time;
		}
		else
		{
			is_special_attack_ready = true;
		}
	}
}

void Biter::ApplySpecialAttackDelay()
{
	current_special_attack_delay = special_attack_delay;
	is_special_attack_ready = false;
}
void Biter::HitSFX(NodeID attack)
{
	switch (attack)
	{
	case NodeID::LIGHT_ONE:
		//audio_source->PlayEvent("play_receive_hit");
		break;
	case NodeID::LIGHT_TWO:
		//audio_source->PlayEvent("play_charge_biter");
		break;
	case NodeID::LIGHT_THREE:
		//audio_source->PlayEvent("play_receive_hit");
		break;
	case NodeID::HEAVY_ONE:
		//audio_source->PlayEvent("play_receive_hit");
		break;
	case NodeID::HEAVY_TWO:
		//audio_source->PlayEvent("play_ground");
		break;
	case NodeID::HEAVY_THREE:
		//audio_source->PlayEvent("play_receive_hit");
		break;
	case NodeID::DOWN_SMASH:
		//audio_source->PlayEvent("play_receive_hit");
		break;
	}
}

void Biter::ChangeAttackEffects(BiterAttackEffects attack_effects)
{
	float percentage = (color_change_time - color_current_change_time) / color_change_time;

	for (int i = 0; i < 4; i++)
	{
		current_color[i] = current_color[i] + (next_color[i] - current_color[i]) * percentage;
	}
	current_color_particles = passive_particles->initial_color;

	switch (attack_effects)
	{
	case BiterAttackEffects::MOVING:
		next_color = basic_color;
		next_color_particles = basic_color_particles;

		passive_particles->Play();
		passive_particles_attack->Stop();
		passive_particles_attack_2->Stop();

		break;
	case BiterAttackEffects::BITE:
		next_color = bite_color;
		next_color_particles = bite_color;

		spark_particles->initial_color = next_color_particles;
		spark_particles->Emit(10);

		passive_particles->Play();
		passive_particles_attack->Play();
		passive_particles_attack_2->Play();
		break;
	case BiterAttackEffects::EXPLOSION:
		next_color = explosion_color;
		next_color_particles = explosion_color;

		spark_particles->initial_color = next_color_particles;
		spark_particles->Emit(10);

		passive_particles_attack->Stop();
		passive_particles_attack_2->Stop();
		break;
	case BiterAttackEffects::CHARGE:
		next_color = charge_color;
		next_color_particles = charge_color;

		spark_particles->initial_color = next_color_particles;
		spark_particles->Emit(10);

		passive_particles->Play();
		passive_particles_attack->Play();
		passive_particles_attack_2->Play();
		break;
	case BiterAttackEffects::DEATH:
		next_color = basic_color;
		next_color_particles = basic_color_particles;

		spark_particles->Emit(30);
		passive_particles->Stop();
		passive_particles_attack->Stop();
		passive_particles_attack_2->Stop();
		break;
	}

	color_current_change_time = color_change_time;
}

void Biter::UpdateAttackEffectsColor()
{
	if (color_current_change_time <= 0)
	{
		return;
	}

	color_current_change_time -= App->time->delta_time;
	if (color_current_change_time < 0)
	{
		color_current_change_time = 0;
	}

	float percentage = (color_change_time - color_current_change_time) / color_change_time;

	for (int i = 0; i < 4; i++)
	{
		biter_material->material_to_render->emissive_color[i] = current_color[i] + (next_color[i] - current_color[i]) * percentage;
		passive_particles->initial_color[i] = current_color_particles[i] + (next_color_particles[i] - current_color_particles[i]) * percentage;
		passive_particles_attack->initial_color[i] = current_color_particles[i] + (next_color_particles[i] - current_color_particles[i]) * percentage;
		passive_particles_attack_2->initial_color[i] = current_color_particles[i] + (next_color_particles[i] - current_color_particles[i]) * percentage;
	}
}

void Biter::DisableAllEffects()
{
	passive_particles->Stop();
	spark_particles->Stop();
	passive_particles_attack->Stop();
	passive_particles_attack_2->Stop();
	explosion_particles->Stop();
	explosion_particles_2->Stop();

	charge_explosion_purple_vfx->Disable();
	charge_explosion_black_vfx->Disable();
	charge_explosion_blue_vfx->Disable();
	inner_circle_explosion_vfx->Disable();
	warning_basic_attack_vfx->Disable();
	warning_basic_attack_orange_vfx->Disable();
	shockwave_vfx->Disable();
}
