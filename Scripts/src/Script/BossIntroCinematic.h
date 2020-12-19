#ifndef  __BOSSINTROCINEMATIC_H__
#define  __BOSSINTROCINEMATIC_H__

#include "Cinematic.h"


class Dissolver;
class ComponentMeshRenderer;
class UIManager;

class BossIntroCinematic : public Cinematic, Timeline::CustomTrackListener
{
public:
	BossIntroCinematic(ModuleScene& scene, ModuleTime& time);
	~BossIntroCinematic() = default;

	void Update() override;
	void Play() override;
	void Stop() override;
	bool IsPlaying() const override;

private:
	void CreateBossIntroCinematic();
	void Trigger(const std::string& trigger_name, const void* data) override;
	Timeline boss_intro_timeline  = Timeline(30.0f);
	ComponentMeshRenderer* boss_mesh = nullptr;
	// boss
	float4x4 boss_initial;
	float4x4 boss_final;
	Dissolver* boss_dissolve = nullptr;

	UIManager* ui_manager = nullptr;

};
#endif