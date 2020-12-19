#include "SystemCursorUI.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>


SystemCursorUI* SystemCursorUIDLL()
{
	SystemCursorUI* instance = new SystemCursorUI();
	return instance;
}

SystemCursorUI::SystemCursorUI()
{
}

// Use this for initialization before Start()
void SystemCursorUI::Awake()
{
	base_cursor = static_cast<ComponentImage*>(owner->children[0]->GetComponent(Component::ComponentType::UI_IMAGE));
	glow_cursor = static_cast<ComponentImage*>(owner->children[1]->GetComponent(Component::ComponentType::UI_IMAGE));
}

// Use this for initialization
void SystemCursorUI::Start()
{
}

// Update is called once per frame
void SystemCursorUI::Update()
{
	switch (current_state)
	{
	case SystemCursorUI::SystemCursorUIState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transitioning_time);

		base_cursor->color.w = progress;

		if (progress == 1.f)
		{
			current_state = SystemCursorUI::SystemCursorUIState::SHOWN;
		}
	}
	break;

	case SystemCursorUI::SystemCursorUIState::SHOWN:
	{
		glow_cursor->color.w = (math::Sin(App->time->time * 0.005f - pi / 2.f) + 1) * 0.5f;
	}
	break;

	case SystemCursorUI::SystemCursorUIState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transitioning_time);

		base_cursor->color.w = 1.f - progress;

		if (progress == 1.f)
		{
			current_state = SystemCursorUI::SystemCursorUIState::HIDDEN;
		}
	}
	break;

	case SystemCursorUI::SystemCursorUIState::MOVING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transitioning_time);

		glow_cursor->color.w = (math::Sin(App->time->time * 0.005f - pi / 2.f) + 1) * 0.5f;

		float next_translation_height = math::Lerp(previous_height, translations[current_position], progress);
		float3 current_translation = owner->transform_2d.GetTranslation();
		current_translation.y = next_translation_height;
		owner->transform_2d.SetTranslation(current_translation);

		if (progress == 1.f)
		{
			current_state = SystemCursorUI::SystemCursorUIState::SHOWN;
		}
	}
	break;

	default:
		break;
	}

}

void SystemCursorUI::Show()
{
	base_cursor->color.w = 0.f;
	glow_cursor->color.w = 0.f;
	current_state = SystemCursorUI::SystemCursorUIState::SHOWING;
	current_time = 0.f;
}

void SystemCursorUI::Hide()
{
	base_cursor->color.w = 1.f;
	glow_cursor->color.w = 0.f;
	current_state = SystemCursorUI::SystemCursorUIState::HIDING;
	current_time = 0.f;
}

void SystemCursorUI::FastHide()
{
	base_cursor->color.w = 0.f;
	glow_cursor->color.w = 0.f;
	current_state = SystemCursorUI::SystemCursorUIState::HIDDEN;
}

void SystemCursorUI::MoveUp()
{
	if (current_state != SystemCursorUIState::MOVING && current_state != SystemCursorUIState::SHOWN)
	{
		return;
	}

	int next_position = current_position - 1;
	if (next_position == -1)
	{
		next_position = 3;
	}
	SetCurrentPosition(next_position);
}

void SystemCursorUI::MoveDown()
{
	if (current_state != SystemCursorUIState::MOVING && current_state != SystemCursorUIState::SHOWN)
	{
		return;
	}

	int next_position = (current_position + 1) % 4;
	SetCurrentPosition(next_position);
}

void SystemCursorUI::SetCurrentPosition(int position)
{
	previous_height = owner->transform_2d.GetTranslation().y;
	current_position = position;
	current_state = SystemCursorUI::SystemCursorUIState::MOVING;
	current_time = 0.f;
}

int SystemCursorUI::GetCurrentPosition() const
{
	return current_position;
}