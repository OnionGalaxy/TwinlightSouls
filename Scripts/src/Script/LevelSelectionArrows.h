#ifndef  __LEVELSELECTIONARROWS_H__
#define  __LEVELSELECTIONARROWS_H__

#include "Script.h"

class ComponentImage;

class LevelSelectionArrows : public Script
{
public:
	enum class LevelSelectionArrowsState
	{
		SHOWING,
		UNSELECTED,
		SELECTED,
		HIDING,
		HIDDEN
	};

	LevelSelectionArrows();
	~LevelSelectionArrows() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Show();
	void Hide();
	void FastHide();

	void Select();

private:
	LevelSelectionArrowsState state = LevelSelectionArrowsState::HIDDEN;
	float current_time = 0.f;
	float glow_timer = 0.f;
	float transition_time = 125.f;
	
	ComponentImage* selected = nullptr;
	ComponentImage* unselected = nullptr;
	ComponentImage* background = nullptr;
};
extern "C" SCRIPT_API LevelSelectionArrows* LevelSelectionArrowsDLL(); //This is how we are going to load the script
#endif