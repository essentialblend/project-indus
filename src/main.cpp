import core_util;

static Color computeRayColor(const Ray& inputRay)
{
    const Color gradientColor{ 0.5, 0.7, 1.0 };

    const Vec3 inputRayDir{ getUnit(inputRay.getDirection()) };
    double lerpFactor{ 0.5 * (inputRayDir.getY() + 1.0) };
    Vec3 returnedColor{ ((1.0 - lerpFactor) * Color(1.0).getBaseVec()) + (lerpFactor * gradientColor.getBaseVec()) };

    const Color finalReturnColor(returnedColor.getX(), returnedColor.getY(), returnedColor.getZ());
    return finalReturnColor;
}

int main()
{
    constexpr AspectRatio imageAspectRatio{16.0, 9.0, 16.0 / 9.0};
    constexpr int numChannels{ 3 };
    constexpr double viewportHeight{ 2.0 };
    std::vector<Color> localPixelBuffer{};

    constexpr PixelResolution primaryResolution{ 1920, std::max(1, static_cast<int>(primaryResolution.widthInPixels / imageAspectRatio.absoluteWidthByHeightARValue)) };
    constexpr ImageProperties currentFrameImageProps{ primaryResolution, imageAspectRatio, numChannels};
    constexpr ViewportDimension primaryViewport{ viewportHeight * static_cast<double>(primaryResolution.widthInPixels / primaryResolution.heightInPixels), viewportHeight};

    const Vec3 focalLength{ 0, 0, 1 };
    const Vec3 cameraCenter{ 0, 0, 0 };

    const Vec3 viewportU{ primaryViewport.widthInWorldSpaceUnits, 0, 0 };
    const Vec3 viewportV{ 0, -primaryViewport.heightInWorldSpaceUnits, 0 };

    const Point viewportTopLeft{ cameraCenter - focalLength - (viewportU / 2) - (viewportV / 2) };

    PixelDimension topLeftPixel{ 
        viewportU / primaryResolution.widthInPixels,
        viewportV / primaryResolution.heightInPixels,
        viewportTopLeft + (0.5 * (topLeftPixel.lateralSpanInAbsoluteValue + topLeftPixel.verticalSpanInAbsoluteValue)) };

    // Render.
    sf::RenderWindow window(sf::VideoMode(primaryResolution.widthInPixels /2 , primaryResolution.heightInPixels / 2), "indus_prelim");
    sf::View mainView{ sf::FloatRect(0, 0, static_cast<float>(primaryResolution.widthInPixels), static_cast<float>(primaryResolution.heightInPixels) ) };
    window.setView(mainView);

    // Primary frame texture.
    sf::Texture mainFrameTexture{};
    mainFrameTexture.create(primaryResolution.widthInPixels, primaryResolution.heightInPixels);

    sf::Sprite mainFrameSprite{};
    mainFrameSprite.setTexture(mainFrameTexture);

    bool shouldUpdateTex{ true };

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (shouldUpdateTex)
        {
            for (int i = 0; i < primaryResolution.heightInPixels; ++i) {
                for (int j = 0; j < primaryResolution.widthInPixels; ++j) {

                    const Point currentPixelCenter{ topLeftPixel.centerPoint + (j * topLeftPixel.lateralSpanInAbsoluteValue) + (i * topLeftPixel.verticalSpanInAbsoluteValue) };
                    const Ray currentPixelRay{ cameraCenter, currentPixelCenter - cameraCenter };
                    const Color normCurrentPixelColor{ computeRayColor(currentPixelRay) };

                    localPixelBuffer.push_back(normCurrentPixelColor);

                    sf::Uint8 pixelData[4] =
                    {
                        static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getX() * 255),
                        static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getY() * 255),
                        static_cast<sf::Uint8>(normCurrentPixelColor.getBaseVec().getZ() * 255),
                        255
                    };

                    mainFrameTexture.update(pixelData, 1, 1, j, i);
                }
                window.clear();
                window.draw(mainFrameSprite);
                window.display();
            }
        }
        shouldUpdateTex = false;
        window.clear();
        window.draw(mainFrameSprite);
        window.display();
    }

    PNGImage currFrameImage(currentFrameImageProps, localPixelBuffer);

	return 0;
}