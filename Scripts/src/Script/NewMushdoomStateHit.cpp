#include "NewMushdoomStateHit.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentScript.h"

#include "Main/GameObject.h"

#include "NewMushdoom.h"
#include "MushdoomBall.h"
#include "PlayerAttack.h"
#include "PlayerController.h"

NewMushdoomStateHit::NewMushdoomStateHit(NewMushdoom* enemy) : EnemyState(enemy)
{
	strcpy_s(state_name, "Hit");
}

void NewMushdoomStateHit::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Hit"))
	{
		enemy->animation->ActiveAnimation("hity");
	}
}

void NewMushdoomStateHit::OnStateUpdate()
{
	enemy->KnockBack();

	if (enemy->animation->GetCurrentClipPercentatge() >= total_percentatge_hitted)
	{
		Exit(enemy->pursue_state);
	}
}

void NewMushdoomStateHit::OnStateExit()
{
}
