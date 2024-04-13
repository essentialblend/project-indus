import core_util;

int main()
{
	const Vec3 camFocalLength{ Vec3(0, 0, 1) };

	PixelResolution imageResolution{ .widthInPixels{1920}, .heightInPixels{0} };
	AspectRatio viewportAR{ .widthInAbsVal{16.0}, .heightInAbsVal{9.0} };

	Camera mainCamera{ imageResolution, viewportAR };

	mainCamera.setCameraCenter();
	mainCamera.setFocalLength(camFocalLength);
	mainCamera.setViewportHeight(2.0);

	mainCamera.setupCamera();

	Renderer mainRenderer{ mainCamera };

	mainRenderer.renderFrame();

	return 0;
}