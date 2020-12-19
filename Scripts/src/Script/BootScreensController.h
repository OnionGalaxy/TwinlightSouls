#ifndef  __BOOTSCREENSCONTROLLER_H__
#define  __BOOTSCREENSCONTROLLER_H__

#include "Script.h"

class ComponentImage;
class ComponentVideoPlayer;
class BootScreensController : public Script
{
public:
	enum class BootScreenState
	{
		FADING_IN,
		SHOWING,
		FADING_OUT
	};

	BootScreensController();
	~BootScreensController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Init();
	bool HasFinished() const;
	
	void OnInspector(ImGuiContext*) override;
	void Save(Config& config) const override;
	void Load(const Config& config) override;

private:
	std::vector<ComponentImage*> boot_screens;

	bool has_finished = true;

	BootScreenState current_state = BootScreenState::FADING_IN;
	ComponentVideoPlayer* video_player = nullptr;
	GameObject* video_player_go = nullptr;
	int current_screen = 0;
	
	float boot_screen_transition_time = 1000.f;
	float boot_screen_showing_time = 2000.f;
	float current_time = 0.f;
	float showing_video = false;
};
extern "C" SCRIPT_API BootScreensController* BootScreensControllerDLL(); //This is how we are going to load the script
#endif