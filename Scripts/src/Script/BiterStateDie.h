#ifndef  __BITERSTATEDIE_H__
#define  __BITERSTATEDIE_H__

#include "EnemyState.h"

class Biter;

class BiterStateDie : public EnemyState<Biter>
{
public:
	BiterStateDie(Biter* enemy);
	~BiterStateDie() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	bool activate_timer = false;
	float current_time = 0.f;
	int seconds_to_disappear = 1;
};

#endif