export module indus.film.box_filter;

import indus.core.types;

import indus.film.filter;

export class BoxFilter final : public Filter
{
public:
  BoxFilter() noexcept;
  explicit BoxFilter(const Vec2f& supportRadius) noexcept;

  [[nodiscard]] Float getWeightAtOffset(const Point2f& pixelOffset) const noexcept override;
  [[nodiscard]] Float getIntegral() const noexcept override;
  [[nodiscard]] Float evaluateSamplingPDF(const Point2f& pixelOffset) const noexcept override;
  [[nodiscard]] FilterSample getFilterSampleAtOffset(const Point2f& unitSquarePoint) const noexcept override;

  [[nodiscard]] std::string toString() const override;

  ~BoxFilter() override = default;
};

BoxFilter::BoxFilter() noexcept : Filter{ Vec2f{ 0.5, 0.5 } } {}

BoxFilter::BoxFilter(const Vec2f& supportRadius) noexcept : Filter{ supportRadius } {}

Float BoxFilter::getWeightAtOffset(const Point2f& pixelOffset) const noexcept
{
  // For box-filters, we only consider in-box contribution, everything else is 0  
  const bool inX{ (pixelOffset[0] >= -m_supportRadius[0]) && (pixelOffset[0] <= m_supportRadius[0]) };
  const bool inY{ (pixelOffset[1] >= -m_supportRadius[1]) && (pixelOffset[1] <= m_supportRadius[1]) };

  return (inX && inY) ? Float{ 1 } : Float{};
}

Float BoxFilter::getIntegral() const noexcept
{
  return Float{ 4 } * m_supportRadius[0] * m_supportRadius[1];
}

Float BoxFilter::evaluateSamplingPDF(const Point2f& pixelOffset) const noexcept
{
  const Float area{ getIntegral() };

  if (area <= Float{}) return Float{};
  
  return getWeightAtOffset(pixelOffset) > Float{} ? (Float{ 1 } / area) : Float{};
}

FilterSample BoxFilter::getFilterSampleAtOffset(const Point2f& unitSquarePoint) const noexcept
{
  // Cache radius and area-integral
  const Float rx{ m_supportRadius[0] };
  const Float ry{ m_supportRadius[1] };

  const Float area{ getIntegral() };

  if (area <= Float{}) return FilterSample{ Point2f{}, Float{ 1 } };

  // Scale the uniform sample and return a sample point within the entire box to capture contributions from surrounding pixels by importance sampling
  const Float px{ (-rx) + (Float{ 2 } * rx * unitSquarePoint[0]) };
  const Float py{ (-ry) + (Float{ 2 } * ry * unitSquarePoint[1]) };

  const Float f{ getWeightAtOffset({ px, py }) };
  const Float pdf{ evaluateSamplingPDF({ px, py }) };

  const Float weightOverPDF{ (pdf > Float{} && f > Float{}) ? (f / pdf) : Float{} };

  return FilterSample{ Point2f{ px, py }, weightOverPDF };
}

std::string BoxFilter::toString() const
{
  return "box";
}