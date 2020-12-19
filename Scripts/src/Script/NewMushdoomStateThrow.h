#ifndef  __MUSHDOOMSTATETHROW_H__
#define  __MUSHDOOMSTATETHROW_H__

#include "EnemyState.h"

class NewMushdoom;

class NewMushdoomStateThrow : public EnemyState<NewMushdoom>
{
public:
	NewMushdoomStateThrow(NewMushdoom* enemy);
	~NewMushdoomStateThrow() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	float throw_delay = 0.75f;
	float finish_animation = 0.85f;

	bool throw_ball = false;
};

#endif