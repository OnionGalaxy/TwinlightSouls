#ifndef  __LIGHTORB_H__
#define  __LIGHTORB_H__

#include "EnemyDrop.h"

class ComponentAudioSource;
class ComponentBillboard;
class ComponentTrail;
class WorldManager;


struct Player;

class LightOrb : public EnemyDrop
{
public:
	LightOrb();
	~LightOrb() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Spawn(const float3& spawn_position);

	void CloneProperties(LightOrb* original_orb);

private:
	ComponentBillboard* small_texture_light_orb = nullptr;
	ComponentBillboard* shine_texture_light_orb = nullptr;
	ComponentBillboard* exterior_texture_light_orb = nullptr;
	ComponentTrail* trail_light_orb_VFX = nullptr;

	float healing_value = 15.f;
	float special_value = 0.5f;
};
extern "C" SCRIPT_API LightOrb* LightOrbDLL(); //This is how we are going to load the script
#endif