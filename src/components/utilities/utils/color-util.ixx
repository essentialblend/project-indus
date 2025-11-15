export module colorutil;

import std;
import types;
import cameraconstructs;
import mathfp;
import colorrgb;

export
{
  Float encodeScalar(ColorEncoding colorEncodingTag, Float v) noexcept
  {
    v = clampUnit(v);

    if (colorEncodingTag == ColorEncoding::Linear) return v;

    return (v <= Float{ 0.0031308 }) ? (v * Float{ 12.92 }) : (Float{ 1.055 } * std::pow(v, Float{ 1.0 / 2.4 }) - Float{ 0.055 });
  }

  ColorRGB encodeColor(ColorEncoding colorEncodingTag, const ColorRGB& c) noexcept
  {
    return ColorRGB{ encodeScalar(colorEncodingTag, c[0]), encodeScalar(colorEncodingTag, c[1]), encodeScalar(colorEncodingTag, c[2]) };
  }

  std::uint8_t quantizeToU8(Float v) noexcept 
  {
    if (!isFinite(v)) v = Float{};

    if (v < Float{}) v = Float{};
    if (v > Float{ 1 }) v = Float{ 1 };

    const auto q{ std::lround(static_cast<double>(v) * 255.0) };

    return static_cast<std::uint8_t>(std::clamp(q, 0L, 255L));
  }

  std::string toBase36(std::uint64_t v) 
  {
    std::string s; 
    const char* d{ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    
    do 
    { 
      s.push_back(d[v % 36]); 
      v /= 36; 
    } while (v);
    
    std::reverse(s.begin(), s.end()); 
    
    return s;
  }
}