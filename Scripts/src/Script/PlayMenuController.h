#ifndef  __PLAYMENUCONTROLLER_H__
#define  __PLAYMENUCONTROLLER_H__

#include "MainMenuSubMenuController.h"
#include "MainMenuCamera.h"

class ComponentAnimation;
class ComponentAudioSource;
class ComponentMeshRenderer;
class ComponentImage;

class InputManager;
class LevelSelectionArrows;
class MainMenuButton;
class WorldManager;

class PlayMenuController : public MainMenuSubMenuController
{
public:
	enum class PlayMenuControllerState
	{
		SHOWING,
		PLAYER_SELECTION,
		PLAYER_SELECTION_TO_LEVEL_SELECTION,
		LEVEL_SELECTION,
		LEVEL_SELECTION_TO_PLAYER_SELECTION,
		HIDING,
		HIDDEN
	};

	struct PlayerSelector
	{
		ComponentAnimation* player_animator = nullptr;
		ComponentMeshRenderer* player_mesh = nullptr;
		float3 indicator_position = float3::zero;
	};

	struct LevelSelector
	{
		MainMenuButton* level_title = nullptr;
		MainMenuButton* level_description = nullptr;
		MainMenuCamera::MainMenuCameraPresetName camera_preset;
	};

	PlayMenuController();
	~PlayMenuController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Show() override;
	void FastHide() override;

private:
	void SelectPlayer();
	void SelectLevel(int new_level);

private:
	WorldManager* world_manager = nullptr;
	InputManager* input_manager = nullptr;
	MainMenuCamera* main_menu_camera = nullptr;

	ComponentAudioSource* menu_audio_source = nullptr;

	ComponentImage* background_vignette = nullptr;

	// ================================================

	ComponentImage* player_selection_title = nullptr;

	MainMenuButton* player_selection_arrow_button_controller = nullptr;
	MainMenuButton* player_selection_arrow_button_keyboard = nullptr;
	MainMenuButton* player_selection_arrow_button = nullptr;

	MainMenuButton* player_selection_confirm_button_controller = nullptr;
	MainMenuButton* player_selection_confirm_button_keyboard = nullptr;
	MainMenuButton* player_selection_confirm_button = nullptr;

	MainMenuButton* player_selection_back_button_controller = nullptr;
	MainMenuButton* player_selection_back_button_keyboard = nullptr;
	MainMenuButton* player_selection_back_button = nullptr;

	MainMenuButton* p1_selection_button = nullptr;
	MainMenuButton* p2_selection_button_aux = nullptr;
	MainMenuButton* p2_selection_button_start = nullptr;
	MainMenuButton* p2_selection_button = nullptr;

	int selected_player = 0;
	bool multiplayer = false;
	PlayerSelector furro_selector;
	PlayerSelector furra_selector;

	PlayerSelector* selected_player_selector = nullptr;
	PlayerSelector* other_selected_player_selector = nullptr;

	// ================================================

	ComponentImage* level_selection_title = nullptr;
	std::vector<LevelSelector> level_selectors;
	int selected_level = 0;

	LevelSelectionArrows* level_selection_left_arrow_controller = nullptr;
	LevelSelectionArrows* level_selection_left_arrow_keyboard = nullptr;
	LevelSelectionArrows* level_selection_left_arrow = nullptr;

	LevelSelectionArrows* level_selection_right_arrow_controller = nullptr;
	LevelSelectionArrows* level_selection_right_arrow_keyboard = nullptr;
	LevelSelectionArrows* level_selection_right_arrow = nullptr;

	MainMenuButton* level_selection_confirm_button_controller = nullptr;
	MainMenuButton* level_selection_confirm_button_keyboard = nullptr;
	MainMenuButton* level_selection_confirm_button = nullptr;

	MainMenuButton* level_selection_back_button_controller = nullptr;
	MainMenuButton* level_selection_back_button_keyboard = nullptr;
	MainMenuButton* level_selection_back_button = nullptr;

	// ================================================

	PlayMenuControllerState current_state = PlayMenuControllerState::HIDDEN;
	float current_time = 0.f;
	float button_transition_time = 125.f;
	float menu_transition_time = 500.f;
};
extern "C" SCRIPT_API PlayMenuController* PlayMenuControllerDLL(); //This is how we are going to load the script
#endif