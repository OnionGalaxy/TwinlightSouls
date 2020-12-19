#ifndef  __SHIELDBAR_H__
#define  __SHIELDBAR_H__

#include "Script.h"

class ComponentTransform2D;

class ShieldBar : public Script
{
public:
	ShieldBar() = default;
	~ShieldBar() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void SetProgress(float new_progress);

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	void SetPlayerPosition(const float3& position);
	void SetShieldStatus() const;
	void UpdatePlayerDistToCamera();
	void UpdateShieldTransform();

	float progress = 1.0f;
	float y_offset = 85.0f;
	float2 shield_canvas_position;
	bool active = false;
	float dist_to_camera = 0.0f; 
	float custom_dist = 18.223f;
	float scale_factor = 30.0f;
	float pos_factor = 20.0f;
	float rad_y_factor = 2.5f;
	float far_factor = 2.5f;
	float close_factor = 1.0f;
	float3 player_pos = float3::one;

private:
	bool inverted = false;
	bool vertical = false;

	float3 rotation = float3::zero;

	GameObject* fill_bar_game_object = nullptr;

	ComponentTransform2D* fill_bar_transform = nullptr;
	ComponentTransform2D* progress_bar_transform = nullptr;

};
extern "C" SCRIPT_API ShieldBar* ShieldBarDLL(); //This is how we are going to load the script
#endif