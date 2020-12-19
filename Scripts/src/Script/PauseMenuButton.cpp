#include "PauseMenuButton.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

float PauseMenuButton::glow_timer = 0.f;

PauseMenuButton* PauseMenuButtonDLL()
{
	PauseMenuButton* instance = new PauseMenuButton();
	return instance;
}

PauseMenuButton::PauseMenuButton()
{
}

// Use this for initialization before Start()
void PauseMenuButton::Awake()
{
	glow_image = static_cast<ComponentImage*>(owner->children[2]->GetComponent(Component::ComponentType::UI_IMAGE));
	text_image = static_cast<ComponentImage*>(owner->children[0]->GetComponent(Component::ComponentType::UI_IMAGE));
	highlight_image = static_cast<ComponentImage*>(owner->children[1]->GetComponent(Component::ComponentType::UI_IMAGE));
}

// Use this for initialization
void PauseMenuButton::Start()
{

}

// Update is called once per frame
void PauseMenuButton::Update()
{
	switch (state)
	{
	case PauseMenuButton::PauseMenuButtonState::NOT_SELECTED:

		break;

	case PauseMenuButton::PauseMenuButtonState::FADING_IN:
	{
		current_time += App->time->real_time_delta_time;
		float progress = math::Min(current_time / fade_time, 1.f);
		highlight_image->color.w = progress;

		if (progress == 1)
		{
			state = PauseMenuButton::PauseMenuButtonState::SELECTED;
			current_time = 0.f;
		}
	}
	break;

	case PauseMenuButton::PauseMenuButtonState::SELECTED:
		glow_timer += App->time->real_time_delta_time;
		glow_image->color.w = (math::Sin(glow_timer * 0.001f - pi / 2.f) + 1) * 0.5f;

		break;

	case PauseMenuButton::PauseMenuButtonState::FADING_OUT:
	{
		current_time += App->time->real_time_delta_time;
		float progress = math::Min(current_time / fade_time, 1.f);
		highlight_image->color.w = 1.f - progress;

		if (progress == 1)
		{
			state = PauseMenuButton::PauseMenuButtonState::NOT_SELECTED;
			current_time = 0.f;
		}
	}
	break;

	case PauseMenuButton::PauseMenuButtonState::SHOWING:
	{
		current_time += App->time->real_time_delta_time;
		float progress = math::Min(current_time / fade_time, 1.f);
		text_image->color.w = progress;

		if (progress == 1)
		{
			state = PauseMenuButton::PauseMenuButtonState::NOT_SELECTED;
			current_time = 0.f;
		}
	}
	break;

	case PauseMenuButton::PauseMenuButtonState::HIDING:
	{
		current_time += App->time->real_time_delta_time;
		float progress = math::Min(current_time / fade_time, 1.f);
		text_image->color.w = 1.f - progress;

		if (progress == 1)
		{
			state = PauseMenuButton::PauseMenuButtonState::HIDDEN;
			current_time = 0.f;
		}
	}
	break;

	default:
		break;
	}
}

void PauseMenuButton::Select()
{
	state = PauseMenuButton::PauseMenuButtonState::FADING_IN;
	text_image->color.w = 1.f;
	glow_image->color.w = 0.f;
	current_time = 0.f;
}

void PauseMenuButton::Unselect()
{
	state = PauseMenuButton::PauseMenuButtonState::FADING_OUT;
	current_time = 0.f;
	glow_image->color.w = 0.f;
}

void PauseMenuButton::Show()
{
	state = PauseMenuButton::PauseMenuButtonState::SHOWING;
	current_time = 0.f;
}

void PauseMenuButton::Hide()
{
	state = PauseMenuButton::PauseMenuButtonState::HIDING;
	highlight_image->color.w = 0.f;
	glow_image->color.w = 0.f;
	current_time = 0.f;
}

void PauseMenuButton::FastHide()
{
	state = PauseMenuButton::PauseMenuButtonState::HIDDEN;
	highlight_image->color.w = 0.f;
	text_image->color.w = 0.f;
	glow_image->color.w = 0.f;
}

// Use this for showing variables on inspector
void PauseMenuButton::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}