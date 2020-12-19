#include "NewMushdoomStateAttack.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"

#include "EnemyManager.h"
#include "NewMushdoom.h"

NewMushdoomStateAttack::NewMushdoomStateAttack(NewMushdoom* enemy) : EnemyState(enemy)
{
	strcpy_s(state_name, "Attack");
}

void NewMushdoomStateAttack::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Attack"))
	{
		enemy->animation->ActiveAnimation("attack");
		enemy->audio_source->PlayEvent("play_make_attack");
	}
	attacked = true;
}

void NewMushdoomStateAttack::OnStateUpdate()
{
	float current_percentage = enemy->animation->GetCurrentClipPercentatge();

	if (current_percentage >= attack_damage_start && current_percentage <= attack_damage_end && enemy->PlayerInAttackRange() && attacked)
	{
		enemy->Attack();
		attacked = false;
		enemy->audio_source->PlayEvent("play_receive_hit");
	}
	else if (current_percentage >= animation_end)
	{
		Exit(enemy->pursue_state);
	}
}

void NewMushdoomStateAttack::OnStateExit()
{
	enemy->is_attacking = false;
}
