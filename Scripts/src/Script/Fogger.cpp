#include "Fogger.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

#include "CameraController.h"

Fogger* FoggerDLL()
{
	Fogger* instance = new Fogger();
	return instance;
}

Fogger::Fogger()
{
}

// Use this for initialization before Start()
void Fogger::Awake()
{
	GameObject* camera_controller_go = App->scene->GetGameObjectByName("Camera Holder");
	ComponentScript* camera_controller_component = camera_controller_go->GetComponentScript("CameraController");
	camera_controller = static_cast<CameraController*>(camera_controller_component->script);
}

// Use this for initialization
void Fogger::Start()
{
	App->renderer->SetFog(true);
}

// Update is called once per frame
void Fogger::Update()
{
	App->renderer->SetFogColor(fog_color);

	if (recovering)
	{
		current_time += App->time->delta_time;
		float recovery_progress = math::Clamp01(current_time / recovering_time);
	
		App->renderer->SetFogDensity((1.f - recovery_progress) * (final_fog_density - initial_fog_density) + initial_fog_density);
		
		recovering = recovery_progress != 1.f;
		return;
	}

	if (disabled)
	{
		return;
	}

	App->renderer->SetFogDensity(camera_controller->progress_level * (final_fog_density - initial_fog_density) + initial_fog_density);
}

void Fogger::Enable()
{
	disabled = false;
}

void Fogger::Disable()
{
	disabled = true;
}

void Fogger::Recover()
{
	recovering = true;
	current_time = 0.f;
}

// Use this for showing variables on inspector
void Fogger::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ImGui::ColorPicker4("Fog Color", fog_color.ptr());
	ImGui::DragFloat("Initial Fog Density", &initial_fog_density, 0.01f, 0.f, final_fog_density);
	ImGui::DragFloat("Final Fog Density", &final_fog_density, 0.01f, initial_fog_density, 100.f);
	ImGui::DragFloat("Reocvering Time", &recovering_time, 1.f, 10.f, 10000.f);
}

void Fogger::Save(Config& config) const
{
	Script::Save(config);
	config.AddFloat(recovering_time, "Recovering Time");
	config.AddFloat(initial_fog_density, "Initial Fog Density");
	config.AddFloat(final_fog_density, "Final Fog Density");
	config.AddColor(fog_color, "Fog Color");
}

//Use this for linking GO AND VARIABLES automatically
void Fogger::Load(const Config& config)
{
	Script::Load(config);
	recovering_time = config.GetFloat("Recovering Time", 2000.f);
	initial_fog_density = config.GetFloat("Initial Fog Density", 0.f);
	final_fog_density = config.GetFloat("Final Fog Density", 1.f);
	config.GetColor("Fog Color", fog_color, float4::one);
}