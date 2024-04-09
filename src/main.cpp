import std;
import image;

int main()
{
	constexpr int width{ 300 };
	constexpr int height{ 300 };
	std::vector<std::uint8_t> pixBuff(static_cast<long long int>(width * height * 3));

    for (int i = 0; i < height; ++i) { // Iterate over rows
        for (int j = 0; j < width; ++j) { // Iterate over columns
            auto red = static_cast<double>(j) / (width - 1);
            auto green = static_cast<double>(i) / (height - 1);
            auto blue = 0.25;

            int convertedRed = static_cast<int>(255.999 * red);
            int convertedGreen = static_cast<int>(255.999 * green);
            int convertedBlue = static_cast<int>(255.999 * blue);

            int index = 3 * (i * width + j); // i is row, j is column
            pixBuff[index] = static_cast<std::uint8_t>(convertedRed);
            pixBuff[index + 1] = static_cast<std::uint8_t>(convertedGreen);
            pixBuff[index + 2] = static_cast<std::uint8_t>(convertedBlue);
        }
    }

	PNGImage x(width, height, pixBuff);
	x.createImage();

	return 0;
}