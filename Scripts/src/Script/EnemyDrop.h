#ifndef  __ENEMYDROP_H__
#define  __ENEMYDROP_H__

#include "Script.h"

class ComponentAudioSource;
class ComponentMeshRenderer;
class WorldManager;

struct Player;

class EnemyDrop : public Script
{
public:
	enum class EnemyDropLifeState
	{
		SPAWNING,
		WAITING,
		TRAVELLING,
		DEAD
	};

	EnemyDrop();
	~EnemyDrop() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Spawn(const float3& spawn_position);
	bool IsAlive() const;

	void SetMeshEnvironment(const std::vector<ComponentMeshRenderer*>& mesh_environment);
	std::vector<ComponentMeshRenderer*> GetMeshEnvironment() const;

	void CloneProperties(EnemyDrop* original_orb);

protected:
	Player* GetClosestAlivePlayer() const;
	float3 GetClosestTerrainPosition(const float3& spawn_position) const;

protected:

	WorldManager* world = nullptr;
	Player* player_to_deliver = nullptr;
	Player* player_1 = nullptr;
	Player* player_2 = nullptr;

	ComponentAudioSource* audio_source = nullptr;
	std::vector<ComponentMeshRenderer*> mesh_environment;

	float current_time = 0.f;
	EnemyDropLifeState life_state = EnemyDropLifeState::DEAD;
	bool changed_state = false;
	bool picked = false;

	float spawning_time = 700.f;
	const float GRAVITY = 0.00002f;
	float spread = 2.5f;
	float3 spawning_position = float3::zero;
	float3 initial_speed = float3::zero;

	float stil_time = 7500.f;
	float3 waiting_position = float3::zero;
	float oscillation_speed = 0.008f;
	float max_oscillation_height = 0.2f;

	float travel_time = 300.f;
	float attracting_radio = 3.5f;

	float ray_lenght = 50.f;
	float vertical_offset = 0.4f;
};
//extern "C" SCRIPT_API PowerUp* EnemyDropDLL(); //This is how we are going to load the script

#endif //__ENEMYDROP_H__