module;
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../dep/stb_image_write.h"

export module rgbfilm;

import std;
import filmbase;
import pixel;
import squarematrix;
import types;
import cameraconstructs;
import mathconstants;
import engineconstructs;
import rendertimer;
import colorutils;
import mathfp;
import pixelsensor;

export class RGBFilm final : public FilmBase
{
public:
  explicit RGBFilm(const Point2i& fullRes, const Bounds2i& crop, Float diagMM, std::unique_ptr<Filter> filmFilter, const PixelSensor& pixelSensor) noexcept;

  void addSample(const Point2f& pFilm, const ColorRGB& L, Float64 weight) noexcept override;

  void addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept override;

  [[nodiscard]] ColorRGB getPixelColor(const Point2i& p, Float splatScale) const noexcept override;

  void writeImage(const IndusConfig& indusConfig, const RenderTimer& renderTimer, const std::string& filename = {}) const override;
  void clear() noexcept override;

  ~RGBFilm() override = default;

private:
  std::vector<Pixel> m_pixels{};
  
  Point2i m_extent{};
  
  bool  m_writeFP16{ true };
  
  Float m_maxComponentValue{ infinity<Float> };
  Float m_filterIntegral{};

  Idx pixelIndex(const Point2i& p) const noexcept;
  [[nodiscard]] bool inFilmBounds(const Point2i& p, const Bounds2i& pixelBounds) const noexcept;
};

RGBFilm::RGBFilm(const Point2i& fullRes, const Bounds2i& crop, Float diagMM, std::unique_ptr<Filter> filmFilter, const PixelSensor& sensor) noexcept : FilmBase(fullRes, crop, diagMM, std::move(filmFilter), sensor)
{
  const Bounds2i pb{ getPixelBounds() };

  m_extent = Point2i
  {
    pb.getMax()[0] - pb.getMin()[0],
    pb.getMax()[1] - pb.getMin()[1]
  };

  m_pixels = std::vector<Pixel>(static_cast<Idx>(m_extent[0]) * static_cast<Idx>(m_extent[1]));

  m_filterIntegral = m_filter->getIntegral();

  if (m_filterIntegral == Float{}) m_filterIntegral = Float{ 1 };
}

void RGBFilm::addSample(const Point2f& pFilm, const ColorRGB& L, Float64 weight) noexcept
{
  const Point2i p{ static_cast<int>(pFilm[0]), static_cast<int>(pFilm[1]) };

  if(!inFilmBounds(p, getPixelBounds())) return;

  const Float m{ std::max({ L[0], L[1], L[2] }) };
  const Float clampScale{ (m > m_maxComponentValue) ? (m_maxComponentValue / m) : Float{ 1 } };

  const Idx idx{ pixelIndex(p) };

  Pixel& px{ m_pixels[idx] };
  
  const ColorRGB rgb{ L[0] * clampScale, L[1] * clampScale, L[2] * clampScale };

  px.addRadiance(rgb, weight);
}

[[nodiscard]] bool RGBFilm::inFilmBounds(const Point2i& p, const Bounds2i& pixelBounds) const noexcept
{
  return !(p[0] < pixelBounds.getMin()[0] || p[0] >= pixelBounds.getMax()[0] || p[1] < pixelBounds.getMin()[1] || p[1] >= pixelBounds.getMax()[1]);
}

// Pending some deeper understanding. addSplat remains unused as of now
void RGBFilm::addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept
{
  const Bounds2i pixelBounds{ getPixelBounds() };
  const Vec2f supportRadius{ getFilter().getSupportRadius() };

  const int x0{ std::max(static_cast<int>(std::ceil(pFilm[0] - supportRadius[0] + Float{ 0.5 })), pixelBounds.getMin()[0]) };

  const int y0{ std::max(static_cast<int>(std::ceil(pFilm[1] - supportRadius[1] + Float{ 0.5 })), pixelBounds.getMin()[1]) };

  const int x1{ std::min(static_cast<int>(std::floor(pFilm[0] + supportRadius[0] - Float{ 0.5 })) + 1, pixelBounds.getMax()[0]) };

  const int y1{ std::min(static_cast<int>(std::floor(pFilm[1] + supportRadius[1] - Float{ 0.5 })) + 1, pixelBounds.getMax()[1]) };

  for (int y{ y0 }; y < y1; ++y)
  {
    for (int x{ x0 }; x < x1; ++x)
    {
      const Point2f d{ pFilm[0] - (static_cast<Float>(x) + static_cast<Float>(0.5)), pFilm[1] - (static_cast<Float>(y) + static_cast<Float>(0.5)) };
      
      const Float w{ getFilter().getWeightAtOffset(d) };
      
      if (w == Float{}) continue;

      const Idx idx{ pixelIndex(Point2i{ x, y }) };

      Pixel& px{ m_pixels[idx] };

      const ColorRGB wl{ L[0] * w, L[1] * w, L[2] * w };

      px.addSplat(wl);
    }
  }
}

