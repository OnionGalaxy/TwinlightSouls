#include "NewMushdoomStateIdle.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "NewMushdoom.h"
#include "MushdoomBall.h"
#include "PlayerController.h"

NewMushdoomStateIdle::NewMushdoomStateIdle(NewMushdoom* enemy) : EnemyState(enemy)
{
	strcpy_s(state_name, "Idle");
}

void NewMushdoomStateIdle::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Idle"))
	{
		enemy->animation->ActiveAnimation("idle");
		enemy->audio_source->PlayEvent("play_idle");
	}

}

void NewMushdoomStateIdle::OnStateUpdate()
{
	if (enemy->PlayerInSight())
	{
		enemy->Move();
		if (enemy->is_moving)
		{
			Exit(enemy->pursue_state);
		}
	}
}

void NewMushdoomStateIdle::OnStateExit()
{
	
}
