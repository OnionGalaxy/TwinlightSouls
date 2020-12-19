#ifndef  __DISSOLVER_H__
#define  __DISSOLVER_H__

#include "Script.h"

class ComponentMeshRenderer;
class Dissolver : public Script
{
public:
	Dissolver();
	~Dissolver() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Dissolve();
	void SetStartingDissolveProgress(float dissolve_progress);
	void ResetDissolve();
	bool IsDissolving() const;
	void SetDissolvingTime(float time);
	void EnableInvertedDissolver(bool invert_dissolver);

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();

	std::vector<ComponentMeshRenderer*> GetMeshes();

	void RestartDisolve();

	void Save(Config & config) const override;
	void Load(const Config & config) override;

	void Finish();

private:
	std::vector<ComponentMeshRenderer*> dissolving_meshes;
	int current_time = 0;
	int dissolving_time = 3000;
	bool owner_go = true;
	bool dissolving = false;
	bool invert_dissolving = false;
	float starting_dissolve_progress = 0.f;

};
extern "C" SCRIPT_API Dissolver* DissolverDLL(); //This is how we are going to load the script
#endif