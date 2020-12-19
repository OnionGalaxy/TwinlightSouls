#ifndef  __PROGRESSBARSETTINGUI_H__
#define  __PROGRESSBARSETTINGUI_H__

#include "SystemSettingUI.h"

class ComponentImage;
class ArrowUI;

class ProgressBarSettingUI : public SystemSettingUI
{
public:
	class ProgressBarSettingUIProgressBar
	{
	public:
		enum class ProgressBarSettingUIProgressBarState
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

		void SetProgress(float progress);
		float GetProgress() const;

	public:
		ComponentImage* base_bar = nullptr;
		ComponentImage* fill_bar = nullptr;

	private:
		float current_time = 0.f;
		const float transition_time = 125.f;

		const float BAR_WIDTH = 486.7f;

		float progress_bar_progress = 0.f;
		ProgressBarSettingUIProgressBarState current_state = ProgressBarSettingUIProgressBarState::HIDDEN;
	};

	//////////////////////////////////////////////////

	ProgressBarSettingUI();
	~ProgressBarSettingUI() = default;

	void Awake() override;
	void Start() override;
	void Update() override;
	 
	void Show() override;
	void Hide() override;
	void FastHide() override;

	void LeftPressed() override;
	void RightPressed() override;

	void SetProgress(float progress);
	float GetProgress() const;

private:
	ProgressBarSettingUIProgressBar progress_bar;
	const float PROGRESS_BAR_SPEED = 0.1f;
};
extern "C" SCRIPT_API ProgressBarSettingUI* ProgressBarSettingUIDLL(); //This is how we are going to load the script
#endif