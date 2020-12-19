#ifndef  __ENEMYMANAGER_H__
#define  __ENEMYMANAGER_H__

#include "Script.h"

#include <list>
#include <map>

class EventManager;
class EnemyController;
class MushdoomBall;
class WorldManager;
class Mushdoom;
class Necroplant;

enum class EnemyType;

class EnemyManager : public Script
{
public:
	struct InitialEnemySpawn
	{
		float3 position;
		EnemyType enemy_type;
	};

	EnemyManager();
	~EnemyManager();

	void Awake() override;
	void Start() override;
	void Update() override;

	void AddEnemy(EnemyController* enemy);
	void KillEnemy(EnemyController* enemy);

	void SpawnEnemy(EnemyType type, const float3& spawn_position, bool is_wave_enemy, bool invincible = false);
	void SpawnWave(std::vector<float3>& spawns, unsigned enemies_per_wave, EnemyType enemy_type);
	void InitEnemies();

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	bool ArePlayersInDanger() const;
	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void CancelMeleeAttack(EnemyController* enemy);

	void CancelRangedAttack(EnemyController* enemy);
	void KillAllTheEnemies();

	void SpawnInitialEnemies();

	int GetCurrentSlot(unsigned int player, std::vector<std::pair<unsigned int, int>> &slots);
	int UpdateCurrentSlot(unsigned int player, std::vector<std::pair<unsigned int, int>> &slots, int value);

	int CreateSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots, EnemyController* new_enemy);
	int GetCurrentSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots);
	void RemoveSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots, EnemyController* enemy_to_remove);
	void ClearAllSlots(unsigned int player);
	void ClearSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots);
	std::vector<EnemyController*>* GetEnemiesOnSlot(unsigned int player, std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> &slots);

private:
	bool CheckSpawnAvailability(float3& spawn_position);

public:
	std::vector<EnemyController*> enemies;

	float attack_distance = 2.0f;
	float shoot_distance = 3.5f;
	float seek_distance = 5.0f;
	float spawn_distance = 8.0f;
	float angle = 45.0f;
	float angle_rad = DegToRad(angle);

	int max_attackers = 2;
	int max_shooters = 2;
	std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> current_shooters_for_player;
	std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> current_attackers_for_player;
	std::vector<std::pair<unsigned int, std::vector<EnemyController*>>> current_seekers_for_player;
	std::vector<InitialEnemySpawn> initial_enemy_spawns;

	bool debug_hit_shoot_slots = false;

private:
	//We need a reference to an existing mushdoom to duplicate_him
	GameObject* mushdoom_go = nullptr;
	EventManager* event_manager = nullptr;
	WorldManager* world_manager = nullptr;
	std::vector<std::pair<float3, EnemyType>> enemies_spawning_queue;
	unsigned total_enemies_killed = 0;
	int max_double_z_spawn = 5;

	size_t current_melee_mushdoom_attackers = 0;
	int simultaneous_attackers = 3;

	size_t current_ranged_mushdoom_attackers = 0;
	int simultaneous_ranged_attackers = 4;

	int mushdooms_to_spawn = 0;
	int biters_to_spawn = 0;

	const unsigned MAX_NUMBER_OF_ENEMIES = 20;
	const float3 graveyard_position = float3(100.f, 100.f, 100.f);
};
extern "C" SCRIPT_API EnemyManager* EnemyManagerDLL(); //This is how we are going to load the script
#endif