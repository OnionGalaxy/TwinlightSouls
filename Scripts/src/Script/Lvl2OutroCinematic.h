#ifndef  __LVL2OUTROCINEMATIC_H__
#define  __LVL2OUTROCINEMATIC_H__

#include "Cinematic.h"

class EventManager;

class Lvl2OutroCinematic : public Cinematic, Timeline::CustomTrackListener
{
public:
	Lvl2OutroCinematic(ModuleScene& scene, ModuleTime& time);
	~Lvl2OutroCinematic() = default;

	void Update() override;
	void Play() override;
	void Stop() override;
	bool IsPlaying() const override;
private:
	void CreateLvl2OutroCinematic();
	void Trigger(const std::string& trigger_name, const void* data) override;
	Timeline lvl2_outro_timeline = Timeline(10.5f);
	EventManager* event_manager = nullptr;
	GameObject* p1_pos = nullptr;
	GameObject* p2_pos = nullptr;

	GameObject* p1_to_door = nullptr;
	GameObject* p2_to_door = nullptr;

	GameObject* p1_final = nullptr;
	GameObject* p2_final = nullptr;

	GameObject* door_right_final = nullptr;
	GameObject* door_left_final = nullptr;

};
#endif