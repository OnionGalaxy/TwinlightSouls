#ifndef  __ENEMYDROPMANAGER_H__
#define  __ENEMYDROPMANAGER_H__

#include "Script.h"

#include <array>

class ComponentMeshRenderer;
class LightOrb;
class PowerUp;

class EnemyDropManager : public Script
{
public:
	EnemyDropManager();
	~EnemyDropManager() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void SpawnOrbs(const float3& enemy_position, int number_orbs);
	void SpawnPowerup(const float3& position);

	inline unsigned int GetTotalOrbsSpawned() const { return total_orbs_spawned; }

private:
	std::vector<ComponentMeshRenderer*> GetMeshCollider();

	std::array<LightOrb*, 30> light_orbs;
	std::array<PowerUp*, 10> powerups;

	GameObject* light_orb_game_object = nullptr;
	GameObject* attack_powerup_game_object = nullptr;

	int deaths_last_powerup = 0;
	unsigned int total_orbs_spawned = 0;

private:
	bool HasToSpawnPowerUp() const;
	void AssignPowerUpType(PowerUp* powerup) const;

};
extern "C" SCRIPT_API EnemyDropManager* EnemyDropManagerDLL(); //This is how we are going to load the script

#endif //__ENEMYDROPMANAGER_H__