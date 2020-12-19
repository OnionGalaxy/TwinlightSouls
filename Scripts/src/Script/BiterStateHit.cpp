#include "BiterStateHit.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"

#include "Biter.h"
#include "PlayerAttack.h"
#include "PlayerController.h"

BiterStateHit::BiterStateHit(Biter* enemy) : EnemyState(enemy)
{
	strcpy_s(state_name, "Hit");
}

void BiterStateHit::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Hit"))
	{
		enemy->animation->ActiveAnimation("hit");
	}

	enemy->audio_source->PlayEvent("Play_Biter_Hit");
}

void BiterStateHit::OnStateUpdate()
{
	Exit(enemy->pursue_state);
}

void BiterStateHit::OnStateExit()
{
}