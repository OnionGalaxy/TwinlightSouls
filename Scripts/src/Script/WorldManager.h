#ifndef  __WORLDMANAGER_H__
#define  __WORLDMANAGER_H__

#include "Script.h"

class BossController;
class CameraController;
class ComponentCollider;
class ComponentImage;
class EventManager;
class InputManager;
class LevelSelectionMenuController;
class PlayerController;
class UIManager;

enum class Level
{
	MAIN_MENU,
	FIRST,
	SECOND,
	BOSS
};

struct Player 
{
	GameObject* player_go;
	ComponentScript* player_component;
	PlayerController* player_controller;
};

struct Boss
{
	GameObject* boss_go;
	BossController* boss_controller;
};

class WorldManager : public Script
{
public:
	WorldManager();
	~WorldManager();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void LoadLevel(Level scene_position) const;
	void LoadNextLevel();
	void InitializeHUD(bool enable);

	float GetEnemyAmountMultiplier();
	float GetHealthMultiplier();
	float GetEnemyDamageMultiplier();

	Player* GetPlayer1() const;
	Player* GetPlayer2() const;
	UIManager* GetUIManager() const;
	bool ThereIsBoss() const;
	Boss* GetBoss() const;

	void StartTimer();
	float GetTotalTime() const;

	void DisableHUDReadyLightBall(int player);

private:
	void InitPlayers();
	void InitPlayer1(GameObject* player_go, ComponentScript* player_component, PlayerController* controller);
	void InitPlayer2(GameObject* player_go, ComponentScript* player_component, PlayerController* controller);
	void InitBoss(GameObject* boss_go, BossController* controller);
	void DisablePlayer(GameObject* player, PlayerController* player_controller);

public:
	static bool multiplayer;
	static bool show_boot_screens;
	//Which player is each one
	static bool player1_choice;
	static bool level_selection_activated;
	static bool is_game_finished;

	//Game is paused
	bool on_pause = false;
	static Level current_level;

	//Tries for the game
	int tries = 3;

	static int difficulty;

	float enemy_amount_multiplier_easy = 0.6f;
	float enemy_amount_multiplier_hard = 1.4f;

	float health_multiplier_easy = 1.2f;
	float health_multiplier_hard = 0.8f;

	float enemy_damage_multiplier_easy = 0.75f;
	float enemy_damage_multiplier_hard = 1.2f;


private:
	EventManager* event_manager = nullptr;
	InputManager* input_manager = nullptr;
	UIManager* ui_manager = nullptr;
	bool on_main_menu = false;

	//Player's info
	Player* player_1 = nullptr;
	Player* player_2 = nullptr;

	//Boss
	bool is_boss = false;
	Boss* boss = nullptr;

	bool loading = false;
	bool timer_started = false;
	//statistics
	float starting_time = 0.f;
	GameObject* player1_HUD = nullptr;
	GameObject* player2_HUD = nullptr;

};
extern "C" SCRIPT_API WorldManager* WorldManagerDLL(); //This is how we are going to load the script
#endif