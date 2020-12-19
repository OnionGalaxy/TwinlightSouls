#ifndef  __BOSSOUTROCINEMATIC_H__
#define  __BOSSOUTROCINEMATIC_H__

#include "Cinematic.h"
class EventManager;
class Dissolver;
class ComponentBillboard;
class BossOutroCinematic : public Cinematic, Timeline::CustomTrackListener
{
public:
	BossOutroCinematic(ModuleScene& scene, ModuleTime& time);
	~BossOutroCinematic() = default;

	void Update() override;
	void Play() override;
	void Stop() override;
	bool IsPlaying() const override;

private:
	void CreateBossOutroCinematic();
	void Trigger(const std::string& trigger_name, const void* data) override;
	Timeline boss_outro_timeline  = Timeline(30.0f);
	EventManager* event_manager = nullptr;
	Dissolver* roots_dissolve = nullptr;
	// boss
	float4x4 boss_initial;
	float4x4 boss_final;

	bool lightball_scale_start = false;
	float current_scale_animation_time = 0.0f;
};
#endif