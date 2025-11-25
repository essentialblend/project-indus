export module indus.film.filtersampler;

import indus.core.types;
import indus.core.geom.bounds;

import indus.camera.constructs;

import indus.film.filter;

import indus.sampling.piecewiseconstant2D;

export class FilterSampler final
{
public:
  explicit FilterSampler(const Filter& filter, Int SPPUnitExtent = 32) noexcept;

  [[nodiscard]] Bounds2f getDomainBounds() const noexcept;
  [[nodiscard]] Point2i getTableResolution() const noexcept;
  [[nodiscard]] Float getFunctionIntegral() const noexcept;

  [[nodiscard]] FilterSample sampleFromFilter(const Point2f& uniformDistrSample) const noexcept;
  [[nodiscard]] Float evaluateSamplingPDF(const Point2f& sampleOffset) const noexcept;

private:
  Bounds2f m_domainBounds{};
  Point2i m_tableResolution{};
  std::vector<Float> m_tabulatedFilterValues{};
  PiecewiseConstant2D m_distr;
};

FilterSampler::FilterSampler(const Filter& filter, Int samplesPerUnitExtent) noexcept : m_domainBounds{}, m_tableResolution{}, m_tabulatedFilterValues{}, m_distr{}
{
  const Vec2f supportRadius{ filter.getSupportRadius() };

  const Float xMin{ -supportRadius[0] };
  const Float xMax{ supportRadius[0] };
  const Float yMin{ -supportRadius[1] };
  const Float yMax{ supportRadius[1] };

  m_domainBounds = Bounds2f{ Point2f{ xMin, yMin }, Point2f{ xMax, yMax } };

  const Float width{ xMax - xMin };
  const Float height{ yMax - yMin };

  Int nx{ static_cast<Int>(width * static_cast<Float>(samplesPerUnitExtent)) };
  Int ny{ static_cast<Int>(height * static_cast<Float>(samplesPerUnitExtent)) };

  if (nx < 1) nx = 1;
  if (ny < 1) ny = 1;

  m_tableResolution = Point2i{ nx, ny };

  m_tabulatedFilterValues.clear();
  m_tabulatedFilterValues.resize(static_cast<std::size_t>(nx) * static_cast<std::size_t>(ny));

  const Float dx{ width / static_cast<Float>(nx) };
  const Float dy{ height / static_cast<Float>(ny) };

  for (Int j{}; j < ny; ++j)
  {
    const Float y{ yMin + (static_cast<Float>(j) + Float{ 0.5 }) * dy };

    for (Int i{}; i < nx; ++i)
    {
      const Float x{ xMin + (static_cast<Float>(i) + Float{ 0.5 }) * dx };
      const std::size_t idx{ static_cast<std::size_t>(j) * static_cast<std::size_t>(nx) + static_cast<std::size_t>(i) };

      m_tabulatedFilterValues[idx] = filter.getWeightAtOffset(Point2f{ x, y });
    }
  }

  m_distr = PiecewiseConstant2D{ m_tabulatedFilterValues, m_domainBounds, m_tableResolution };
}

Bounds2f FilterSampler::getDomainBounds() const noexcept
{
  return m_domainBounds;
}

Point2i FilterSampler::getTableResolution() const noexcept
{
  return m_tableResolution;
}

Float FilterSampler::getFunctionIntegral() const noexcept
{
  return m_distr.getFunctionIntegral();
}

FilterSample FilterSampler::sampleFromFilter(const Point2f& uniformDistrSample) const noexcept
{
  const PiecewiseConstant2DSample distrSample{ m_distr.generateSampleFromDistr(uniformDistrSample) };

  const Point2f sampleOffset{ distrSample.coordinate };
  const Float pdf{ distrSample.probDensity };

  if (pdf <= Float{}) return FilterSample{ sampleOffset, Float{} };

  const Int nx{ m_tableResolution[0] };
  const Point2i discreteIndex{ distrSample.sampleIdx };

  Int ix{ discreteIndex[0] };
  Int iy{ discreteIndex[1] };

  if (ix < 0) ix = 0; else if (ix >= nx) ix = nx - 1;
  if (iy < 0) iy = 0; else if (iy >= m_tableResolution[1]) iy = m_tableResolution[1] - 1;

  const std::size_t flatIndex{ static_cast<std::size_t>(iy) * static_cast<std::size_t>(nx) + static_cast<std::size_t>(ix) };

  const Float functionValue{ m_tabulatedFilterValues[flatIndex] };
  const Float weight{ functionValue > Float{} ? functionValue / pdf : Float{} };

  return FilterSample{ sampleOffset, weight };
}

Float FilterSampler::evaluateSamplingPDF(const Point2f& sampleOffset) const noexcept
{
  return m_distr.evaluateCoordPDF(sampleOffset);
}
