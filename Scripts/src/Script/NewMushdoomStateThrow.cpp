#include "NewMushdoomStateThrow.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentParticleSystem.h"

#include "Main/GameObject.h"

#include "EnemyManager.h"
#include "NewMushdoom.h"
#include "NewMushdoomBall.h"

NewMushdoomStateThrow::NewMushdoomStateThrow(NewMushdoom* enemy) : EnemyState(enemy)
{
	strcpy(state_name, "Throw");
}

void NewMushdoomStateThrow::OnStateEnter()
{
	if (enemy->animation) 
	{
		if(!enemy->animation->IsOnState("Throw"))
		{
			enemy->animation->ActiveAnimation("throw");
		}
	}
	throw_ball = false;

	enemy->RotateTowardsPlayer();
}

void NewMushdoomStateThrow::OnStateUpdate()
{
	float current_percentage = enemy->animation->GetCurrentClipPercentatge();

	if (current_percentage < throw_delay)
	{
		enemy->RotateTowardsPlayer();
	}
	else if (current_percentage >= throw_delay && !throw_ball)
	{
		enemy->mushdoom_ball->owner->SetEnabled(true);
		enemy->mushdoom_ball->ResetBall();
		enemy->audio_source->PlayEvent("play_throw");
		if (enemy->throw_effect)
		{
			enemy->throw_effect->Stop();
			enemy->throw_effect->active = false;
		}
		throw_ball = true;
	}
	else if (current_percentage >= finish_animation && throw_ball)
	{
		Exit(enemy->pursue_state);
	}
}

void NewMushdoomStateThrow::OnStateExit()
{
	enemy->is_throwing = false;
}
