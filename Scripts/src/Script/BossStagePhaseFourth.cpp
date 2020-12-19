#include "BossStagePhaseFourth.h"

#include "BossAttackAvalanche.h"
#include "BossAttackCrystalsGuided.h"
#include "BossAttackEarthquake.h"
#include "BossAttackSlam.h"


namespace
{
	const float total_time_on_idle = 1500.f;
	const float total_time_not_slaming = 1500.f;
	const float attacking_vulnerable_time = 5000.f;
}

BossStagePhaseFourth::BossStagePhaseFourth(BossController* boss_controller) : BossStage(boss_controller)
{
	//Init attacks
	attack_slam = std::make_unique<BossAttackSlam>(boss_controller);
	attack_avalanche = std::make_unique<BossAttackAvalanche>(boss_controller);

	attack_earthquake = std::make_unique<BossAttackEarthquake>(boss_controller);
	attack_crystals_guided = std::make_unique<BossAttackCrystalsGuided>(boss_controller);
	attack_crystals_guided->SetChargingFromGround(600.f);

	which_attack_generator = std::mt19937(random_device());
	which_attack_distribution = std::uniform_int_distribution<>(0, 1);

	current_state = StatePhaseFour::TRANSITIONING_FROM_PHASE_TWO;
	name = "PhaseFour";
}

void BossStagePhaseFourth::Update(float delta_time)
{
	switch (current_state)
	{
		case StatePhaseFour::TRANSITIONING_FROM_PHASE_TWO:
			TransitionFromPhaseThreeBehaviour(delta_time);
			break;

		case StatePhaseFour::IDLE:
			IdleBehaviour(delta_time);
			break;

		case StatePhaseFour::AVALANCHE_WHILE_SLAM:
			AvalancheWhileSlamBehaviour(delta_time);
			break;

		case StatePhaseFour::EARTHQUAKE_WHILE_GUIDED_CRYTALS:
			EarthquakeWhileGuidedCrystalsBehaviour(delta_time);
			break;

		case StatePhaseFour::VULNERABLE:
			VulnerableBehaviour(delta_time);
			break;

		default:
			break;
	}

	boss_controller->UpdateCrystals();
}

std::string BossStagePhaseFourth::GetCurrentStateName() const
{
	switch (current_state)
	{
		case StatePhaseFour::TRANSITIONING_FROM_PHASE_TWO:
			return std::string("TRANSITIONING FROM PHASE TWO");

		case StatePhaseFour::IDLE:
			return std::string("IDLE");

		case StatePhaseFour::AVALANCHE_WHILE_SLAM:
			return std::string("AVALANCHE WHILE SLAM");

		case StatePhaseFour::EARTHQUAKE_WHILE_GUIDED_CRYTALS:
			return std::string("EARTHQUAKE WHILE GUIDED CRYTALS");

		case StatePhaseFour::VULNERABLE:
			return std::string("VULNERABLE");

		default:
			break;
	}

	return std::string();
}

void BossStagePhaseFourth::TransitionFromPhaseThreeBehaviour(float delta_time)
{
	if(StandupToIdle())
	{
		current_state = StatePhaseFour::IDLE;
	}
}

void BossStagePhaseFourth::IdleBehaviour(float delta_time)
{

	if(current_time_on_idle >= total_time_on_idle && boss_controller->IsAnimationOnState("Idle"))
	{

		current_time_on_idle = 0.f;
		/** Use random distribution for choosing which attack follows **/
		if (which_attack_distribution(which_attack_generator) == 0)
		{
			current_state = StatePhaseFour::AVALANCHE_WHILE_SLAM;
		}
		else
		{
			current_state = StatePhaseFour::EARTHQUAKE_WHILE_GUIDED_CRYTALS;
		}

	}
	current_time_on_idle += delta_time;
	StandupToIdle();
}

void BossStagePhaseFourth::AvalancheWhileSlamBehaviour(float delta_time)
{
	//First we cast avalanche
	if(!avalanche_casted && !boss_controller->IsAnimationOnState("First_Avalanche"))
	{
		boss_controller->TriggerBossAnimation("avalanche");
	}

	if(!avalanche_casted && boss_controller->IsAnimationOnState("Avalanche_Cast"))
	{
		boss_controller->TriggerBossAnimation("idle");
		avalanche_casted = true;

		//Activate avalanche attack
		attack_avalanche->ExecuteAttack();
	}

	if(avalanche_casted)
	{
		attack_avalanche->Update(delta_time);
		if(attack_avalanche->IsAttackExecuting())
		{
			//Try slam
			if(!slam_casted)
			{
				if(current_time_not_slaming >= total_time_not_slaming)
				{
					slam_casted = true;
					attack_slam->ExecuteAttack();
					current_time_not_slaming = 0.f;

					if (boss_controller->IsMultiplayer())
					{
						target_player = static_cast<PlayerController::WhichPlayer>(std::rand() % 2 == 0);
					}
				}
				current_time_not_slaming += delta_time;
			}
			else
			{
				attack_slam->Update(delta_time);
			}

			if(slam_casted && boss_controller->IsAnimationOnState("Slam_Right_Waiting"))
			{
				boss_controller->TriggerBossAnimation("return");
				slam_casted = false;
			}

			if(slam_casted && !attack_slam->IsAttackExecuting())
			{
				boss_controller->TriggerBossAnimation("waiting");
			}

			boss_controller->RotateBoss(target_player);

		}
		else
		{
			avalanche_casted = false;
			StunBoss();
		}
	}
}

