#include "LightOrb.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTrail.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "imgui.h"

#include "PlayerController.h"
#include "WorldManager.h"

LightOrb* LightOrbDLL()
{
	LightOrb* instance = new LightOrb();
	return instance;
}

LightOrb::LightOrb()
{
	
}

// Use this for initialization before Start()
void LightOrb::Awake()
{
	EnemyDrop::Awake();

	for (const auto ch : owner->children)
	{
		if (ch->tag == "small_texture_light_orb_VFX")
		{
			small_texture_light_orb = static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
			small_texture_light_orb->loop = true;
			small_texture_light_orb->Disable();
		}
		if (ch->tag == "shine_texture_light_orb_VFX")
		{
			shine_texture_light_orb = static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
			shine_texture_light_orb->loop = true;
			shine_texture_light_orb->Disable();
		}	
		if (ch->tag == "exterior_texture_light_orb_VFX")
		{
			exterior_texture_light_orb= static_cast<ComponentBillboard*>(ch->GetComponent(Component::ComponentType::BILLBOARD));
			exterior_texture_light_orb->loop = true;
			exterior_texture_light_orb->Disable();
		}
		
		if (ch->tag == "trail_light_orb_VFX")
		{
			trail_light_orb_VFX = static_cast<ComponentTrail*>(ch->GetComponent(Component::ComponentType::TRAIL));
			trail_light_orb_VFX->Disable();
		}
	}
}

// Update is called once per frame
void LightOrb::Update()
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
		small_texture_light_orb->Disable();
		shine_texture_light_orb->Disable();
		exterior_texture_light_orb->Disable();

		owner->children[0]->SetEnabled(false);

		if (picked)
		{
			PlayerController* player_to_deliver_controller = static_cast<PlayerController*>(player_to_deliver->player_go->GetComponentScript("PlayerController")->script);
			player_to_deliver_controller->Heal(healing_value);
			player_to_deliver_controller->OrbGettedByPlayer();
			audio_source->PlayEvent("play_orbs_pickup");
			player_to_deliver_controller->EmitHealVFX();
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

void LightOrb::Spawn(const float3& spawn_position)
{
	EnemyDrop::Spawn(spawn_position);

	owner->children[0]->SetEnabled(true);

	small_texture_light_orb->Play();
	shine_texture_light_orb->Play();
	exterior_texture_light_orb->Play();
	trail_light_orb_VFX->Enable();

}

void LightOrb::CloneProperties(LightOrb* original_orb)
{
	EnemyDrop::CloneProperties(original_orb);
	
	healing_value = original_orb->healing_value;
	special_value = original_orb->special_value;
}

// Use this for showing variables on inspector
void LightOrb::OnInspector(ImGuiContext* context)
{
	EnemyDrop::OnInspector(context);

	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), "Healing");
	ImGui::Spacing();
	ImGui::DragFloat("Healing Value", &healing_value);
	ImGui::DragFloat("Special Value", &special_value);
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
void LightOrb::Save(Config& config) const
{
	EnemyDrop::Save(config);

	config.AddFloat(healing_value, "HealingValue");
	config.AddFloat(special_value, "SpecialValue");
}

// //Use this for linking GO AND VARIABLES automatically
void LightOrb::Load(const Config& config)
{
	EnemyDrop::Load(config);

	healing_value = config.GetFloat("HealingValue", 1.f);
	special_value = config.GetFloat("SpecialValue", 0.1f);
}
