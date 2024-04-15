export module core_constructs;

import color;
import vec3;
import <vector>;

import <SFML/Graphics.hpp>;

export struct PixelResolution
{
	int widthInPixels{};
	int heightInPixels{};
};

export struct AspectRatio
{
	double widthInAbsVal{};
	double heightInAbsVal{};
};

export struct ViewportProperties
{
	double widthInWorldSpaceUnits{};
	double heightInWorldSpaceUnits{};
	Vec3 horizontalPixelSpan{};
	Vec3 verticalPixelSpan{};
};

export struct ImageProperties
{
	PixelResolution pixelResolutionObj{};
	AspectRatio aspectRatioObj{};
	int numColorChannels{3};
};

export struct PixelDimension
{
	Vec3 lateralSpanInAbsVal{};
	Vec3 verticalSpanInAbsVal{};
	Point pixelCenter{};
};

export struct CameraProperties
{
	ImageProperties camImgPropsObj{};
	ViewportProperties camViewportPropsObj{};
	PixelDimension camPixelDimObj{};

	Point camCenter{ 0.0, 0.0, 0.0 };
	Vec3 camFocalLength{};
};

export struct SFMLWindowProperties
{
	std::unique_ptr<sf::RenderWindow> renderWindowObj{};
	std::unique_ptr<sf::View> viewObj{};
	std::unique_ptr<sf::Texture> texObj{};
	std::unique_ptr<sf::Sprite> spriteObj{};
	unsigned int prefFPSInIntegral{ 30 };
	float windowedResScale{ 0.5 };
};

export struct OverlayStatistic
{
	sf::Text statTitleObj{};
	sf::Text statResultObj{};
};

export struct OverlayProperties
{
	OverlayStatistic modeObj{};
	OverlayStatistic renderObj{};
	OverlayStatistic timeObj{};
	OverlayStatistic GUIObj{};

	std::vector<std::reference_wrapper<OverlayStatistic>> getStatsCollection()
	{
		return { std::ref(modeObj), std::ref(renderObj), std::ref(timeObj), std::ref(GUIObj) };
	}
};

