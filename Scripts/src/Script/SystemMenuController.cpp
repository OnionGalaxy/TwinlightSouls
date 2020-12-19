#include "SystemMenuController.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "InputManager.h"
#include "MainMenuButton.h"
#include "ProgressBarSettingUI.h"
#include "SystemCursorUI.h"
#include "SwitchSettingUI.h"
#include "UIMainMenuInputController.h"

const float SystemMenuController::SHOWING_TIME = 250.f;

SystemMenuController* SystemMenuControllerDLL()
{
	SystemMenuController* instance = new SystemMenuController();
	return instance;
}

SystemMenuController::SystemMenuController()
{
}

// Use this for initialization before Start()
void SystemMenuController::Awake()
{
	input_manager = static_cast<InputManager*>(App->scene->GetGameObjectByName("Input Manager")->GetComponentScript("InputManager")->script);
	menu_audio_source = static_cast<ComponentAudioSource*>(App->scene->GetGameObjectByName("Main Camera")->GetComponent(Component::ComponentType::AUDIO_SOURCE));

	GameObject* tag_helper_controller_go = App->scene->GetGameObjectByName("System Menu")->GetChildrenWithName("Tag Helper Controller");
	tag_helper_controller = static_cast<MainMenuButton*>(tag_helper_controller_go->GetComponentScript("MainMenuButton")->script);

	GameObject* tag_helper_keyboard_go = App->scene->GetGameObjectByName("System Menu")->GetChildrenWithName("Tag Helper Keyboard");
	tag_helper_keyboard = static_cast<MainMenuButton*>(tag_helper_keyboard_go->GetComponentScript("MainMenuButton")->script);

	tag_helper_buttons = input_manager->total_game_controllers > 0 ? tag_helper_controller : tag_helper_keyboard;

	/////////////////////////////////////////////////////////////

	GameObject* back_button_controller_go = App->scene->GetGameObjectByName("System Menu")->GetChildrenWithName("Back Button Controller");
	back_button_controller = static_cast<MainMenuButton*>(back_button_controller_go->GetComponentScript("MainMenuButton")->script);

	GameObject* back_button_keyboard_go = App->scene->GetGameObjectByName("System Menu")->GetChildrenWithName("Back Button Keyboard");
	back_button_keyboard = static_cast<MainMenuButton*>(back_button_keyboard_go->GetComponentScript("MainMenuButton")->script);

	back_button = input_manager->total_game_controllers > 0 ? back_button_controller : back_button_keyboard;

	/////////////////////////////////////////////////////////////

	GraphicsSystemTag* graphics_tag = new GraphicsSystemTag(App, input_manager, menu_audio_source);
	MainMenuButton* graphics_tag_name = static_cast<MainMenuButton*>(App->scene->GetGameObjectByName("Settings")->GetChildrenWithName("Tag Name")->GetComponentScript("MainMenuButton")->script);
	graphics_tag->SetTagName(graphics_tag_name);
	graphics_tag->Awake();

	SimpleImageSystemTag* controller_image_tag = new SimpleImageSystemTag(App, input_manager, menu_audio_source);
	ComponentImage* controller_image = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Controller")->GetChildrenWithName("Image")->GetComponent(Component::ComponentType::UI_IMAGE));
	controller_image_tag->SetImage(controller_image);
	MainMenuButton* controller_image_tag_name = static_cast<MainMenuButton*>(App->scene->GetGameObjectByName("Controller")->GetChildrenWithName("Tag Name")->GetComponentScript("MainMenuButton")->script);
	controller_image_tag->SetTagName(controller_image_tag_name);

	SimpleImageSystemTag* keyboard_image_tag = new SimpleImageSystemTag(App, input_manager, menu_audio_source);
	ComponentImage* keyboard_image = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Keyboard")->GetChildrenWithName("Image")->GetComponent(Component::ComponentType::UI_IMAGE));
	keyboard_image_tag->SetImage(keyboard_image);
	MainMenuButton* keyboard_tag_name = static_cast<MainMenuButton*>(App->scene->GetGameObjectByName("Keyboard")->GetChildrenWithName("Tag Name")->GetComponentScript("MainMenuButton")->script);
	keyboard_image_tag->SetTagName(keyboard_tag_name);

	system_tags.push_back(graphics_tag);
	system_tags.push_back(controller_image_tag);
	system_tags.push_back(keyboard_image_tag);
}

