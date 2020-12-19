#ifndef  __NEWMUSHDOOMSTATEPURSUE_H__
#define  __NEWMUSHDOOMSTATEPURSUE_H__

#include "EnemyState.h"

class NewMushdoom;

class NewMushdoomStatePursue : public EnemyState<NewMushdoom>
{
public:
	NewMushdoomStatePursue(NewMushdoom* enemy);
	~NewMushdoomStatePursue() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;
};

#endif