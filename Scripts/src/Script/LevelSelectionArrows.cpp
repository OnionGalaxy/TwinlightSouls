#include "LevelSelectionArrows.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>


LevelSelectionArrows* LevelSelectionArrowsDLL()
{
	LevelSelectionArrows* instance = new LevelSelectionArrows();
	return instance;
}

LevelSelectionArrows::LevelSelectionArrows()
{
	
}

// Use this for initialization before Start()
void LevelSelectionArrows::Awake()
{
	unselected = static_cast<ComponentImage*>(owner->GetChildrenWithName("Arrow Unselected")->GetComponent(Component::ComponentType::UI_IMAGE));
	selected = static_cast<ComponentImage*>(owner->GetChildrenWithName("Arrow Selected")->GetComponent(Component::ComponentType::UI_IMAGE));
	background = static_cast<ComponentImage*>(owner->GetChildrenWithName("Arrow Background")->GetComponent(Component::ComponentType::UI_IMAGE));
}

// Use this for initialization
void LevelSelectionArrows::Start()
{

}

// Update is called once per frame
void LevelSelectionArrows::Update()
{
	if (state == LevelSelectionArrows::LevelSelectionArrowsState::SELECTED || state == LevelSelectionArrows::LevelSelectionArrowsState::UNSELECTED)
	{
		glow_timer += App->time->delta_time;
		background->color.w = (math::Sin(glow_timer * 0.001f - pi / 2.f) + 1) * 0.5f;
	}

	switch (state)
	{
	case LevelSelectionArrows::LevelSelectionArrowsState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		unselected->color.w = progress;

		if (progress == 1.f)
		{
			state = LevelSelectionArrows::LevelSelectionArrowsState::UNSELECTED;
			glow_timer = 0.f;
		}
	}
	break;

	case LevelSelectionArrows::LevelSelectionArrowsState::SELECTED:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		selected->color.w = math::PingPongMod(progress * 2.f, 1.f);

		if (progress == 1.f)
		{
			state = LevelSelectionArrows::LevelSelectionArrowsState::UNSELECTED;
		}
	}
	break;

	case LevelSelectionArrows::LevelSelectionArrowsState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / transition_time);

		unselected->color.w = 1.f - progress;

		if (progress == 1.f)
		{
			state = LevelSelectionArrows::LevelSelectionArrowsState::HIDDEN;
		}
	}
	break;

	default:
		break;
	}

}

void LevelSelectionArrows::Show()
{
	selected->color.w = 0.f;
	background->color.w = 0.f;
	state = LevelSelectionArrows::LevelSelectionArrowsState::SHOWING;
	current_time = 0.f;
}

void LevelSelectionArrows::Hide()
{
	selected->color.w = 0.f;
	background->color.w = 0.f;
	state = LevelSelectionArrows::LevelSelectionArrowsState::HIDING;
	current_time = 0.f;
}

void LevelSelectionArrows::FastHide()
{
	state = LevelSelectionArrows::LevelSelectionArrowsState::HIDDEN;
	
	unselected->color.w = 0.f;
	selected->color.w = 0.f;
	background->color.w = 0.f;
}

void LevelSelectionArrows::Select()
{
	state = LevelSelectionArrows::LevelSelectionArrowsState::SHOWING;
	current_time = 0.f;
}