// Use this for initialization
void SystemMenuController::Start()
{
	static_cast<GraphicsSystemTag*>(system_tags[0])->Start();
}

// Update is called once per frame
void SystemMenuController::Update()
{
	MainMenuButton* last_helper_button = tag_helper_buttons;
	tag_helper_buttons = input_manager->total_game_controllers > 0 ? tag_helper_controller : tag_helper_keyboard;
	if (tag_helper_buttons != last_helper_button)
	{
		last_helper_button->FastHide();
		if (current_state != SystemMenuState::HIDING && current_state != SystemMenuState::HIDDEN)
		{
			tag_helper_buttons->Select();
		}
	}

	MainMenuButton* last_back_button = back_button;
	back_button = input_manager->total_game_controllers > 0 ? back_button_controller : back_button_keyboard;
	if (back_button != last_back_button)
	{
		last_back_button->FastHide();
		if (current_state != SystemMenuState::HIDING && current_state != SystemMenuState::HIDDEN)
		{
			back_button->Select();
		}
	}

	switch (current_state)
	{

	case SystemMenuState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / SHOWING_TIME);

		if (progress == 1.f)
		{
			tag_helper_buttons->Select();
			back_button->Select();
			current_state = SystemMenuState::TAG_OPENED;
		}
	}
	break;

	case SystemMenuState::TAG_OPENED:
	{
		if (UIMainMenuInputController::LeftSwitchPressed(input_manager))
		{
			system_tags[current_tag]->Hide();
			if (--current_tag < 0)
			{
				current_tag = system_tags.size() - 1;
			}
			menu_audio_source->PlayEvent("menu_hover");
			system_tags[current_tag]->Show();
		}
		else if (UIMainMenuInputController::RightSwitchPressed(input_manager))
		{
			system_tags[current_tag]->Hide();
			current_tag = (current_tag + 1) % system_tags.size();
			menu_audio_source->PlayEvent("menu_hover");
			system_tags[current_tag]->Show();
		}
		else if (UIMainMenuInputController::CancelButtonPressed(input_manager))
		{
			tag_helper_buttons->Hide();
			back_button->Hide();
			for (size_t i = 0; i < system_tags.size(); ++i)
			{
				if (i == current_tag)
				{
					system_tags[i]->Hide();
				}
				else
				{
					system_tags[i]->FastHide();
				}
			}
			current_time = 0.f;
			menu_audio_source->PlayEvent("menu_select");
			current_state = SystemMenuState::HIDING;
		}
	}

	break;

	case SystemMenuState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / SHOWING_TIME);
		
		if (progress == 1.f)
		{
			current_state = SystemMenuState::HIDDEN;
			closed = true;
		}
	}

	break;

	default:
		break;
	}

	for (size_t i = 0; i < system_tags.size(); ++i)
	{
		system_tags[i]->Update();
	}
}

void SystemMenuController::Show()
{
	closed = false;
	tag_helper_buttons->Show();
	back_button->Show();

	current_tag = 0;
	system_tags[current_tag]->Show();

	current_time = 0.f;
	current_state = SystemMenuState::SHOWING;
}

void SystemMenuController::FastHide()
{
	closed = true;
	tag_helper_buttons->FastHide();
	tag_helper_controller->FastHide();
	tag_helper_keyboard->FastHide();

	back_button->FastHide();
	back_button_controller->FastHide();
	back_button_keyboard->FastHide();

	tag_helper_buttons->FastHide();

	for (size_t i = 0; i < system_tags.size(); ++i)
	{
		system_tags[i]->FastHide();
	}

	current_state = SystemMenuState::HIDDEN;
}

// Use this for showing variables on inspector
void SystemMenuController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
}

//////////////////////////////////////////////////

