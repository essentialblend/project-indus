#include "./headers/base/util.h"

int main()
{
	// Engine init.
	std::vector<ColorVec3> pixelBuffer;
	bool useMT{ true };
	int jitterSamplesAA{ 150 };
	int maxRayBouncesDepth{ 150 };

	// Materials
	auto materialGround = std::make_shared<MLambertian>(ColorVec3(0.8f, 0.1f, 0.f));
	auto materialSphere = std::make_shared<MLambertian>(ColorVec3(0.8f, 0.3f, 0.6f));



	// World-objects init.
	WorldObjectList mainWorld;
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0, 0, -1), 0.5, materialSphere));
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0, -100.5, -1), 100, materialGround));

	// Camera init.
	Camera mainCamera((16.0 / 9.0), 1920, Vec3(0, 0, 1), Vec3(), pixelBuffer, useMT, jitterSamplesAA, maxRayBouncesDepth);

	mainCamera.renderFrame(mainWorld);

	return 0;
}

