export module pixel;

import std;
import vector;
import colorrgbd;
import types;

/* Pixel.ixx
   Pixel accumulator for Film.
   Based on PBRT’s RGBFilm::Pixel, but adapted to modern C++23.
   Included:
   - rgbSum[3], weightSum, rgbSplat[3] (atomic) for radiance accumulation.
   - addRadiance(), addSplat(), normalizedColor(), clear().
   Omitted (TODOs):
   - Variance estimators (for adaptive sampling).
   - G-buffer attributes (normals, positions, albedo).
   - Spectral bucket accumulation.
   These will be added once Indus supports adaptive sampling, auxiliary AOVs,
   or spectral rendering. For now, Pixel is RGB-only and thread-safe.
*/

export class Pixel 
{
public:
  constexpr Pixel() noexcept;

  void addRadiance(const ColorRGBd&, double) noexcept;
  constexpr void addSplat(const ColorRGBd&) noexcept;

  [[nodiscard]] ColorRGBd normalizedColor(Float splatScale = 1.0) const noexcept;

  constexpr void clear() noexcept;

private:
  ColorRGBd m_rgbSum{};
  double m_weightSum{};
  std::array<std::atomic<Float>, 3> m_rgbSplat{};
};

constexpr Pixel::Pixel() noexcept : m_rgbSum{ 0.0, 0.0, 0.0 }, m_weightSum{ 0.0 }, m_rgbSplat{ 0.0, 0.0, 0.0 } {}

void Pixel::addRadiance(const ColorRGBd& L, double weight) noexcept
{
  if (!std::isfinite(weight)) return;
  if (!(std::isfinite(L[0]) && std::isfinite(L[1]) && std::isfinite(L[2]))) return;

  for (Idx c{}; c < 3; ++c)
  {
    m_rgbSum[c] += L[c] * weight;
  }

  m_weightSum += weight;
}

constexpr void Pixel::addSplat(const ColorRGBd& L) noexcept
{
  for (Idx c{}; c < 3; ++c)
  {
    m_rgbSplat[c].fetch_add(static_cast<Float>(L[c]), std::memory_order_relaxed);
  }
}

ColorRGBd Pixel::normalizedColor(Float splatScale) const noexcept
{
  ColorRGBd c{};

  if (m_weightSum > 0.0 && std::isfinite(m_weightSum)) 
  {
    for (Idx i{}; i < 3; ++i) 
      c[i] = m_rgbSum[i] / m_weightSum;
  }

  // Add splats only if finite
  for (Idx i{}; i < 3; ++i) 
  {
    const Float s = m_rgbSplat[i].load(std::memory_order_relaxed);
    if (std::isfinite(s)) c[i] += splatScale * s;
  }

  // Final sanitize
  for (Idx i{}; i < 3; ++i) if (!std::isfinite(c[i])) c[i] = 0.0;
  return c;
}

constexpr void Pixel::clear() noexcept
{
  m_rgbSum = {};
  m_weightSum = 0.0;
  for (auto& s : m_rgbSplat) s.store(0.0, std::memory_order_relaxed);
}