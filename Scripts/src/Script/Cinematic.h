#ifndef  __CINEMATIC_H__
#define  __CINEMATIC_H__

#include "Timeline.h"

class WorldManager;
struct Player;
struct Boss;
class LightBall;
class ComponentCamera;
class CameraShake;
class ComponentText;
class ModuleScene;
class ModuleTime;
class Cinematic 
{
public:
	Cinematic(ModuleScene& scene, ModuleTime& time);
	virtual ~Cinematic() = default;
	virtual void Update() = 0;
	virtual void Play() = 0;
	virtual void Stop() = 0;

	virtual bool IsPlaying() const = 0;
protected:
	void EnableTimeline(bool enable);
	void SetMainCamera(bool cinematic) const;
	WorldManager* world_manager = nullptr;
	GameObject* dialogue_manager = nullptr;
	ModuleScene& scene_module;
	ModuleTime& time_module;

	//Animated entities
	Player* player_1 = nullptr;
	Player* player_2 = nullptr;
	Boss* boss = nullptr;
	LightBall* light_ball = nullptr;
	GameObject* cinematic_camera_holder = nullptr;
	GameObject* boss_music = nullptr;

	//camera
	CameraShake* camera_shake = nullptr;
	ComponentCamera* main_camera = nullptr;
	ComponentCamera* cinematic_camera = nullptr;

	//dialogue
	ComponentText* dialogue_text = nullptr;

	bool is_stoping = false;

};
#endif