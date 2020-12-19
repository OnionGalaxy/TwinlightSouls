#ifndef  __LVL1INTROCINEMATIC_H__
#define  __LVL1INTROCINEMATIC_H__

#include "Cinematic.h"

class EventManager;
class UIManager;

class Lvl1IntroCinematic : public Cinematic, Timeline::CustomTrackListener
{
public:
	Lvl1IntroCinematic(ModuleScene& scene, ModuleTime& time);
	~Lvl1IntroCinematic() = default;

	void Update() override;
	void Play() override;
	void Stop() override;
	bool IsPlaying() const override;
private:
	void CreateLvl1IntroCinematic();
	void Trigger(const std::string& trigger_name, const void* data) override;
	Timeline lvl1_intro_timeline = Timeline(25.f);
	EventManager* event_manager = nullptr;
	GameObject* p1_entrance = nullptr;
	GameObject* p2_entrance = nullptr;
	GameObject* p1_end = nullptr;
	GameObject* p2_end = nullptr;

	UIManager* ui_manager = nullptr;

};
#endif