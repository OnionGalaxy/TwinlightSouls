#ifndef  __AVALANCHEZONE_H__
#define  __AVALANCHEZONE_H__

#include "Script.h"

class CameraShake;
class WorldManager;

struct Player;

class AvalancheZone : public Script
{
public:
	struct RayLine
	{
		GameObject* game_object = nullptr;
		float hit_distance = 0.0f;
		float3 hit_point = float3::zero;
	};
	enum class RockStatus
	{
		UNSPAWNED,
		SPAWNED,
		GOING_DOWN,
		DESTROYED
	};
	struct Rock{
		GameObject* rock_go = nullptr;
		RockStatus status = RockStatus::UNSPAWNED;
		float remaining_time_up = 0.f;
		float total_time_up = 0.f;
		bool player_already_hitted = false;

		Rock() = default;
		Rock(GameObject* rock)
		{
			this->rock_go = rock;
		}
	};
	AvalancheZone();
	~AvalancheZone() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	void CheckPlayersInTheZone();
	void CheckPlayersOutTheZone();
	void GetRocks();
	void UpdateRocks() const;
	bool IsRockInTheEnd(Rock* rock) const;
	void SpawnRock() const;
	bool ComputeDamageZone(Rock* rock) const;
	float3 ComputeNextPosition(Rock* rock) const;
	void ComputeVerticalPosition(float3& rock_current_position) const;
	void SetCameraTrauma(float trauma) const;
	void CheckEndedRocksKillRocks();

private:
	WorldManager* world = nullptr;
	Player* player_1 = nullptr;
	Player* player_2 = nullptr;
	CameraShake* camera_shake = nullptr;
	bool is_multiplayer = false;

	GameObject* starting_point = nullptr;
	GameObject* end_point = nullptr;
	GameObject* destruction_point = nullptr;

	std::vector<Rock*> rocks;
	std::vector<GameObject*> mesh_collider;

	float maximum_time_moving = 200000.f;
	float maximum_time_up = 3500.f;
	float delay_between_rocks = 2500.f;
	float current_delay = 3000.f;
	float time_stun_players = 2000.f;
	float damage = 20.f;
	float radius = 1.5f;
	float deactivation_distance = 24.f;
	bool activated = false;
	bool started = false;
	bool ended = false;
};

extern "C" SCRIPT_API AvalancheZone* AvalancheZoneDLL(); //This is how we are going to load the script
#endif