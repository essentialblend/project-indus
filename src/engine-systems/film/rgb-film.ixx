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

  //virtual void notifyTileComplete(const Bounds2i& tile) noexcept override;

  void addSample(const Point2f& pFilm, const ColorRGB& L, Float64 weight) noexcept override;

  void addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept override;

  [[nodiscard]] ColorRGB getPixelColor(const Point2i& p, Float splatScale) const noexcept override;

  void writeImage(const IndusConfig& indusConfig, const RenderTimer& renderTimer, const std::string& filename = {}) const override;
  
  void clear() noexcept override;

  [[nodiscard]] std::vector<std::uint8_t> bakeDisplay() const noexcept override;

  ~RGBFilm() override = default;

private:
  std::vector<Pixel> m_pixels{};
  
  Point2i m_extent{};
  
  bool  m_writeFP16{ true };
  
  Float m_maxComponentValue{ infinity<Float> };
  Float m_filterIntegral{};

  std::mutex m_dirtyMutex;
  std::vector<Bounds2i> m_dirtyRects;
  std::atomic<UInt64> m_dirtyEpoch{ 0 };

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

//void RGBFilm::notifyTileComplete(const Bounds2i& tile) noexcept
//{
//  const auto& loF{ m_pixelBounds.getMin() };
//  const auto& hiF{ m_pixelBounds.getMax() };
//
//  const auto& loT{ tile.getMin() };
//  const auto& hiT{ tile.getMax() };
//
//  const Bounds2i clamped{
//    Point2i{ std::max(loT[0], loF[0]), std::max(loT[1], loF[1]) },
//    Point2i{ std::min(hiT[0], hiF[0]), std::min(hiT[1], hiF[1]) }
//  };
//
//  if (clamped.isEmpty()) return;
//
//  // Record the dirty rectangle thread-safely for later UI consumption.
//  {
//    std::scoped_lock lock{ m_dirtyMutex };
//    m_dirtyRects.push_back(clamped);
//  }
//
//  // Bump an epoch so a display thread can poll and repaint incrementally.
//  m_dirtyEpoch.fetch_add(1, std::memory_order_relaxed);
//}

void RGBFilm::addSample(const Point2f& pFilm, const ColorRGB& L, Float64 weight) noexcept
{
  const Point2i p{ static_cast<int>(pFilm[0]), static_cast<int>(pFilm[1]) };

  if (!inFilmBounds(p, getPixelBounds())) return;

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
  const auto r = m_filter->getSupportRadius();

  const Int x0{ static_cast<Int>(std::floor(pFilm[0] - r[0] + 0.5f)) };
  const Int x1{ static_cast<Int>(std::ceil(pFilm[0] + r[0] + 0.5f)) };
  const Int y0{ static_cast<Int>(std::floor(pFilm[1] - r[1] + 0.5f)) };
  const Int y1{ static_cast<Int>(std::ceil(pFilm[1] + r[1] + 0.5f)) };

  const auto& pMin{ m_pixelBounds.getMin() };
  const auto& pMax{ m_pixelBounds.getMax() };
  
  for (Int y{ std::max(y0, pMin[1]) }; y < std::min(y1, pMax[1]); ++y)
  {
    for (Int x{ std::max(x0, pMin[0]) }; x < std::min(x1, pMax[0]); ++x)
    {
      const Point2i pi{ x, y };
      const Point2f center{ static_cast<Float>(x) + 0.5f, static_cast<Float>(y) + 0.5f };
      const Point2f d{ pFilm[0] - center[0], pFilm[1] - center[1] };
      
      const Float w{ m_filter->getWeightAtOffset(d) };
      
      if (w == Float{}) continue;
      
      const ColorRGB Lw{ L[0] * w, L[1] * w, L[2] * w };
      
      m_pixels[pixelIndex(pi)].addSplat(Lw);
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
      const ColorRGB linear{ getPixelColor(Point2i{ x, y }, Float{ 1 }) };
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

std::vector<std::uint8_t> RGBFilm::bakeDisplay() const noexcept
{
  const Bounds2i pixelBounds{ getPixelBounds() };
  const int extentWidth{ m_extent[0] };
  const int extentHeight{ m_extent[1] };

  std::vector<std::uint8_t> bytes{};

  bytes.resize(static_cast<std::size_t>(extentWidth) * static_cast<std::size_t>(extentHeight) * 4u);

  std::size_t k{};
  
  for (int y{ pixelBounds.getMin()[1] }; y < pixelBounds.getMax()[1]; ++y)
  {
    for (int x{ pixelBounds.getMin()[0] }; x < pixelBounds.getMax()[0]; ++x) 
    {
      const ColorRGB linearRGB{ getPixelColor(Point2i{ x, y }, Float{ 1 }) };
      const ColorRGB encodedRGB{ encodeColor(ColorEncoding::sRGB, linearRGB) };
      
      bytes[k++] = quantizeToU8(encodedRGB[0]);
      bytes[k++] = quantizeToU8(encodedRGB[1]);
      bytes[k++] = quantizeToU8(encodedRGB[2]);
      bytes[k++] = 255;
    }
  }

  return bytes;
}

Idx RGBFilm::pixelIndex(const Point2i& p) const noexcept
{
  const Bounds2i pixelBounds{ getPixelBounds() };

  const int ox{ p[0] - pixelBounds.getMin()[0] };
  const int oy{ p[1] - pixelBounds.getMin()[1] };
  
  return static_cast<Idx>(oy) * static_cast<Idx>(m_extent[0]) + static_cast<Idx>(ox);
}
