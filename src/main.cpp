import core_constructs;
import indus;
import vec3;
import u_timer;
import core_tests;
//import es_timermanager;

int main()
{
	PixelResolution windowResObj(1920, 1080);
	PixelResolution imgResObj(2560, 1440);
	AspectRatio aspectRatioObj(16, 9);

	// PixelResolution& windowPixResObj, PixelResolution& imagePixResObj, AspectRatio& aspectRatioObj, int SPP, int rayBounceDepth = 0, bool isDiagOutputEnabled = false, bool isRenderSavedToDisk = true, bool isMultithreaded = true
	Indus mainInstance(windowResObj, imgResObj, aspectRatioObj, 150, 0, true, true, true);

	mainInstance.initializeEngine();

	mainInstance.runEngine();
}

