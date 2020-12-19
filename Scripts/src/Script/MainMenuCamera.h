#ifndef  __MAINMENUCAMERA_H__
#define  __MAINMENUCAMERA_H__

#include "Script.h"

class ComponentImage;

class MainMenuCamera : public Script
{
public:
	enum class MainMenuCameraState
	{
		IDLE,
		TRANSITIONING
	};

	enum class MainMenuCameraPresetName
	{
		MAIN_MENU,
		PLAYER_SELECTION,
		LEVEL_1,
		LEVEL_2,
		LEVEL_3,
		CREDITS
	};

	struct MainMenuCameraPreset
	{
		float3 translation;
		Quat rotation;
		bool fade;
	};

	MainMenuCamera();
	~MainMenuCamera() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void TransitionTo(MainMenuCameraPresetName main_menu_camera_preset);

private:
	ComponentImage* background_image = nullptr;
	
	MainMenuCameraState current_state = MainMenuCameraState::IDLE;
	float current_time = 0.f;
	float translation_time = 500.f;

	MainMenuCameraPresetName current_preset = MainMenuCameraPresetName::MAIN_MENU;
	MainMenuCameraPresetName next_preset = MainMenuCameraPresetName::PLAYER_SELECTION;

	std::vector<MainMenuCameraPreset> main_menu_camera_presets;

};
extern "C" SCRIPT_API MainMenuCamera* MainMenuCameraDLL(); //This is how we are going to load the script
#endif