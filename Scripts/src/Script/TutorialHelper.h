#pragma once

#include "DialogueManager.h"
#include "LightBall.h"
class GameObject;
class ComponentImage;
class ComponentText;
class WorldManager;
class InputManager;
class ModuleScene;
class UIImageDissolver;

class TutorialHelper
{
public:
	TutorialHelper() = default;
	~TutorialHelper() = default;

	void InitDependencies(const ModuleScene &scene);
	void UpdateUI(LightBall::TutorialCheckPoints & shield_tutorial);
	bool CheckShieldTutorialDialogue(LightBall::TutorialCheckPoints & shield_tutorial, const DialogueManager::NextDialogueCallback & success, const DialogueManager::NextDialogueCallback & failure);
	void ResetTutorial(LightBall::TutorialCheckPoints & shield_tutorial);
	void DisableAllImages() const;
	ComponentImage* tutorial_request = nullptr;

private:
	GameObject* lightshield_tutorial_helper_ui = nullptr;
	UIImageDissolver* tutorial_shield_throw = nullptr;
	UIImageDissolver* tutorial_shield_prepare = nullptr;
	UIImageDissolver* tutorial_shield_use = nullptr;

	UIImageDissolver* tutorial_bounce0 = nullptr;
	UIImageDissolver* tutorial_bounce1 = nullptr;
	UIImageDissolver* tutorial_bounce2 = nullptr;
	UIImageDissolver* tutorial_bounce3 = nullptr;

	UIImageDissolver* hint_lightball_keyboard = nullptr;
	UIImageDissolver* hint_lightball_controller = nullptr;
	UIImageDissolver* hint_lightball_controller_player2 = nullptr;
	UIImageDissolver* hint_shield_keyboard = nullptr;
	UIImageDissolver* hint_shield_controller = nullptr;
	UIImageDissolver* hint_shield_controller_player2 = nullptr;

	ComponentText* tutorial_hint_text = nullptr;
	ComponentText* tutorial_hint_text_p2 = nullptr;
	ComponentText* bounce_counter_text = nullptr;
	ComponentText* next_step_text = nullptr;

	DialogueManager* dialogue_manager = nullptr;
	WorldManager* world = nullptr;
	InputManager* input_manager = nullptr;

	bool a = false;
	bool b = false;
	bool c = false;
};

