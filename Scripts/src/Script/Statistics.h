#ifndef  __STATISTICS_H__
#define  __STATISTICS_H__

#include "Script.h"

class ComponentText;
class EnemyDropManager;
class InputManager;
class UIManager;
class WorldManager;

struct Player;

class Statistics : public Script
{
public:
	Statistics();
	~Statistics() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void ShowStatistics();
	void SetTime();

private:
	void InitPanel();
	void UpdatePanel();
	void DisablePanel() const;

private:
	WorldManager* world_manager = nullptr;
	InputManager* input_manager = nullptr;
	UIManager* ui_manager = nullptr;
	EnemyDropManager* light_orbs_manager = nullptr;

	Player* player_1 = nullptr;
	Player* player_2 = nullptr;

	GameObject* statistics_panel = nullptr;

	ComponentText* enemies_killed_player1 = nullptr;
	ComponentText* total_damage_player1 = nullptr;
	ComponentText* orbs_statistics_player1 = nullptr;

	ComponentText* enemies_killed_player2 = nullptr;
	ComponentText* total_damage_player2 = nullptr;
	ComponentText* orbs_statistics_player2 = nullptr;

	ComponentText* player_2_stats = nullptr;

	ComponentText* total_time = nullptr;
	ComponentText* press_next = nullptr;

	bool showing_statistics = false;
	float timer_to_show = 8000.f;
	float current_time = 0.f;

	int player1_kills = 0;
	int player1_orbs = 0;
	float player1_damage = 0.0f;

	int player2_kills = 0;
	int player2_orbs = 0;
	float player2_damage = 0.0f;
	float deploy_time = 0.0f;


};

extern "C" SCRIPT_API Statistics* StatisticsDLL(); //This is how we are going to load the script

#endif