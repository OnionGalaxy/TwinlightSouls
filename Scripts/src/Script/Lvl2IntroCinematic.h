#ifndef  __LVL2INTROCINEMATIC_H__
#define  __LVL2INTROCINEMATIC_H__

#include "Cinematic.h"
class UIManager;

class Lvl2IntroCinematic : public Cinematic, Timeline::CustomTrackListener
{
public:
	Lvl2IntroCinematic(ModuleScene& scene, ModuleTime& time);
	~Lvl2IntroCinematic() = default;

	void Update() override;
	void Play() override;
	void Stop() override;
	bool IsPlaying() const override;
private:
	void CreateLvl2IntroCinematic();
	void Trigger(const std::string& trigger_name, const void* data) override;
	Timeline lvl2_intro_timeline = Timeline(32.0f);
	GameObject* p1_entrance = nullptr;
	GameObject* p2_entrance = nullptr;
	GameObject* p1_end = nullptr;
	GameObject* p2_end = nullptr;

	UIManager* ui_manager = nullptr;

};
#endif