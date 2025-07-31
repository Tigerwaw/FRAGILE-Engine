#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	Timer(const Timer& aTimer) = delete;
	Timer& operator=(const Timer& aTimer) = delete;
	void Update();
	void SetTimeScale(float aTimeScale) { myTimeScale = aTimeScale; }
	const float GetTimeScale() const { return myTimeScale; }
	float GetDeltaTime() const;
	float GetUnscaledDeltaTime() const;
	double GetTimeSinceProgramStart() const;
	float GetFrameTimeMS() const;
	int GetAverageFPS() const;
	float GetAverageFrameTimeMS() const;
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> myStartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> myLastFrameTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> myCurrentFrameTime;

	int myCurrentFPSCountFrame = 0;
	int myMaxFPSCountFrame = 100;
	float myTotalFPS = 0;
	int myAverageFPS = 0;
	float myTotalFrametime = 0;
	float myAverageFrametime = 0.0f;
	float myTimeScale = 1.0f;
};