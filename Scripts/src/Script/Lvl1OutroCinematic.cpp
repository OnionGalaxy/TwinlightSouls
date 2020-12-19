#include "Lvl1OutroCinematic.h"

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
#include "Lvl1OutroCinematic.h"

namespace
{
	const std::string FINISH_TRIGGER = "Finish";
	const std::string LIGHTBALL_FIRST_DIALOGUE = "Good! Now let's keep moving!";
	const std::string LIGHTBALL_SECOND_DIALOGUE = "We got to hurry!";
	const std::string SHOW_LIGHT_BALL = "ShowLightBall";
	const std::string HIDE_LIGHT_BALL = "HideLightBall";
	const std::string DIALOGUE_START = "DialogueStart";
	const std::string DIALOGUE_FINISH = "DialogueFinish";
}


Lvl1OutroCinematic::Lvl1OutroCinematic(ModuleScene& scene, ModuleTime& time) : Cinematic(scene, time)
{
	event_manager = static_cast<EventManager*>(scene_module.GetGameObjectByName("EventManager")->GetComponentScript("EventManager")->script);

}

// Update is called once per frame
void Lvl1OutroCinematic::Update()
{
	lvl1_outro_timeline.Update(time_module.delta_time);
}


void Lvl1OutroCinematic::Play()
{
	CreateLvl1OutroCinematic();
	lvl1_outro_timeline.Play();
	EnableTimeline(true);
}

void Lvl1OutroCinematic::Stop()
{
	lvl1_outro_timeline.Stop();
}

bool Lvl1OutroCinematic::IsPlaying() const
{
	return lvl1_outro_timeline.IsPlaying();
}

void Lvl1OutroCinematic::CreateLvl1OutroCinematic()
{
	lvl1_outro_timeline.tracks.clear();

	//Camera path
	GameObject* path_1 = scene_module.GetGameObjectByName("outro1");
	GameObject* path_2 = scene_module.GetGameObjectByName("outro2");

	//Camera
	GameObject* camera = scene_module.GetGameObjectByName("Timeline Camera");
	cinematic_camera_holder->transform.SetGlobalModelMatrix(path_1->transform.GetGlobalModelMatrix());

	float4x4 first_position_camera = path_1->transform.GetGlobalModelMatrix();
	Timeline::TranslationTrack* camera_track = new Timeline::TranslationTrack(cinematic_camera_holder);
	camera_track->AddKeyframe(0.1f, path_1->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(1.f, path_2->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(5.5f, path_2->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(8.f, path_1->transform.GetGlobalModelMatrix());

	lvl1_outro_timeline.tracks.emplace_back(camera_track);


	if (!world_manager->multiplayer)
	{
		p1_pos = scene_module.GetGameObjectByName("single_player_pos");
		p1_to_cave = scene_module.GetGameObjectByName("single_player_pos_2");
		p1_final = scene_module.GetGameObjectByName("single_player_end");

	}
	
	else
	{
		p1_pos = scene_module.GetGameObjectByName("p1_pos");
		p2_pos = scene_module.GetGameObjectByName("p2_pos");

		p1_to_cave = scene_module.GetGameObjectByName("p1_pos_2");
		p2_to_cave = scene_module.GetGameObjectByName("p2_pos_2");

		p1_final = scene_module.GetGameObjectByName("p1_end");
		p2_final = scene_module.GetGameObjectByName("p2_end");

		Timeline::AnimationTrack* player2_track_animation = new Timeline::AnimationTrack(player_2->player_go);
		player2_track_animation->AddKeyframe(0.1f, "Run");
		player2_track_animation->AddKeyframe(1.6f, "Idle");
		player2_track_animation->AddKeyframe(7.6f, "Run");
		player2_track_animation->AddKeyframe(8.2f, "Idle");
		player2_track_animation->AddKeyframe(8.4f, "Run");

		lvl1_outro_timeline.tracks.emplace_back(player2_track_animation);

		Timeline::TranslationTrack* player2_track = new Timeline::TranslationTrack(player_2->player_go);
		player2_track->AddKeyframe(0.1f, player_2->player_go->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(1.6f, p2_pos->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(7.6f, p2_pos->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(8.2f, p2_to_cave->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(10.2f, p2_final->transform.GetGlobalModelMatrix());

		lvl1_outro_timeline.tracks.emplace_back(player2_track);
	}

	Timeline::AnimationTrack* player1_track_animation = new Timeline::AnimationTrack(player_1->player_go);
	player1_track_animation->AddKeyframe(0.1f, "Run");
	player1_track_animation->AddKeyframe(1.6f, "Idle");
	player1_track_animation->AddKeyframe(7.6f, "Run");
	player1_track_animation->AddKeyframe(8.2f, "Idle");
	player1_track_animation->AddKeyframe(8.4f, "Run");

	lvl1_outro_timeline.tracks.emplace_back(player1_track_animation);

	Timeline::TranslationTrack* player1_track = new Timeline::TranslationTrack(player_1->player_go);
	player1_track->AddKeyframe(0.1f, player_1->player_go->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(1.6f, p1_pos->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(7.6f, p1_pos->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(8.2f, p1_to_cave->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(10.2f, p1_final->transform.GetGlobalModelMatrix());

	lvl1_outro_timeline.tracks.emplace_back(player1_track);

	//Lightball
	GameObject* lightball_pos = scene_module.GetGameObjectByName("LB_pos");
	Timeline::TranslationTrack* lb_track = new Timeline::TranslationTrack(light_ball->owner);
	lb_track->AddKeyframe(2.1f, lightball_pos->transform.GetGlobalModelMatrix());
	lb_track->AddKeyframe(7.f, lightball_pos->transform.GetGlobalModelMatrix());
	lb_track->AddKeyframe(8.2f, p1_pos->transform.GetGlobalModelMatrix());
	lvl1_outro_timeline.tracks.emplace_back(lb_track);


	//Custom 
	Timeline::CustomTrack* custom_track = new Timeline::CustomTrack(*this);
	custom_track->AddKeyframe(1.6f, SHOW_LIGHT_BALL);
	custom_track->AddKeyframe(2.f, DIALOGUE_START, LIGHTBALL_FIRST_DIALOGUE.c_str());
	custom_track->AddKeyframe(5.0f, DIALOGUE_START, LIGHTBALL_SECOND_DIALOGUE.c_str());
	custom_track->AddKeyframe(8.1f, DIALOGUE_FINISH);
	custom_track->AddKeyframe(8.4f, HIDE_LIGHT_BALL);


	custom_track->AddKeyframe(10.2f, FINISH_TRIGGER);
	lvl1_outro_timeline.tracks.emplace_back(custom_track);


}

void Lvl1OutroCinematic::Trigger(const std::string & trigger_name, const void* data)
{
	if (FINISH_TRIGGER == trigger_name)
	{
		EnableTimeline(false);
		event_manager->outro_lvl1_finished = true;
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
