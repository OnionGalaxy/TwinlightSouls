#ifndef  __CREDITSMANAGER_H__
#define  __CREDITSMANAGER_H__

#include "Script.h"

class EventManager;
class UIImageDissolver;
class UIManager;

class CreditsManager : public Script
{
public:
	CreditsManager();
	~CreditsManager() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void InitPublicGameObjects();

	void Enable();
	void Disable();

	void EndCredits();

	void FastHide();

	bool AreCreditsActive() const;

private:
	bool are_credits_active = false;
	bool pay_respect = false;
	std::vector<UIImageDissolver*> teams;
	size_t current_index = 0;
	
	float current_time_on_screen = 0.f;
	float total_time_on_screen = 2500.f;
	const float total_time_teams_on_screen = 9000.f;
	const float next_team_time = 750.f;
	bool next_team = false;

	UIManager* ui_manager = nullptr;
	EventManager* event_manager = nullptr;
};

extern "C" SCRIPT_API CreditsManager* CreditsManagerDLL(); //This is how we are going to load the script

#endif