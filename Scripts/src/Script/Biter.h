#ifndef  __BITER_H__
#define  __BITER_H__

#include "EnemyController.h"
#include "EnemyManager.h"
#include "EnemyState.h"

class ComponentMeshRenderer;

class Biter : public EnemyController
{
public:
	enum class BiterAttackEffects
	{
		MOVING,
		BITE,
		CHARGE,
		EXPLOSION,
		DEATH
	};

	Biter();
	~Biter();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void TakeDamage(float damage, const float3& damage_source_position, float knockback = 0.f) override;
	void Kill() override;
	void ResetEnemy() override;

	void AirMovementAI();
	void Charge();
	void ChargeStatus();
	void MoveIntoDirection(const float3 direction, float speed);
	void AirMoveTo(const float3& destination);
	void ComputeHorizontalVelocity(const float3 direction, float speed);
	void ExplosionStatus();
	void AttackPosition();
	void UpdateChargeCooldown();
	void UpdateExplosionCooldown();
	void UpdateOffset();
	void ApplySpecialAttackDelay();
	void UpdateSpecialAttackDelay();
	void HitSFX(NodeID attack) override;

	void ChangeAttackEffects(BiterAttackEffects attack_effects);
	void UpdateAttackEffectsColor();
	void DisableAllEffects();

public:
	typedef EnemyState<Biter> BiterState;
	BiterState* current_state = nullptr;
	BiterState* previous_state = nullptr;
	BiterState* idle_state = nullptr;
	BiterState* pursue_state = nullptr;
	BiterState* attack_state = nullptr;
	BiterState* charge_state = nullptr;
	BiterState* explosion_state = nullptr;
	BiterState* hit_state = nullptr;
	BiterState* die_state = nullptr;

public:
	//float separation_distance = 2.25f;
	float3 offset;
	float3 current_velocity;
	float3 current_position;
	float3 next_position;
	float acceleration_factor = 0.2F;
	float time_count_delay = 0.0F;

	float current_special_attack_delay = 0.0F;
	float special_attack_delay = 3000.0F;
	bool is_special_attack_ready = false;

	float bite_distance = 0.8F;
	float bite_radius = 1.5F;

	bool is_charging = false;
	float charge_damage = 25.0F;
	float charge_radius = 1.5F;
	float charge_distance_detection = 15.0F;
	float charge_total_ability_time = 1500.f;
	float charge_before_dash_time = 1000.f;
	float charge_current_time = 0.0f;
	float frontal_collision_detection_distance = 1.5f;
	float speed_horizontal = 11.f;
	float charge_cooldown = 7000.f;
	float charge_current_cooldown = 7000.0f;
	float charge_speed = 15.f;
	float3 charge_target = float3::zero;
	float3 desired_velocity = float3::zero;
	float3 velocity = float3::zero;
	float3 direction = float3::zero;

	bool is_exploding = false;
	float explosion_damage = 125.0F;
	float explosion_radius = 3.0F;
	float explosion_cooldown = 8000.0F;
	float explosion_current_cooldown = 8000.0F;
	float explosion_total_ability_time = 3500.0F;
	float explosion_delay_time = 2500.0F;
	float explosion_current_time = 0.0F;
	float explosion_cast_range = 10.0F;

	/*static const int NORMAL_EMISSIVE_UUID = 2382772288;
	static const int ATTACK_EMISSIVE_UUID = 3550378246;
	static const int CHARGE_EMISSIVE_UUID = 4246360906;
	static const int EXPLOSION_EMISSIVE_UUID = 4105421387;*/

	ComponentParticleSystem* passive_particles = nullptr;
	ComponentParticleSystem* passive_particles_attack = nullptr;
	ComponentParticleSystem* passive_particles_attack_2 = nullptr;
	ComponentParticleSystem* spark_particles = nullptr;
	ComponentParticleSystem* explosion_particles = nullptr;
	ComponentParticleSystem* explosion_particles_2 = nullptr;
	ComponentBillboard* charge_explosion_purple_vfx = nullptr;
	ComponentBillboard* charge_explosion_black_vfx = nullptr;
	ComponentBillboard* charge_explosion_blue_vfx = nullptr;
	ComponentBillboard* inner_circle_explosion_vfx = nullptr;
	ComponentBillboard* warning_basic_attack_vfx = nullptr;
	ComponentBillboard* warning_basic_attack_orange_vfx = nullptr;
	ComponentBillboard* shockwave_vfx = nullptr;

	/*float4 particle_color_bite = float4(0.736, 0.928, 0, 0.5);
	float4 particle_color_charge = float4(0.475, 0.035, 0.035, 0.5);
	float4 particle_color_explosion = float4(0.812, 0.324, 0, 0.5);*/
	ComponentMeshRenderer* biter_material = nullptr;

	float4 basic_color = float4(0.5f, 0.0f, 0.5f, 0.2f);
	float4 basic_color_particles = float4(0.029f, 0.313f, 0.995f, 0.8f);
	float4 bite_color = float4(0.029f, 0.313f, 0.995f, 0.8f);
	float4 explosion_color = float4(1.0f, 0.0f, 0.204f, 0.8f);
	float4 charge_color = float4(0.0f, 0.800f, 0.118f, 0.8f);

	float4 current_color = basic_color;
	float4 next_color = basic_color;
	float4 current_color_particles = basic_color_particles;
	float4 next_color_particles = basic_color_particles;
	float color_change_time = 1000.0f;
	float color_current_change_time = 0.0f;
};
extern "C" SCRIPT_API Biter* BiterDLL(); //This is how we are going to load the script
#endif