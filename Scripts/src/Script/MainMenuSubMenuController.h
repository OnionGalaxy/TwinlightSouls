#ifndef  __MAINMENUSUBMENUCONTROLLER_H__
#define  __MAINMENUSUBMENUCONTROLLER_H__

#include "Script.h"

class MainMenuSubMenuController : public Script
{
public:
	MainMenuSubMenuController();
	~MainMenuSubMenuController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	virtual void Show() {};
	virtual void FastHide() {};

	inline bool Closed() const { return closed; };

	void OnInspector(ImGuiContext*) override;

public:
	bool hide_main_menu_background = false;

protected:
	bool closed = true;
};
extern "C" SCRIPT_API MainMenuSubMenuController* MainMenuSubMenuControllerDLL(); //This is how we are going to load the script
#endif