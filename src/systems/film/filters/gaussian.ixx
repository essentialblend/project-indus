export module indus.film.gaussian_filter;

import std;

import indus.core.types;

import indus.film.filter;

// Rare/stray forward decl, restr.
[[nodiscard]] static inline Float gaussian1D(Float x, Float mu, Float sigma) noexcept;
[[nodiscard]] static inline Float gaussianIntegral(Float a, Float b, Float mu, Float sigma) noexcept;

export class GaussianFilter final : public Filter
{
public:
  explicit GaussianFilter(const Vec2f& supportRadius, Float sigma = Float{ 0.5f }) noexcept;
  
  [[nodiscard]] Float getWeightAtOffset(const Point2f& pixelOffset) const noexcept override;
  [[nodiscard]] Float getIntegral() const noexcept override;
  [[nodiscard]] Float evaluateSamplingPDF(const Point2f& pixelOffset) const noexcept override;
  [[nodiscard]] FilterSample getFilterSampleAtOffset(const Point2f& unitSquarePoint) const noexcept override;
  
  [[nodiscard]] std::string toString() const override;

  ~GaussianFilter() override = default;

private:
  Float m_sigma{};
  Float m_xExp{};
  Float m_yExp{};
};

GaussianFilter::GaussianFilter(const Vec2f& supportRadius, Float sigma) noexcept : Filter(supportRadius), m_sigma{ sigma }, m_xExp{ gaussian1D(supportRadius[0], Float{}, sigma) }
, m_yExp{ gaussian1D(supportRadius[1], Float{}, sigma) } {}

Float GaussianFilter::getWeightAtOffset(const Point2f& pixelOffset) const noexcept
{
  const Float g_x{ gaussian1D(pixelOffset[0], Float{}, m_sigma) - m_xExp };
  const Float g_y{ gaussian1D(pixelOffset[1], Float{}, m_sigma) - m_yExp };

  const Float w_x{ std::max(Float{}, g_x) };
  const Float w_y{ std::max(Float{}, g_y) };

  return w_x * w_y;
}

Float GaussianFilter::getIntegral() const noexcept
{
  const Float r_x{ m_supportRadius[0] };
  const Float r_y{ m_supportRadius[1] };

  const Float xInt{ gaussianIntegral(-r_x, r_x, Float{}, m_sigma) - Float{ 2 } * r_x * m_xExp };
  const Float yInt{ gaussianIntegral(-r_y, r_y, Float{}, m_sigma) - Float{ 2 } * r_y * m_yExp };

  const Float integral{ xInt * yInt };
  
  return (integral > Float{}) ? integral : Float{};
}

Float GaussianFilter::evaluateSamplingPDF(const Point2f& pixelOffset) const noexcept
{
  const Float r_x{ m_supportRadius[0] };
  const Float r_y{ m_supportRadius[1] };

  const bool xIn{ (pixelOffset[0] >= -r_x) && (pixelOffset[0] <= r_x) };
  const bool yIn{ (pixelOffset[1] >= -r_y) && (pixelOffset[1] <= r_y) };
  
  if (!(xIn && yIn)) return Float{};

  const Float area{ Float{ 4 } * r_x * r_y };
  
  return (area > Float{}) ? (Float{ 1 } / area) : Float{};
}

FilterSample GaussianFilter::getFilterSampleAtOffset(const Point2f& unitSquarePoint) const noexcept
{
  const Float r_x{ m_supportRadius[0] };
  const Float r_y{ m_supportRadius[1] };

  const Float p_x{ -r_x + (Float{ 2 } * r_x * unitSquarePoint[0]) };
  const Float p_y{ -r_y + (Float{ 2 } * r_y * unitSquarePoint[1]) };
  const Point2f pOffset{ p_x, p_y };

  const Float pdf{ evaluateSamplingPDF(pOffset) };
  const Float f{ getWeightAtOffset(pOffset) };

  const Float weightOverPDF{ (pdf > Float{} && f > Float{}) ? (f / pdf) : Float{} };

  return FilterSample{ pOffset, weightOverPDF };
}

std::string GaussianFilter::toString() const
{
  std::ostringstream oss{};

  oss << "gaussian [r=(" << m_supportRadius[0] << "," << m_supportRadius[1]
    << "),sigma=" << m_sigma << "]";
  
  return oss.str();
}

[[nodiscard]] static inline Float gaussian1D(Float x, Float mu, Float sigma) noexcept
{
  if (sigma <= Float{}) return Float{};

  constexpr Float invSqrt2Pi{ Float{ 0.39894228040143267794 } };
  const Float invSigma{ Float{ 1 } / sigma };
  
  const Float t{ (x - mu) * invSigma };

  return invSqrt2Pi * invSigma * std::exp(-Float{ 0.5 } * t * t);
}

[[nodiscard]] static inline Float gaussianIntegral(Float a, Float b, Float mu, Float sigma) noexcept
{
  if (sigma <= Float{}) return Float{};

  constexpr Float invSqrt2{ Float{ 0.70710678118654752440 } };
  const Float invSigma{ Float{ 1 } / sigma };

  const Float t0{ (a - mu) * invSigma * invSqrt2 };
  const Float t1{ (b - mu) * invSigma * invSqrt2 };

  return Float{ 0.5 } * (std::erf(t1) - std::erf(t0));
}
