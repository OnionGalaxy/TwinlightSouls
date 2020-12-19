#ifndef  __PopupText_H__
#define  __PopupText_H__

#include "Script.h"

#include "Dialogues.h"

class DialogueManager;
class ComponentLight;
class ComponentImage;
class ComponentParticleSystem;

class PopupText : public Script
{

public:

	PopupText();
	~PopupText() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	bool CheckPlayerDistance(const float3 & player_position);
	void ActivateEvent();
	bool CheckEvent() const;
	void EndEvent();

	void EnablePopUp();
	void DisablePopUp();
	bool IsPopUpEnabled();

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void InitPublicGameObjects();

private:
	DialogueManager* dialogue_manager = nullptr;

	bool message_activated = false;
	bool enable_popup = false;
	bool disable_popup = false;
	float current_time_to_blend = 0.f;
	float total_time_to_blend = 200.f;

	float range_detection = 7.5f;

	GameObject* current_pop_up = nullptr;
	ComponentLight* light = nullptr;
	ComponentParticleSystem* sparkle = nullptr;

	ComponentImage* ui_popup_go = nullptr;

	std::string pop_up_name;

	int index = 0;

};

extern "C" SCRIPT_API PopupText* PopupTextDLL(); //This is how we are going to load the script

#endif