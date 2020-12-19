#include "TutorialHelper.h"

#include "Module/ModuleScene.h"
#include "Main/GameObject.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentText.h"

#include "WorldManager.h"
#include "InputManager.h"
#include "UIImageDissolver.h"

using TCP = LightBall::TutorialCheckPoints::CheckPointStatus;
namespace
{
	const size_t MAX_BOUNCES_REQUIRED = 3;
}

void TutorialHelper::InitDependencies(const ModuleScene& scene)
{
	input_manager = static_cast<InputManager*>(scene.GetGameObjectByName("Input Manager")->GetComponentScript("InputManager")->script);
	world = static_cast<WorldManager*>(scene.GetGameObjectByName("World Manager")->GetComponentScript("WorldManager")->script);
	dialogue_manager = static_cast<DialogueManager*>(scene.GetGameObjectByName("Dialogue Manager")->GetComponentScript("DialogueManager")->script);

	lightshield_tutorial_helper_ui = scene.GetGameObjectByName("Tutorial UI");
	if (lightshield_tutorial_helper_ui)
	{
		//tutorial_hint_text = static_cast<ComponentText*>(lightshield_tutorial_helper_ui->children[0]->GetComponent(Component::ComponentType::UI_TEXT));
		//tutorial_hint_text_p2 = static_cast<ComponentText*>(lightshield_tutorial_helper_ui->children[1]->GetComponent(Component::ComponentType::UI_TEXT));
		if (lightshield_tutorial_helper_ui->GetChildrenWithName("tutorial_skippable") != nullptr)
		{
			tutorial_request = static_cast<ComponentImage*>(lightshield_tutorial_helper_ui->children[4]->GetComponent(Component::ComponentType::UI_IMAGE));
		}
	}

#if GAME
	if(world->current_level != Level::FIRST)
	{
		return;
	}
#endif

	GameObject* first_go = scene.GetGameObjectByName("TutorialShieldThrow");

	if(first_go == nullptr)
	{
		return;
	}

	tutorial_shield_throw = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("TutorialShieldThrow")->GetComponentScript("UIImageDissolver")->script);
	tutorial_shield_prepare = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("TutorialShieldPrepare")->GetComponentScript("UIImageDissolver")->script);
	tutorial_shield_use = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("TutorialShieldUse")->GetComponentScript("UIImageDissolver")->script);
	

	tutorial_bounce0 = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("TutorialShieldBounce0")->GetComponentScript("UIImageDissolver")->script);
	tutorial_bounce1 = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("TutorialShieldBounce1")->GetComponentScript("UIImageDissolver")->script);
	tutorial_bounce2 = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("TutorialShieldBounce2")->GetComponentScript("UIImageDissolver")->script);
	tutorial_bounce3 = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("TutorialShieldBounce3")->GetComponentScript("UIImageDissolver")->script);
	hint_lightball_keyboard = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("PressButtonLightballKeyboard")->GetComponentScript("UIImageDissolver")->script);
	hint_lightball_controller = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("PressButtonLightballController")->GetComponentScript("UIImageDissolver")->script);
	hint_shield_keyboard = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("PressButtonShieldKeyboard")->GetComponentScript("UIImageDissolver")->script);
	hint_shield_controller = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("PressButtonShieldController")->GetComponentScript("UIImageDissolver")->script);

	hint_shield_controller_player2 = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("PressButtonShieldControllerPlayer2")->GetComponentScript("UIImageDissolver")->script);
	hint_lightball_controller_player2 = static_cast<UIImageDissolver*>(scene.GetGameObjectByName("PressButtonLightballControllerPlayer2")->GetComponentScript("UIImageDissolver")->script);
	
}

bool TutorialHelper::CheckShieldTutorialDialogue(LightBall::TutorialCheckPoints& shield_tutorial, const DialogueManager::NextDialogueCallback& success, const DialogueManager::NextDialogueCallback& failure)
{
	bool dialogue = false;
	DisableAllImages();
	//Check
	bool all_checkpoints_complete = true;
	all_checkpoints_complete &= shield_tutorial.lightball_throw == TCP::SUCCESS;
	all_checkpoints_complete &= shield_tutorial.lightball_bounces >= MAX_BOUNCES_REQUIRED;

	if (all_checkpoints_complete)
	{
		ResetTutorial(shield_tutorial);
		dialogue_manager->StartDialogue(DialogueID::LIGHTSHIELD_TUTORIAL_END, success);
		dialogue = true;
	}
	else 
	{
		ResetTutorial(shield_tutorial);
		dialogue_manager->StartDialogue(DialogueID::LIGHTSHIELD_TUTORIAL_REPEAT, failure);
		dialogue = true;
	}
	return dialogue;
}

