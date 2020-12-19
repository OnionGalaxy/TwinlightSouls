#ifndef  __NEWMUSHDOOMSTATEATTACK_H__
#define  __NEWMUSHDOOMSTATEATTACK_H__

#include "EnemyState.h"

class NewMushdoom;

class NewMushdoomStateAttack : public EnemyState<NewMushdoom>
{
public:
	NewMushdoomStateAttack(NewMushdoom* enemy);
	~NewMushdoomStateAttack() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	float attack_damage_start = 0.4f;
	float attack_damage_end = 0.6f;

	bool attacked = false;
};

#endif