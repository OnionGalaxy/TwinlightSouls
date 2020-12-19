#include "UIImageDissolver.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include <imgui.h>

UIImageDissolver* UIImageDissolverDLL()
{
	UIImageDissolver* instance = new UIImageDissolver();
	return instance;
}

UIImageDissolver::UIImageDissolver()
{

}

// Use this for initialization before Start()
void UIImageDissolver::Awake()
{
	image = static_cast<ComponentImage*>(owner->GetComponent(Component::ComponentType::UI_IMAGE));
	image->color.w = 0.f;
}


// Use this for initialization
void UIImageDissolver::Start()
{

}

// Update is called once per frame
void UIImageDissolver::Update()
{
	if (image_enabled && image->color.w < 1.f)
	{
		current_blending_time += App->time->delta_time;
		image->color.w = min(current_blending_time / total_blending_time, 1.f);
	}

	if (!image_enabled && image->color.w > 0.f)
	{
		current_blending_time += App->time->delta_time;
		image->color.w = 1.f - min(current_blending_time / total_blending_time, 1.f);
	}

}

void UIImageDissolver::Enable()
{
	if(!current_blending_time)
	{
		current_blending_time = 0.f;
	}
	image_enabled = true;
}

void UIImageDissolver::Disable()
{
	if(image_enabled)
	{
		current_blending_time = 0.f;
	}
	current_alpha = image->color.w;
	image_enabled = false;
}

void UIImageDissolver::FastHide()
{
	image_enabled = false;
	image->color.w = 0.f;
}

void UIImageDissolver::SetTotalBlendingTime(float blending_time)
{
	total_blending_time = blending_time;
}


// Use this for showing variables on inspector
void UIImageDissolver::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Checkbox("Image Enabled", &image_enabled);
	ImGui::DragFloat("current_blending_time", &current_blending_time);
}


//Use this for linking JUST GO automatically 
void UIImageDissolver::InitPublicGameObjects()
{
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}

}