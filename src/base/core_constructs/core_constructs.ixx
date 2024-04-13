export module core_constructs;
import color;
import vec3;

export struct PixelResolution
{
	int widthInPixels{};
	int heightInPixels{};
};

export struct AspectRatio
{
	double widthInAbsoluteValue{};
	double heightInAbsoluteValue{};
	double absoluteWidthByHeightARValue{ widthInAbsoluteValue / heightInAbsoluteValue };
};

export struct ViewportDimension
{
	double widthInWorldSpaceUnits{};
	double heightInWorldSpaceUnits{};
};

export struct ImageProperties
{
	PixelResolution resolutionInPixels{};
	AspectRatio aspectRatioInAbsoluteValue{};
	int numColorChannels{};
};

export struct PixelDimension
{
	Vec3 lateralSpanInAbsoluteValue{};
	Vec3 verticalSpanInAbsoluteValue{};
	Point centerPoint{};
};

export struct CameraProperties
{

};