#pragma once

import std;

void writeColorToBuffer(std::ostream& outStr, ColorVec3 pixelColor)
{
	outStr << static_cast<int>(255.999 * pixelColor.getX()) << ' ' << static_cast<int>(255.999 * pixelColor.getY()) << ' ' << static_cast<int>(255.999 * pixelColor.getZ()) << '\n';
}