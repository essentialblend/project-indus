export module bounds2;

import std;

import vector;
import point;
import concepts;

export template <Arithmetic T>
class Bounds2 final
{
public:
  constexpr Bounds2() noexcept;
  constexpr explicit Bounds2(const Point<T, 2>&) noexcept;
  constexpr explicit Bounds2(const Point<T, 2>&, const Point<T, 2>&) noexcept;

  constexpr const Point<T, 2>& operator[](std::size_t) const & noexcept;
  constexpr Point<T, 2>& operator[](std::size_t) & noexcept;

  constexpr Vector<T, 2> extent() const noexcept;
  constexpr T area() const noexcept;
  constexpr bool contains(const Point<T, 2>&) const noexcept;

  static constexpr Bounds2<T> getEnclosingBounds(const Bounds2<T>&, const Point<T, 2>&);
  static constexpr Bounds2<T> getEnclosingBounds(const Bounds2<T>&, const Bounds2<T>&);

  // Deferred: corner, intersect, overlaps, expand, lerp, offset

private:
  Point<T, 2> pMin{};
  Point<T, 2> pMax{};
};

template<Arithmetic T>
constexpr Bounds2<T>::Bounds2() noexcept : pMin{ std::numeric_limits<T>::max(), std::numeric_limits<T>::max() }, pMax{ std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest() } {}

template<Arithmetic T>
constexpr Bounds2<T>::Bounds2(const Point<T, 2>& p) noexcept : pMin{ p }, pMax{ p } {}

template<Arithmetic T>
constexpr Bounds2<T>::Bounds2(const Point<T, 2>& p1, const Point<T, 2>& p2) noexcept : pMin{ { std::min(p1[0], p2[0]), std::min(p1[1], p2[1]) } }, pMax{ { std::max(p1[0], p2[0]), std::max(p1[1], p2[1]) } } {}

template <Arithmetic T>
constexpr const Point<T, 2>& Bounds2<T>::operator[](std::size_t i) const& noexcept
{
  return i == 0 ? pMin : pMax;
}

template <Arithmetic T>
constexpr Point<T, 2>& Bounds2<T>::operator[](std::size_t i) & noexcept
{
  return const_cast<Point<T, 2>&>(std::as_const(*this)[i]);
}


template<Arithmetic T>
constexpr Vector<T, 2> Bounds2<T>::extent() const noexcept
{
  return { pMax[0] - pMin[0], pMax[1] - pMin[1] };
}

template<Arithmetic T>
constexpr T Bounds2<T>::area() const noexcept
{
  const auto e{ extent() };
  return e[0] * e[1];
}

template<Arithmetic T>
constexpr bool Bounds2<T>::contains(const Point<T, 2>& p) const noexcept
{
  //return (p[0] >= pMin[0] && p[0] <= pMax[0] && p[1] >= pMin[1] && p[1] <= pMax[1]);

  if constexpr (IntegralArithmetic<T>) 
  {
    return (p[0] >= pMin[0] && p[0] < pMax[0] && p[1] >= pMin[1] && p[1] < pMax[1]);
  }
  else 
  {
    return (p[0] >= pMin[0] && p[0] <= pMax[0] && p[1] >= pMin[1] && p[1] <= pMax[1]);
  }
}

template <Arithmetic T>
constexpr Bounds2<T> Bounds2<T>::getEnclosingBounds(const Bounds2<T>& b, const Point<T, 2>& p) 
{
  return Bounds2<T>(Point<T, 2>{std::min(b.pMin[0], p[0]), std::min(b.pMin[1], p[1])}, Point<T, 2>{std::max(b.pMax[0], p[0]), std::max(b.pMax[1], p[1])});
}

template <Arithmetic T>
constexpr Bounds2<T> Bounds2<T>::getEnclosingBounds(const Bounds2<T>& bx, const Bounds2<T>& by) 
{
  return Bounds2<T>(Point<T, 2>{std::min(bx.pMin[0], by.pMin[0]), std::min(bx.pMin[1], by.pMin[1])}, Point<T, 2>{std::max(bx.pMax[0], by.pMax[0]), std::max(bx.pMax[1], by.pMax[1])});
}








