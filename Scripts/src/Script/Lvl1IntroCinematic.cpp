#include "Lvl1IntroCinematic.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"
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
#include "CameraShake.h"
#include "EventManager.h"
#include "LightBall.h"
#include "Log/EngineLog.h"
#include "PlayerController.h"
#include "PlayerMovement.h"
#include "ProgressBar.h"
#include "UIManager.h"
#include "WorldManager.h"
#include "Lvl1IntroCinematic.h"

namespace
{
	const std::string FINISH_TRIGGER = "Finish";
}


Lvl1IntroCinematic::Lvl1IntroCinematic(ModuleScene& scene, ModuleTime& time) : Cinematic(scene, time)
{
	event_manager = static_cast<EventManager*>(scene_module.GetGameObjectByName("EventManager")->GetComponentScript("EventManager")->script);
	event_manager->outro_lvl1_finished = false;
	ui_manager = static_cast<UIManager*>(scene_module.GetGameObjectByName("UIManager")->GetComponentScript("UIManager")->script);
}

// Update is called once per frame
void Lvl1IntroCinematic::Update()
{
	lvl1_intro_timeline.Update(time_module.delta_time);
}


void Lvl1IntroCinematic::Play()
{
	CreateLvl1IntroCinematic();
	lvl1_intro_timeline.Play();
	EnableTimeline(true);
}

void Lvl1IntroCinematic::Stop()
{
	lvl1_intro_timeline.Stop();
}

bool Lvl1IntroCinematic::IsPlaying() const
{
	return lvl1_intro_timeline.IsPlaying();

}

void Lvl1IntroCinematic::CreateLvl1IntroCinematic()
{
	lvl1_intro_timeline.tracks.clear();
	
	//Camera path
	GameObject* path_1 = scene_module.GetGameObjectByName("intro1");
	GameObject* path_2 = scene_module.GetGameObjectByName("intro2");
	GameObject* path_2_2 = scene_module.GetGameObjectByName("intro2_2");

	GameObject* path_3 = scene_module.GetGameObjectByName("intro3");
	GameObject* path_4 = scene_module.GetGameObjectByName("intro4");
	GameObject* path_5 = scene_module.GetGameObjectByName("intro5");
	GameObject* path_6 = scene_module.GetGameObjectByName("intro6");
	GameObject* path_7 = scene_module.GetGameObjectByName("intro7");
	GameObject* path_8 = scene_module.GetGameObjectByName("intro8");
	GameObject* path_9 = scene_module.GetGameObjectByName("intro9");
	GameObject* path_10 = scene_module.GetGameObjectByName("intro10");
	GameObject* path_11 = scene_module.GetGameObjectByName("intro11");
	GameObject* path_12 = scene_module.GetGameObjectByName("intro12");
	GameObject* path_13 = scene_module.GetGameObjectByName("intro13");
	GameObject* path_14 = scene_module.GetGameObjectByName("intro14");
	GameObject* path_15 = scene_module.GetGameObjectByName("intro15");
	GameObject* path_16 = scene_module.GetGameObjectByName("intro16");
	GameObject* path_17 = scene_module.GetGameObjectByName("intro17");
	GameObject* path_18 = scene_module.GetGameObjectByName("intro18");
	GameObject* path_19 = scene_module.GetGameObjectByName("intro19");
	GameObject* path_20 = scene_module.GetGameObjectByName("intro20");
	GameObject* path_21 = scene_module.GetGameObjectByName("intro21");
	GameObject* path_22 = scene_module.GetGameObjectByName("intro22");
	GameObject* begin_of_outro = scene_module.GetGameObjectByName("outro1");

	p1_entrance = scene_module.GetGameObjectByName("p1_begin");
	p2_entrance = scene_module.GetGameObjectByName("p2_begin");

	p1_end = scene_module.GetGameObjectByName("p1_intro_end");
	p2_end = scene_module.GetGameObjectByName("p2_intro_end");

	//Camera
	GameObject* camera = scene_module.GetGameObjectByName("Timeline Camera");
	float4x4 first_position_camera = path_1->transform.GetGlobalModelMatrix();
	Timeline::TranslationTrack* camera_track = new Timeline::TranslationTrack(cinematic_camera_holder);
	//camera_track->AddKeyframe(1.f, path_1->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(1.f, path_2->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(2.f, path_2_2->transform.GetGlobalModelMatrix());

	camera_track->AddKeyframe(3.f, path_3->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(4.f, path_4->transform.GetGlobalModelMatrix());

	camera_track->AddKeyframe(5.f, path_5->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(6.f, path_6->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(7.f, path_7->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(8.f, path_8->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(9.f, path_9->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(10.f, path_10->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(11.f, path_11->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(12.f, path_12->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(13.f, path_13->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(14.f, path_14->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(15.f, path_15->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(16.f, path_16->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(17.f, path_17->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(18.f, path_18->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(19.f, path_19->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(20.f, path_20->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(21.f, path_21->transform.GetGlobalModelMatrix());
	//camera_track->AddKeyframe(23.f, path_22->transform.GetGlobalModelMatrix());

	lvl1_intro_timeline.tracks.emplace_back(camera_track);

	

	//Players
	Timeline::AnimationTrack* player1_track_animation = new Timeline::AnimationTrack(player_1->player_go);
	player1_track_animation->AddKeyframe(16.f, "Run");
	player1_track_animation->AddKeyframe(20.f, "Idle");
	lvl1_intro_timeline.tracks.emplace_back(player1_track_animation);

	Timeline::TranslationTrack* player1_track = new Timeline::TranslationTrack(player_1->player_go);
	player_1->player_go->transform.SetGlobalModelMatrix(p1_entrance->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(16.f, p1_entrance->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(20.f, p1_end->transform.GetGlobalModelMatrix());
	lvl1_intro_timeline.tracks.emplace_back(player1_track);


	if (world_manager->multiplayer)
	{
		Timeline::AnimationTrack* player2_track_animation = new Timeline::AnimationTrack(player_2->player_go);
		player2_track_animation->AddKeyframe(16.f, "Run");
		player2_track_animation->AddKeyframe(20.f, "Idle");
		lvl1_intro_timeline.tracks.emplace_back(player2_track_animation);

		Timeline::TranslationTrack* player2_track = new Timeline::TranslationTrack(player_2->player_go);
		player_2->player_go->transform.SetGlobalModelMatrix(p2_entrance->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(16.f, p2_entrance->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(20.f, p2_end->transform.GetGlobalModelMatrix());
		lvl1_intro_timeline.tracks.emplace_back(player2_track);
	}

	

	//Custom 
	Timeline::CustomTrack* custom_track = new Timeline::CustomTrack(*this);
	custom_track->AddKeyframe(24.3f, FINISH_TRIGGER);
	lvl1_intro_timeline.tracks.emplace_back(custom_track);

	


}

void Lvl1IntroCinematic::Trigger(const std::string & trigger_name, const void* data)
{
	if (FINISH_TRIGGER == trigger_name)
	{
		EnableTimeline(false);
		ui_manager->HideLightballUI(world_manager->multiplayer);
	}
}

