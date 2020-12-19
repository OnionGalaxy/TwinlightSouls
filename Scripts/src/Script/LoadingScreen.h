#ifndef  __LOADINGSCREEN_H__
#define  __LOADINGSCREEN_H__

#include "Script.h"
#include "Animation/Tween/LOTween.h"
#include "Animation/Tween/Tween.h"
#include "Animation/Tween/TweenSequence.h"

class ComponentText;
class InputManager;
class ProgressBar;

class LoadingScreen : public Script
{
public:
	LoadingScreen();
	~LoadingScreen() = default;

	//void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();

private:
	void InitHints();
	void UpdateHints();
	void UpdateNumbers(int percentage);
	void UpdateTips();
	void GetNextTip();

private:
	ComponentText* percentatge_text = nullptr;
	ComponentText* hints_text = nullptr;
	TweenSequence* sequence = nullptr;
	InputManager* input_manager = nullptr;
	std::vector<std::string> hints;
	float hints_buffer = 0.0f;
	const float hints_change_time = 10.0f;
	int current_hint_index = 0;

	GameObject* current_unit_number = nullptr;
	std::vector<GameObject*> units;
	GameObject* current_ten_number = nullptr;
	std::vector<GameObject*> tens;
	GameObject* current_cent_number = nullptr;
	std::vector<GameObject*> cents;

	GameObject* current_tip = nullptr;
	std::vector<GameObject*> tips;
	float tips_buffer = 0.0f;
	const float tips_change_time = 10.0f;
	int current_tip_index = 0;

	ProgressBar* progress_bar = nullptr;

};
extern "C" SCRIPT_API LoadingScreen* LoadingScreenDLL(); //This is how we are going to load the script
#endif