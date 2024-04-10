import core_util;

int main()
{
	constexpr int width{ 300 };
	constexpr int height{ 300 };
    std::vector<Color> basePixelBuffer{};

    for (int i = 0; i < height; ++i) { 
        for (int j = 0; j < width; ++j) {

            const Color normPixelColor{ static_cast<double>(j) / (width - 1), static_cast<double>(i) / (height - 1), 0.25 };
            basePixelBuffer.push_back(normPixelColor);
        }
    }

    PNGImage x(width, height, 3, basePixelBuffer);
    x.createImage();

	return 0;
}