ColorRGB RGBFilm::getPixelColor(const Point2i& p, Float splatScale) const noexcept
{
  const Bounds2i pixelBounds{ getPixelBounds() };

  if (!inFilmBounds(p, pixelBounds)) return ColorRGB{};

  const Float s{ (m_filterIntegral != Float{}) ? (splatScale / m_filterIntegral) : splatScale };

  const ColorRGB sensor{ m_pixels[pixelIndex(p)].normalizedColor(s) };

  return getPixelSensor().toOutputRGB(sensor);
}

void RGBFilm::writeImage(const IndusConfig& indusConfig, const RenderTimer& renderTimer, const std::string& filename) const
{
  const Bounds2i pixelBounds{ getPixelBounds() };
  const int width{ m_extent[0] };
  const int height{ m_extent[1] };

  std::vector<std::uint8_t> bytes;
  bytes.reserve(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 3);

  constexpr ColorEncoding encodingTag{ ColorEncoding::sRGB };

  for (int y{ pixelBounds.getMin()[1] }; y < pixelBounds.getMax()[1]; ++y)
  {
    for (int x{ pixelBounds.getMin()[0] }; x < pixelBounds.getMax()[0]; ++x)
    {
      const ColorRGB linear{ getPixelColor(Point2i{ x, y }, Float{1 }) };
      const ColorRGB encoded{ encodeColor(encodingTag, linear) };

      bytes.push_back(quantizeToU8(encoded[0]));
      bytes.push_back(quantizeToU8(encoded[1]));
      bytes.push_back(quantizeToU8(encoded[2]));
    }
  }
    
  std::filesystem::create_directories("renders");

  const auto elapsedMs{ renderTimer.getMillisec() };
  const int minutes{ static_cast<int>(elapsedMs / 60000) };
  const int seconds{ static_cast<int>((elapsedMs / 1000) % 60) };
  const std::string secondsTwoDigits{ (seconds < 10 ? "0" : "") + std::to_string(seconds) };

  const int GCD{ std::gcd(width, height) };

  const std::string aspect{ std::to_string(width / GCD) + "x" + std::to_string(height / GCD) };
  const std::string resolutionLabel{ std::to_string(height) + "p" };

  const auto now{ std::chrono::system_clock::now().time_since_epoch() };

  const auto secs{ static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(now).count()) };

  const std::string shortId{ toBase36(secs % 2176782336ULL) };

  const std::string base{ filename.empty() ? "indus" : filename };

  const std::string outPath{ "renders/" + base + "_" + aspect + "_" + resolutionLabel + "_spp" + std::to_string(indusConfig.samplerCfg.samplesPerPixel) + "_" + std::to_string(minutes) + "m" + secondsTwoDigits + "s_" + shortId + ".png" };

  stbi_write_png(outPath.c_str(), width, height, 3, bytes.data(), width * 3);
}

void RGBFilm::clear() noexcept
{
  for (auto& px : m_pixels) 
  {
    px.clear();
  }
}

Idx RGBFilm::pixelIndex(const Point2i& p) const noexcept
{
  const Bounds2i pixelBounds{ getPixelBounds() };

  const int ox{ p[0] - pixelBounds.getMin()[0] };
  const int oy{ p[1] - pixelBounds.getMin()[1] };
  
  return static_cast<Idx>(oy) * static_cast<Idx>(m_extent[0]) + static_cast<Idx>(ox);
}