void TutorialHelper::UpdateUI(LightBall::TutorialCheckPoints& shield_tutorial)
{
		//lightshield_tutorial_helper_ui->SetEnabled(true);
		const bool lightball_in_progress = shield_tutorial.lightball_throw == TCP::IN_PROGRESS;
		const bool hint_is_for_player_1 = (shield_tutorial.player1_is_owner && lightball_in_progress) || !world->multiplayer || (!shield_tutorial.player1_is_owner && !lightball_in_progress);
		const bool show_key_hint = lightball_in_progress || shield_tutorial.parry_enabled;

		//Bounces
		int bounces = min(shield_tutorial.lightball_bounces, MAX_BOUNCES_REQUIRED);
		if(bounces == 0)
		{
 			tutorial_bounce0->Enable();
			tutorial_bounce1->Disable();
			tutorial_bounce2->Disable();
			tutorial_bounce3->Disable();;
		}
		else if(bounces == 1)
		{
			tutorial_bounce0->Disable();
			tutorial_bounce1->Enable();
			tutorial_bounce2->Disable(); 
			tutorial_bounce3->Disable();
		}
		else if(bounces == 2)
		{
			tutorial_bounce0->Disable();
			tutorial_bounce1->Disable();
			tutorial_bounce2->Enable();
			tutorial_bounce3->Disable();
		}
		else
		{
			tutorial_bounce0->Disable();
			tutorial_bounce1->Disable();
			tutorial_bounce2->Disable();
			tutorial_bounce3->Enable();
		}

		if(!lightball_in_progress && shield_tutorial.parry_enabled)
		{
			tutorial_shield_use->Enable();
			tutorial_shield_throw->Disable();
			tutorial_shield_prepare->Disable();
		}
		else if(lightball_in_progress)
		{
			tutorial_shield_use->Disable();
			tutorial_shield_throw->Enable();
			tutorial_shield_prepare->Disable();
		}
		else
		{
			tutorial_shield_use->Disable();
			tutorial_shield_throw->Disable();
			tutorial_shield_prepare->Enable();
		}

		if (input_manager->total_game_controllers <= 0 || world->multiplayer)
		{
			if(lightball_in_progress)
			{
				hint_lightball_keyboard->Enable();
				hint_shield_keyboard->Disable();
			}
			else
			{
				hint_lightball_keyboard->Disable();
				hint_shield_keyboard->Enable();
			}
		}
		
		if(input_manager->total_game_controllers > 0 && !world->multiplayer)
		{
			if(lightball_in_progress)
			{
					hint_lightball_controller->Enable();
				hint_shield_controller->Disable();
			}
			else
			{
				hint_lightball_controller->Disable();
				hint_shield_controller->Enable();
			}
		}

		if((input_manager->total_game_controllers == 1 && world->multiplayer))
		{
			if (lightball_in_progress)
			{
				hint_lightball_controller_player2->Enable();
				hint_shield_controller_player2->Disable();
			}
			else
			{
				hint_lightball_controller_player2->Disable();
				hint_shield_controller_player2->Enable();
			}
		}

		tutorial_request->Disable();
}
void TutorialHelper::ResetTutorial(LightBall::TutorialCheckPoints& shield_tutorial)
{
	shield_tutorial.lightball_throw = TCP::IN_PROGRESS;
	shield_tutorial.player1_bounce = TCP::IN_PROGRESS;
	shield_tutorial.player2_bounce = TCP::IN_PROGRESS;
	shield_tutorial.parry_enabled = false;
	shield_tutorial.lightball_bounces = 0;
}

void TutorialHelper::DisableAllImages() const
{
	tutorial_shield_throw->Disable();
	tutorial_shield_prepare->Disable();
	tutorial_shield_use->Disable();
	tutorial_bounce0->Disable();
	tutorial_bounce1->Disable();
	tutorial_bounce2->Disable();
	tutorial_bounce3->Disable();
	hint_lightball_keyboard->Disable();
	hint_lightball_controller->Disable();
	hint_shield_keyboard->Disable();
	hint_shield_controller->Disable();
}
