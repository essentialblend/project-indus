import core_constructs;
import indus;
import vec3;

int main()
{
	PixelResolution windowResObj{ .widthInPixels{1920}, .heightInPixels{1080} };
	PixelResolution imgResObj{ .widthInPixels{ 1920 / 2 }, .heightInPixels{ 1080 / 2 } };
	//PixelResolution imgResObj{ .widthInPixels{1920}, .heightInPixels{1080} };
	AspectRatio aspectRatioObj{ .widthInAbsVal{16}, .heightInAbsVal{9} };

	Indus mainInstance(windowResObj, imgResObj, aspectRatioObj);

	mainInstance.initializeEngine();

	mainInstance.runEngine();

	return 0;
}

