#include "BossIntroCinematic.h"

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
#include "BossController.h"
#include "CameraShake.h"
#include "LightBall.h"
#include "Log/EngineLog.h"
#include "PlayerController.h"
#include "PlayerMovement.h"
#include "ProgressBar.h"
#include "UIManager.h"
#include "WorldManager.h"
#include "Dissolver.h"
#include "BossIntroCinematic.h"

namespace
{
	const std::string FINISH_TRIGGER = "Finish";
	const std::string SHOW_LIGHT_BALL = "ShowLightBall";
	const std::string HIDE_LIGHT_BALL = "HideLightBall";
	const std::string CAMERA_SHAKE = "CameraShake";
	const std::string DIALOGUE_START = "DialogueStart";
	const std::string DIALOGUE_FINISH = "DialogueFinish";
	const std::string DISSOLVER_START = "DissolverStart";

	//dialogue
	const std::string LIGHTBALL_FIRST_DIALOGUE = "The heart of the forest, we've arrived! we can save Niberry!";
	const std::string LIGHTBALL_SECOND_DIALOGUE = "Oh no! Something is not ok...";
	const std::string LIGHTBALL_THIRD_DIALOGUE = "The corruption has taken shape!";
	const std::string LIGHTBALL_FOURTH_DIALOGUE = "We need to fight back, be careful!";
}


BossIntroCinematic::BossIntroCinematic(ModuleScene& scene, ModuleTime& time) : Cinematic(scene, time)
{
	boss_dissolve = static_cast<Dissolver*>(boss->boss_go->GetComponentScript("Dissolver")->script);
	ui_manager = static_cast<UIManager*>(scene_module.GetGameObjectByName("UIManager")->GetComponentScript("UIManager")->script);
	if (boss->boss_go->GetChildrenWithName("Boss1") != nullptr)
	{
		boss_mesh = static_cast<ComponentMeshRenderer*>(boss->boss_go->GetChildrenWithName("Boss1")->GetComponent(Component::ComponentType::MESH_RENDERER));
	}
}

// Update is called once per frame
void BossIntroCinematic::Update()
{
	boss_intro_timeline.Update(time_module.delta_time);
}


void BossIntroCinematic::Play()
{
	CreateBossIntroCinematic();
	boss_intro_timeline.Play();
	EnableTimeline(true);
	if (world_manager->multiplayer)
	{
		int player = 1;
		ui_manager->FillLightballUI(static_cast<uint32_t>(player), 0.f);
	}
}

void BossIntroCinematic::Stop()
{
	is_stoping = true;
	boss_intro_timeline.Stop();
}

bool BossIntroCinematic::IsPlaying() const
{
	return boss_intro_timeline.IsPlaying();
}

