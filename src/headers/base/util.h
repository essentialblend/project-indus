#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <numbers>
#include <fstream>
#include <random>
#include <thread>
#include <format>

const double Uinf = std::numeric_limits<double>::infinity();
const double Upi = std::numbers::pi;

// Utility functions.

// Geometry
inline double UDegreesToRadians(double degValue)
{
	return degValue * (Upi / 180.f);
}

inline double URadiansToDegrees(double radValue)
{
	return radValue * (180.f / Upi);
}

// RNG
inline double UGenRNGDouble()
{
	// Use high res clock as a seed.
	auto rngSeed{ std::chrono::high_resolution_clock::now().time_since_epoch().count() };
	std::mt19937 rngEng(static_cast<unsigned int>(rngSeed));

	// Define the dist. range [0.f, 1.f).
	std::uniform_real_distribution<double> uniDist(0.f, 1.f);

	return uniDist(rngEng);
}

inline double UGenRNGDouble(double minVal, double maxVal)
{
	// Use high res clock as a seed.
	auto rngSeed{ std::chrono::high_resolution_clock::now().time_since_epoch().count() };
	std::mt19937 rngEng(static_cast<unsigned int>(rngSeed));

	// Define the dist. range [0.f, 1.f).
	std::uniform_real_distribution<double> uniDist(minVal, maxVal);

	return uniDist(rngEng);
}

void UWriteToClog(const std::string& outString)
{
	std::clog << outString << "\n";
}

void UWriteToCout(const std::string& outString)
{
	std::cout << outString << "\n";
}

inline void UPrintSuccessLog(std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<__int64, std::ratio<1, 1000000000>>>& start, std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<__int64, std::ratio<1, 1000000000>>>& end, int totalPixels, int jitterAA, bool useMT)
{
	auto logTotalTimeSeconds = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	auto logHours = logTotalTimeSeconds.count() / 3600;
	auto logMinutes = (logTotalTimeSeconds.count() % 3600) / 60;
	auto logSeconds = logTotalTimeSeconds.count() % 60;

	std::string timeDetails = std::format("Time taken: {}h {}m {}s...", logHours, logMinutes, logSeconds);
	std::string graphicsDetails = std::format("\nJittered aliasing with {} spp...", jitterAA);
	std::string pixelProcessDetails = std::format("Processing speed: {} pixels per second...", totalPixels / logSeconds);
	std::string renderingModeDetails = std::format("Rendering mode: {} ", useMT ? "Multi-threaded" : "Single-threaded");


	UWriteToClog(renderingModeDetails);
	UWriteToClog(timeDetails);
	UWriteToClog(graphicsDetails);
	UWriteToClog(pixelProcessDetails);
}

// Includes.
#include "interval.h"
#include "vec3.h"
#include "ray.h"
#include "worldObject.h"
#include "worldObjectList.h"
#include "../world_objects/sphere.h"
#include "camera.h"