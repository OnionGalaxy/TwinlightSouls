#ifndef  __SWITCHSETTINGUI_H__
#define  __SWITCHSETTINGUI_H__

#include "SystemSettingUI.h"

class SwitchSettingUI : public SystemSettingUI
{
public:
	class SwitchSettingOption
	{
	public:
		enum class SwitchSettingOptionState
		{
			SHOWING,
			SHOWN,
			HIDING,
			HIDDEN
		};

		void Update(float delta_time);
		void Show();
		void Hide();
		void FastHide();

		ComponentImage* switch_options_name = nullptr;

	private:
		float current_time = 0.f;
		float transition_time = 125.f;
		SwitchSettingOptionState current_state = SwitchSettingOptionState::HIDDEN;
	};

	SwitchSettingUI();
	~SwitchSettingUI() = default;

	void Awake() override;
	void Start() override;
	void Update() override;
	 
	void Show() override;
	void Hide() override;
	void FastHide() override;

	void RightPressed() override;
	void LeftPressed() override;

	int GetCurrentOption() const;
	void SetCurrentOption(int current_option);

private:
	std::vector<SwitchSettingOption> switch_options;
	int current_option = 0;
};
extern "C" SCRIPT_API SwitchSettingUI* SwitchSettingUIDLL(); //This is how we are going to load the script
#endif