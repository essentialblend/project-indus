#include "./headers/base/util.h"

int main()
{
	// Engine init.
	std::vector<ColorVec3> pixelBuffer;
	bool useMT{ true };
	int jitterSamplesAA{ 50 };
	int maxRayBouncesDepth{ 50 };

	// World-objects init.
	WorldObjectList mainWorld;
	auto material_ground = std::make_shared<MLambertian>(ColorVec3(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<MLambertian>(ColorVec3(0.15, 0.2, 0.5));
	auto material_left = std::make_shared<MDielectric>(1.5f);
	auto material_right = std::make_shared<MMetal>(ColorVec3(0.8, 0.6, 0.2), 0.0);

	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0.0, -100.5, -1.0), 100.0, material_ground));
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0.0, 0.0, -1.0), 0.5, material_center));
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(-1.0, 0.0, -1.0), 0.5, material_left));
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(1.0, 0.0, -1.0), 0.5, material_right));

	// Camera init.
	Camera mainCamera((16.0 / 9.0), 400, Vec3(0, 0, 1), Vec3(), pixelBuffer, useMT, jitterSamplesAA, maxRayBouncesDepth);

	mainCamera.renderFrame(mainWorld);

	return 0;
}

