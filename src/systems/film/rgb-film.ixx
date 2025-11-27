module;
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../dep/stb_image_write.h"

export module indus.film.rgb;

import indus.core.types;
import indus.core.math.constants.i;

import indus.film.base;
import indus.film.image;
import indus.film.color_util;
import indus.film.pixel;

import indus.utilities.basictimer;

export class RGBFilm final : public FilmBase
{
public:
  explicit RGBFilm(const Point2i& fullRes, const Bounds2i& crop, Float diagMM, std::unique_ptr<Filter> filmFilter, const PixelSensor& pixelSensor) noexcept;

  void addSample(const Point2f& pFilm, const ColorRGB& L, Float weight) noexcept override;

  void addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept override;

  [[nodiscard]] ColorRGB getPixelColor(const Point2i& p, Float splatScale) const noexcept override;

  void writeImage(int spp, const BasicTimer& renderTimer, const std::string& filename = {}) const override;

  virtual Image toImageU8(ColorEncoding colorEncoding, Float splatScale = 1) const noexcept override;
  virtual Image toImageF32() const noexcept override;
  
  void clear() noexcept override; 

  [[nodiscard]] std::string toString() const override;

  ~RGBFilm() override = default;

private:
  std::vector<Pixel> m_pixels{};
  
  Point2i m_extent{};
  
  bool  m_writeFP16{ true };
  
  Float m_maxComponentValue{ infinity<Float> };
  Float m_filterIntegral{};

  Idx pixelIndex(const Point2i& p) const noexcept;
  [[nodiscard]] bool inFilmBounds(const Point2i& p, const Bounds2i& pixelBounds) const noexcept;
  [[nodiscard]] std::vector<UInt8> packEncodedBytes(ColorEncoding colorEncoding, Float splatScale, Bounds2i bounds, bool withAlpha = false) const noexcept;
};

RGBFilm::RGBFilm(const Point2i& fullRes, const Bounds2i& crop, Float diagMM, std::unique_ptr<Filter> filmFilter, const PixelSensor& sensor) noexcept : FilmBase(fullRes, crop, diagMM, std::move(filmFilter), sensor)
{
  const Bounds2i pb{ getPixelBounds() };

  m_extent = Point2i{ pb.getMax()[0] - pb.getMin()[0], pb.getMax()[1] - pb.getMin()[1] };

  m_pixels = std::vector<Pixel>(static_cast<Idx>(m_extent[0]) * static_cast<Idx>(m_extent[1]));

  m_filterIntegral = m_filter->getIntegral();

  if (m_filterIntegral == Float{}) m_filterIntegral = Float{ 1 };
}

void RGBFilm::addSample(const Point2f& pFilm, const ColorRGB& L, Float weight) noexcept
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

std::vector<UInt8> RGBFilm::packEncodedBytes(ColorEncoding colorEncoding, Float splatScale, Bounds2i bounds, bool withAlpha) const noexcept
{
  const int xMin{ bounds.getMin()[0] };
  const int yMin{ bounds.getMin()[1] };
  const int xMax{ bounds.getMax()[0] };
  const int yMax{ bounds.getMax()[1] };

  const std::size_t chans{ withAlpha ? 4u : 3u };

  std::vector<UInt8> out{};

  out.reserve(static_cast<std::size_t>(xMax - xMin) * static_cast<std::size_t>(yMax - yMin) * chans);

  for (int y{ yMin }; y < yMax; ++y)
  {
    for (int x{ xMin }; x < xMax; ++x)
    {
      const ColorRGB rgb{ encodeColor(colorEncoding, getPixelColor(Point2i{ x, y }, splatScale)) };

      out.push_back(quantizeToU8(rgb[0]));
      out.push_back(quantizeToU8(rgb[1]));
      out.push_back(quantizeToU8(rgb[2]));

      if (withAlpha) out.push_back(255u);
    }
  }
  
  return out;
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

void RGBFilm::writeImage(int spp, const BasicTimer& renderTimer, const std::string& filename) const
{
  const Image img{ toImageF32() };

  const Point2i res{ img.getImagePixelResolution() };
  
  const int width{ res[0] };
  const int height{ res[1] };
  
  const std::vector<float>& P32{ img.getP32() };

  // Convert linear F32 to sRGB-encoded 8-bit
  std::vector<UInt8> bytes{};
  bytes.reserve(static_cast<std::size_t>(width) * height * 3u);

  for (int y{}; y < height; ++y)
  {
    for (int x{}; x < width; ++x)
    {
      const std::size_t i{ static_cast<std::size_t>((y * width + x) * 3) };

      const ColorRGB linear{ P32[i + 0], P32[i + 1], P32[i + 2] };

      const ColorRGB enc{ encodeColor(ColorEncoding::sRGB, linear) };

      bytes.push_back(quantizeToU8(enc[0]));
      bytes.push_back(quantizeToU8(enc[1]));
      bytes.push_back(quantizeToU8(enc[2]));
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

  const auto secs{ static_cast<UInt64>(std::chrono::duration_cast<std::chrono::seconds>(now).count()) };

  const std::string shortId{ toBase36(secs % 2176782336ULL) };

  const std::string base{ filename.empty() ? "indus" : filename };

  const std::string outPath{ "renders/" + base + "_" + aspect + "_" + resolutionLabel + "_spp" + std::to_string(spp) + "_" + std::to_string(minutes) + "m" + secondsTwoDigits + "s_" + shortId + ".png" };

  stbi_write_png(outPath.c_str(), width, height, 3, bytes.data(), width * 3);
}

Image RGBFilm::toImageU8(ColorEncoding colorEncoding, Float splatScale) const noexcept
{
  const Bounds2i bounds{ getPixelBounds() };
  auto bytes{ packEncodedBytes(colorEncoding, splatScale, bounds, true) };
  
  return Image(std::move(bytes), getFilmResolution(), std::vector<std::string>{"R", "G", "B", "A"}, colorEncoding);
}

Image RGBFilm::toImageF32() const noexcept
{
  const auto res{ getFilmResolution() };

  const int W{ res[0] }; const int H{ res[1] };
  
  std::vector<float> p; 
  p.resize(static_cast<std::size_t>(W) * H * 3u);
  
  std::size_t i{};
  
  for (int y{}; y < H; ++y)
  {
    for (int x{}; x < W; ++x) 
    {
      const ColorRGB c{ getPixelColor(Point2i{ x, y }, Float{ 1 }) };

      p[i++] = c[0]; 
      p[i++] = c[1]; 
      p[i++] = c[2];
    }
  }
    
  return Image(std::move(p), res, std::vector<std::string>{"R", "G", "B"}, ColorEncoding::Linear);
}

void RGBFilm::clear() noexcept
{
  for (auto& px : m_pixels) 
  {
    px.clear();
  }
}

std::string RGBFilm::toString() const
{
  return "RGBFilm";
}

Idx RGBFilm::pixelIndex(const Point2i& p) const noexcept
{
  const Bounds2i pixelBounds{ getPixelBounds() };

  const int ox{ p[0] - pixelBounds.getMin()[0] };
  const int oy{ p[1] - pixelBounds.getMin()[1] };
  
  return static_cast<Idx>(oy) * static_cast<Idx>(m_extent[0]) + static_cast<Idx>(ox);
}
