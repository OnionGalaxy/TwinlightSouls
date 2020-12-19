#ifndef  __NEWMUSHDOOMSTATESCREAM_H__
#define  __NEwMUSHDOOMSTATESCREAM_H__

#include "EnemyState.h"

class NewMushdoom;

class NewMushdoomStateScream : public EnemyState<NewMushdoom>
{
public:
	NewMushdoomStateScream(NewMushdoom* enemy);
	~NewMushdoomStateScream() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;
};

#endif