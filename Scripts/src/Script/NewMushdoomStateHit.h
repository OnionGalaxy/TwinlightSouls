#ifndef  __NEWMUSHDOOMSTATEHIT_H__
#define  __NEWMUSHDOOMSTATEHIT_H__

#include "EnemyState.h"

class NewMushdoom;

class NewMushdoomStateHit : public EnemyState<NewMushdoom>
{
public:
	NewMushdoomStateHit(NewMushdoom* enemy);
	~NewMushdoomStateHit() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	float total_percentatge_hitted = 0.95f;
};

#endif