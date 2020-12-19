#include "BiterStateDie.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentParticleSystem.h"

#include "Module/ModuleDebugDraw.h"

#include "Main/Application.h"
#include "Module/ModuleTime.h"

#include "Biter.h"
#include "DebugModeScript.h"

BiterStateDie::BiterStateDie(Biter* enemy) : EnemyState(enemy)
{
	strcpy_s(state_name, "Die");
}

void BiterStateDie::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Die"))
	{
		enemy->animation->ActiveAnimation("die");
	}

	enemy->audio_source->PlayEvent("Play_Biter_Death");

	enemy->ChangeAttackEffects(Biter::BiterAttackEffects::MOVING);
	enemy->DisableAllEffects();
	enemy->is_dying = true;
}

void BiterStateDie::OnStateUpdate()
{
	enemy->current_position -= float3(0.f, 0.007f, 0.f);
	enemy->ExplosionStatus();

	if (!activate_timer && enemy->animation->IsOnState("Die") && enemy->animation->GetCurrentClipPercentatge() >= animation_end)
	{
		activate_timer = true;
	}

	if (activate_timer)
	{
		current_time += enemy->App->time->delta_time;

		if (current_time > (seconds_to_disappear/* * 1000*/))
		{
			activate_timer = false;
			current_time = 0;
			OnStateExit();
		}
	}
}

void BiterStateDie::OnStateExit()
{
	enemy->enemy_manager->KillEnemy(enemy);
	enemy->charge_current_cooldown = enemy->charge_cooldown;
	enemy->explosion_current_cooldown = enemy->explosion_cooldown;
}