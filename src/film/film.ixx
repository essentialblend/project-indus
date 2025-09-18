module;
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../dep/stb_image_write.h"

export module film;

import std;
import constructs;
import vector;
import types;
import pixel;
import point;
import colorrgb;

// TODOs: PixelSensor and color space transforms, variance estimators for adaptive sampling, G-buffer attributes, EXR/FP16 output, current PNG

export class Film 
{
public:
  Film(const Point2i& sceneResPixels) noexcept;

  void addSample(const Point2f& pFilm, const ColorRGB& L, Float weight) noexcept;
  void addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept;

  [[nodiscard]] ColorRGB getPixelColor(const Point2i& p, Float splatScale = 1.0) const noexcept;
  void writeImage(const std::string& filename) const;

  [[nodiscard]] Point2i getFilmResolution() const noexcept;

  void clear() noexcept;


private:
  Point2i m_filmResolution{};
  // Bounds2i m_pixelBounds{};
  std::vector<Pixel> m_pixels{};
};

Film::Film(const Point2i& sceneResPixels) noexcept : m_filmResolution{ sceneResPixels }, m_pixels(m_filmResolution[0] * m_filmResolution[1]) {}

void Film::addSample(const Point2f& pFilm, const ColorRGB& L, Float weight) noexcept
{
  // Clamp to pixel bounds
  if (pFilm[0] < 0.0 || pFilm[0] >= static_cast<Float>(m_filmResolution[0]) || pFilm[1] < 0.0 || pFilm[1] >= static_cast<Float>(m_filmResolution[1]))
  {
    return;
  }

  // Round down to integer pixel index (PBRT would filter here)
  int ix = static_cast<int>(std::floor(pFilm[0]));
  int iy = static_cast<int>(std::floor(pFilm[1]));

  m_pixels[static_cast<size_t>(iy * m_filmResolution[0] + ix)].addRadiance(ColorRGBd{ L[0], L[1], L[2] }, weight);
}

void Film::addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept
{
  if (pFilm[0] < 0.0 || pFilm[0] >= static_cast<double>(m_filmResolution[0]) || pFilm[1] < 0.0 || pFilm[1] >= static_cast<double>(m_filmResolution[1]))
  {
    return;
  }

  int ix = static_cast<int>(std::floor(pFilm[0]));
  int iy = static_cast<int>(std::floor(pFilm[1]));

  m_pixels[static_cast<size_t>(iy * m_filmResolution[0] + ix)].addSplat(ColorRGBd{ L[0], L[1], L[2] });
}

ColorRGB Film::getPixelColor(const Point2i& p, Float splatScale) const noexcept
{
  if (p[0] < 0 || p[0] >= m_filmResolution[0] || p[1] < 0 || p[1] >= m_filmResolution[1]) {
    return ColorRGB{ 0.0 };
  }

  auto c = m_pixels[static_cast<size_t>(p[1] * m_filmResolution[0] + p[0])].normalizedColor(splatScale);
  
  return ColorRGB(Vec3f{ static_cast<Float>(c[0]), static_cast<Float>(c[1]), static_cast<Float>(c[2]) });
}

void Film::writeImage(const std::string& filename) const
{
  const int resWidth = m_filmResolution[0];
  const int resHeight = m_filmResolution[1];

  std::vector<std::uint8_t> fb;
  fb.reserve(static_cast<std::size_t>(resWidth * resHeight * 3));

  for (int y = 0; y < resHeight; ++y)
  {
    for (int x = 0; x < resWidth; ++x) 
    { 
      ColorRGBd cd = m_pixels[y * resWidth + x].normalizedColor();
      
      ColorRGB c(cd);
      
      c[0] = std::clamp(c[0], Float(0), Float(1));
      c[1] = std::clamp(c[1], Float(0), Float(1));
      c[2] = std::clamp(c[2], Float(0), Float(1));

      const auto gammaEncode = [](Float v) -> Float 
      {
        if (!std::isfinite(v)) return Float(0);
        v = std::clamp(v, Float(0), Float(1));
        return std::pow(v, Float(1.0 / 2.2));
      };

      fb.push_back(static_cast<std::uint8_t>(255 * gammaEncode(c[0])));
      fb.push_back(static_cast<std::uint8_t>(255 * gammaEncode(c[1])));
      fb.push_back(static_cast<std::uint8_t>(255 * gammaEncode(c[2])));
    }
  }

  stbi_write_png(filename.c_str(), resWidth, resHeight, 3, fb.data(), resWidth * 3);
}

void Film::clear() noexcept
{
  for (auto& p : m_pixels)
  {
    p.clear();
  }
}

Point2i Film::getFilmResolution() const noexcept
{
  return m_filmResolution;
}