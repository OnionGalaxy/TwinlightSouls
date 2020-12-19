#ifndef  __OPTIONSMENUCONTROLLER_H
#define  __OPTIONSMENUCONTROLLER_H

#include "Script.h"

#include "Animation/Tween/LOTween.h"
#include "Animation/Tween/Tween.h"
#include "Animation/Tween/TweenSequence.h"


class ComponentAudioSource;
class ComponentButton;
class ComponentTransform2D;
class ComponentText;

class AudioManager;
class InputManager;
class LevelSelectionMenuController;
class MainMenuController;
class WorldManager;

class OptionsMenuController : public Script
{
public:
	OptionsMenuController();
	~OptionsMenuController() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext * context) override;
	void InitPublicGameObjects() override;
	void MoveCursor();
	void ManageVolumeUI();
	void ManageDifficultyUI();

	void Open();
	void Close();

private:
	GameObject* options_panel = nullptr;
	GameObject* main_menu_panel = nullptr;

	bool enabled = false;
	bool just_opened = false;
	int current_button = 0;
	int current_volume = 5;

	GameObject* main_menu_game_object = nullptr;
	MainMenuController* main_menu_controller = nullptr;

	GameObject* back_button_game_object = nullptr;
	ComponentButton* back_button = nullptr;

	GameObject* cursor;
	ComponentTransform2D* cursor_transform;

	GameObject* world_manager_game_object = nullptr;
	WorldManager* world_manager = nullptr;

	GameObject* input_manager_game_object = nullptr;
	InputManager* input_manager = nullptr;

	GameObject* audio_controller = nullptr;
	ComponentAudioSource* audio_source = nullptr;

	GameObject* volume_settings = nullptr;
	GameObject* resolution_settings = nullptr;
	GameObject* difficulty_settings = nullptr;

	GameObject* gamepad_icons = nullptr;
	GameObject* pc_icons = nullptr;

	std::vector <ComponentTransform2D*> cursor_positions;

	//Volume buttons
	GameObject* less_volume_go = nullptr;
	GameObject* more_volume_go = nullptr;
	ComponentButton* less_volume_button = nullptr;
	ComponentButton* more_volume_button = nullptr;


	//Resolution Buttons
	GameObject* less_res_go = nullptr;
	GameObject* more_res_go = nullptr;
	ComponentButton* less_res_button = nullptr;
	ComponentButton* more_res_button = nullptr;

	//Resolution Text
	GameObject* res_text_go = nullptr;
	ComponentText* res_text = nullptr;

	bool set_once = false; //sorry

	AudioManager* audio_manager = nullptr;
	int difficulty;

};
extern "C" SCRIPT_API OptionsMenuController* OptionsMenuControllerDLL(); //This is how we are going to load the script
#endif
