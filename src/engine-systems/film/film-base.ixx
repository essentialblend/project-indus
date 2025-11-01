export module filmbase;

import types;
import bounds;
import colorrgb;
import filter;
import engineconstructs;
import rendertimer;
import pixelsensor;

export class FilmBase
{
public:
  explicit FilmBase(const Point2i& fullRes, const Bounds2i& crop, Float diagMM, std::unique_ptr<Filter> filmFilter, const PixelSensor& sensor) noexcept;

  [[nodiscard]] virtual Point2i getFilmResolution() const noexcept;
  [[nodiscard]] virtual Bounds2i getPixelBounds() const noexcept;
  [[nodiscard]] virtual Float getDiagonal() const noexcept;
  [[nodiscard]] virtual Bounds2f getSampleBounds() const noexcept;
  [[nodiscard]] virtual const Filter& getFilter() const noexcept;
  [[nodiscard]] virtual const PixelSensor& getPixelSensor() const noexcept;

  virtual void addSample(const Point2f& pFilm, const ColorRGB& L, Float64 weight) noexcept = 0;

  //virtual void notifyTileComplete(const Bounds2i& tile) noexcept = 0;

  virtual void addSplat(const Point2f& pFilm, const ColorRGB& L) noexcept = 0;

  [[nodiscard]] virtual ColorRGB getPixelColor(const Point2i& p, Float splatScale) const noexcept = 0;


  virtual void writeImage(const IndusConfig& indusConfig, const RenderTimer& renderTimer, const std::string& filename = {}) const = 0;

  [[nodiscard]] virtual std::vector<std::uint8_t> bakeDisplay() const noexcept = 0;

  virtual void clear() noexcept = 0;

  virtual ~FilmBase() = default;

protected:
  Point2i m_fullResolution{};
  Bounds2i m_pixelBounds{};
  Float m_diagonal{};
  std::unique_ptr<Filter> m_filter{};
  PixelSensor m_pixelSensor;
};

FilmBase::FilmBase(const Point2i& fullRes, const Bounds2i& crop, Float diagMM, std::unique_ptr<Filter> filmFilter, const PixelSensor& sensor) noexcept : m_fullResolution{ fullRes }, m_diagonal{ diagMM * Float{ 1e-3 } }, m_filter{ std::move(filmFilter) }, m_pixelSensor{ sensor }
{
  const Point2i legalMin{};
  const Point2i legalMax{ fullRes[0], fullRes[1] };

  const Point2i cropMin{ std::clamp(crop.getMin()[0], legalMin[0], legalMax[0]), std::clamp(crop.getMin()[1], legalMin[1], legalMax[1]) };

  const Point2i cropMax{ std::clamp(crop.getMax()[0], legalMin[0], legalMax[0]), std::clamp(crop.getMax()[1], legalMin[1], legalMax[1]) };

  const Point2i clampedMax{ std::max(cropMax[0], cropMin[0]), std::max(cropMax[1], cropMin[1]) };

  m_pixelBounds = Bounds2i{ cropMin, clampedMax };
}


Bounds2f FilmBase::getSampleBounds() const noexcept 
{
  const Vec2f r{ m_filter->getSupportRadius() };

  const Vec2f half{ Float{ 0.5 } , Float{ 0.5 } };

  const Point2f pMin{ Point2f(m_pixelBounds.getMin()) - r + half };
  const Point2f pMax{ Point2f(m_pixelBounds.getMax()) + r - half };
  
  return Bounds2f{ pMin, pMax };
}

Bounds2i FilmBase::getPixelBounds() const noexcept
{
  return m_pixelBounds;
}

const Filter& FilmBase::getFilter() const noexcept 
{
  return *m_filter;
}

const PixelSensor& FilmBase::getPixelSensor() const noexcept 
{
  return m_pixelSensor;
}

Point2i FilmBase::getFilmResolution() const noexcept 
{ 
  return m_fullResolution; 
}

Float FilmBase::getDiagonal() const noexcept 
{ 
  return m_diagonal; 
}