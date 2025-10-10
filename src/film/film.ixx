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
import rendertimer;
import mathfp;

// TODOs: PixelSensor and color space transforms, variance estimators for adaptive sampling, G-buffer attributes, EXR/FP16 output, current PNG

export class Film 
{
public:
  Film(const Point2i& sceneResPixels) noexcept;

  void addSample(const Point2f& pFilm, const ColorRGB& L, Float weight) noexcept;
  void addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept;

  [[nodiscard]] ColorRGB getPixelColor(const Point2i& p, Float splatScale = 1.0) const noexcept;
  void writeImage(const IndusConfig& indusConfig, const RenderTimer& renderTimer, const std::string& filename = {}) const;

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
  if (pFilm[0] < Float{} || pFilm[0] >= static_cast<Float>(m_filmResolution[0]) || pFilm[1] < Float{} || pFilm[1] >= static_cast<Float>(m_filmResolution[1]))
  {
    return;
  }

  // Round down to integer pixel index (PBRT would filter here)
  Int ix{ static_cast<Int>(std::floor(pFilm[0])) };
  Int iy{ static_cast<Int>(std::floor(pFilm[1])) };

  m_pixels[static_cast<std::size_t>(iy * m_filmResolution[0] + ix)].addRadiance(ColorRGBd{ L[0], L[1], L[2] }, weight);
}

void Film::addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept
{
  if (pFilm[0] < Float{} || pFilm[0] >= static_cast<Float>(m_filmResolution[0]) || pFilm[1] < Float{} || pFilm[1] >= static_cast<Float>(m_filmResolution[1]))
  {
    return;
  }

  const Int ix{ static_cast<Int>(std::floor(pFilm[0])) };
  const Int iy{ static_cast<Int>(std::floor(pFilm[1])) };

  m_pixels[static_cast<Idx>(iy * m_filmResolution[0] + ix)].addSplat(ColorRGBd{ L[0], L[1], L[2] });
}

ColorRGB Film::getPixelColor(const Point2i& p, Float splatScale) const noexcept
{
  if (p[0] < 0 || p[0] >= m_filmResolution[0] || p[1] < 0 || p[1] >= m_filmResolution[1]) {
    return ColorRGB{};
  }

  auto c = m_pixels[static_cast<std::size_t>(p[1] * m_filmResolution[0] + p[0])].normalizedColor(splatScale);
  
  return ColorRGB(Vec3f{ static_cast<Float>(c[0]), static_cast<Float>(c[1]), static_cast<Float>(c[2]) });
}

void Film::writeImage(const IndusConfig& indusConfig, const RenderTimer& renderTimer, const std::string& filename) const
{
  const Int resWidth{ m_filmResolution[0] };
  const Int resHeight{ m_filmResolution[1] };

  const auto srgbEncode = [](Float v) -> Float 
  {
    if (!std::isfinite(v)) return Float{};

    v = clamp(v, Float{}, Float{ 1.0 });

    return v <= Float{ 0.0031308 } ? v * Float{ 12.92 } : Float{ 1.055 } * std::pow(v, Float{ 1.0 / 2.4 }) - Float{ 0.055 };
  };

  std::vector<std::uint8_t> frameBytes{};
  frameBytes.reserve(static_cast<std::size_t>(resWidth * resHeight * 3));

  for (Int y{}; y < resHeight; ++y)
  {
    for (Int x{}; x < resWidth; ++x)
    {
      ColorRGBd linear = m_pixels[y * resWidth + x].normalizedColor();
      ColorRGB c(linear);

      c[0] = srgbEncode(c[0]); c[1] = srgbEncode(c[1]); c[2] = srgbEncode(c[2]);

      frameBytes.push_back(static_cast<std::uint8_t>(255 * c[0]));
      frameBytes.push_back(static_cast<std::uint8_t>(255 * c[1]));
      frameBytes.push_back(static_cast<std::uint8_t>(255 * c[2]));
    }
  }

  std::filesystem::create_directories("renders");

  const auto ms{ renderTimer.getMillisec() };
  const Int mm{ static_cast<Int>(ms / 60000) };
  const Int ss{ static_cast<Int>((ms / 1000) % 60) };
  
  const std::string ss2{ (ss < 10 ? "0" : "") + std::to_string(ss) };

  const auto gcd = [](Int a, Int b) 
  { 
    while (b) 
    { 
      Int t = a % b; 
      a = b; 
      b = t; 
    } 
    return a; 
  };
  
  const Int g{ gcd(resWidth, resHeight) };
  const std::string ar{ std::to_string(resWidth / g) + "x" + std::to_string(resHeight / g) };
  const std::string resLabel{ std::to_string(resHeight) + "p" };

  const auto now{ std::chrono::system_clock::now().time_since_epoch() };
  const auto secs{ static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(now).count()) };
  
  const auto base36 = [](std::uint64_t v) 
  { 
    std::string s;
    const char* d{ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    do 
    { 
      s.push_back(d[v % 36]); 
      v /= 36; 
    } while (v);
    std::reverse(s.begin(), s.end()); return s; 
  };
  
  const std::string shortId{ base36(secs % 2176782336ULL) };
  const std::string base{ filename.empty() ? "indus" : filename };
  const std::string outPath{ "renders/" + base + "_" + ar + "_" + resLabel +
    "_spp" + std::to_string(indusConfig.samplerCfg.samplesPerPixel) +
    "_" + std::to_string(mm) + "m" + ss2 + "s_" + shortId + ".png" };

  stbi_write_png(outPath.c_str(), resWidth, resHeight, 3, frameBytes.data(), resWidth * 3);
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