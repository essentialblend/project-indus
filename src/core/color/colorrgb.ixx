export module indus.core.colorrgb;

import indus.core.types;

export class ColorRGB final
{
public:
  constexpr ColorRGB() = default;
  [[nodiscard]] constexpr explicit ColorRGB(Float) noexcept;
  [[nodiscard]] constexpr explicit ColorRGB(Float, Float, Float) noexcept;
  [[nodiscard]] explicit constexpr ColorRGB(const Vec3f&) noexcept;

  [[nodiscard]] constexpr const Float& operator[](Idx) const & noexcept;
  [[nodiscard]] constexpr Float& operator[](Idx) & noexcept;

  [[nodiscard]] constexpr ColorRGB operator-() const noexcept;

  constexpr ColorRGB& operator+=(const ColorRGB&) noexcept;
  constexpr ColorRGB& operator-=(const ColorRGB&) noexcept;
  constexpr ColorRGB& operator*=(Float) noexcept;
  constexpr ColorRGB& operator*=(const ColorRGB&) noexcept;
  constexpr ColorRGB& operator/=(Float) noexcept;

  [[nodiscard]] constexpr ColorRGB operator+(const ColorRGB&) const noexcept;
  [[nodiscard]] constexpr ColorRGB operator-(const ColorRGB&) const noexcept;
  [[nodiscard]] constexpr ColorRGB operator*(Float) const noexcept;
  [[nodiscard]] ColorRGB operator*(const ColorRGB&) const noexcept;

  [[nodiscard]] constexpr ColorRGB operator/(Float) const noexcept;

private:
  Float m_red{}; 
  Float m_green{};
  Float m_blue{};
};

// Implementation

constexpr ColorRGB::ColorRGB(Float col) noexcept : m_red{ col }, m_green{ col }, m_blue{ col } {}

constexpr ColorRGB::ColorRGB(Float red, Float green, Float blue) noexcept : m_red{ red }, m_green{ green }, m_blue{ blue } {}

constexpr ColorRGB::ColorRGB(const Vec3f& v) noexcept : m_red{ v[0] }, m_green{ v[1] }, m_blue{ v[2] } {}

constexpr const Float& ColorRGB::operator[](Idx i) const & noexcept
{
  switch (i) 
  {
    case 0:
      return m_red;
    case 1: 
      return m_green;
    case 2: 
      return m_blue;
    default: 
      std::unreachable();
  }
}

constexpr Float& ColorRGB::operator[](Idx i) & noexcept
{
  return const_cast<Float&>(std::as_const(*this)[i]);
}

constexpr ColorRGB ColorRGB::operator-() const noexcept
{
  return ColorRGB{ -m_red, -m_green, -m_blue };
}

constexpr ColorRGB& ColorRGB::operator+=(const ColorRGB& c) noexcept
{
  m_red += c.m_red;
  m_green += c.m_green;
  m_blue += c.m_blue;

  return *this;
}

constexpr ColorRGB& ColorRGB::operator-=(const ColorRGB& c) noexcept
{
  return (*this) += -c;
}

constexpr ColorRGB& ColorRGB::operator*=(Float s) noexcept
{
  m_red *= s;
  m_green *= s;
  m_blue *= s;

  return *this;
}

constexpr ColorRGB& ColorRGB::operator*=(const ColorRGB& c) noexcept
{
  m_red *= c[0];
  m_green *= c[1];
  m_blue *= c[2];

  return *this;
}

constexpr ColorRGB& ColorRGB::operator/=(Float s) noexcept
{
  const Float inv{ Float{ 1 } / s };
  m_red *= inv;
  m_green *= inv;
  m_blue *= inv;

  return *this;
}

constexpr ColorRGB ColorRGB::operator+(const ColorRGB& c) const noexcept
{
  auto result{ (*this) };
  result += c;
  return result;
}

constexpr ColorRGB ColorRGB::operator-(const ColorRGB& c) const noexcept
{
  auto result{ (*this) };
  result -= c;
  return result;
}

constexpr ColorRGB ColorRGB::operator*(Float c) const noexcept
{
  auto result{ (*this) };
  result *= c;
  return result;
}

ColorRGB ColorRGB::operator*(const ColorRGB& c) const noexcept
{
  auto result{ (*this) };
  result *= c;
  return result;
}

constexpr ColorRGB ColorRGB::operator/(Float c) const noexcept
{
  auto result{ (*this) };
  result /= c;
  return result;
}

// Symmetric free-operators

export constexpr ColorRGB operator*(Float s, const ColorRGB& c) noexcept 
{
  return c * s;
}