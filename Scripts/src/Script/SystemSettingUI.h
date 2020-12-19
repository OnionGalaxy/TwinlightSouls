#ifndef  __SYSTEMSETTINGUI_H__
#define  __SYSTEMSETTINGUI_H__

#include "Script.h"

class ComponentImage;
class ArrowUI;

class SystemSettingUI : public Script
{
public:
	enum class SystemSettingUIState
	{
		SHOWING,
		SHOWN,
		HIDING,
		HIDDEN
	};

	SystemSettingUI();
	~SystemSettingUI() = default;

	void Awake() override;
	void Start() override;
	void Update() override;
	 
	virtual void Show() {};
	virtual void Hide() {};
	virtual void FastHide() {};

	virtual void RightPressed() {};
	virtual void LeftPressed() {};

	void OnInspector(ImGuiContext*) override;

protected:
	ComponentImage* name = nullptr;
	ArrowUI* left_arrow = nullptr;
	ArrowUI* right_arrow = nullptr;

	SystemSettingUIState current_state = SystemSettingUIState::HIDDEN;
	float current_time = 0.f;
	float transition_time = 125.f;
};
extern "C" SCRIPT_API SystemSettingUI* SystemSettingUIDLL(); //This is how we are going to load the script
#endif