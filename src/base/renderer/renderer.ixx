export module renderer;

import <iostream>;
import <vector>;

import core_constructs;
import color;
import ray;
import camera;
import vec3;

import <SFML/Graphics.hpp>;



export class Renderer
{
public:
	explicit Renderer() noexcept = default;
    explicit Renderer(const Camera& cameraObj) noexcept : m_rendererCam{ cameraObj } {}

	void renderFrame();
    

private:
    Camera m_rendererCam{};
    
    Color computeRayColor(const Ray& inputRay) const;
};

Color Renderer::computeRayColor(const Ray& inputRay) const
{
    const Color gradientColor{ 0.5, 0.7, 1.0 };

    const Vec3 inputRayDir{ getUnit(inputRay.getDirection()) };
    const double lerpFactor{ 0.5 * (inputRayDir.getY() + 1.0) };
    const Vec3 returnedColor{ ((1.0 - lerpFactor) * Color(1.0).getBaseVec()) + (lerpFactor * gradientColor.getBaseVec()) };

    const Color finalReturnColor(returnedColor.getX(), returnedColor.getY(), returnedColor.getZ());

    return finalReturnColor;
}


void Renderer::renderFrame()
{
    PixelResolution tempPixelRes{ m_rendererCam.getCameraProperties().camImgPropsObj.pixelResolutionObj };
    PixelDimension tempPixelDim{ m_rendererCam.getCameraProperties().camPixelDimObj };
    Point tempCamCenter{ m_rendererCam.getCameraProperties().camCenter };

    std::vector<Color> localPixelBuffer{};

    // Render.
    sf::RenderWindow window(sf::VideoMode(tempPixelRes.widthInPixels / 2, tempPixelRes.heightInPixels / 2), "indus_prelim");
    window.setFramerateLimit(30);
    sf::View mainView{ sf::FloatRect(0, 0, static_cast<float>(tempPixelRes.widthInPixels), static_cast<float>(tempPixelRes.heightInPixels)) };
    window.setView(mainView);

    // Primary frame texture.
    sf::Texture mainFrameTexture{};
    mainFrameTexture.create(tempPixelRes.widthInPixels, tempPixelRes.heightInPixels);

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
            for (int i = 0; i < tempPixelRes.heightInPixels; ++i) {
                for (int j = 0; j < tempPixelRes.widthInPixels; ++j) {

                    const Point currentPixelCenter{ tempPixelDim.pixelCenter + (j * tempPixelDim.lateralSpanInAbsVal) + (i * tempPixelDim.verticalSpanInAbsVal)};
                    const Ray currentPixelRay{ tempCamCenter, currentPixelCenter - tempCamCenter };

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
}


