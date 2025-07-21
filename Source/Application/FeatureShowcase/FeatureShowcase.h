#pragma once
#include "Application/Application.h"
#include "Application/EntryPoint.h"

class FeatureShowcase : public Application
{
public:
	FeatureShowcase() {}
	~FeatureShowcase() {}

	void InitializeApplication() override;
	void UpdateApplication() override;
	void UpdateDebug() override;
private:
	void TopMenuBar();
	void FeatureOptions();
	void LightingSettings();
	void PerformanceInfo();
	void ControlsInfo();
	void ResolutionOptions();
	void PhysicsPlayground();

	bool myShowFeatureOptions = false;
	bool myShowLightingSettings = false;
	bool myShowPerformanceInfo = true;
	bool myShowControlsInfo = false;
	bool myShowResolutionOptions = false;
	bool myShowPhysicsPlayground = true;

	int currentAnimation = 0;
	std::vector<std::string> animationNames = {
		"Idle",
		"Walk",
		"Run",
		"Wave"
	};

	unsigned currentDebugMode = 0;
	unsigned currentTonemapper = 1;

	std::shared_ptr<GameObject> myControllablePhysicsObject;
};