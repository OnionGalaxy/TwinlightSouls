#include "Lvl2IntroCinematic.h"

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
#include "LightBall.h"
#include "Log/EngineLog.h"
#include "PlayerController.h"
#include "PlayerMovement.h"
#include "ProgressBar.h"
#include "UIManager.h"
#include "WorldManager.h"
#include "Lvl2IntroCinematic.h"

namespace
{
	const std::string FINISH_TRIGGER = "Finish";
	const std::string LIGHTBALL_FIRST_DIALOGUE = "I sense that something's wrong...";
	const std::string LIGHTBALL_SECOND_DIALOGUE = "Be very careful, this place is dangerous!";
	const std::string LIGHTBALL_THIRD_DIALOGUE = "Let's get to the end of the cave!";

	const std::string SHOW_LIGHT_BALL = "ShowLightBall";
	const std::string HIDE_LIGHT_BALL = "HideLightBall";
	const std::string DIALOGUE_START = "DialogueStart";
	const std::string DIALOGUE_FINISH = "DialogueFinish";
}


Lvl2IntroCinematic::Lvl2IntroCinematic(ModuleScene& scene, ModuleTime& time) : Cinematic(scene, time)
{
	ui_manager = static_cast<UIManager*>(scene_module.GetGameObjectByName("UIManager")->GetComponentScript("UIManager")->script);
}

// Update is called once per frame
void Lvl2IntroCinematic::Update()
{
	lvl2_intro_timeline.Update(time_module.delta_time);
}


void Lvl2IntroCinematic::Play()
{
	CreateLvl2IntroCinematic();
	lvl2_intro_timeline.Play();
	EnableTimeline(true);
	if (world_manager->multiplayer)
	{
		int player = 1;
		ui_manager->FillLightballUI(static_cast<uint32_t>(player), 0.f);
	}
}

void Lvl2IntroCinematic::Stop()
{
	lvl2_intro_timeline.Stop();
}

bool Lvl2IntroCinematic::IsPlaying() const 
{
	return lvl2_intro_timeline.IsPlaying();

}