void BossStagePhaseFourth::EarthquakeWhileGuidedCrystalsBehaviour(float delta_time)
{
	if (current_repetition < maximum_repetition) 
	{
		if(!boss_controller->IsAnimationOnState("Earthquake") && !earthquake_done)
		{
			earthquake_done = true;
			attack_earthquake->ExecuteAttack();
		}
		else if(!guided_done)
		{
			attack_crystals_guided->ExecuteAttack();
			attack_crystals_guided->SetGuidedState(GuidedProjectilesState::COMPUTING_NEW_POSITION);
			guided_done = true;
		}

		if (earthquake_done) 
		{
			if (!attack_earthquake->IsAttackExecuting()) 
			{
				earthquake_done = false;
				++current_repetition;
			}
			else 
			{
				attack_earthquake->Update(delta_time);
			}
		}
		if (guided_done) 
		{
			if (!attack_crystals_guided->IsAttackExecuting())
			{
				if(current_guided_cooldown >= total_guided_cooldown)
				{
					guided_done = false;
					current_guided_cooldown = 0.f;
				}
				current_guided_cooldown += delta_time;
			}
			else
			{
				attack_crystals_guided->Update(delta_time);
			}
		}

	}
	else 
	{
		StunBoss();
		current_repetition = 0;
		boss_controller->UnspawnGuidedEffect();
	}
}

void BossStagePhaseFourth::VulnerableBehaviour(float delta_time)
{
	if (current_vulnerable_time >= attacking_vulnerable_time)
	{
		current_vulnerable_time = 0.f;
		current_state = StatePhaseFour::IDLE;
		boss_controller->TriggerBossAnimation("standup");
		boss_controller->ActiveSound(BossBone::CHEST, "Play_BossStandup");
		boss_controller->vulnerable = false;

		boss_controller->SetEnableVulnerableZone(BossBone::CHEST, false, false);
		boss_controller->SetEnableVulnerableZone(BossBone::HEAD, false, false);

		boss_controller->SetEnableVulnerableZone(BossBone::RIGHT_FOREARM, false, false);
		boss_controller->SetEnableVulnerableZone(BossBone::RIGHT_FINGER, false, false);

		boss_controller->stunned = false;
		audio_triggered = false;
	}
	if (boss_controller->IsAnimationOnState("Boss_Stunned") && boss_controller->GetAnimationPercentage() > 0.5f)
	{
		boss_controller->ComputeIfSmashedPlayer(5000.f, 0, CrowdControl::STUN);
		if (boss_controller->IsMultiplayer())
		{
			boss_controller->ComputeIfSmashedPlayer(5000.f, 1, CrowdControl::STUN);
		}
	}

	if (!audio_triggered && boss_controller->IsAnimationOnState("Boss_Stunned") && boss_controller->GetAnimationPercentage() > 0.8f)
	{
		boss_controller->ActiveSound(BossBone::RIGHT_HAND, "Play_BossDown");
		audio_triggered = true;
	}
	current_vulnerable_time += delta_time;
}

bool BossStagePhaseFourth::StandupToIdle() const
{
	if ((boss_controller->IsAnimationOnState("Boss_Standard") && boss_controller->GetAnimationPercentage() >= 0.75f) || boss_controller->IsAnimationOnState("Avalanche_Cast"))
	{
		boss_controller->TriggerBossAnimation("idle");
		return true;
	}

	return false;
}

void BossStagePhaseFourth::StunBoss()
{
	current_state = StatePhaseFour::VULNERABLE;
	boss_controller->vulnerable = true;
	boss_controller->TriggerBossAnimation("stunned");

	boss_controller->SetEnableVulnerableZone(BossBone::CHEST, false, true);
	boss_controller->SetEnableVulnerableZone(BossBone::HEAD, true, true);

	boss_controller->SetEnableVulnerableZone(BossBone::RIGHT_FOREARM, false, true);
	boss_controller->SetEnableVulnerableZone(BossBone::RIGHT_FINGER, false, true);

	boss_controller->stunned = true;
}
