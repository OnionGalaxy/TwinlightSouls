#include "Cinematic.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "AudioManager.h"
#include "BossController.h"
#include "CameraShake.h"
#include "LightBall.h"
#include "Log/EngineLog.h"
#include "PlayerController.h"
#include "PlayerMovement.h"
#include "ProgressBar.h"
#include "UIManager.h"
#include "WorldManager.h"

Cinematic::Cinematic(ModuleScene& scene, ModuleTime& time) : scene_module(scene), time_module(time)
{
	world_manager = static_cast<WorldManager*>(scene_module.GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	GameObject* audio_manager_go = scene_module.GetGameObjectByName("AudioManager");
	ComponentScript* audio_manager_component = audio_manager_go->GetComponentScript("AudioManager");
	dialogue_manager = scene_module.GetGameObjectByName("Dialogue Manager");
	dialogue_text = static_cast<ComponentText*>(dialogue_manager->children[0]->GetComponent(Component::ComponentType::UI_TEXT));

	player_1 = world_manager->GetPlayer1();
	player_2 = world_manager->GetPlayer2();

	boss = world_manager->GetBoss();

	light_ball = static_cast<LightBall*>(scene_module.GetGameObjectByName("LightBall")->GetComponentScript("LightBall")->script);
	GameObject* camera_go = scene_module.GetGameObjectByName("Timeline Camera");
	cinematic_camera = static_cast<ComponentCamera*>(camera_go->GetComponent(Component::ComponentType::CAMERA));
	cinematic_camera_holder = camera_go->parent;
	camera_shake = static_cast<CameraShake*>(camera_go->GetComponentScript("CameraShake")->script);
	main_camera = static_cast<ComponentCamera*>(scene_module.GetGameObjectByName("Main Camera")->GetComponent(Component::ComponentType::CAMERA));

	boss_music = scene_module.GetGameObjectByName("TimelineManager");
}

void Cinematic::EnableTimeline(bool enable)
{
	world_manager->on_pause = enable;
	SetMainCamera(enable);

	world_manager->InitializeHUD(!enable);
	if (boss)
	{
		boss->boss_controller->boss_health_bar->owner->SetEnabled(!enable);
	}
}

void Cinematic::SetMainCamera(bool cinematic) const
{
	if (!cinematic)
	{
		main_camera->depth = 5.f;
		cinematic_camera->depth = 0.f;
	}
	else
	{
		cinematic_camera->depth = 5.f;
		main_camera->depth = 0.f;
	}
}