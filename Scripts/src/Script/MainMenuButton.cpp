#include "MainMenuButton.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

float MainMenuButton::glow_timer = 0.f;

MainMenuButton* MainMenuButtonDLL()
{
	MainMenuButton* instance = new MainMenuButton();
	return instance;
}

MainMenuButton::MainMenuButton()
{
}

// Use this for initialization before Start()
void MainMenuButton::Awake()
{
	glow_image = static_cast<ComponentImage*>(owner->children[0]->GetComponent(Component::ComponentType::UI_IMAGE));
	text_image = static_cast<ComponentImage*>(owner->children[1]->GetComponent(Component::ComponentType::UI_IMAGE));
	highlight_image = static_cast<ComponentImage*>(owner->children[2]->GetComponent(Component::ComponentType::UI_IMAGE));
}

// Use this for initialization
void MainMenuButton::Start()
{
	
}

// Update is called once per frame
void MainMenuButton::Update()
{
	switch (state)
	{
	case MainMenuButton::MainMenuButtonState::NOT_SELECTED:

		break;

	case MainMenuButton::MainMenuButtonState::FADING_IN:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(current_time / fade_time, 1.f);
		highlight_image->color.w = progress;

		if (progress == 1)
		{
			state = MainMenuButton::MainMenuButtonState::SELECTED;
			current_time = 0.f;
		}
	}
	break;

	case MainMenuButton::MainMenuButtonState::SELECTED:
		glow_timer += App->time->delta_time;
		glow_image->color.w = (math::Sin(glow_timer * 0.001f - pi/2.f) + 1) * 0.5f;

		break;

	case MainMenuButton::MainMenuButtonState::FADING_OUT:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(current_time / fade_time, 1.f);
		highlight_image->color.w = 1.f - progress;

		if (progress == 1)
		{
			state = MainMenuButton::MainMenuButtonState::NOT_SELECTED;
			current_time = 0.f;
		}
	}
	break;

	case MainMenuButton::MainMenuButtonState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(current_time / fade_time, 1.f);
		text_image->color.w = progress;

		if (progress == 1)
		{
			state = MainMenuButton::MainMenuButtonState::NOT_SELECTED;
			current_time = 0.f;
		}
	}
	break;

	case MainMenuButton::MainMenuButtonState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(current_time / fade_time, 1.f);
		text_image->color.w = 1.f - progress;

		if (progress == 1)
		{
			state = MainMenuButton::MainMenuButtonState::HIDDEN;
			current_time = 0.f;
		}
	}
	break;

	default:
		break;
	}
}

void MainMenuButton::Select()
{
	state = MainMenuButton::MainMenuButtonState::FADING_IN;
	text_image->color.w = 1.f;
	glow_image->color.w = 0.f;
	current_time = 0.f;
}

void MainMenuButton::Unselect()
{
	state = MainMenuButton::MainMenuButtonState::FADING_OUT;
	current_time = 0.f;
	glow_image->color.w = 0.f;
}

void MainMenuButton::Show()
{
	state = MainMenuButton::MainMenuButtonState::SHOWING;
	current_time = 0.f;
}

void MainMenuButton::Hide()
{
	state = MainMenuButton::MainMenuButtonState::HIDING;
	highlight_image->color.w = 0.f;
	glow_image->color.w = 0.f;
	current_time = 0.f;
}

void MainMenuButton::FastHide()
{
	state = MainMenuButton::MainMenuButtonState::HIDDEN;
	highlight_image->color.w = 0.f;
	text_image->color.w = 0.f;
	glow_image->color.w = 0.f;
}

// Use this for showing variables on inspector
void MainMenuButton::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}