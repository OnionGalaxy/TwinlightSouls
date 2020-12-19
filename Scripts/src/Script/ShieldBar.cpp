#include "ShieldBar.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTransform2D.h"

#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleDebugDraw.h"

#include "imgui.h"


ShieldBar* ShieldBarDLL()
{
	ShieldBar* instance = new ShieldBar();
	return instance;
}

// Use this for initialization before Start()
void ShieldBar::Awake()
{
	fill_bar_game_object = owner->children[0];
	fill_bar_transform = &fill_bar_game_object->transform_2d;
	progress_bar_transform = &owner->parent->transform_2d;
}
// Use this for initialization
void ShieldBar::Start()
{
	SetProgress(progress);
	
}

// Update is called once per frame
void ShieldBar::Update()
{
	SetShieldStatus();
	UpdatePlayerDistToCamera();
	UpdateShieldTransform();
}

void ShieldBar::SetProgress(float new_progress)
{
	if (new_progress < 0.f || new_progress > 1.f)
	{
		active = true;
		return;
	}

	progress = new_progress;
	if(progress_bar_transform == nullptr)
	{
		progress_bar_transform = &owner->parent->transform_2d;
		fill_bar_game_object = owner->children[0];
		fill_bar_transform = &fill_bar_game_object->transform_2d;
		owner->SetEnabled(true);
		App->engine_log->Log("PROGRESS BAR TRANSFORM OF: %s IS NULLPTR", owner->name.c_str());
		return;
	}

	float progress_bar_width = progress_bar_transform->GetWidth();
	float progress_bar_height = progress_bar_transform->GetHeight();
	float fill_bar_padding = 0;

	if (!vertical)
	{
		fill_bar_padding = progress_bar_width * (1 - progress);
	}
	else
	{
		fill_bar_padding = progress_bar_height * (1 - progress);
	}
	float3 translation = fill_bar_transform->GetTranslation();

	float sign_value = (inverted) ? 1.f : -1.f;

	if (vertical)
	{
		fill_bar_transform->SetTranslation(float3(translation.x, sign_value * fill_bar_padding, translation.z));
	}
	else
	{
		fill_bar_transform->SetTranslation(float3(sign_value * fill_bar_padding, translation.y, translation.z));
	}
}

// Use this for showing variables on inspector
void ShieldBar::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Checkbox("Inverted", &inverted);
	ImGui::Checkbox("Vertical", &vertical);
	ImGui::DragFloat("Y_Offset", &y_offset, -100.0f, 100.0f);
	ImGui::DragFloat("custom dist", &custom_dist, -10.0f, 10.0f);
	ImGui::DragFloat("dist to camera", &dist_to_camera, -100.0f, 100.0f);
	ImGui::DragFloat("scale factor", &scale_factor, -1000.0f, 1000.0f);
	ImGui::DragFloat("pos_factor", &pos_factor, -100.0f, 100.0f);
	ImGui::DragFloat("rad_y_factor", &rad_y_factor, -1000.0f, 1000.0f);
	ImGui::DragFloat("far_factor", &far_factor, -1000.0f, 1000.0f);
	ImGui::DragFloat("close_factor", &close_factor, -1000.0f, 1000.0f);
}

void ShieldBar::Save(Config& config) const
{
	config.AddBool(inverted, "Inverted");
	config.AddBool(vertical, "Vertical");
	config.AddFloat(custom_dist, "custom_dist");
	config.AddFloat(y_offset, "y_offset");
	config.AddFloat(pos_factor, "pos_factor");
	config.AddFloat(rad_y_factor, "rad_y_factor");
	config.AddFloat(close_factor, "close_factor");
	Script::Save(config);
}

void ShieldBar::Load(const Config& config)
{
	
	inverted = config.GetBool("Inverted", false);
	vertical = config.GetBool("Vertical", false);
	custom_dist = config.GetFloat("custom_dist", 18.223f);
	y_offset = config.GetFloat("y_offset", 28.0f);
	pos_factor = config.GetFloat("pos_factor", 11.0f);
	rad_y_factor = config.GetFloat("rad_y_factor", 2.5f);
	close_factor = config.GetFloat("close_factor", 1.0f);
	Script::Load(config);
}

void ShieldBar::SetPlayerPosition(const float3& position)
{
	player_pos = position;
}

void ShieldBar::SetShieldStatus() const
{
	if (active)
	{
		owner->parent->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Enable();
		owner->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Enable();
		owner->children[0]->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Enable();
		owner->children[1]->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Enable();
	}
	else
	{
		owner->parent->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Disable();
		owner->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Disable();
		owner->children[0]->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Disable();
		owner->children[1]->GetComponent(Component::ComponentType::CANVAS_RENDERER)->Disable();
	}
}

void ShieldBar::UpdatePlayerDistToCamera()
{
	//update dist to camera
	float3 eye = App->cameras->main_camera->camera_frustum.pos;
	dist_to_camera = eye.Distance(player_pos);
}

void ShieldBar::UpdateShieldTransform()
{
	float4 position_float4 = float4(player_pos, 1.f);
	float4 clip_coordinates = App->cameras->main_camera->GetClipMatrix() * position_float4;
	float3 device_coordinates = clip_coordinates.xyz() / clip_coordinates.w;

	shield_canvas_position = float2
	(
		device_coordinates.x * App->ui->main_canvas->GetCanvasScreenSize().x / 2.f,
		device_coordinates.y * App->ui->main_canvas->GetCanvasScreenSize().y / 2.f
	);

	
	shield_canvas_position.x = shield_canvas_position.x + shield_canvas_position.x / pos_factor;
	
	
	if (dist_to_camera - custom_dist < 0)
	{
		shield_canvas_position.y = shield_canvas_position.y + y_offset - (dist_to_camera - custom_dist)*rad_y_factor*close_factor;
		owner->parent->transform_2d.SetScale(float3(min(abs(1.0f + (custom_dist - dist_to_camera)*3 / scale_factor), 2), min(abs(1.0f + (custom_dist - dist_to_camera) * 3 / scale_factor), 2), 1));
	}
	else {
		shield_canvas_position.y = shield_canvas_position.y + y_offset - (dist_to_camera - custom_dist)*rad_y_factor;
		owner->parent->transform_2d.SetScale(float3(max(abs(1.0f + (custom_dist - dist_to_camera) / scale_factor),0.5f), max(abs(1.0f + (custom_dist - dist_to_camera) / scale_factor), 0.5f), 1));
	}
	

	owner->parent->transform_2d.SetGlobalMatrixTranslation(float3(shield_canvas_position, 0.f));
}
