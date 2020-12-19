#pragma once

#include "AllyInteractionsScript.h"

class EventManager;
class InputManager;
class TutorialHelper;
class WorldManager;
class ComponentCamera;
class ComponentImage;
struct Player;
class AllyTrigger : public Script
{
public:
	AllyTrigger() = default;
	~AllyTrigger() = default;

	void Awake() override;
	void Start() override;
	void Update() override;


	void OnInspector(ImGuiContext * context);

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void InitPublicGameObjects() override;
	void SetComplain(bool complain);
	void EnableChildCamera(bool enable);

private:
	void Complain(bool player_1_inside, bool player_2_inside);

	void SetPlayersPositions(bool player_1_inside, bool player_2_inside);
	void SetTutorialSkippable(bool player_1_inside, bool player_2_inside);

	AllyInteractionsScript* ally = nullptr;
	WorldManager* world = nullptr;

	Player* player_1;
	Player* player_2 = nullptr;
	GameObject* return_position_player1 = nullptr;
	GameObject* return_position_player2 = nullptr;
	EventManager* event_manager = nullptr;
	//tutorial
	bool do_tutorial = true;
	TutorialHelper* tutorial_helper = nullptr;
	InputManager* input_manager = nullptr;

	//Settings
	float range = 6.0f;
	AllyInteractionsScript::State required_ally_state = AllyInteractionsScript::State::ANY;
	bool complains = false;
	bool dialogue = true;
	DialogueID dialogue_id = DialogueID::INTRO;

	//LightShield
	bool light_shield_tutorial = false;
	ComponentCamera* main_camera = nullptr;

};
extern "C" SCRIPT_API AllyTrigger* AllyTriggerDLL();

