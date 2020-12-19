#include "SwitchSettingUI.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "ArrowUI.h"

SwitchSettingUI* SwitchSettingUIDLL()
{
	SwitchSettingUI* instance = new SwitchSettingUI();
	return instance;
}

SwitchSettingUI::SwitchSettingUI()
{
}

// Use this for initialization before Start()
void SwitchSettingUI::Awake()
{
	name = static_cast<ComponentImage*>(owner->GetChildrenWithName("Name")->GetComponent(Component::ComponentType::UI_IMAGE));
	left_arrow = static_cast<ArrowUI*>(owner->GetChildrenWithName("Left Arrow")->GetComponentScript("ArrowUI")->script);
	right_arrow = static_cast<ArrowUI*>(owner->GetChildrenWithName("Right Arrow")->GetComponentScript("ArrowUI")->script);

	GameObject* switch_options_go = owner->GetChildrenWithName("Switch");
	for (auto& switch_option_go : switch_options_go->children)
	{
		SwitchSettingOption switch_option;
		switch_option.switch_options_name = static_cast<ComponentImage*>(switch_option_go->GetComponent(Component::ComponentType::UI_IMAGE));
		switch_options.push_back(switch_option);
	}
}

// Use this for initialization
void SwitchSettingUI::Start()
{

}

// Update is called once per frame
void SwitchSettingUI::Update()
{
	switch (current_state)
	{
	case SwitchSettingUI::SystemSettingUIState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		name->color.w = progress;

		if (progress == 1.f)
		{
			current_state = SwitchSettingUI::SystemSettingUIState::SHOWN;
		}
	}
	break;

	case SwitchSettingUI::SystemSettingUIState::SHOWN:
		break;

	case SwitchSettingUI::SystemSettingUIState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		name->color.w = 1 - progress;

		if (progress == 1.f)
		{
			current_state = SwitchSettingUI::SystemSettingUIState::HIDDEN;
		}
	}
	break;

	default:
		break;
	}

	for (size_t i = 0; i < switch_options.size(); ++i)
	{
		switch_options[i].Update(App->time->delta_time);
	}
}

void SwitchSettingUI::Show()
{
	name->color.w = 0.f;
	left_arrow->Show();
	right_arrow->Show();

	current_time = 0.f;
	current_state = SystemSettingUIState::SHOWING;
}

void SwitchSettingUI::Hide()
{
	name->color.w = 0.f;
	left_arrow->Hide();
	right_arrow->Hide();
	for (size_t i = 0; i < switch_options.size(); ++i)
	{
		if (i == current_option)
		{
			switch_options[i].Hide();
		}
		else
		{
			switch_options[i].FastHide();
		}
	}

	current_time = 0.f;
	current_state = SystemSettingUIState::HIDING;
}

void SwitchSettingUI::FastHide()
{
	name->color.w = 0.f;
	left_arrow->FastHide();
	right_arrow->FastHide();
	for (size_t i = 0; i < switch_options.size(); ++i)
	{
		switch_options[i].FastHide();
	}
	current_state = SystemSettingUIState::HIDDEN;
}

void SwitchSettingUI::RightPressed()
{
	switch_options[current_option].Hide();
	current_option = (current_option + 1) % switch_options.size();
	switch_options[current_option].Show();
	right_arrow->Select();
}

void SwitchSettingUI::LeftPressed()
{
	switch_options[current_option].Hide();
	if (--current_option < 0)
	{
		current_option = switch_options.size() - 1;
	}
	switch_options[current_option].Show();
	left_arrow->Select();
}

int SwitchSettingUI::GetCurrentOption() const
{
	return current_option;
}

void SwitchSettingUI::SetCurrentOption(int current_option)
{
	switch_options[current_option].Hide();
	this->current_option = current_option;
	switch_options[current_option].Show();
}

/////////////////////////////////////////////////////////////

void SwitchSettingUI::SwitchSettingOption::Update(float delta_time)
{
	switch (current_state)
	{
	case SwitchSettingUI::SwitchSettingOption::SwitchSettingOptionState::SHOWING:
	{
		current_time += delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		switch_options_name->color.w = progress;

		if (progress == 1.f)
		{
			current_state = SwitchSettingUI::SwitchSettingOption::SwitchSettingOptionState::SHOWN;
		}
	}
	break;

	case SwitchSettingUI::SwitchSettingOption::SwitchSettingOptionState::HIDING:
	{
		current_time += delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		switch_options_name->color.w = 1 - progress;

		if (progress == 1.f)
		{
			current_state = SwitchSettingUI::SwitchSettingOption::SwitchSettingOptionState::HIDDEN;
		}
	}
	break;

	default:
		break;
	}
}

void SwitchSettingUI::SwitchSettingOption::Show()
{
	switch_options_name->color.w = 0.f;

	current_time = 0.f;
	current_state = SwitchSettingUI::SwitchSettingOption::SwitchSettingOptionState::SHOWING;
}

void SwitchSettingUI::SwitchSettingOption::Hide()
{
	switch_options_name->color.w = 1.f;

	current_time = 0.f;
	current_state = SwitchSettingUI::SwitchSettingOption::SwitchSettingOptionState::HIDING;
}

void SwitchSettingUI::SwitchSettingOption::FastHide()
{
	switch_options_name->color.w = 0.f;
	current_state = SwitchSettingUI::SwitchSettingOption::SwitchSettingOptionState::HIDDEN;
}