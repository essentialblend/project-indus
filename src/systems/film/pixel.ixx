export module indus.film.pixel;

import std;

import indus.core.geom.vector;
import indus.core.colorrgb;
import indus.core.types;
import indus.core.math.fp.ii;

/*
TODOs :
- Variance estimators(for adaptive sampling).
- G - buffer attributes(normals, positions, albedo).
- Spectral bucket accumulation.
*/

export class Pixel
{
public:
  Pixel() noexcept;

  void addRadiance(const ColorRGB& L, Float weight) noexcept;
  void addSplat(const ColorRGB& L) noexcept;

  [[nodiscard]] ColorRGB normalizedColor(Float splatScale = Float{ 1 }) const noexcept;

  void clear() noexcept;

private:
  std::array<double, 3> m_rgbSum{};
  double m_weightSum{};
  std::array<std::atomic<double>, 3> m_rgbSplat{};
};

Pixel::Pixel() noexcept : m_rgbSum{ 0.0, 0.0, 0.0 }, m_weightSum{ 0.0 }, m_rgbSplat{} 
{
  for (auto& a : m_rgbSplat) {
    a.store(0.0, std::memory_order_relaxed);
  }
}

void Pixel::addRadiance(const ColorRGB& L, Float weight) noexcept
{
  if (!isFinite(weight) || !isFinite(L)) return;
  for (Idx c{}; c < 3; ++c) m_rgbSum[c] = fusedMultiplyAdd(static_cast<double>(L[c]), static_cast<double>(weight), m_rgbSum[c]);
  
  m_weightSum += static_cast<double>(weight);
}

void Pixel::addSplat(const ColorRGB& L) noexcept
{
  for (Idx c{}; c < 3; ++c) m_rgbSplat[c].fetch_add(static_cast<double>(L[c]), std::memory_order_relaxed);
}

ColorRGB Pixel::normalizedColor(Float splatScale) const noexcept
{
  std::array<double, 3> c{};

  if (m_weightSum > 0.0 && isFinite(m_weightSum))
    for (Idx i{}; i < 3; ++i) 
      c[i] = m_rgbSum[i] / m_weightSum;

  for (Idx i{}; i < 3; ++i) 
  {
    const double s{ m_rgbSplat[i].load(std::memory_order_relaxed) };

    if (isFinite(s)) 
      c[i] = fusedMultiplyAdd(static_cast<double>(splatScale), s, c[i]);
  }

  ColorRGB out{};

  for (Idx i{}; i < 3; ++i)
  {
    out[i] = isFinite(c[i]) ? static_cast<Float>(c[i]) : Float{ 0 };
  }

  return out;
}

void Pixel::clear() noexcept
{
  m_rgbSum = { 0.0, 0.0, 0.0 };
  m_weightSum = 0.0;
  for (auto& s : m_rgbSplat) s.store(0.0, std::memory_order_relaxed);
}