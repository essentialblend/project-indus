export module indus.film.filter;

import std;

import indus.core.types;

import indus.camera.constructs;

export class Filter
{
public:
  explicit Filter() noexcept = default;
  explicit Filter(const Vec2f& supportRadius) noexcept; 

  [[nodiscard]] Vec2f getSupportRadius() const noexcept;
  
  [[nodiscard]] virtual Float getWeightAtOffset(const Point2f& pixelOffset) const noexcept = 0;
  [[nodiscard]] virtual Float getIntegral() const noexcept = 0;
  [[nodiscard]] virtual Float evaluateSamplingPDF(const Point2f& pixelOffset) const noexcept = 0;
  [[nodiscard]] virtual FilterSample getFilterSampleAtOffset(const Point2f& unitSquarePoint) const noexcept = 0;

  [[nodiscard]] virtual std::string toString() const = 0;

  virtual ~Filter() = default;

protected:
  Vec2f m_supportRadius{};
};

Filter::Filter(const Vec2f& supportRadius) noexcept : m_supportRadius{ supportRadius } {}

Vec2f Filter::getSupportRadius() const noexcept
{
  return m_supportRadius;
}
