#include "BossOutroCinematic.h"

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

#include "LightBall.h"
#include "Log/EngineLog.h"
#include "WorldManager.h"
#include "EventManager.h"
#include "BossOutroCinematic.h"
#include "Dissolver.h"
namespace
{
	const std::string FINISH_TRIGGER = "Finish";
	const std::string SHOW_LIGHT_BALL = "ShowLightBall";
	const std::string HIDE_LIGHT_BALL = "HideLightBall";
	const std::string DIALOGUE_START = "DialogueStart";
	const std::string DIALOGUE_FINISH = "DialogueFinish";
	const std::string DISSOLVER_START = "DissolverStart";
	const std::string START_LIGHTBALL_SCALE = "StartScalingEffects";
	//dialogue
	const std::string LIGHTBALL_FIRST_DIALOGUE = "I can finally be where I belong";
	const std::string LIGHTBALL_SECOND_DIALOGUE = "We can stop the corruption now!";
	const std::string LIGHTBALL_THIRD_DIALOGUE = "You have saved Niberry, you have       saved us all!";
	const std::string LIGHTBALL_FOURTH_DIALOGUE = "Thank you!";

	//Lightball scale
	const float MAX_PORCENTAGE = 5.0f;
	const float ANIMATION_TIME = 2000.f;
}


BossOutroCinematic::BossOutroCinematic(ModuleScene& scene, ModuleTime& time) : Cinematic(scene, time)
{
	roots_dissolve = static_cast<Dissolver*>(scene_module.GetGameObjectByName("Mesh level")->GetComponentScript("Dissolver")->script);
	event_manager = static_cast<EventManager*>(scene_module.GetGameObjectByName("EventManager")->GetComponentScript("EventManager")->script);
}

// Update is called once per frame
void BossOutroCinematic::Update()
{
	boss_outro_timeline.Update(time_module.delta_time);
	if (lightball_scale_start)
	{
		current_scale_animation_time += time_module.delta_time;
		float progress = math::Lerp(1.f, MAX_PORCENTAGE, (float)current_scale_animation_time / ANIMATION_TIME);;
		light_ball->SetSizeEffects(progress);
		if (progress >= MAX_PORCENTAGE)
		{
			lightball_scale_start = false;
		}
	}
}


void BossOutroCinematic::Play()
{
	CreateBossOutroCinematic();
	boss_outro_timeline.Play();
	EnableTimeline(true);
}

void BossOutroCinematic::Stop()
{
	boss_outro_timeline.Stop();
}

bool BossOutroCinematic::IsPlaying() const
{
	return boss_outro_timeline.IsPlaying();
}