void SystemMenuController::SimpleImageSystemTag::Update()
{
	switch (state)
	{
	case SystemMenuController::SimpleImageSystemTag::SimpleImageSystemTagState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / SystemMenuController::SHOWING_TIME);

		tag_image->color.w = progress;

		if (progress == 1.f)
		{
			tag_name->Select();
			state = SystemMenuController::SimpleImageSystemTag::SimpleImageSystemTagState::SHOWN;
		}
	}
	break;

	case SystemMenuController::SimpleImageSystemTag::SimpleImageSystemTagState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / SystemMenuController::SHOWING_TIME);

		tag_image->color.w = 1 - progress;

		if (progress == 1.f)
		{
			state = SystemMenuController::SimpleImageSystemTag::SimpleImageSystemTagState::HIDDEN;
		}
	}
	break;

	default:
		break;
	}
}

void SystemMenuController::SimpleImageSystemTag::Show()
{
	state = SystemMenuController::SimpleImageSystemTag::SimpleImageSystemTagState::SHOWING;
	tag_name->Show();
	current_time = 0.f;
}

void SystemMenuController::SimpleImageSystemTag::Hide()
{
	state = SystemMenuController::SimpleImageSystemTag::SimpleImageSystemTagState::HIDING;
	tag_name->Hide();
	current_time = 0.f;
}

void SystemMenuController::SimpleImageSystemTag::FastHide()
{
	state = SystemMenuController::SimpleImageSystemTag::SimpleImageSystemTagState::HIDDEN;
	tag_image->color.w = 0.f;
	tag_name->FastHide();
}

void SystemMenuController::SimpleImageSystemTag::SetImage(ComponentImage * tag_image)
{
	this->tag_image = tag_image;
}

//////////////////////////////////////////////////

void SystemMenuController::GraphicsSystemTag::Awake()
{
	background_image = static_cast<ComponentImage*>(App->scene->GetGameObjectByName("Settings")->GetChildrenWithName("Background Image")->GetComponent(Component::ComponentType::UI_IMAGE));
	
	GameObject* settings_list_go = App->scene->GetGameObjectByName("Settings")->GetChildrenWithName("Settings List");
	
	ProgressBarSettingUI* brightness_setting = static_cast<ProgressBarSettingUI*>(settings_list_go->GetChildrenWithName("Brightness")->GetComponentScript("ProgressBarSettingUI")->script);
	system_settings.push_back(brightness_setting);
	
	SwitchSettingUI* antialiasing_setting = static_cast<SwitchSettingUI*>(settings_list_go->GetChildrenWithName("Antialiasing")->GetComponentScript("SwitchSettingUI")->script);
	system_settings.push_back(antialiasing_setting);
	
	SwitchSettingUI* vsync_setting = static_cast<SwitchSettingUI*>(settings_list_go->GetChildrenWithName("VSync")->GetComponentScript("SwitchSettingUI")->script);
	system_settings.push_back(vsync_setting);
	
	SwitchSettingUI* hdr_setting = static_cast<SwitchSettingUI*>(settings_list_go->GetChildrenWithName("HDR")->GetComponentScript("SwitchSettingUI")->script);
	system_settings.push_back(hdr_setting);

	GameObject* cursor_go = App->scene->GetGameObjectByName("Settings")->GetChildrenWithName("Cursor");
	system_cursor = static_cast<SystemCursorUI*>(cursor_go->GetComponentScript("SystemCursorUI")->script);
}

void SystemMenuController::GraphicsSystemTag::Start()
{
	
}

