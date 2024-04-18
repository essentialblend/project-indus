import core_util;

import <thread>;
import <iostream>;

int main()
{
	PixelResolution windowResolution{ .widthInPixels{1920}, .heightInPixels{1080} };
	AspectRatio aspectRatio{ .widthInAbsVal{16}, .heightInAbsVal{9} };

	Indus mainInstance(windowResolution, windowResolution, aspectRatio);

	mainInstance.initializeEngine();

	mainInstance.runEngine();

	return 0;
}