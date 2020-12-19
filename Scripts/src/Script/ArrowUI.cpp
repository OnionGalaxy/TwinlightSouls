#include "ArrowUI.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>


ArrowUI* ArrowUIDLL()
{
	ArrowUI* instance = new ArrowUI();
	return instance;
}

ArrowUI::ArrowUI()
{
}

// Use this for initialization before Start()
void ArrowUI::Awake()
{
	normal = static_cast<ComponentImage*>(owner->children[0]->GetComponent(Component::ComponentType::UI_IMAGE));
	highlight = static_cast<ComponentImage*>(owner->children[1]->GetComponent(Component::ComponentType::UI_IMAGE));
}

// Use this for initialization
void ArrowUI::Start()
{

}

// Update is called once per frame
void ArrowUI::Update()
{
	switch (state)
	{
	case ArrowUI::ArrowUIState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / showing_time);

		normal->color.w = progress;

		if (progress == 1.f)
		{
			state = ArrowUI::ArrowUIState::UNSELECTED;
		}
	}
	break;

	case ArrowUI::ArrowUIState::SELECTED:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / select_time);

		highlight->color.w = math::PingPongMod(progress * 2.f, 1.f);

		if (progress == 1.f)
		{
			state = ArrowUI::ArrowUIState::UNSELECTED;
		}
	}
	break;

	case ArrowUI::ArrowUIState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / showing_time);

		normal->color.w = 1 - progress;

		if (progress == 1.f)
		{
			state = ArrowUI::ArrowUIState::HIDDEN;
		}
	}
	break;

	default:
		break;
	}

}

void ArrowUI::Select()
{
	state = ArrowUI::ArrowUIState::SELECTED;
	current_time = 0.f;
}

void ArrowUI::Show()
{
	highlight->color.w = 0.f;
	state = ArrowUI::ArrowUIState::SHOWING;
	current_time = 0.f;
}

void ArrowUI::Hide()
{
	highlight->color.w = 0.f;
	state = ArrowUI::ArrowUIState::HIDING;
	current_time = 0.f;
}

void ArrowUI::FastHide()
{
	normal->color.w = 0.f;
	highlight->color.w = 0.f;
	state = ArrowUI::ArrowUIState::HIDDEN;
}

// Use this for showing variables on inspector
void ArrowUI::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
}