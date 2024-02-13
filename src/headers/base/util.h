#pragma once
import std;

#include "vec3.h"
#include "ray.h"
#include "worldObject.h"
#include "worldObjectList.h"
#include "../world_objects/sphere.h"


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

inline void UPrintSuccessLog(std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<__int64, std::ratio<1, 1000000000>>>& start, std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<__int64, std::ratio<1, 1000000000>>>& end)
{
	auto logTotalTimeSeconds = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	auto logHours = logTotalTimeSeconds.count() / 3600;
	auto logMinutes = (logTotalTimeSeconds.count() % 3600) / 60;
	auto logSeconds = logTotalTimeSeconds.count() % 60;
	
	std::clog << "Time: " << logHours << "h " << logMinutes << "m " << logSeconds << "s.\n";
}

template<typename... Args>
void UWriteToClog(const Args&... args)
{
	std::clog << std::format(args...) << "\n";
}