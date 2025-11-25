export module indus.sampling.piecewiseconstant2D;

import std;

import indus.core.types;
import indus.core.math.fp.ii;

import indus.sampling.constructs;
import indus.sampling.piecewiseconstant1D;

import indus.core.geom.bounds;

export class PiecewiseConstant2D final
{
public:
  PiecewiseConstant2D() noexcept = default;
  PiecewiseConstant2D(std::vector<Float> functionValues, const Bounds2f& domainBounds, Point2i sampleResolution) noexcept;

  [[nodiscard]] Bounds2f getDomainBounds() const noexcept;
  [[nodiscard]] Point2i getSampleResolution() const noexcept;
  [[nodiscard]] Float getFunctionIntegral() const noexcept;

  [[nodiscard]] PiecewiseConstant2DSample generateSampleFromDistr(const Point2f& uniformDistrSample) const noexcept;

  [[nodiscard]] Float evaluateCoordPDF(const Point2f& sampleCoord) const noexcept;

private:
  PiecewiseConstant1D m_rowMarginalDistr{};
  std::vector<PiecewiseConstant1D> m_rowConditionalDistr{};
  Bounds2f m_domainBounds{};
  Point2i m_sampleResolution{};
};

PiecewiseConstant2D::PiecewiseConstant2D(std::vector<Float> functionValues, const Bounds2f& domainBounds, Point2i sampleResolution) noexcept : m_domainBounds{ domainBounds }, m_sampleResolution{ sampleResolution }
{
  const Int NX{ m_sampleResolution[0] };
  const Int NY{ m_sampleResolution[1] };

  m_rowConditionalDistr.clear();
  m_rowConditionalDistr.reserve(static_cast<std::size_t>(NY));

  std::vector<Float> rowIntegrals{};
  rowIntegrals.reserve(static_cast<std::size_t>(NY));

  const Float xMin{ m_domainBounds.getMin()[0] };
  const Float xMax{ m_domainBounds.getMax()[0] };

  for (Int y{}; y < NY; ++y)
  {
    const std::size_t rowOffset{ static_cast<std::size_t>(y) * static_cast<std::size_t>(NX) };

    std::vector<Float> rowValues{};
    rowValues.reserve(static_cast<std::size_t>(NX));

    for (Int x{}; x < NX; ++x)
    {
      rowValues.push_back(functionValues[rowOffset + static_cast<std::size_t>(x)]);
    }

    PiecewiseConstant1D rowDistribution{ std::move(rowValues), xMin, xMax };

    rowIntegrals.push_back(rowDistribution.getFunctionIntegral());
    m_rowConditionalDistr.push_back(std::move(rowDistribution));
  }

  const Float yMin{ m_domainBounds.getMin()[1] };
  const Float yMax{ m_domainBounds.getMax()[1] };

  m_rowMarginalDistr = PiecewiseConstant1D{ std::move(rowIntegrals), yMin, yMax };
}

Bounds2f PiecewiseConstant2D::getDomainBounds() const noexcept
{
  return m_domainBounds;
}

Point2i PiecewiseConstant2D::getSampleResolution() const noexcept
{
  return m_sampleResolution;
}

Float PiecewiseConstant2D::getFunctionIntegral() const noexcept
{
  return m_rowMarginalDistr.getFunctionIntegral();
}

PiecewiseConstant2DSample PiecewiseConstant2D::generateSampleFromDistr(const Point2f& uniformDistrSample) const noexcept
{
  const Idx rowCount{ static_cast<Idx>(m_rowConditionalDistr.size()) };

  if (rowCount == Idx{})
  {
    const Point2f domainMin{ m_domainBounds.getMin() };
    return PiecewiseConstant2DSample{ Point2i{}, domainMin, Float{} };
  }

  const PiecewiseConstant1DSample rowMarginalDistrSample{ m_rowMarginalDistr.generateSampleFromDistr(uniformDistrSample[1])};

  Idx rowIndex{ rowMarginalDistrSample.sampleIdx };

  if (rowIndex >= rowCount) rowIndex = rowCount - Idx{ 1 };

  const PiecewiseConstant1D& rowDistribution{ m_rowConditionalDistr[rowIndex] };
  const PiecewiseConstant1DSample columnSample{ rowDistribution.generateSampleFromDistr(uniformDistrSample[0])};
  const Point2i discreteIndex{ static_cast<Int>(columnSample.sampleIdx), static_cast<Int>(rowIndex) };

  const Point2f sampleCoord{ columnSample.coordinate, rowMarginalDistrSample.coordinate };
  const Float PDF{ evaluateCoordPDF(sampleCoord) };

  return PiecewiseConstant2DSample{ discreteIndex, sampleCoord, PDF };
}

Float PiecewiseConstant2D::evaluateCoordPDF(const Point2f& sampleCoord) const noexcept
{
  const Float totalIntegral{ m_rowMarginalDistr.getFunctionIntegral() };

  const Float xMin{ m_domainBounds.getMin()[0] };
  const Float xMax{ m_domainBounds.getMax()[0] };
  const Float yMin{ m_domainBounds.getMin()[1] };
  const Float yMax{ m_domainBounds.getMax()[1] };

  const Int NX{ m_sampleResolution[0] };
  const Int NY{ m_sampleResolution[1] };

  if (NX <= 0 || NY <= 0) return Float{};

  const Float area{ (xMax - xMin) * (yMax - yMin) };
  
  // Uniform PDF fallback for no-area
  if (area <= Float{}) return Float{};
  if (totalIntegral == Float{}) return Float{ 1 } / area;

  const Float xClamped{ clamp(sampleCoord[0], xMin, xMax) };
  const Float yClamped{ clamp(sampleCoord[1], yMin, yMax) };

  const Float dx{ (xMax - xMin) / static_cast<Float>(NX) };
  const Float dy{ (yMax - yMin) / static_cast<Float>(NY) };

  Int ix{ static_cast<Int>((xClamped - xMin) / dx) };
  Int iy{ static_cast<Int>((yClamped - yMin) / dy) };

  if (ix < 0) ix = 0; else if (ix >= NX) ix = NX - 1;
  if (iy < 0) iy = 0; else if (iy >= NY) iy = NY - 1;

  const PiecewiseConstant1D& rowDistribution{ m_rowConditionalDistr[static_cast<std::size_t>(iy)] };
  const Float functionValue{ rowDistribution.getFunctionValueAt(static_cast<Idx>(ix)) };
  
  if (functionValue <= Float{}) return Float{};

  return (functionValue / totalIntegral);
}

