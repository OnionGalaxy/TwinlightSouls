#ifndef  __NEWMUSHDOOMSTATEIDLE_H__
#define  __NEWMUSHDOOMSTATEIDLE_H__

#include "EnemyState.h"

class NewMushdoom;

class NewMushdoomStateIdle : public EnemyState<NewMushdoom>
{
public:
	NewMushdoomStateIdle(NewMushdoom* enemy);
	~NewMushdoomStateIdle() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;
};

#endif