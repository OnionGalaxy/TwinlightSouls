#include "BootScreensController.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentVideoPlayer.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>


BootScreensController* BootScreensControllerDLL()
{
	BootScreensController* instance = new BootScreensController();
	return instance;
}

BootScreensController::BootScreensController()
{
}

// Use this for initialization before Start()
void BootScreensController::Awake()
{
	for (auto child : owner->children)
	{
		ComponentImage* boot_screen = static_cast<ComponentImage*>(child->GetComponent(Component::ComponentType::UI_IMAGE));
		boot_screen->color.w = 0.f;
		boot_screens.push_back(boot_screen);
	}
	video_player_go = App->scene->GetGameObjectByName("IntroVideo");
	if (video_player_go)
	{
		video_player = static_cast<ComponentVideoPlayer*>(video_player_go->GetComponent(Component::ComponentType::VIDEO_PLAYER));
	}
}

// Use this for initialization
void BootScreensController::Start()
{
}

// Update is called once per frame
void BootScreensController::Update()
{
	if (has_finished)
	{
		return;
	}
	if (showing_video)
	{
		if (video_player->IsFinish() || App->input->GetAnyKeyPressedDown())
		{
			video_player->StopVideo();
			video_player_go->SetEnabled(false);
			has_finished = true;
		}
		return;
	}
	current_time += App->time->delta_time;
	float current_progress;
	switch (current_state)
	{
	case BootScreensController::BootScreenState::FADING_IN:
		current_progress = math::Min(1.f, current_time / boot_screen_transition_time);
		boot_screens[current_screen]->color.w = current_progress;

		if (current_progress == 1.f)
		{
			current_time = 0.f;
			current_state = BootScreensController::BootScreenState::SHOWING;
		}

		break;

	case BootScreensController::BootScreenState::SHOWING:
		current_progress = math::Min(1.f, current_time / boot_screen_showing_time);
		if (current_progress == 1.f)
		{
			current_time = 0.f;
			current_state = BootScreensController::BootScreenState::FADING_OUT;
		}
		break;

	case BootScreensController::BootScreenState::FADING_OUT:
		current_progress = math::Min(1.f, current_time / boot_screen_transition_time);
		boot_screens[current_screen]->color.w = 1 - current_progress;

		if (current_progress == 1.f)
		{
			if (++current_screen == boot_screens.size())
			{
				if (video_player_go)
				{
					video_player_go->SetEnabled(true);
					showing_video = true;
					video_player->PlayVideo();
				}
				else
				{
					has_finished = true;
				}
			}
			else
			{
				current_time = 0.f;
				current_state = BootScreensController::BootScreenState::FADING_IN;
			}
		}

		break;

	default:
		break;
	}
}

void BootScreensController::Init()
{
	has_finished = false;
	current_time = 0.f;
	current_screen = 0;
}

bool BootScreensController::HasFinished() const
{
	return has_finished;
}

// Use this for showing variables on inspector
void BootScreensController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

	ImGui::DragFloat("BootScreensTransitionTime", &boot_screen_transition_time);
	ImGui::DragFloat("BootScreensShowingTime", &boot_screen_showing_time);
	ShowDraggedObjects();
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
void BootScreensController::Save(Config& config) const
{
	config.AddFloat(boot_screen_transition_time, "BootScreensTransitionTime");
	config.AddFloat(boot_screen_showing_time, "BootScreensShowingTime");
	Script::Save(config);
}

// //Use this for linking GO AND VARIABLES automatically
void BootScreensController::Load(const Config& config)
{
	boot_screen_transition_time = config.GetFloat("BootScreensTransitionTime", 1000.f);
	boot_screen_showing_time = config.GetFloat("BootScreensShowingTime", 2000.f);
	Script::Load(config);
}