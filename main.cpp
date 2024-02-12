import std;

#include "./src/headers/base/util.h"

static ColorVec3 computePixelColor(const Ray& inputRay);

int main()
{
	// Camera setup.
	// Viewport variables.
	double aspectRatio{ 16.0 / 9.0 };
	Vec3 focalLength{ Vec3(0, 0, 1.f) };
	PointVec3 cameraCenter{ PointVec3() };
	unsigned short imageWidthPixels{ 400 };
	unsigned short imageHeightPixels{ (static_cast<unsigned short>(imageWidthPixels / aspectRatio)) };
	imageHeightPixels = imageHeightPixels < 1 ? 1 : imageHeightPixels;

	float viewportHeight{ 2.f };
	float viewportWidth{ viewportHeight * (static_cast<float>(imageWidthPixels) / imageHeightPixels)};

	Vec3 viewportUX{ Vec3(viewportWidth, 0, 0) };
	Vec3 viewportVY{ Vec3(0, -viewportHeight, 0) };

	Vec3 viewportDeltaX { viewportUX / imageWidthPixels };
	Vec3 viewportDeltaY{ viewportVY / imageHeightPixels };

	PointVec3 viewportUpperLeftPoint = cameraCenter - focalLength - (viewportUX / 2) - (viewportVY / 2);
	PointVec3 topLeftPixelLocation = viewportUpperLeftPoint + (0.5 * (viewportDeltaX + viewportDeltaY));


	// Render
	// Output colors to the PPM image.
	std::println("P3\n {} {} \n255\n", imageWidthPixels, imageHeightPixels);

	for (int j = 0; j < imageHeightPixels; ++j)
	{
		std::clog << "\rScanlines remaining: " << imageHeightPixels - j << '\n' << std::flush;
		for (int i = 0; i < imageWidthPixels; ++i)
		{
			PointVec3 pixelCenter = topLeftPixelLocation + (i * viewportDeltaX) + (j * viewportDeltaY);
			Vec3 rayDirection = pixelCenter - cameraCenter;
			Ray primaryRay(cameraCenter, rayDirection);
			ColorVec3 pixelColor = computePixelColor(primaryRay);
			writeColorToBuffer(std::cout, pixelColor);
		}
	}
	std::clog << "Render complete!\n";

	return 0;
}

static ColorVec3 computePixelColor(const Ray& inputRay)
{
	Vec3 unitDirection = computeUnitVector(inputRay.getRayDirection());
	double lerpFactor = 0.5 * (unitDirection.getY() + 1.f);
	return ((1 - lerpFactor) * ColorVec3(1.f)) + (lerpFactor * ColorVec3(0.5, 0.7, 1.0));

}