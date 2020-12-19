#ifndef  __LVL1OUTROCINEMATIC_H__
#define  __LVL1OUTROCINEMATIC_H__

#include "Cinematic.h"

class EventManager;

class Lvl1OutroCinematic : public Cinematic, Timeline::CustomTrackListener
{
public:
	Lvl1OutroCinematic(ModuleScene& scene, ModuleTime& time);
	~Lvl1OutroCinematic() = default;

	void Update() override;
	void Play() override;
	void Stop() override;
	bool IsPlaying() const override;
private:
	void CreateLvl1OutroCinematic();
	void Trigger(const std::string& trigger_name, const void* data) override;
	Timeline lvl1_outro_timeline = Timeline(10.5f);
	EventManager* event_manager = nullptr;
	GameObject* p1_pos = nullptr;
	GameObject* p2_pos = nullptr;

	GameObject* p1_to_cave = nullptr;
	GameObject* p2_to_cave = nullptr;

	GameObject* p1_final = nullptr;
	GameObject* p2_final = nullptr;





};
#endif