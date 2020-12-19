#ifndef  __FOGGER_H__
#define  __FOGGER_H__

#include "Script.h"

class CameraController;

class Fogger : public Script
{
public:
	Fogger();
	~Fogger() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Enable();
	void Disable();
	void Recover();

	void OnInspector(ImGuiContext*) override;
	void Save(Config& config) const override;
	void Load(const Config& config) override;

private:
	CameraController* camera_controller = nullptr;

	bool disabled = false;

	float4 fog_color = float4::one;
	float initial_fog_density = 0.f;
	float final_fog_density = 1.f;
	float recovering_time = 2000.f;

	bool recovering = false;
	float current_time = 0.f;
};
extern "C" SCRIPT_API Fogger* FoggerDLL(); //This is how we are going to load the script
#endif