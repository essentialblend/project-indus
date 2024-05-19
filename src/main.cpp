import core_constructs;
import indus;
import vec3;

int main()
{
	PixelResolution windowResObj{ .widthInPixels{1920}, .heightInPixels{1080} };
	PixelResolution imgResObj{ .widthInPixels{ 1920 / 4 }, .heightInPixels{ 1080 / 4} };
	//PixelResolution imgResObj{ .widthInPixels{1920}, .heightInPixels{1080} };
	AspectRatio aspectRatioObj{ .widthInAbsVal{16}, .heightInAbsVal{9} };

	Indus mainInstance(windowResObj, imgResObj, aspectRatioObj);

	mainInstance.initializeEngine();

	mainInstance.runEngine();

	return 0;
}

