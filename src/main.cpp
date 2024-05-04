import core_constructs;
import indus;

int main()
{
	PixelResolution windowResObj{ .widthInPixels{1920}, .heightInPixels{1080} };
	PixelResolution imgResObj{ .widthInPixels{640}, .heightInPixels{360} };
	//PixelResolution imgResObj{ .widthInPixels{1920}, .heightInPixels{1080} };
	AspectRatio aspectRatioObj{ .widthInAbsVal{16}, .heightInAbsVal{9} };

	Indus mainInstance(windowResObj, imgResObj, aspectRatioObj);

	mainInstance.initializeEngine();

	mainInstance.runEngine();

	return 0;
}

