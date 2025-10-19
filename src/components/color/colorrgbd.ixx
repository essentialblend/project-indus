export module colorrgbd;

import std;
import types;

export class ColorRGBd final
{
public:
  constexpr ColorRGBd() = default;
  [[nodiscard]] constexpr ColorRGBd(double) noexcept;
  [[nodiscard]] explicit constexpr ColorRGBd(double, double, double);

  constexpr const double& operator[](Idx) const & noexcept;
  constexpr double& operator[](Idx) & noexcept;

  [[nodiscard]] constexpr ColorRGBd operator-() const noexcept;

  constexpr ColorRGBd& operator+=(const ColorRGBd&) noexcept;
  constexpr ColorRGBd& operator-=(const ColorRGBd&) noexcept;
  constexpr ColorRGBd& operator*=(double) noexcept;
  constexpr ColorRGBd& operator/=(double) noexcept;

  [[nodiscard]] constexpr ColorRGBd operator+(const ColorRGBd&) const noexcept;
  [[nodiscard]] constexpr ColorRGBd operator-(const ColorRGBd&) const noexcept;
  [[nodiscard]] constexpr ColorRGBd operator*(double) const noexcept;
  [[nodiscard]] constexpr ColorRGBd operator/(double) const noexcept;

private:
  double m_red{};
  double m_green{};
  double m_blue{};
};

// Implementation

constexpr ColorRGBd::ColorRGBd(double col) noexcept : m_red{ col }, m_green{ col }, m_blue{ col } {}

constexpr ColorRGBd::ColorRGBd(double red, double green, double blue) : m_red{ red }, m_green{ green }, m_blue{ blue } {}

constexpr const double& ColorRGBd::operator[](Idx i) const & noexcept
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

constexpr double& ColorRGBd::operator[](Idx i) & noexcept
{
  return const_cast<double&>(std::as_const(*this)[i]);
}

constexpr ColorRGBd ColorRGBd::operator-() const noexcept
{
  return ColorRGBd{ -m_red, -m_green, -m_blue };
}

constexpr ColorRGBd& ColorRGBd::operator+=(const ColorRGBd& c) noexcept
{
  m_red += c.m_red;
  m_green += c.m_green;
  m_blue += c.m_blue;

  return *this;
}

constexpr ColorRGBd& ColorRGBd::operator-=(const ColorRGBd& c) noexcept
{
  return (*this) += -c;
}

constexpr ColorRGBd& ColorRGBd::operator*=(double s) noexcept
{
  m_red *= s;
  m_green *= s;
  m_blue *= s;

  return *this;
}

constexpr ColorRGBd& ColorRGBd::operator/=(double s) noexcept
{
  const double inv{ double(1) / s };
  m_red *= inv;
  m_green *= inv;
  m_blue *= inv;

  return *this;
}

constexpr ColorRGBd ColorRGBd::operator+(const ColorRGBd& c) const noexcept
{
  auto result{ (*this) };
  result += c;
  return result;
}

constexpr ColorRGBd ColorRGBd::operator-(const ColorRGBd& c) const noexcept
{
  auto result{ (*this) };
  result -= c;
  return result;
}

constexpr ColorRGBd ColorRGBd::operator*(double c) const noexcept
{
  auto result{ (*this) };
  result *= c;
  return result;
}

constexpr ColorRGBd ColorRGBd::operator/(double c) const noexcept
{
  auto result{ (*this) };
  result /= c;
  return result;
}