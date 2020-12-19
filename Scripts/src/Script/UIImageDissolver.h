#ifndef  __UIIMAGEDISSOLVER_H__
#define  __UIIMAGEDISSOLVER_H__

#include "Script.h"

class ComponentImage;

class UIImageDissolver : public Script
{
public:
	UIImageDissolver();
	~UIImageDissolver() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Enable();
	void Disable();

	void FastHide();
	void SetTotalBlendingTime(float blending_time);

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();

private:
	ComponentImage* image = nullptr;
	bool  image_enabled = false;
	float current_blending_time = 0.f;
	float total_blending_time = 200.f;
	float current_alpha = 0.f;
};

extern "C" SCRIPT_API UIImageDissolver* UIImageDissolverDLL(); //This is how we are going to load the script

#endif