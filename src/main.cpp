#include "./headers/base/util.h"

int main()
{
	// Engine init.
	std::vector<ColorVec3> pixelBuffer;
	bool useMT{ true };
	int jitterSamplesAA{ 100 };
	int maxRayBouncesDepth{ 50 };

	// Materials
	auto materialGround = std::make_shared<MLambertian>(ColorVec3(0.8f, 0.01f, 0.5f));
	auto materialSphere1 = std::make_shared<MMetal>(ColorVec3(0.8f, 0.8f, 0.6f), 0.3f);
	auto materialSphere2 = std::make_shared<MMetal>(ColorVec3(0.2f, 0.3f, 0.6f), 1.0f);


	// World-objects init.
	WorldObjectList mainWorld;
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(-0.5, 0, -1), 0.5, materialSphere1));
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(+0.5, 0, -1), 0.5, materialSphere2));
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0, -100.5, -1), 100, materialGround));

	// Camera init.
	Camera mainCamera((16.0 / 9.0), 600, Vec3(0, 0, 1), Vec3(), pixelBuffer, useMT, jitterSamplesAA, maxRayBouncesDepth);

	mainCamera.renderFrame(mainWorld);

	return 0;
}

