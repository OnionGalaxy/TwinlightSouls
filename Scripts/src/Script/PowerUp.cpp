#include "PowerUp.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
//#include "Component/ComponentScript.h"
//#include "Component/ComponentTransform.h"
#include "Component/ComponentTrail.h"

#include "Main/Application.h"
//#include "Main/GameObject.h"
//#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

//#include "imgui.h"

#include "PlayerController.h"
#include "WorldManager.h"

PowerUp* PowerUpDLL()
{
	PowerUp* instance = new PowerUp();
	return instance;
}

PowerUp::PowerUp()
{

}

// Use this for initialization before Start()
void PowerUp::Awake()
{
	EnemyDrop::Awake();

	InitVfx();

	attack_powerup->SetEnabled(false);
	invulnerability_powerup->SetEnabled(false);
}

// Update is called once per frame
void PowerUp::Update()
{
	changed_state = false;
	EnemyDrop::Update();

	if (!changed_state)
	{
		return;
	}

	switch (life_state)
	{
	case EnemyDropLifeState::DEAD:
		attack_powerup->SetEnabled(false);
		invulnerability_powerup->SetEnabled(false);

		if (picked)
		{
			PickupActivatePowerup();
		}
		break;

	case EnemyDropLifeState::SPAWNING:
		break;
		
	case EnemyDropLifeState::WAITING:
		audio_source->PlayEvent("play_orbs_dropping");
		break;

	case EnemyDropLifeState::TRAVELLING:
		break;
	}
}

void PowerUp::InitVfx()
{
	for (const auto go : owner->children)
	{
		if (go->tag == "Powerup_attack")
		{
			attack_powerup = go;
			for (const auto ch : go->children)
			{
				if (ch->tag == "small_texture_light_orb_VFX")
				{
					small_texture_attack = static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
				}
				else if (ch->tag == "shine_texture_light_orb_VFX")
				{
					shine_texture_attack = static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
				}
				else if (ch->tag == "exterior_texture_light_orb_VFX")
				{
					exterior_texture_attack = static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
				}
			}
		}
		else if (go->tag == "Powerup_invulnerability")
		{
			invulnerability_powerup = go;
			for (const auto ch : go->children)
			{
				if (ch->tag == "small_texture_light_orb_VFX")
				{
					small_texture_invul = static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
				}
				else if (ch->tag == "shine_texture_light_orb_VFX")
				{
					shine_texture_invul = static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
				}
				else if (ch->tag == "exterior_texture_light_orb_VFX")
				{
					exterior_texture_invul = static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
				}
			}
		}
	}
}

void PowerUp::PickupActivatePowerup() const
{
	PlayerController* player_to_deliver_controller = static_cast<PlayerController*>(player_to_deliver->player_go->GetComponentScript("PlayerController")->script);
	audio_source->PlayEvent("play_orbs_pickup");

	switch (powerup_type)
	{
	case PowerUpType::ATTACK:
		player_to_deliver_controller->time_remaining_attack_powerup += attack_powerup_duration;
		player_to_deliver_controller->attack_powerup_bonus = 1 + attack_powerup_bonus;
		break;

	case PowerUpType::INVULNERABLE:
		player_to_deliver_controller->time_remaining_invulnerable_powerup += invulnerable_powerup_duration;
		break;
	}
}

void PowerUp::Spawn(const float3& spawn_position)
{
	EnemyDrop::Spawn(spawn_position);

	SpawnSetPowerupVfx();
}

void PowerUp::SpawnSetPowerupVfx() const
{
	switch (powerup_type)
	{
	case PowerUpType::ATTACK:
		attack_powerup->SetEnabled(true);

		small_texture_attack->Play();
		shine_texture_attack->Play();
		exterior_texture_attack->Play();
		break;

	case PowerUpType::INVULNERABLE:
		invulnerability_powerup->SetEnabled(true);

		small_texture_invul->Play();
		shine_texture_invul->Play();
		exterior_texture_invul->Play();
		break;
	}
}

void PowerUp::CloneProperties(PowerUp* original_orb)
{
	EnemyDrop::CloneProperties(original_orb);
}

// Use this for showing variables on inspector
void PowerUp::OnInspector(ImGuiContext* context)
{
	EnemyDrop::OnInspector(context);

	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Attack Powerup");
	ImGui::Spacing();
	ImGui::DragInt("Attack Duration", &attack_powerup_duration);
	ImGui::DragFloat("Attack Increase (%)", &attack_powerup_bonus, 0.01);
	ImGui::DragFloat("Attack Chance", &attack_powerup_chance, 0.01, 0.f);

	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Invulnerability Powerup");
	ImGui::Spacing();
	ImGui::DragInt("Invulnerability Duration", &invulnerable_powerup_duration);
	ImGui::DragFloat("Invulnerability Chance", &invulnerable_powerup_chance, 0.01, 0.f);
}

void PowerUp::Save(Config & config) const
{
	EnemyDrop::Save(config);

	config.AddInt(attack_powerup_duration, "AttackPowerupDuration");
	config.AddFloat(attack_powerup_bonus, "AttackPowerupBonus");
	config.AddFloat(attack_powerup_chance, "AttackPowerupChance");

	config.AddInt(invulnerable_powerup_duration, "InvulnerablePowerupDuration");
	config.AddFloat(invulnerable_powerup_chance, "InvulnerablePowerupChance");
}

void PowerUp::Load(const Config & config)
{
	EnemyDrop::Load(config);

	attack_powerup_duration = config.GetInt("AttackPowerupDuration", 20);
	attack_powerup_bonus = config.GetFloat("AttackPowerupBonus", 0.3f);
	attack_powerup_chance = config.GetFloat("AttackPowerupChance", 0.5f);

	invulnerable_powerup_duration = config.GetInt("InvulnerablePowerupDuration", 10);
	invulnerable_powerup_chance = config.GetFloat("InvulnerablePowerupChance", 0.5f);
}
