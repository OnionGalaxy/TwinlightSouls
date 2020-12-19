#ifndef  __NEWMUSHDOOMSTATEDIE_H__
#define  __NEWMUSHDOOMSTATEDIE_H__

#include "EnemyState.h"

class NewMushdoom;

class NewMushdoomStateDie : public EnemyState<NewMushdoom>
{
public:
	NewMushdoomStateDie(NewMushdoom* enemy);
	~NewMushdoomStateDie() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	bool activate_timer = false;
	float current_time = 0.f;
	int seconds_to_disappear = 1;
};

#endif