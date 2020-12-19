#ifndef  __TIMELINEMANAGER_H__
#define  __TIMELINEMANAGER_H__

#include "Script.h"
#include "Cinematic.h"
#include <unordered_map>


enum CinematicID
{
	LVL1_INTRO,
	LVL1_OUTRO,
	LVL2_INTRO,
	LVL2_OUTRO,
	BOSS_INTRO,
	BOSS_OUTRO
};

class InputManager;
class Player;
class UIManager;
class WorldManager;
class TimelineManager : public Script
{
public:
	TimelineManager() = default;
	~TimelineManager() = default;
	void Awake() override;
	void Start() override;
	void Update() override;
	void PlayTimeline(CinematicID cinematic_id);
	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();

private:
	WorldManager* world_manager = nullptr;
	std::unordered_map<CinematicID, std::unique_ptr<Cinematic>> cinematics;
	UIManager* ui_manager = nullptr;
	InputManager* input_manager = nullptr;
	Player* player_1 = nullptr;
	float current_playing_time = 0;

};
extern "C" SCRIPT_API TimelineManager* TimelineManagerDLL(); //This is how we are going to load the script
#endif