void BossOutroCinematic::CreateBossOutroCinematic()
{
	boss_outro_timeline.tracks.clear();
	
	Timeline::AnimationTrack* player1_track_animation = new Timeline::AnimationTrack(player_1->player_go);
	player1_track_animation->AddKeyframe(0.1f, "Idle");
	boss_outro_timeline.tracks.emplace_back(player1_track_animation);
	if (world_manager->multiplayer)
	{
		Timeline::AnimationTrack* player2_track_animation = new Timeline::AnimationTrack(player_2->player_go);
		player2_track_animation->AddKeyframe(0.1f, "Idle");
		boss_outro_timeline.tracks.emplace_back(player2_track_animation);
	}
	//Lightball 
	GameObject* lightball_one = scene_module.GetGameObjectByName("LB_position004");
	GameObject* lightball_two = scene_module.GetGameObjectByName("LB_position005");
	GameObject* lightball_three = scene_module.GetGameObjectByName("LB_position006");
	GameObject* lightball_four = scene_module.GetGameObjectByName("LB_position007");

	Timeline::TranslationTrack* lightball_track = new Timeline::TranslationTrack(light_ball->owner);
	lightball_track->AddKeyframe(5.f, lightball_one->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(10.f, lightball_two->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(15.f, lightball_three->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(20.f, lightball_one->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(23.f, lightball_four->transform.GetGlobalModelMatrix());
	boss_outro_timeline.tracks.emplace_back(lightball_track);

	Timeline::AudioTrack* lightball_audio_track = new Timeline::AudioTrack(light_ball->owner);
	lightball_audio_track->AddKeyframe(5.f, "play_light_shield_loop");
	lightball_audio_track->AddKeyframe(5.1f, "play_lightball_cast");
	lightball_audio_track->AddKeyframe(10.f, "play_lightball_cast");
	lightball_audio_track->AddKeyframe(15.f, "play_lightball_cast");
	lightball_audio_track->AddKeyframe(20.f, "play_lightball_cast");
	lightball_audio_track->AddKeyframe(23.f, "stop_light_shield_loop");

	boss_outro_timeline.tracks.emplace_back(lightball_audio_track);
	//Camera
	GameObject* camera_one = scene_module.GetGameObjectByName("boss_camera001");
	camera_one->transform.SetGlobalModelMatrix(main_camera->owner->transform.GetGlobalModelMatrix());
	cinematic_camera_holder->transform.SetGlobalModelMatrix(main_camera->owner->transform.GetGlobalModelMatrix());
	Timeline::TranslationTrack* camera_track = new Timeline::TranslationTrack(cinematic_camera_holder);
	camera_one->transform.Translate(float3(0.0f,0.0f,-6.0f));
	camera_track->AddKeyframe(5.f, camera_one->transform.GetGlobalModelMatrix());
	camera_one->transform.LookAt(lightball_two->transform.GetGlobalTranslation());
	camera_track->AddKeyframe(10.f, camera_one->transform.GetGlobalModelMatrix());

	camera_one->transform.LookAt(lightball_three->transform.GetGlobalTranslation());
	camera_track->AddKeyframe(15.f, camera_one->transform.GetGlobalModelMatrix());

	camera_one->transform.LookAt(lightball_four->transform.GetGlobalTranslation());
	camera_one->transform.Translate(float3(0.0f, 0.0f, 6.0f));
	camera_track->AddKeyframe(20.f, camera_one->transform.GetGlobalModelMatrix());
	boss_outro_timeline.tracks.emplace_back(camera_track);

	Timeline::AudioTrack* camera_intro_audio_track = new Timeline::AudioTrack(boss_music);
	camera_intro_audio_track->AddKeyframe(3.0f, "Play_BossOutroMusic");
	boss_outro_timeline.tracks.emplace_back(camera_intro_audio_track);

	//Custom 
	Timeline::CustomTrack* custom_track = new Timeline::CustomTrack(*this);
	custom_track->AddKeyframe(1.f, SHOW_LIGHT_BALL);
	//custom_track->AddKeyframe(25.2f, HIDE_LIGHT_BALL);
	custom_track->AddKeyframe(0.5f, DIALOGUE_START, LIGHTBALL_FIRST_DIALOGUE.c_str());
	custom_track->AddKeyframe(5.0f, DIALOGUE_START, LIGHTBALL_SECOND_DIALOGUE.c_str());
	custom_track->AddKeyframe(10.0f, DIALOGUE_START, LIGHTBALL_THIRD_DIALOGUE.c_str());
	custom_track->AddKeyframe(15.0f, DIALOGUE_START, LIGHTBALL_FOURTH_DIALOGUE.c_str());
	custom_track->AddKeyframe(23.0f, START_LIGHTBALL_SCALE);
	custom_track->AddKeyframe(21.1f, DIALOGUE_FINISH);
	custom_track->AddKeyframe(27.0f, FINISH_TRIGGER);
	custom_track->AddKeyframe(23.5f, DISSOLVER_START, roots_dissolve);
	boss_outro_timeline.tracks.emplace_back(custom_track);	
}

void BossOutroCinematic::Trigger(const std::string & trigger_name, const void* data)
{
	if (FINISH_TRIGGER == trigger_name)
	{
		EnableTimeline(false);
		event_manager->TriggerCredits();
	}
	if (SHOW_LIGHT_BALL == trigger_name)
	{
		light_ball->SetEffectProperties();
		light_ball->SetActiveVFX(true);
	}
	if (HIDE_LIGHT_BALL == trigger_name)
	{
		light_ball->SetActiveVFX(false);
	}
	if (DISSOLVER_START == trigger_name)
	{
		const_cast<Dissolver*>(static_cast<const Dissolver*>(data))->Dissolve();
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
	if (START_LIGHTBALL_SCALE == trigger_name)
	{
		lightball_scale_start = true;
		current_scale_animation_time = 0.0f;
	}
}