void SystemMenuController::GraphicsSystemTag::Update()
{
	switch (state)
	{
	case SystemMenuController::GraphicsSystemTag::GraphicSystemTagState::SHOWING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / SystemMenuController::SHOWING_TIME);

		background_image->color.w = progress;

		if (progress == 1.f)
		{
			tag_name->Select();
			state = SystemMenuController::GraphicsSystemTag::GraphicSystemTagState::SHOWN;
		}
	}
	break;

	case SystemMenuController::GraphicsSystemTag::GraphicSystemTagState::SHOWN:
	{
		if (UIMainMenuInputController::ConfirmMovedUp(input_manager))
		{
			system_cursor->MoveUp();
			current_setting = system_cursor->GetCurrentPosition();
			menu_audio_source->PlayEvent("menu_hover");
		}

		else if (UIMainMenuInputController::ConfirmMovedDown(input_manager))
		{
			system_cursor->MoveDown();
			current_setting = system_cursor->GetCurrentPosition();
			menu_audio_source->PlayEvent("menu_hover");
		}
		else if (UIMainMenuInputController::ConfirmMovedLeft(input_manager))
		{
			system_settings[current_setting]->LeftPressed();
			if (current_setting == 0)
			{
				App->renderer->exposure = static_cast<ProgressBarSettingUI*>(system_settings[current_setting])->GetProgress() * 2.f;
			}
			else if (current_setting == 1)
			{
				App->renderer->SetAntialiasing(static_cast<SwitchSettingUI*>(system_settings[current_setting])->GetCurrentOption());
			}
			else if (current_setting == 2)
			{
				App->renderer->SetVSync(static_cast<SwitchSettingUI*>(system_settings[current_setting])->GetCurrentOption());
			}
			else if (current_setting == 3)
			{
				App->renderer->SetHDRType(static_cast<ModuleRender::HDRType>(static_cast<SwitchSettingUI*>(system_settings[current_setting])->GetCurrentOption()));
			}
			menu_audio_source->PlayEvent("menu_hover");
		}

		else if (UIMainMenuInputController::ConfirmMovedRight(input_manager))
		{
			system_settings[current_setting]->RightPressed();
			if (current_setting == 0)
			{
				App->renderer->exposure = static_cast<ProgressBarSettingUI*>(system_settings[current_setting])->GetProgress() * 2.f;
			}
			else if (current_setting == 1)
			{
				App->renderer->SetAntialiasing(static_cast<SwitchSettingUI*>(system_settings[current_setting])->GetCurrentOption());
			}
			else if (current_setting == 2)
			{
				App->renderer->SetVSync(static_cast<SwitchSettingUI*>(system_settings[current_setting])->GetCurrentOption());
			}
			else if (current_setting == 3)
			{
				App->renderer->SetHDRType(static_cast<ModuleRender::HDRType>(static_cast<SwitchSettingUI*>(system_settings[current_setting])->GetCurrentOption()));
			}
			menu_audio_source->PlayEvent("menu_hover");
		}
	}
	break;

	case SystemMenuController::GraphicsSystemTag::GraphicSystemTagState::HIDING:
	{
		current_time += App->time->delta_time;
		float progress = math::Min(1.f, current_time / SystemMenuController::SHOWING_TIME);

		background_image->color.w = 1 - progress;

		if (progress == 1.f)
		{
			state = SystemMenuController::GraphicsSystemTag::GraphicSystemTagState::HIDDEN;
		}
	}
	break;

	default:
		break;
	}
}

void SystemMenuController::GraphicsSystemTag::Show()
{
	state = SystemMenuController::GraphicsSystemTag::GraphicSystemTagState::SHOWING;
	tag_name->Show();
	system_cursor->Show();

	static_cast<ProgressBarSettingUI*>(system_settings[0])->SetProgress(0.5f * App->renderer->exposure);
	system_settings[0]->Show();

	static_cast<SwitchSettingUI*>(system_settings[1])->SetCurrentOption(App->renderer->antialiasing ? 1 : 0);
	system_settings[1]->Show();

	static_cast<SwitchSettingUI*>(system_settings[2])->SetCurrentOption(App->renderer->vsync ? 1 : 0);
	system_settings[2]->Show();

	static_cast<SwitchSettingUI*>(system_settings[3])->SetCurrentOption(static_cast<int>(App->renderer->hdr_type));
	system_settings[3]->Show();
	
	current_time = 0.f;
}

void SystemMenuController::GraphicsSystemTag::Hide()
{
	state = SystemMenuController::GraphicsSystemTag::GraphicSystemTagState::HIDING;
	tag_name->Hide();
	system_cursor->Hide();
	for (auto& system_setting : system_settings)
	{
		system_setting->Hide();
	}
	current_time = 0.f;
}

void SystemMenuController::GraphicsSystemTag::FastHide()
{
	state = SystemMenuController::GraphicsSystemTag::GraphicSystemTagState::HIDDEN;
	background_image->color.w = 0.f;
	tag_name->FastHide();
	system_cursor->FastHide();
	for (auto& system_setting : system_settings)
	{
		system_setting->FastHide();
	}
}
