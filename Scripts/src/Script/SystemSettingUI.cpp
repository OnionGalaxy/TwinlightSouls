#include "SystemSettingUI.h"

#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include <imgui.h>

#include "ArrowUI.h"

SystemSettingUI* SystemSettingUIDLL()
{
	SystemSettingUI* instance = new SystemSettingUI();
	return instance;
}

SystemSettingUI::SystemSettingUI()
{
}

// Use this for initialization before Start()
void SystemSettingUI::Awake()
{
}

// Use this for initialization
void SystemSettingUI::Start()
{

}

// Update is called once per frame
void SystemSettingUI::Update()
{


}

// Use this for showing variables on inspector
void SystemSettingUI::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
}