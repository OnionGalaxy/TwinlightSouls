#ifndef  __ARROWUI_H__
#define  __ARROWUI_H__

#include "Script.h"

class ComponentImage;

class ArrowUI : public Script
{
public:
	enum class ArrowUIState
	{
		SHOWING,
		UNSELECTED,
		SELECTED,
		HIDING,
		HIDDEN
	};

	ArrowUI();
	~ArrowUI() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Select();

	void Show();
	void Hide();
	void FastHide();

	void OnInspector(ImGuiContext*) override;

private:
	ComponentImage* normal = nullptr;
	ComponentImage* highlight = nullptr;

	float current_time = 0.f;
	float select_time = 250.f;
	float showing_time = 150.f;

	ArrowUIState state = ArrowUIState::HIDDEN;
};
extern "C" SCRIPT_API ArrowUI* ArrowUIDLL(); //This is how we are going to load the script
#endif