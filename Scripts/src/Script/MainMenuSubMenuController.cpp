#include "MainMenuSubMenuController.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include <imgui.h>


MainMenuSubMenuController* MainMenuSubMenuControllerDLL()
{
	MainMenuSubMenuController* instance = new MainMenuSubMenuController();
	return instance;
}

MainMenuSubMenuController::MainMenuSubMenuController()
{
}

// Use this for initialization before Start()
void MainMenuSubMenuController::Awake()
{

}

// Use this for initialization
void MainMenuSubMenuController::Start()
{

}

// Update is called once per frame
void MainMenuSubMenuController::Update()
{


}

// Use this for showing variables on inspector
void MainMenuSubMenuController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}