
import core_util;
import threadpool;

import <thread>;
import <iostream>;
import <print>;
import <latch>;

int main()
{
	PixelResolution windowResolution{ .widthInPixels{1920}, .heightInPixels{1080} };
	PixelResolution imgRes{ .widthInPixels{1920}, .heightInPixels{1920} };
	AspectRatio aspectRatio{ .widthInAbsVal{16}, .heightInAbsVal{9} };

	Indus mainInstance(windowResolution, imgRes, aspectRatio);

	mainInstance.initializeEngine();

	mainInstance.runEngine();

	return 0;
}

