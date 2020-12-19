#ifndef  __BOSSSTAGEPHASEFOURTH_H__
#define  __BOSSSTAGEPHASEFOURTH_H__

#include "BossStage.h"
#include <MathGeoLib.h>

class BossAttackAvalanche;
class BossAttackCrystalsCone;
class BossAttackCrystalsGuided;
class BossAttackEarthquake;
class BossAttackSlam;

enum class StatePhaseFour
{
	TRANSITIONING_FROM_PHASE_TWO,
	IDLE,
	AVALANCHE_WHILE_SLAM,
	EARTHQUAKE_WHILE_GUIDED_CRYTALS,
	VULNERABLE
};

class BossStagePhaseFourth : public BossStage
{
public:
	BossStagePhaseFourth(BossController* boss_controller);
	~BossStagePhaseFourth() = default;

	void Update(float delta_time) override;

	std::string GetCurrentStateName() const override;

private:
	/** Functions with the logic of each state (they are all called on the update) */
	void TransitionFromPhaseThreeBehaviour(float delta_time);
	void IdleBehaviour(float delta_time);
	void AvalancheWhileSlamBehaviour(float delta_time);
	void EarthquakeWhileGuidedCrystalsBehaviour(float delta_time);
	void VulnerableBehaviour(float delta_time);
	/***/

	bool StandupToIdle() const;
	void StunBoss();


private:
	std::unique_ptr<BossAttackAvalanche> attack_avalanche;
	std::unique_ptr<BossAttackSlam> attack_slam;

	std::unique_ptr<BossAttackEarthquake> attack_earthquake;
	std::unique_ptr<BossAttackCrystalsGuided> attack_crystals_guided;

	//Idle variables
	float current_time_on_idle = 0.f;

	//Avalanche while slam variables
	bool avalanche_casted = false;
	bool slam_casted = false;
	float current_time_not_slaming = 0.f;

	//Earthquake
	bool earthquake_done = false;
	bool guided_done = false;
	unsigned int maximum_repetition = 3;
	unsigned int current_repetition = 0;
	float current_guided_cooldown = 0.f;
	float total_guided_cooldown = 3000.f;

	//Vulnerable
	bool audio_triggered = false;
	float current_vulnerable_time = 0.f;

	PlayerController::WhichPlayer target_player = PlayerController::WhichPlayer::PLAYER1;

	std::random_device random_device;
	std::mt19937 which_attack_generator;
	std::uniform_int_distribution<> which_attack_distribution;

	StatePhaseFour current_state;
};

#endif __BOSSSTAGEPHASEFOURTH_H__
