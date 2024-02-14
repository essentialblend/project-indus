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

inline void UWriteToClog(const std::string& outString)
{
	std::clog << outString << "\n";
}

inline void UWriteToCout(const std::string& outString)
{
	std::cout << outString << "\n";
}

inline void UPrintSuccessLog(
    const std::chrono::time_point<std::chrono::steady_clock>& start,
    const std::chrono::time_point<std::chrono::steady_clock>& end,
    int totalPixels,
    int jitterAA,
    bool useMT, int maxBounces)
{
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto logTotalTimeSeconds = duration.count() / 1000.0; // Convert to seconds as a floating-point.
    auto logHours = static_cast<int>(logTotalTimeSeconds / 3600);
    auto logMinutes = static_cast<int>((logTotalTimeSeconds / 60)) % 60;


    std::string timeDetails = std::format("Time taken: {}h {}m {:.2f}s...", logHours, logMinutes, logTotalTimeSeconds - logHours * 3600 - logMinutes * 60);
    std::string graphicsDetails = std::format("Jittered aliasing with {} spp and {} max. ray bounces...", jitterAA, maxBounces);
    std::string pixelProcessDetails;
    if (logTotalTimeSeconds > 0) {
        double pixelsPerSecond = totalPixels / logTotalTimeSeconds;
        pixelProcessDetails = std::format("Processing speed: {:.2f} pixels per second...", pixelsPerSecond);
    }
    else 
    {
        pixelProcessDetails = "Processing speed: Calculation not possible due to short duration...";
    }
    std::string renderingModeDetails = std::format("\nRendering mode: {}", useMT ? "Multi-threaded..." : "Single-threaded...");

    // Assuming UWriteToClog is defined elsewhere and is thread-safe.
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
#include "material.h"
#include "../materials/lambertian.h"
#include "../materials/metal.h"
#include "../materials/dielectric.h"
#include "camera.h"