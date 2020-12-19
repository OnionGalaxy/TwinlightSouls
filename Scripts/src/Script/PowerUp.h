#ifndef  __POWERUP_H__
#define  __POWERUP_H__

#include "EnemyDrop.h"

class ComponentBillboard;
class ComponentTrail;

class PowerUp : public EnemyDrop
{
public:
	enum class PowerUpType
	{
		ATTACK,
		INVULNERABLE,
	};

	PowerUp();
	~PowerUp() = default;

	void Awake() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Spawn(const float3& spawn_position);

	void CloneProperties(PowerUp* original_orb);

	void InitVfx();
	void SpawnSetPowerupVfx() const;

private:
	void PickupActivatePowerup() const;

private:
	GameObject* attack_powerup = nullptr;
	ComponentBillboard* small_texture_attack = nullptr;
	ComponentBillboard* shine_texture_attack = nullptr;
	ComponentBillboard* exterior_texture_attack = nullptr;

	GameObject* invulnerability_powerup = nullptr;
	ComponentBillboard* small_texture_invul = nullptr;
	ComponentBillboard* shine_texture_invul = nullptr;
	ComponentBillboard* exterior_texture_invul = nullptr;

	int attack_powerup_duration = 15;
	float attack_powerup_bonus = 0.3f;
	int invulnerable_powerup_duration = 10;

	float4 attack_color = float4(255.0f, 0.0f, 0.0f, 1.0f);
	float4 invulnerable_color = float4(0.0f, 0.0f, 255.0f, 1.0f);

public:
	PowerUpType powerup_type = PowerUpType::ATTACK;

	float attack_powerup_chance = 0.5f;
	float invulnerable_powerup_chance = 0.5f;
};
extern "C" SCRIPT_API PowerUp* PowerUpDLL(); //This is how we are going to load the script

#endif //__POWERUP_H__