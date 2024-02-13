#include "./headers/base/util.h"

int main()
{
	// Engine init.
	std::vector<ColorVec3> pixelBuffer;
	bool useMT{ true };
	int jitterSamplesAA{ 50 };

	// World-objects init.
	WorldObjectList mainWorld;
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0, 0, -1), 0.5));
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0, -100.5, -1), 100));

	// Camera init.
	Camera mainCamera((16.0 / 9.0), 400, Vec3(0, 0, 1), Vec3(), pixelBuffer, useMT, jitterSamplesAA);

	mainCamera.renderFrame(mainWorld);

	return 0;
}

