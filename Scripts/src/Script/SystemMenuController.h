#ifndef  __SYSTEMMENUCONTROLLER_H__
#define  __SYSTEMMENUCONTROLLER_H__

#include "MainMenuSubMenuController.h"

class Application;
class ComponentAudioSource;
class ComponentImage;
class InputManager;
class MainMenuButton;
class SystemCursorUI;
class SystemSettingUI;

class SystemMenuController : public MainMenuSubMenuController
{
public:
	class SystemTag
	{
	public:
		SystemTag() = default;
		SystemTag(Application* app, InputManager* input_manager, ComponentAudioSource* menu_audio_source) : App(app), input_manager(input_manager), menu_audio_source(menu_audio_source){};
		~SystemTag() = default;

		virtual void Update() {};
		virtual void Show() = 0;
		virtual void Hide() = 0;
		virtual void FastHide() = 0;

		inline void SetTagName(MainMenuButton* tag_name) { this->tag_name = tag_name; }

	protected:
		Application* App = nullptr;
		InputManager* input_manager = nullptr;
		ComponentAudioSource* menu_audio_source = nullptr;
		float current_time = 0.f;

		MainMenuButton* tag_name = nullptr;
	};

	/////////////////////////////////////////////////

	class SimpleImageSystemTag : public SystemTag
	{
	public:
		enum class SimpleImageSystemTagState
		{
			SHOWING,
			SHOWN,
			HIDING,
			HIDDEN
		};

		SimpleImageSystemTag(Application* app, InputManager* input_manager, ComponentAudioSource* menu_audio_source) : SystemTag(app, input_manager, menu_audio_source) {};

		void Update() override;
		void Show() override;
		void Hide() override;
		void FastHide() override;

		void SetImage(ComponentImage* tag_image);

	private:
		SimpleImageSystemTagState state = SimpleImageSystemTagState::HIDDEN;
		ComponentImage* tag_image = nullptr;
	};

	///////////////////////////////////////////////////

	class GraphicsSystemTag : public SystemTag
	{
	public:
		enum class GraphicSystemTagState
		{
			SHOWING,
			SHOWN,
			HIDING,
			HIDDEN
		};

		GraphicsSystemTag(Application* app, InputManager* input_manager, ComponentAudioSource* menu_audio_source) : SystemTag(app, input_manager, menu_audio_source) {};

		void Awake();
		void Start();
		void Update() override;
		void Show() override;
		void Hide() override;
		void FastHide() override;


	private:
		GraphicSystemTagState state = GraphicSystemTagState::HIDDEN;
		ComponentImage* background_image = nullptr;

		std::vector<SystemSettingUI*> system_settings;
		SystemCursorUI* system_cursor = nullptr;
		int current_setting = 0;
	};

	//////////////////////////////////////////////////

	enum class SystemMenuState
	{
		SHOWING,
		TAG_OPENED,
		HIDING,
		HIDDEN
	};

	SystemMenuController();
	~SystemMenuController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Show() override;
	void FastHide() override;

	void OnInspector(ImGuiContext*) override;

private:
	InputManager* input_manager = nullptr;

	ComponentAudioSource* menu_audio_source = nullptr;

	MainMenuButton* tag_helper_buttons = nullptr;
	MainMenuButton* tag_helper_controller = nullptr;
	MainMenuButton* tag_helper_keyboard = nullptr;

	MainMenuButton* back_button_controller = nullptr;
	MainMenuButton* back_button_keyboard = nullptr;
	MainMenuButton* back_button = nullptr;

	SystemMenuState current_state = SystemMenuState::HIDDEN;

	int current_tag = 0;
	std::vector<SystemTag*> system_tags;

	float current_time = 0.f;
	static const float SHOWING_TIME;
};
extern "C" SCRIPT_API SystemMenuController* SystemMenuControllerDLL(); //This is how we are going to load the script
#endif