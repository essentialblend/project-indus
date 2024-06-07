//export module es_pointsampler;
//
//import <glm/glm.hpp>;
//
//import eu_color;
//import core_constructs;
//import es_pixelsampler;
//import point3;
//
//export class ESPointSampler final : public ESPixelSampler
//{
//public:
//	explicit ESPointSampler() noexcept = default;
//	ESPointSampler(const ESPointSampler&) = default;
//	ESPointSampler& operator=(const ESPointSampler&) = default;
//	ESPointSampler(ESPointSampler&&) noexcept = default;
//	ESPointSampler& operator=(ESPointSampler&&) noexcept = default;
//
//	void initializeEntity([[maybe_unused]] const std::vector<std::any>&) override;
//	HRay getRayForPixelSample([[maybe_unused]] const std::vector<std::any>&) override;
//
//	~ESPointSampler() noexcept = default;
//private:
//	PixelDimension m_pixelDimensionObj{};
//	Point3c m_cameraCenter{};
//	Point3c m_tempSamplePoint{};
//};
//
//