void BossIntroCinematic::CreateBossIntroCinematic()
{
	boss_intro_timeline.tracks.clear();
	//Player 1 track
	GameObject* first_bridge_position = scene_module.GetGameObjectByName("first_bridge_position");
	GameObject* start_transform = scene_module.GetGameObjectByName("start_transform");
	GameObject* finish_transform = scene_module.GetGameObjectByName("finish_transform");

	Timeline::TranslationTrack* player1_track = new Timeline::TranslationTrack(player_1->player_go);
	player_1->player_go->transform.SetGlobalModelMatrix(start_transform->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(0.1f, start_transform->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(3.f, first_bridge_position->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(24.f, first_bridge_position->transform.GetGlobalModelMatrix());
	player1_track->AddKeyframe(25.0f, finish_transform->transform.GetGlobalModelMatrix());
	boss_intro_timeline.tracks.emplace_back(player1_track);

	Timeline::AnimationTrack* player1_track_animation = new Timeline::AnimationTrack(player_1->player_go);
	player1_track_animation->AddKeyframe(0.1f, "Run");
	player1_track_animation->AddKeyframe(3.f, "Idle");
	player1_track_animation->AddKeyframe(12.f, "Stunned");
	player1_track_animation->AddKeyframe(16.f, "Idle");
	player1_track_animation->AddKeyframe(23.8f, "Knocked", 100.f);
	player1_track_animation->AddKeyframe(25.0f, "Idle");
	boss_intro_timeline.tracks.emplace_back(player1_track_animation);

	//Player 2 track
	if (world_manager->multiplayer)
	{
		first_bridge_position->transform.Translate(float3(1.7, 0, 0));
		start_transform->transform.Translate(float3(1.7, 0, 0));
		finish_transform->transform.Translate(float3(1.7, 0, 0));
		Timeline::TranslationTrack* player2_track = new Timeline::TranslationTrack(player_2->player_go);
		player_2->player_go->transform.SetGlobalModelMatrix(start_transform->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(0.1f, start_transform->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(3.f, first_bridge_position->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(24.f, first_bridge_position->transform.GetGlobalModelMatrix());
		player2_track->AddKeyframe(25.0f, finish_transform->transform.GetGlobalModelMatrix());
		boss_intro_timeline.tracks.emplace_back(player2_track);

		Timeline::AnimationTrack* player2_track_animation = new Timeline::AnimationTrack(player_2->player_go);
		player2_track_animation->AddKeyframe(0.1f, "Run");
		player2_track_animation->AddKeyframe(3.f, "Idle");
		player2_track_animation->AddKeyframe(12.f, "Stunned");
		player2_track_animation->AddKeyframe(16.f, "Idle");
		player2_track_animation->AddKeyframe(23.8f, "Knocked", 100.f);
		player2_track_animation->AddKeyframe(25.0f, "Idle");
		boss_intro_timeline.tracks.emplace_back(player2_track_animation);
	}

	//Boss
	Timeline::CustomTrack* boss_track_custom = new Timeline::CustomTrack(*this);
	boss_dissolve->SetDissolvingTime(4000.f);
	boss_dissolve->SetStartingDissolveProgress(0.f);
	boss_dissolve->ResetDissolve();
	boss_track_custom->AddKeyframe(16.f, DISSOLVER_START, boss_dissolve);
	boss_intro_timeline.tracks.emplace_back(boss_track_custom);

	if (boss_mesh)
	{
		boss_mesh->active = false;
	}

	Timeline::AnimationTrack* boss_track_animation = new Timeline::AnimationTrack(boss->boss_go);
	boss_track_animation->AddKeyframe(1.f, "Idle");
	boss_track_animation->AddKeyframe(16.f, "Phase_Two_Transition");
	boss_track_animation->AddKeyframe(19.f, "Idle");
	boss_track_animation->AddKeyframe(21.75f, "Slam_Right", 500.f);
	boss_track_animation->AddKeyframe(24.f, "Slam_Right_Return");
	boss_track_animation->AddKeyframe(25.f, "Idle");
	boss_track_animation->AddKeyframe(26.f, "Transition_Three");
	boss_track_animation->AddKeyframe(29.f, "Idle");
	boss_intro_timeline.tracks.emplace_back(boss_track_animation);

	Timeline::AudioTrack* boss_audio_track = new Timeline::AudioTrack(boss->boss_controller->GetBoneGameObject(BossBone::HEAD));
	boss_audio_track->AddKeyframe(16.5f, "Play_Scream1");
	boss_audio_track->AddKeyframe(27.f, "Play_BossKingKong");
	boss_audio_track->AddKeyframe(27.1f, "Play_ScreamKingKong");
	boss_intro_timeline.tracks.emplace_back(boss_audio_track);

	Timeline::AudioTrack* boss_audio_track_slam = new Timeline::AudioTrack(boss->boss_controller->GetBoneGameObject(BossBone::RIGHT_HAND));
	boss_audio_track_slam->AddKeyframe(23.00f, "Play_BossSwing");
	boss_audio_track_slam->AddKeyframe(23.7f, "Play_BossSlam");
	boss_intro_timeline.tracks.emplace_back(boss_audio_track_slam);

	//Lightball 
	GameObject* lightball_one = scene_module.GetGameObjectByName("LB_position001");
	GameObject* lightball_two = scene_module.GetGameObjectByName("LB_position002");
	GameObject* lightball_three = scene_module.GetGameObjectByName("LB_position003");

	Timeline::TranslationTrack* lightball_track = new Timeline::TranslationTrack(light_ball->owner);
	light_ball->owner->transform.SetGlobalModelMatrix(start_transform->transform.GetGlobalModelMatrix());
	light_ball->SetActiveVFX(false);
	lightball_track->AddKeyframe(5.f, first_bridge_position->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(6.f, lightball_one->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(7.f, lightball_one->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(8.f, lightball_two->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(10.f, lightball_three->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(12.f, lightball_three->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(16.f, lightball_one->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(20.f, lightball_one->transform.GetGlobalModelMatrix());
	lightball_track->AddKeyframe(21.f, first_bridge_position->transform.GetGlobalModelMatrix());

	boss_intro_timeline.tracks.emplace_back(lightball_track);

	Timeline::AudioTrack* lightball_audio_track = new Timeline::AudioTrack(light_ball->owner);
	lightball_audio_track->AddKeyframe(5.1f, "play_light_shield_loop");
	lightball_audio_track->AddKeyframe(5.f, "play_lightball_cast");
	lightball_audio_track->AddKeyframe(7.f, "play_lightball_cast");
	lightball_audio_track->AddKeyframe(8.f, "play_lightball_cast");
	lightball_audio_track->AddKeyframe(20.f, "play_lightball_cast");
	lightball_audio_track->AddKeyframe(20.1f, "stop_light_shield_loop");
	lightball_audio_track->AddKeyframe(23.8f, "play_backflip_player");

	boss_intro_timeline.tracks.emplace_back(lightball_audio_track);

	//Camera
	GameObject* camera_one = scene_module.GetGameObjectByName("boss_camera001");
	GameObject* end_transform = scene_module.GetGameObjectByName("boss_camera002");
	float4x4 first_position_camera = camera_one->transform.GetGlobalModelMatrix();
	Timeline::TranslationTrack* camera_track = new Timeline::TranslationTrack(cinematic_camera_holder);
	cinematic_camera_holder->transform.SetGlobalModelMatrix(main_camera->owner->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(5.f, camera_one->transform.GetGlobalModelMatrix());
	camera_one->transform.Translate(float3(0,-1.f,0));
	lightball_one->transform.Translate(float3(2.5f,0.f,0.f));
	camera_one->transform.LookAt(lightball_one->transform.GetGlobalTranslation());
	camera_track->AddKeyframe(6.f, camera_one->transform.GetGlobalModelMatrix());
	camera_one->transform.LookAt(lightball_two->transform.GetGlobalTranslation());
	camera_track->AddKeyframe(8.f, camera_one->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(12.f, camera_one->transform.GetGlobalModelMatrix());
	camera_one->transform.LookAt(boss_initial.TranslatePart());
	camera_track->AddKeyframe(16.f, camera_one->transform.GetGlobalModelMatrix());
	boss_initial.SetTranslatePart(boss_initial.TranslatePart() + float3(0, 10.f, 0));
	camera_one->transform.LookAt(boss_initial.TranslatePart());
	camera_one->transform.Translate(float3(0,0,-2));
	camera_track->AddKeyframe(20.f, camera_one->transform.GetGlobalModelMatrix());
	camera_track->AddKeyframe(22.f, first_position_camera);
	camera_track->AddKeyframe(30.f, end_transform->transform.GetGlobalModelMatrix());
	boss_intro_timeline.tracks.emplace_back(camera_track);

	Timeline::AudioTrack* camera_intro_audio_track = new Timeline::AudioTrack(boss_music);
	camera_intro_audio_track->AddKeyframe(15.0f, "Play_BossIntroMusic");
	boss_intro_timeline.tracks.emplace_back(camera_intro_audio_track);

	//Custom 
	Timeline::CustomTrack* custom_track = new Timeline::CustomTrack(*this);
	custom_track->AddKeyframe(5.f, SHOW_LIGHT_BALL);
	custom_track->AddKeyframe(21.f, HIDE_LIGHT_BALL);
	custom_track->AddKeyframe(12.f, CAMERA_SHAKE);
	custom_track->AddKeyframe(5.5f, DIALOGUE_START, LIGHTBALL_FIRST_DIALOGUE.c_str());
	custom_track->AddKeyframe(11.0f, DIALOGUE_START, LIGHTBALL_SECOND_DIALOGUE.c_str());
	custom_track->AddKeyframe(16.0f, DIALOGUE_START, LIGHTBALL_THIRD_DIALOGUE.c_str());
	custom_track->AddKeyframe(19.0f, DIALOGUE_START, LIGHTBALL_FOURTH_DIALOGUE.c_str());
	custom_track->AddKeyframe(21.1f, DIALOGUE_FINISH);
	custom_track->AddKeyframe(29.f, FINISH_TRIGGER);
	boss_intro_timeline.tracks.emplace_back(custom_track);


	Timeline::AudioTrack* ambient_audio_track = new Timeline::AudioTrack(scene_module.GetGameObjectByName("DustRootsVFX"));
	lightball_audio_track->AddKeyframe(12.f, "play_EarthQuake");
	lightball_audio_track->AddKeyframe(17.5f, "stop_EarthQuake");
	boss_intro_timeline.tracks.emplace_back(ambient_audio_track);
	
}

void BossIntroCinematic::Trigger(const std::string & trigger_name, const void* data)
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
		light_ball->SetEffectProperties();
		light_ball->SetActiveVFX(true);
	}
	if (HIDE_LIGHT_BALL == trigger_name)
	{
		light_ball->SetActiveVFX(false);
	}
	if (CAMERA_SHAKE == trigger_name)
	{
		camera_shake->trauma = 1.f;
	}
	if (DISSOLVER_START == trigger_name)
	{
		auto disolver = const_cast<Dissolver*>(static_cast<const Dissolver*>(data));
		disolver->Dissolve();
		if (is_stoping)
		{
			disolver->Finish();
		}
		if (boss_mesh)
		{
			boss_mesh->active = true;
		}

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
