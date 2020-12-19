#ifndef  __SYSTEMCURSORUI_H__
#define  __SYSTEMCURSORUI_H__

#include "Script.h"

#include <array>

class ComponentImage;

class SystemCursorUI : public Script
{
public:
	enum class SystemCursorUIState
	{
		SHOWING,
		SHOWN,
		HIDING,
		HIDDEN,
		MOVING
	};

	SystemCursorUI();
	~SystemCursorUI() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Show();
	void Hide();
	void FastHide();
	void MoveUp();
	void MoveDown();

	void SetCurrentPosition(int position);
	int GetCurrentPosition() const;

private:
	float current_time = 0.f;
	float transitioning_time = 125.f;

	float previous_height = 0.f;
	std::array<float, 4> translations = { 210.f, 94.f, -22.f, -138.f };

	int current_position = 0;

	ComponentImage* base_cursor = nullptr;
	ComponentImage* glow_cursor = nullptr;

	SystemCursorUIState current_state = SystemCursorUIState::HIDDEN;

};
extern "C" SCRIPT_API SystemCursorUI* SystemCursorUIDLL(); //This is how we are going to load the script
#endif