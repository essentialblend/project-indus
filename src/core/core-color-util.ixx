export module core_color_util;

import vec3;
import constructs;
import pixel;
import point;

import <SFML/Graphics.hpp>;

export ColorRGB applyWeights(const ColorRGB& color, double w) {
  return color * (1.0 / w);
}

export ColorRGB applyGamma(const ColorRGB& color, double gamma) {
  ColorRGB out;
  for (int i = 0; i < 3; ++i)
    out[i] = (color[i] <= 0) ? 0 : std::pow(color[i], 1.0 / gamma);
  return out;
}

export sf::Color toSFMLColor(const ColorRGB& c) {
  auto clamp = [](double x) { return std::clamp(x, 0.0, 0.999); };
  return sf::Color(
    static_cast<sf::Uint8>(256 * clamp(c[0])),
    static_cast<sf::Uint8>(256 * clamp(c[1])),
    static_cast<sf::Uint8>(256 * clamp(c[2])),
    255
  );
}

export std::vector<std::uint8_t> convertToRGBA8(const std::vector<ColorRGB>& framebuffer, std::size_t width, std::size_t height)
{
  std::vector<std::uint8_t> out;
  out.reserve(static_cast<size_t>(width) * height * 4);

  for (const ColorRGB& colorLinear : framebuffer)
  {
    // Gamma correct
    ColorRGB c = applyGamma(colorLinear, 2.2);

    // Clamp & convert
    sf::Color sfCol = toSFMLColor(c);

    out.push_back(sfCol.r);
    out.push_back(sfCol.g);
    out.push_back(sfCol.b);
    out.push_back(sfCol.a);
  }

  return out;
}

static double encodeChannel(double v, ColorEncoding enc)
{
  // Tonemapping standardization needed
  v = std::clamp(v, 0.0, 1.0);

  switch (enc)
  {
  case ColorEncoding::Linear:
    return v;
  case ColorEncoding::Gamma22:
    return std::pow(v, 1.0 / 2.2);
  case ColorEncoding::sRGB:
    if (v <= 0.0031308) return v * 12.92;
    return 1.055 * std::pow(v, 1.0 / 2.4) - 0.055;
  }

  return v;
}

export std::vector<std::uint8_t> convertToRGBA8(const std::vector<Pixel>& framebuffer, const Point2i& sceneRes,
  ColorEncoding enc) 
{ 
  std::vector<std::uint8_t> out;
  out.reserve(sceneRes[0] * sceneRes[1] * 4);

  for (const Pixel& pixel : framebuffer) 
  {
    ColorRGB c{ pixel.normalizedColor() };

    out.push_back(static_cast<std::uint8_t>(encodeChannel(c[0], enc) * 255.0 + 0.5));
    out.push_back(static_cast<std::uint8_t>(encodeChannel(c[1], enc) * 255.0 + 0.5));
    out.push_back(static_cast<std::uint8_t>(encodeChannel(c[2], enc) * 255.0 + 0.5));
    out.push_back(255);
  }
  
  return out;
}