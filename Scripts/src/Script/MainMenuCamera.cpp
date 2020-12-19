#include "MainMenuCamera.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>


MainMenuCamera* MainMenuCameraDLL()
{
	MainMenuCamera* instance = new MainMenuCamera();
	return instance;
}

MainMenuCamera::MainMenuCamera()
{
}

// Use this for initialization before Start()
void MainMenuCamera::Awake()
{
	background_image = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Background")->GetComponent(Component::ComponentType::UI_IMAGE));

	main_menu_camera_presets.push_back({
		float3(84.382f, 10.673f, 75.043f),
		Quat::FromEulerXYZ(math::DegToRad(145.978f), math::DegToRad(-44.185f), math::DegToRad(160.f)),
		false
	});

	main_menu_camera_presets.push_back({
		float3(81.872f, 4.623f, 73.493f),
		Quat::FromEulerXYZ(math::DegToRad(-21.5f), math::DegToRad(145.6f), math::DegToRad(11.4f)),
		false
	});

	main_menu_camera_presets.push_back({
		float3(38.987f, 12.659f, -14.464f),
		Quat::FromEulerXYZ(math::DegToRad(146.395f), math::DegToRad(22.708f), math::DegToRad(-161.6f)),
		true
	});

	main_menu_camera_presets.push_back({
		float3(-30.374f, 6.488f, -73.483f),
		Quat::FromEulerXYZ(math::DegToRad(37.791f), math::DegToRad(-43.746f), math::DegToRad(30.304f)),
		true
	});

	main_menu_camera_presets.push_back({
		float3(-96.632f, 0.606f, -65.655f),
		Quat::FromEulerXYZ(math::DegToRad(134.407f), math::DegToRad(-93.969f), math::DegToRad(135.068f)),
		true
	});

	main_menu_camera_presets.push_back({
	float3(22.425f, 14.853f, 77.379f),
	Quat::FromEulerXYZ(math::DegToRad(152.778f), math::DegToRad(-5.900f), math::DegToRad(180.0f)),
	true
	});
}

// Use this for initialization
void MainMenuCamera::Start()
{

}

// Update is called once per frame
void MainMenuCamera::Update()
{
	switch (current_state)
	{
	case MainMenuCamera::MainMenuCameraState::IDLE:
		break;

	case MainMenuCamera::MainMenuCameraState::TRANSITIONING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / translation_time);

		int previous_preset_int = static_cast<int>(current_preset);
		float3 previous_translation = main_menu_camera_presets[previous_preset_int].translation;
		Quat previous_rotation = main_menu_camera_presets[previous_preset_int].rotation;

		int next_preset_int = static_cast<int>(next_preset);
		float3 next_translation = main_menu_camera_presets[next_preset_int].translation;
		Quat next_rotation = main_menu_camera_presets[next_preset_int].rotation;

		float3 current_translation = float3::Lerp(previous_translation, next_translation, progress);
		owner->transform.SetGlobalMatrixTranslation(current_translation);

		Quat current_rotation = Quat::Slerp(previous_rotation, next_rotation, progress);
		owner->transform.SetGlobalMatrixRotation(current_rotation);

		if (main_menu_camera_presets[next_preset_int].fade || main_menu_camera_presets[previous_preset_int].fade)
		{
			background_image->color.w = math::PingPongMod(progress * 2.f, 1.f);
		}

		if (progress == 1.f)
		{
			current_state = MainMenuCamera::MainMenuCameraState::IDLE;
			current_preset = next_preset;
		}
	}
	break;

	default:
		break;
	}

}

void MainMenuCamera::TransitionTo(MainMenuCameraPresetName main_menu_camera_preset)
{
	next_preset = main_menu_camera_preset;
	current_time = 0.f;
	current_state = MainMenuCamera::MainMenuCameraState::TRANSITIONING;
}