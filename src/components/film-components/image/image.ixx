export module image;

import std;
import types;
import cameraconstructs;

export class Image
{
public:
  Image() noexcept = default;

  explicit Image(std::vector<std::uint8_t> p8, Point2i res, std::vector<std::string> channels, ColorEncoding enc);

  explicit Image(std::vector<std::uint16_t> p16, Point2i res, std::vector<std::string> channels);

  explicit Image(std::vector<float> p32, Point2i res, std::vector<std::string> channels, ColorEncoding enc);

  explicit operator bool() const noexcept;

  PixelFormat getPixelFormat() const noexcept;

  Point2i getImagePixelResolution() const noexcept;

  int getNumChannels() const noexcept;

  const std::vector<std::string>& getChannelNames() const noexcept;

  ColorEncoding getColorEncoding() const noexcept;

  const std::vector<std::uint8_t>& getP8() const noexcept;

  const std::vector<std::uint16_t>& getP16() const noexcept;

  const std::vector<float>& getP32() const noexcept;

  ImageMetadata& getMetadata() noexcept;

  const ImageMetadata& getMetadata() const noexcept;

private:
  PixelFormat m_pixelFormat{ PixelFormat::U8 };
  Point2i m_resolution{};

  std::vector<std::string> m_channels{};

  ColorEncoding m_encoding{};
  ImageMetadata m_meta{};

  std::vector<std::uint8_t> m_P8{};
  std::vector<std::uint16_t> m_P16{};
  std::vector<float> m_P32{};
};

Image::Image(std::vector<std::uint8_t> p8, Point2i res, std::vector<std::string> channels, ColorEncoding enc) : m_pixelFormat(PixelFormat::U8), m_resolution(res), m_channels(std::move(channels)), m_encoding(enc), m_P8(std::move(p8)) {}

Image::Image(std::vector<std::uint16_t> p16, Point2i res, std::vector<std::string> channels) : m_pixelFormat(PixelFormat::F16), m_resolution(res), m_channels(std::move(channels)), m_P16(std::move(p16)) {}

Image::Image(std::vector<float> p32, Point2i res, std::vector<std::string> channels, ColorEncoding enc) : m_pixelFormat(PixelFormat::F32), m_resolution(res), m_channels(std::move(channels)), m_encoding(enc), m_P32(std::move(p32)) {}

PixelFormat Image::getPixelFormat() const noexcept
{
  return m_pixelFormat;
}

Point2i Image::getImagePixelResolution() const noexcept
{
  return m_resolution;
}

int Image::getNumChannels() const noexcept
{
  return static_cast<int>(m_channels.size());
}

const std::vector<std::string>& Image::getChannelNames() const noexcept
{
  return m_channels;
}

ColorEncoding Image::getColorEncoding() const noexcept
{
  return m_encoding;
}

const std::vector<std::uint8_t>& Image::getP8() const noexcept
{
  return m_P8;
}

const std::vector<std::uint16_t>& Image::getP16() const noexcept
{
  return m_P16;
}

const std::vector<float>& Image::getP32() const noexcept
{
  return m_P32;
}

ImageMetadata& Image::getMetadata() noexcept
{
  return m_meta;
}

const ImageMetadata& Image::getMetadata() const noexcept
{
  return m_meta;
}


