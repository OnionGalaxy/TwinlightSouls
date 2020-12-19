#include "ProgressBarSettingUI.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "ArrowUI.h"

ProgressBarSettingUI* ProgressBarSettingUIDLL()
{
	ProgressBarSettingUI* instance = new ProgressBarSettingUI();
	return instance;
}

ProgressBarSettingUI::ProgressBarSettingUI()
{
}

// Use this for initialization before Start()
void ProgressBarSettingUI::Awake()
{
	name = static_cast<ComponentImage*>(owner->GetChildrenWithName("Name")->GetComponent(Component::ComponentType::UI_IMAGE));
	left_arrow = static_cast<ArrowUI*>(owner->GetChildrenWithName("Left Arrow")->GetComponentScript("ArrowUI")->script);
	right_arrow = static_cast<ArrowUI*>(owner->GetChildrenWithName("Right Arrow")->GetComponentScript("ArrowUI")->script);

	GameObject* progress_bar_go = owner->GetChildrenWithName("Progress Bar");
	progress_bar.base_bar = static_cast<ComponentImage*>(progress_bar_go->GetChildrenWithName("Base")->GetComponent(Component::ComponentType::UI_IMAGE));
	progress_bar.fill_bar = static_cast<ComponentImage*>(progress_bar_go->GetChildrenWithName("Fill")->GetComponent(Component::ComponentType::UI_IMAGE));
}

// Use this for initialization
void ProgressBarSettingUI::Start()
{

}

// Update is called once per frame
void ProgressBarSettingUI::Update()
{
	switch (current_state)
	{
	case SystemSettingUI::SystemSettingUIState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		name->color.w = progress;

		if (progress == 1.f)
		{
			current_state = SystemSettingUI::SystemSettingUIState::SHOWN;
		}
	}
	break;

	case SystemSettingUI::SystemSettingUIState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		name->color.w = 1.f - progress;

		if (progress == 1.f)
		{
			current_state = SystemSettingUI::SystemSettingUIState::HIDDEN;
		}
	}
	break;

	default:
		break;
	}

	progress_bar.Update(App->time->delta_time);
}

void ProgressBarSettingUI::Show()
{
	name->color.w = 0.f;
	left_arrow->Show();
	right_arrow->Show();
	progress_bar.Show();

	current_time = 0.f;
	current_state = SystemSettingUIState::SHOWING;
}

void ProgressBarSettingUI::Hide()
{
	name->color.w = 1.f;
	left_arrow->Hide();
	right_arrow->Hide();
	progress_bar.Hide();

	current_time = 0.f;
	current_state = SystemSettingUIState::HIDING;
}

void ProgressBarSettingUI::FastHide()
{
	name->color.w = 0.f;
	left_arrow->FastHide();
	right_arrow->FastHide();
	progress_bar.FastHide();

	current_state = SystemSettingUIState::HIDDEN;
}

void ProgressBarSettingUI::LeftPressed()
{
	float current_progress = progress_bar.GetProgress();
	float next_progress = math::Max(0.f, current_progress - PROGRESS_BAR_SPEED);
	progress_bar.SetProgress(next_progress);
	left_arrow->Select();
}

void ProgressBarSettingUI::RightPressed()
{
	float current_progress = progress_bar.GetProgress();
	float next_progress = math::Min(1.f, current_progress + PROGRESS_BAR_SPEED);
	progress_bar.SetProgress(next_progress);
	right_arrow->Select();
}

void ProgressBarSettingUI::SetProgress(float progress)
{
	progress_bar.SetProgress(progress);
}

float ProgressBarSettingUI::GetProgress() const
{
	return progress_bar.GetProgress();
}

////////////////////////////////////////////////

void ProgressBarSettingUI::ProgressBarSettingUIProgressBar::Update(float delta_time)
{
	switch (current_state)
	{
	case ProgressBarSettingUI::ProgressBarSettingUIProgressBar::ProgressBarSettingUIProgressBarState::SHOWING:
	{
		current_time += delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		fill_bar->color.w = progress;
		base_bar->color.w = progress;

		if (progress == 1.f)
		{
			current_state = ProgressBarSettingUI::ProgressBarSettingUIProgressBar::ProgressBarSettingUIProgressBarState::SHOWN;
		}
	}
	break;

	case ProgressBarSettingUI::ProgressBarSettingUIProgressBar::ProgressBarSettingUIProgressBarState::HIDING:
	{
		current_time += delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		fill_bar->color.w = 1.f - progress;
		base_bar->color.w = 1.f - progress;

		if (progress == 1.f)
		{
			current_state = ProgressBarSettingUI::ProgressBarSettingUIProgressBar::ProgressBarSettingUIProgressBarState::HIDDEN;
		}
	}
	break;

	default:
		break;
	}
}

void ProgressBarSettingUI::ProgressBarSettingUIProgressBar::Show()
{
	fill_bar->color.w = 0.f;
	base_bar->color.w = 0.f;

	current_time = 0.f;
	current_state = ProgressBarSettingUI::ProgressBarSettingUIProgressBar::ProgressBarSettingUIProgressBarState::SHOWING;
}

void ProgressBarSettingUI::ProgressBarSettingUIProgressBar::Hide()
{
	fill_bar->color.w = 1.f;
	base_bar->color.w = 1.f;

	current_time = 0.f;
	current_state = ProgressBarSettingUI::ProgressBarSettingUIProgressBar::ProgressBarSettingUIProgressBarState::HIDING;
}

void ProgressBarSettingUI::ProgressBarSettingUIProgressBar::FastHide()
{
	fill_bar->color.w = 0.f;
	base_bar->color.w = 0.f;

	current_state = ProgressBarSettingUI::ProgressBarSettingUIProgressBar::ProgressBarSettingUIProgressBarState::HIDDEN;
}

void ProgressBarSettingUI::ProgressBarSettingUIProgressBar::SetProgress(float progress)
{
	this->progress_bar_progress = progress;

	float3 current_translation = fill_bar->owner->transform_2d.GetTranslation();
	current_translation.x = (progress - 1.f) * BAR_WIDTH;

	fill_bar->owner->transform_2d.SetTranslation(current_translation);
}

float ProgressBarSettingUI::ProgressBarSettingUIProgressBar::GetProgress() const
{
	return progress_bar_progress;
}
