
import core_util;
import threadpool;

import <thread>;
import <iostream>;
import <print>;
import <latch>;

int main()
{
	PixelResolution windowResObj{ .widthInPixels{1920}, .heightInPixels{1080} };
	PixelResolution imgResObj{ .widthInPixels{1920}, .heightInPixels{1080} };
	AspectRatio aspectRatioObj{ .widthInAbsVal{16}, .heightInAbsVal{9} };

	Indus mainInstance(windowResObj, imgResObj, aspectRatioObj);

	mainInstance.initializeEngine();

	mainInstance.runEngine();

	return 0;
}