void Lvl2IntroCinematic::CreateLvl2IntroCinematic()
{
	lvl2_intro_timeline.tracks.clear();

	//Camera path
	GameObject* path_1 = scene_module.GetGameObjectByName("intro1");
	GameObject* path_2 = scene_module.GetGameObjectByName("intro2");
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

	//Camera
	GameObject* camera = scene_module.GetGameObjectByName("Timeline Camera");
	Timeline::TranslationTrack* camera_track = new Timeline::TranslationTrack(cinematic_camera_holder);
	camera_track->AddKeyframe(1.f, path_1->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(2.f, path_2->transform.GetGlobalModelMatrix());
	//camera_track->AddKeyframe(4.f, path_3->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(3.f, path_4->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(4.f, path_5->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(5.f, path_6->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(6.f, path_7->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(7.f, path_8->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(8.f, path_9->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(9.f, path_10->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(10.f, path_11->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(11.f, path_12->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(12.f, path_13->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(13.f, path_14->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(14.f, path_15->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(15.f, path_16->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(16.f, path_17->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(17.5f, path_18->transform.GetGlobalModelMatrix());
	/*camera_track->AddKeyframe(20.f, path_19->transform.GetGlobalModelMatrix());*/

	lvl2_intro_timeline.tracks.emplace_back(camera_track);

	//Players

	if (!world_manager->multiplayer)
	{
		p1_entrance = scene_module.GetGameObjectByName("p1_single_begin");
		p1_end = scene_module.GetGameObjectByName("p1_single_end");
	}

	else
	{
		Timeline::AnimationTrack* player2_track_animation = new Timeline::AnimationTrack(player_2->player_go);
		player2_track_animation->AddKeyframe(16.f, "Run");
		player2_track_animation->AddKeyframe(18.5f, "Idle");
		lvl2_intro_timeline.tracks.emplace_back(player2_track_animation);

		p1_entrance = scene_module.GetGameObjectByName("p1_begin");
		p2_entrance = scene_module.GetGameObjectByName("p2_begin");

		p1_end = scene_module.GetGameObjectByName("p1_end");
		p2_end = scene_module.GetGameObjectByName("p2_end");

		Timeline::TranslationTrack* player2_track = new Timeline::TranslationTrack(player_2->player_go);
		player_2->player_go->transform.SetGlobalModelMatrix(p2_entrance->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(16.f, p2_entrance->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(18.5f, p2_end->transform.GetGlobalModelMatrix());
		lvl2_intro_timeline.tracks.emplace_back(player2_track);
	}
	
	Timeline::AnimationTrack* player1_track_animation = new Timeline::AnimationTrack(player_1->player_go);
	player1_track_animation->AddKeyframe(16.f, "Run");
	player1_track_animation->AddKeyframe(18.5f, "Idle");
	lvl2_intro_timeline.tracks.emplace_back(player1_track_animation);

	Timeline::TranslationTrack* player1_track = new Timeline::TranslationTrack(player_1->player_go);
	player_1->player_go->transform.SetGlobalModelMatrix(p1_entrance->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(16.f, p1_entrance->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(18.5f, p1_end->transform.GetGlobalModelMatrix());
	lvl2_intro_timeline.tracks.emplace_back(player1_track);

	

	//Lightball
	GameObject* lightball_pos = scene_module.GetGameObjectByName("lightball_intro_pos");
	Timeline::TranslationTrack* lb_track = new Timeline::TranslationTrack(light_ball->owner);
	lb_track->AddKeyframe(16.f, player_1->player_go->transform.GetGlobalModelMatrix());
	lb_track->AddKeyframe(18.8f, lightball_pos->transform.GetGlobalModelMatrix());
	lb_track->AddKeyframe(29.8f, lightball_pos->transform.GetGlobalModelMatrix());
	lb_track->AddKeyframe(30.8f, p1_end->transform.GetGlobalModelMatrix());
	lvl2_intro_timeline.tracks.emplace_back(lb_track);


	//Custom 
	Timeline::CustomTrack* custom_track = new Timeline::CustomTrack(*this);
	custom_track->AddKeyframe(0.1f, HIDE_LIGHT_BALL);
	custom_track->AddKeyframe(18.f, SHOW_LIGHT_BALL);
	custom_track->AddKeyframe(19.8f, DIALOGUE_START, LIGHTBALL_FIRST_DIALOGUE.c_str());
	custom_track->AddKeyframe(23.8f, DIALOGUE_START, LIGHTBALL_SECOND_DIALOGUE.c_str());
	custom_track->AddKeyframe(26.8f, DIALOGUE_START, LIGHTBALL_THIRD_DIALOGUE.c_str());

	custom_track->AddKeyframe(29.8f, DIALOGUE_FINISH);
	custom_track->AddKeyframe(30.8f, HIDE_LIGHT_BALL);
	custom_track->AddKeyframe(31.5f, FINISH_TRIGGER);
	lvl2_intro_timeline.tracks.emplace_back(custom_track);

}

void Lvl2IntroCinematic::Trigger(const std::string & trigger_name, const void* data)
{
	if (FINISH_TRIGGER == trigger_name)
	{
		EnableTimeline(false);
		if (world_manager->multiplayer)
		{
			int player = 1;
			ui_manager->FillLightballUI(static_cast<uint32_t>(player), 0.f);
		}
	}

	if (SHOW_LIGHT_BALL == trigger_name)
	{
		light_ball->SetActiveVFX(true);
	}
	if (HIDE_LIGHT_BALL == trigger_name)
	{
		light_ball->SetActiveVFX(false);
	}

	if (DIALOGUE_START == trigger_name)
	{
		std::string dialogue(static_cast<const char*>(data));
		dialogue_manager->children[0]->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Enable();
		dialogue_text->SetText(dialogue);
	}
	if (DIALOGUE_FINISH == trigger_name)
	{
		dialogue_manager->children[0]->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Disable();
	}
}
