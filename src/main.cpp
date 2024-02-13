#include "../src/headers/base/util.h"

static ColorVec3 computePixelColor(const Ray& inputRay, const WorldObject& mainWorld);
void renderRowSegments(int imageWidth, int startRow, int endRow, const WorldObjectList& mainWorld, std::vector<ColorVec3>& pixelBuffer, const PointVec3& topLeftPixelLoc, const Vec3& viewpDX, const Vec3& viewpDY, const PointVec3& cameraCenter);

int main()
{
	// Init.
	std::vector<ColorVec3> pixelBuffer;
	// Multi-threading check.
	bool useMT{ false };

	// Create world.
	WorldObjectList mainWorld;
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0, 0, -1), 0.5));
	mainWorld.addToWorld(std::make_shared<WOSphere>(PointVec3(0, -100.5, -1), 100));

	// Camera and viewport variables.
	double aspectRatio{ 16.0 / 9.0 };
	Vec3 focalLength{ Vec3(0, 0, 1.f) };
	PointVec3 cameraCenter{ PointVec3() };
	unsigned short imageWidthPixels{ 400 };
	unsigned short imageHeightPixels{ (static_cast<unsigned short>(imageWidthPixels / aspectRatio)) };
	imageHeightPixels = imageHeightPixels < 1 ? 1 : imageHeightPixels;
	pixelBuffer.resize(imageWidthPixels * imageHeightPixels);
	float viewportHeight{ 2.f };
	float viewportWidth{ viewportHeight * (static_cast<float>(imageWidthPixels) / imageHeightPixels)};

	Vec3 viewportUX{ Vec3(viewportWidth, 0, 0) };
	Vec3 viewportVY{ Vec3(0, -viewportHeight, 0) };

	Vec3 viewportDeltaX { viewportUX / imageWidthPixels };
	Vec3 viewportDeltaY{ viewportVY / imageHeightPixels };

	PointVec3 viewportUpperLeftPoint = cameraCenter - focalLength - (viewportUX / 2) - (viewportVY / 2);
	PointVec3 topLeftPixelLocation = viewportUpperLeftPoint + (0.5 * (viewportDeltaX + viewportDeltaY));

	// 12 for my 6c12t system.
	unsigned int numAvailableThreads = std::thread::hardware_concurrency();
	// Set up MT.
	unsigned int rowsPerThread = imageHeightPixels / numAvailableThreads;
	auto logTimeStart = std::chrono::high_resolution_clock::now();
	// Render.
	if (useMT)
	{
		std::println("Starting multi-threaded rendering with {} threads.", numAvailableThreads);
		std::vector<std::thread> mainThreads;

		for (unsigned short t = 0; t < numAvailableThreads; ++t)
		{
			int startRow = t * rowsPerThread;
			int endRow = (t + 1) * rowsPerThread;

			// Cover remaining rows with the last thread.
			if (t == numAvailableThreads - 1)
			{
				endRow = imageHeightPixels;
			}

			std::println("Thread {} processing pixel rows {} to {}...", t, startRow, endRow);

			mainThreads.emplace_back(renderRowSegments, imageWidthPixels, startRow, endRow, std::cref(mainWorld), std::ref(pixelBuffer), topLeftPixelLocation, viewportDeltaX, viewportDeltaY, cameraCenter);
			std::println("Thread {} finished!", t);
		}

		for (auto& thread : mainThreads)
		{
			thread.join();
		}
	}
	else
	{
		renderRowSegments(imageWidthPixels, 0, imageHeightPixels, std::cref(mainWorld), std::ref(pixelBuffer), topLeftPixelLocation, viewportDeltaX, viewportDeltaY, cameraCenter);
	}

	// Write to image file.
	std::ofstream outPPM("image.ppm");
	if (!outPPM)
	{
		std::cerr << "Error: Could not open output image." << std::endl;
		return -1;
	}
	outPPM << "P3\n" << imageWidthPixels << " " << imageHeightPixels << "\n255\n";
	for (const auto& pixel : pixelBuffer)
	{
		outPPM << pixel.getX() << " " << pixel.getY() << " " << pixel.getZ() << "\n";
	}
	outPPM.close();
	auto logTimeEnd = std::chrono::high_resolution_clock::now();
	UPrintSuccessLog(logTimeStart, logTimeEnd);
	

	return 0;
}

static ColorVec3 computePixelColor(const Ray& inputRay, const WorldObject& mainWorld)
{
	HitRecord tempRec;
	if (mainWorld.checkHit(inputRay, 0, Uinf, tempRec))
	{
		return (0.5 * (tempRec.normalVec + ColorVec3(1.f)));
	}

	Vec3 unitDirection = computeUnitVector(inputRay.getRayDirection());
	double lerpFactor = 0.5 * (unitDirection.getY() + 1.f);
	return ((1 - lerpFactor) * ColorVec3(1.f)) + (lerpFactor * ColorVec3(0.5, 0.7, 1.0));
}

void renderRowSegments(int imageWidth, int startRow, int endRow, const WorldObjectList& mainWorld, std::vector<ColorVec3>& pixelBuffer, const PointVec3& topLeftPixelLoc, const Vec3& viewpDX, const Vec3& viewpDY, const PointVec3& cameraCenter)
{
	for (int j = startRow; j < endRow; ++j)
	{
		for (int i = 0; i < imageWidth; ++i)
		{
			PointVec3 pixelCenter = topLeftPixelLoc + (i * viewpDX) + (j * viewpDY);
			Vec3 rayDirection = pixelCenter - cameraCenter;
			Ray primaryRay(cameraCenter, rayDirection);
			ColorVec3 pixelColor{ computePixelColor(primaryRay, mainWorld) };

			int bufferIndex = j * imageWidth + i;
			pixelBuffer[bufferIndex] = ColorVec3(static_cast<int>(255.999 * pixelColor.getX()), static_cast<int>(255.999 * pixelColor.getY()), static_cast<int>(255.999 * pixelColor.getZ()));
		}
	}
}