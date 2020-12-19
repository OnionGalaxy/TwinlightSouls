#include "Dissolver.h"

#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/Resources/Material.h"
#include "Log/EngineLog.h"
#include <imgui.h>


Dissolver* DissolverDLL()
{
	Dissolver* instance = new Dissolver();
	return instance;
}

Dissolver::Dissolver()
{

}

// Use this for initialization before Start()
void Dissolver::Awake()
{
	dissolving_meshes.clear();
	if (owner_go)
	{
		for (auto& child : owner->children)
		{
			ComponentMeshRenderer* mesh_renderer = static_cast<ComponentMeshRenderer*>(child->GetComponent(Component::ComponentType::MESH_RENDERER));
			if (mesh_renderer != nullptr && mesh_renderer->material_to_render != nullptr
				&& (mesh_renderer->material_to_render->material_type == Material::MaterialType::MATERIAL_DISSOLVING || mesh_renderer->material_to_render->material_type == Material::MaterialType::MATERIAL_LIQUID))
			{
				dissolving_meshes.emplace_back(mesh_renderer);
			}
		}
	}
	else
	{
		ComponentMeshRenderer* mesh_renderer = static_cast<ComponentMeshRenderer*>(owner->GetComponent(Component::ComponentType::MESH_RENDERER));
		if (mesh_renderer != nullptr && mesh_renderer->material_to_render != nullptr
			&& (mesh_renderer->material_to_render->material_type == Material::MaterialType::MATERIAL_DISSOLVING || mesh_renderer->material_to_render->material_type == Material::MaterialType::MATERIAL_LIQUID))
		{
			dissolving_meshes.emplace_back(mesh_renderer);
		}
		dissolving = true;
	}
}

// Use this for initialization
void Dissolver::Start()
{
	for (auto& meshes : dissolving_meshes)
	{
		meshes->material_to_render->SetDissolveProgress(starting_dissolve_progress);
	}
}

// Update is called once per frame
void Dissolver::Update()
{
	if (dissolving)
	{
		current_time += App->time->delta_time;
		float progress =  math::Min((float)current_time/dissolving_time * (1 - starting_dissolve_progress) + starting_dissolve_progress, 1.f);
		float current_progress = invert_dissolving ? 1.0f - progress : progress;
		for (auto& meshes : dissolving_meshes)
		{
			meshes->material_to_render->SetDissolveProgress(current_progress);
		}

		if (current_time > dissolving_time && owner_go)
		{
			dissolving = false;
		}
		else if(current_time > dissolving_time && !owner_go)
		{
			Dissolve();
		}
	}
}

// Update is called once per frame
void Dissolver::SetStartingDissolveProgress(float dissolve_progress)
{
	starting_dissolve_progress = dissolve_progress;
}

void Dissolver::ResetDissolve()
{
	for (auto& meshes : dissolving_meshes)
	{
		meshes->material_to_render->SetDissolveProgress(starting_dissolve_progress);
	}
	invert_dissolving = false;
}

bool Dissolver::IsDissolving() const
{
	return dissolving;
}

void Dissolver::SetDissolvingTime(float time)
{
	dissolving_time = time;
}

void Dissolver::EnableInvertedDissolver(bool invert_dissolver)
{
	invert_dissolving = invert_dissolver;
}

void Dissolver::Dissolve()
{
	dissolving = true;
	current_time = 0;
}

// Use this for showing variables on inspector
void Dissolver::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
	ImGui::DragFloat("Starting Disolve Progress", &starting_dissolve_progress);
	ImGui::DragInt("Dissolving Time", &dissolving_time);
	ImGui::Checkbox("Is Owner", &owner_go);
	
}

//Use this for linking JUST GO automatically 
void Dissolver::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

std::vector<ComponentMeshRenderer*> Dissolver::GetMeshes()
{
	return dissolving_meshes;
}

void Dissolver::RestartDisolve()
{
	for (auto& meshes : dissolving_meshes)
	{
		meshes->material_to_render->SetDissolveProgress(starting_dissolve_progress);
	}
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
 void Dissolver::Save(Config& config) const
 {
 	config.AddFloat(starting_dissolve_progress, "StartingDissolveProgress");
 	config.AddInt(dissolving_time, "dissolving_time");
 	config.AddBool(owner_go, "owner_go");
 	Script::Save(config);
}

// //Use this for linking GO AND VARIABLES automatically
void Dissolver::Load(const Config& config)
{
	starting_dissolve_progress = config.GetFloat("StartingDissolveProgress", 0.f);
	dissolving_time = config.GetInt("dissolving_time", 3000.f);
	owner_go = config.GetBool("owner_go", true);
 	Script::Load(config);
}

void Dissolver::Finish()
{
	if (dissolving)
	{
		for (auto& meshes : dissolving_meshes)
		{
			meshes->material_to_render->SetDissolveProgress(1.0f);
		}
		dissolving = false;
	}
}