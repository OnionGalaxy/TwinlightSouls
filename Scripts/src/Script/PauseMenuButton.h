#ifndef  __PAUSEMENUBUTTON_H__
#define  __PAUSEMENUBUTTON_H__

#include "Script.h"

class ComponentImage;

class PauseMenuButton : public Script
{
public:

	enum class PauseMenuButtonState
	{
		HIDING,
		HIDDEN,
		SHOWING,
		NOT_SELECTED,
		FADING_IN,
		SELECTED,
		FADING_OUT
	};

	PauseMenuButton();
	~PauseMenuButton() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Select();
	void Unselect();

	void Show();
	void Hide();
	void FastHide();

	void OnInspector(ImGuiContext*) override;

public:
	float fade_time = 150.f;

private:
	float current_time = 0.f;
	static float glow_timer;

	ComponentImage* text_image = nullptr;
	ComponentImage* highlight_image = nullptr;
	ComponentImage* glow_image = nullptr;

	PauseMenuButtonState state = PauseMenuButtonState::NOT_SELECTED;

};
extern "C" SCRIPT_API PauseMenuButton* PauseMenuButtonDLL(); //This is how we are going to load the script
#endif
