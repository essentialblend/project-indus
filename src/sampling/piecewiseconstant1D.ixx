export module indus.sampling.piecewiseconstant1D;

import std;

import indus.core.types;
import indus.core.math.fp.ii;
import indus.core.math.misc.vi;

import indus.sampling.constructs;

export class PiecewiseConstant1D final
{
public:
  PiecewiseConstant1D() noexcept = default;

  PiecewiseConstant1D(std::vector<Float> functionValues, Float domainStart, Float domainEnd) noexcept;

  [[nodiscard]] Idx getSampleCount() const noexcept;
  [[nodiscard]] Float getFunctionIntegral() const noexcept;
  [[nodiscard]] Float getDomainStart() const noexcept;
  [[nodiscard]] Float getDomainEnd() const noexcept;
  [[nodiscard]] Float getFunctionValueAt(Idx idx) const noexcept;

  [[nodiscard]] PiecewiseConstant1DSample generateSampleFromDistr(Float uniformDistrSample) const noexcept;
  [[nodiscard]] std::optional<Float> findCoordForCDFValue(Float unitCDFValue) const noexcept;


private:
  std::vector<Float> m_functionValues{};
  std::vector<Float> m_CDFValues{};

  Float m_domainStart{};
  Float m_domainEnd{};
  
  Float m_functionIntegral{};

  std::optional<PiecewiseConstant1DSample> lookupCDFValueCoord(Float unitCDFValue) const noexcept;
};

PiecewiseConstant1D::PiecewiseConstant1D(std::vector<Float> functionValues, Float domainStart, Float domainEnd) noexcept : m_functionValues{ std::move(functionValues) }, m_domainStart{ domainStart }, m_domainEnd{ domainEnd }
{
  const Idx sampleCount{ static_cast<Idx>(m_functionValues.size()) };

  m_CDFValues.resize(sampleCount + 1);

  if (sampleCount == 0) 
  {
    m_CDFValues[0] = Float{};
    return;
  }

  for (Float& functionValue : m_functionValues) functionValue = std::abs(functionValue);

  m_CDFValues[0] = Float{};

  const Float intervalWidth{ (m_domainEnd - m_domainStart) / static_cast<Float>(sampleCount) };

  for (Idx idx{ 1 }; idx <= sampleCount; ++idx) 
    m_CDFValues[idx] = m_CDFValues[idx - 1] + (m_functionValues[idx - 1] * intervalWidth);

  m_functionIntegral = m_CDFValues[sampleCount];

  if (m_functionIntegral == Float{})
  {
    for (Idx idx{ 1 }; idx <= sampleCount; ++idx)
      m_CDFValues[idx] = static_cast<Float>(idx) / static_cast<Float>(sampleCount);
  }
  else 
  {
    for (Idx idx{ 1 }; idx <= sampleCount; ++idx) 
      m_CDFValues[idx] /= m_functionIntegral;
  }
}

Idx PiecewiseConstant1D::getSampleCount() const noexcept
{
  return static_cast<Idx>(m_functionValues.size());
}

Float PiecewiseConstant1D::getFunctionIntegral() const noexcept
{
  return m_functionIntegral;
}

Float PiecewiseConstant1D::getDomainStart() const noexcept
{
  return m_domainStart;
}

Float PiecewiseConstant1D::getDomainEnd() const noexcept
{
  return m_domainEnd;
}

PiecewiseConstant1DSample PiecewiseConstant1D::generateSampleFromDistr(Float uniformDistrSample) const noexcept
{
  const auto lookupResult{ lookupCDFValueCoord(uniformDistrSample) };

  if (!lookupResult.has_value()) return PiecewiseConstant1DSample{ Idx{}, m_domainStart, Float{}, };

  const Float domainLength{ m_domainEnd - m_domainStart };

  Float probabilityDensity{};

  if (m_functionIntegral > Float{})
  {
    probabilityDensity = m_functionValues[static_cast<std::size_t>(lookupResult->sampleIdx)] / m_functionIntegral;
  }
  else if (domainLength > Float{})
  {
    probabilityDensity = Float{ 1 } / domainLength;
  }

  return PiecewiseConstant1DSample{ lookupResult->sampleIdx, lookupResult->coordinate, probabilityDensity };
}

std::optional<Float> PiecewiseConstant1D::findCoordForCDFValue(Float unitCDFValue) const noexcept
{
  const auto lookupResult{ lookupCDFValueCoord(unitCDFValue) };

  if (!lookupResult.has_value()) return std::nullopt;
  
  return lookupResult->coordinate;
}

Float PiecewiseConstant1D::getFunctionValueAt(Idx idx) const noexcept
{
  return m_functionValues[idx];
}

std::optional<PiecewiseConstant1DSample> PiecewiseConstant1D::lookupCDFValueCoord(Float unitCDFVal) const noexcept
{
  Float localOffsetInInterval{};
  const Idx sampleCount{ getSampleCount() };

  if (sampleCount == 0) return std::nullopt;

  const Float clampedCDFVal{ clampUnit(unitCDFVal) };

  const Idx intervalIndex
  {
    static_cast<Idx>(findInterval(static_cast<Int>(sampleCount + 1), [this, clampedCDFVal](Idx idx) noexcept
    {
      return m_CDFValues[static_cast<Idx>(idx)] <= clampedCDFVal;
    }))
  };

  const Float CDFLower{ m_CDFValues[static_cast<std::size_t>(intervalIndex)] };
  const Float CDFUpper{ m_CDFValues[static_cast<std::size_t>(intervalIndex + 1)] };
  const Float intervalCDFWidth{ CDFUpper - CDFLower };

  if (intervalCDFWidth > Float{}) localOffsetInInterval = (clampedCDFVal - CDFLower) / intervalCDFWidth;

  const Float domainLength{ m_domainEnd - m_domainStart };
  const Float intervalWidth{ domainLength / static_cast<Float>(sampleCount) };

  const Float coordinateBase{ m_domainStart + (intervalWidth * static_cast<Float>(intervalIndex)) };
  const Float coordinate{ coordinateBase + (localOffsetInInterval * intervalWidth) };

  return PiecewiseConstant1DSample{ intervalIndex, coordinate, /*PDF=0*/ };
}
