#ifndef  __MUSHDOOM_H__
#define  __MUSHDOOM_H__

#include "EnemyController.h"
#include "EnemyState.h"

class ComponentMeshRenderer;
class ComponentParticleSystem;
class DebugModeScript;
class MushdoomBall;
class WorldManager;
enum class NodeID;

struct RayHit
{
	GameObject* game_object = nullptr;
	float hit_distance = 0.0f;
	float3 hit_point = float3::zero;
};

class Mushdoom : public EnemyController
{
public:
	Mushdoom();
	~Mushdoom();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void SetProperties(EnemyController* original_enemy) override;
	void KnockBack() override;

	void TakeDamage(float damage, const float3& damage_source_position, float knockback = 0.f) override;
	void Kill() override;
	void ResetEnemy() override;
	void HitSFX(NodeID attack) override;

	void RotateTowardsPlayer() const;
	void Move();

private:
	bool DetectCollisions(int collision_direction);
	bool IsColliding(RayHit* collision, const LineSegment& ray, bool only_mesh = false);
	bool EnemyInFront(const float3& enemy_position) const;

	void ComputeHorizontalVelocity(float3 direction, float speed);
	void ComputeVerticalVelocity(float3 direction, float speed, bool apply_gravity);
	void SetNewPosition(const float3 & new_position) const;
	void ApplyVelocity();

	void SpawnJump() const;
	void ResetMovement();

	void HandleAttackZones(float3& new_position);
	void LimitMovementAtDistance(float3& new_position, float max_distance);
	void RotateAtDistance(float3& new_position, float max_distance, float angle);
	void NewThrow();
	void AttackPlayer();

	bool IsOutsideOfRange() const;

	void AvoidEnemies(float3& new_position, std::vector<EnemyController*>* enemies);
	bool IsNextTo(float3& initial, float3& goal, bool is_left = false);

public:
	typedef EnemyState<Mushdoom> MushdoomState;
	MushdoomState* current_state = nullptr;
	MushdoomState* previous_state = nullptr;
	MushdoomState* idle_state = nullptr;
	MushdoomState* scream_state = nullptr;
	MushdoomState* pursue_state = nullptr;
	MushdoomState* attack_state = nullptr;
	MushdoomState* die_state = nullptr;
	MushdoomState* hit_state = nullptr;
	MushdoomState* throw_state = nullptr;

	MushdoomBall* mushdoom_ball = nullptr;
	ComponentParticleSystem* throw_effect = nullptr;
	ComponentParticleSystem* ball_die_effect = nullptr;
	ComponentParticleSystem* mushdoom_footsteps = nullptr;

	mutable bool is_spawning = false;
	mutable bool has_ended_spawn = false;
	bool is_inside_circle = false;

	int current_time_to_throw = 0;
	int current_time_to_attack = 0;
	bool is_throwing = false;
	bool can_attack = false;
	bool can_throw = false;
	bool is_moving = false;

	int attack_slot = -1;
	int shoot_slot = -1;
	int seek_slot = -1;

private:
	unsigned int current_player;
	std::vector<GameObject*> mesh_collider;
	std::vector<ComponentMeshRenderer*> boss_zones;
	float3 velocity = float3::zero;

	bool grounded = false;
	float3 direction = float3::zero;
	float gravity = 30.f;

	mutable float3 final_spawn_position = float3::zero;
	mutable float3 initial_spawn_position = float3::zero;
	mutable float current_spawning_time = 0.0f;
	float time_count_delay = 0.0F;

	const float MAX_SPAWN_TIME = 400.f;
	float smooth_rotation = 0.01f;

	float3 target = float3::zero;
	float3 position = float3::zero;
	float3 desired_velocity = float3::zero;

	float distance = 0.0f;

	float throw_delay = 2.f;
	float hit_delay = 1.2f;

	float3 fixed_pos = float3::unitY;
	//New Collision check
	//Horizontal
	float horizontal_threshold = 0.6f;
	RayHit* horizontal_ray_detector_middle = nullptr;
	float horizontal_ray_middle_position = 0.5f;
	float horizontal_ray_middle_length = 1.5f;

	RayHit* horizontal_ray_detector_knee = nullptr;
	float horizontal_ray_knee_position = 0.3f;
	float horizontal_ray_knee_length = 0.5f;

	//Vertical
	float vertical_offset_start = 0.3f;
	float vertical_offset_end = 0.f;
	float vertical_offset_minimum = 4.f;
	float vertical_offset_maximum = 100.f;

	RayHit* main_vertical_ray_detector = nullptr;
	float vertical_ray_main_position = 0.3f;
	bool main_vertical_ray = false;

	RayHit* border_vertical_ray_detector = nullptr;
	float vertical_ray_border_position = 0.3f;
	bool border_vertical_ray = false;
	float border_ray_offset = 0.4f;

	float minimum_y = -10.f;
	float vertical_threshold = 0.08f;

	//Rotation
	float rotation_angle = 0.3f;
	float rotation_threshold = 0.2f;
	float rotation_ray_length = 0.2f;

	//Diagonal
	RayHit* main_diagonal_ray_detector = nullptr;
	bool main_diagonal_ray = false;

	//Extra
	float range_collision = 5.f;

};
extern "C" SCRIPT_API Mushdoom* MushdoomDLL(); // This is how we are going to load the script
#endif