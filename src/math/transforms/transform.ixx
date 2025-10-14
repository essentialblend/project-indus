export module transform;

import std;
import concepts;
import matrix;
import vector;
import point;
import ray;
import types;
import mathfp;
import mathalgebra;
import mathtrig;
import normal;
import point;
import interval;
import mathconstants;

export template<Arithmetic T>
class Transform final
{
public:
  constexpr Transform() noexcept = default;
  constexpr Transform(const Matrix4<T>&, const Matrix4<T>&) noexcept;
  constexpr Transform(const Matrix4<T>&) noexcept;

  constexpr Transform(const Transform&) noexcept = default;
  constexpr Transform(Transform&&) noexcept = default;

  constexpr Transform& operator=(const Transform&) noexcept = default;
  constexpr Transform& operator=(Transform&&) noexcept = default;

  constexpr auto operator<=>(const Transform&) const noexcept = delete;

  constexpr Vector<T, 3> operator()(const Vector<T, 3>&) const noexcept;
  constexpr Normal<T> operator()(const Normal<T>&) const noexcept;
  constexpr Point<T, 3> operator()(const Point<T, 3>&) const noexcept;
  constexpr Ray operator()(const Ray& r) const;
  constexpr Point<Interval<T>, 3> operator()(const Point<Interval<T>, 3>& p) const noexcept;
  constexpr Vector<Interval<T>, 3> operator()(const Vector<Interval<T>, 3>& v) const noexcept;

  constexpr Point<T, 3> applyInverse(const Point<T, 3>&) const noexcept;
  constexpr Vector<T, 3> applyInverse(const Vector<T, 3>&) const noexcept;
  constexpr Normal<T> applyInverse(const Normal<T>&) const noexcept;
  constexpr Ray applyInverse(const Ray&) const noexcept;
  
  constexpr Transform getInverseTransform() const noexcept;

  constexpr bool hasScale() const noexcept;

  constexpr Transform operator*(const Transform&) const noexcept;

  constexpr const Matrix4<T>& get() const noexcept;
  constexpr const Matrix4<T>& getInv() const noexcept;

  constexpr bool swapsHandedness() const noexcept;

  static Transform lookAt(const Point<T, 3>&, const Point<T, 3>&, const Vector<T, 3>&);
  static Transform translate(const Vector<T, 3>&);
  static Transform scale(const Vector<T, 3>&);
  //static Transform rotateX(T);
  //static Transform rotateY(T);
  //static Transform rotateZ(T);
  static Transform perspective(T, T, T);

  ~Transform() noexcept = default;

private:
  Matrix4<T> m_forward{};
  Matrix4<T> m_inverse{};
};

export using Transform4f = Transform<Float>;

// Implementation
template<Arithmetic T>
constexpr Transform<T>::Transform(const Matrix4<T>& forward, const Matrix4<T>& inverse) noexcept : m_forward{ forward }, m_inverse{ inverse } {}

template<Arithmetic T>
constexpr Transform<T>::Transform(const Matrix4<T>& forward) noexcept : m_forward{ forward }, m_inverse{ forward.inverse() } {}

template<Arithmetic T>
constexpr Vector<T, 3> Transform<T>::operator()(const Vector<T, 3>& v) const noexcept
{
  const Vector<T, 4> hv{ v[0], v[1], v[2], T{ 0 } };
  const Vector<T, 4> res{ m_forward * hv };

  return { res[0], res[1], res[2] };
}

// Uses inverse transpose to transform normals
template<Arithmetic T>
constexpr Normal<T> Transform<T>::operator()(const Normal<T>& n) const noexcept
{
  const T x{ n[0] }; const T y{ n[1] }; const T z{ n[2] };

  const T nx{ m_inverse[0, 0] * x + m_inverse[1, 0] * y + m_inverse[2, 0] * z };
  const T ny{ m_inverse[0, 1] * x + m_inverse[1, 1] * y + m_inverse[2, 1] * z };
  const T nz{ m_inverse[0, 2] * x + m_inverse[1, 2] * y + m_inverse[2, 2] * z };

  return Normal<T>{ nx, ny, nz };
}

template<Arithmetic T>
constexpr Point<T, 3> Transform<T>::operator()(const Point<T, 3>& p) const noexcept
{
  const Vector<T, 4> hp{ p[0], p[1], p[2], T{ 1 } };
  const Vector<T, 4> res{ m_forward * hp };

  if (res[3] != T{ 0 } && res[3] != T{ 1 })
  {
    const T invW{ T{ 1 } / res[3] };

    return { res[0] * invW, res[1] * invW, res[2] * invW };
  }

  return { res[0], res[1], res[2] };
}

template<Arithmetic T>
constexpr Ray Transform<T>::operator()(const Ray& r) const
{
  const Point<T, 3> o2{ (*this)(r.getOrigin()) };
  const Vector<T, 3> d2{ (*this)(r.getDirection()) };

  return Ray{ o2, d2 };
}

template<Arithmetic T>
constexpr Point<Interval<T>, 3> Transform<T>::operator()(const Point<Interval<T>, 3>& p) const noexcept
{
  const Vector<T, 3> Cx{ (*this)(Vector<T, 3>{ 1, 0, 0 }) };
  const Vector<T, 3> Cy{ (*this)(Vector<T, 3>{ 0, 1, 0 }) };
  const Vector<T, 3> Cz{ (*this)(Vector<T, 3>{ 0, 0, 1 }) };
  const Point<T, 3> Tr{ (*this)(Point<T, 3>{ 0, 0, 0 }) };

  const Interval<T> x{ p[0] }; const Interval<T> y{ p[1] }; const Interval<T> z{ p[2] };

  const auto ax = [&](Int i)
    {
      return fmaI(x, Interval<T>{ Cx[i] }, fmaI(y, Interval<T>{ Cy[i] }, fmaI(z, Interval<T>{ Cz[i] }, Interval<T>{ Tr[i] })));
    };

  return Point<Interval<T>, 3>{ ax(0), ax(1), ax(2) };
}

template<Arithmetic T>
constexpr Vector<Interval<T>, 3> Transform<T>::operator()(const Vector<Interval<T>, 3>& v) const noexcept
{
  const Vector<T, 3> Cx{ (*this)(Vector<T,3>{1, 0, 0}) };
  const Vector<T, 3> Cy{ (*this)(Vector<T,3>{0, 1, 0}) };
  const Vector<T, 3> Cz{ (*this)(Vector<T,3>{0, 0, 1}) };

  const Interval<T> x{ v[0] }, y{ v[1] }, z{ v[2] };

  const auto ax = [&](int i) -> Interval<T> 
  {
    return fmaI(x, Interval<T>{Cx[i]}, fmaI(y, Interval<T>{ Cy[i]}, z * Interval<T>{ Cz[i] }));
  };

  return Vector<Interval<T>, 3>{ ax(0), ax(1), ax(2) };
}

template<Arithmetic T>
constexpr Point<T, 3> Transform<T>::applyInverse(const Point<T, 3>& p) const noexcept
{
  return Transform{ m_inverse, m_forward }(p);
}

template<Arithmetic T>
constexpr Vector<T, 3> Transform<T>::applyInverse(const Vector<T, 3>& v) const noexcept
{
  return Transform{ m_inverse, m_forward }(v);
}

template<Arithmetic T>
constexpr Normal<T> Transform<T>::applyInverse(const Normal<T>& n) const noexcept
{
  return Transform{ m_inverse, m_forward }(n);
}

template<Arithmetic T>
constexpr Ray Transform<T>::applyInverse(const Ray& r) const noexcept
{
  return Transform{ m_inverse, m_forward }(r);
}

template<Arithmetic T>
constexpr Transform<T> Transform<T>::getInverseTransform() const noexcept
{
  return Transform{ m_inverse, m_forward };
}

template<Arithmetic T>
constexpr Transform<T> Transform<T>::operator*(const Transform<T>& other) const noexcept
{
  return Transform<T>{ m_forward * other.m_forward, other.m_inverse * m_inverse };
}

template<Arithmetic T>
constexpr const Matrix4<T>& Transform<T>::get() const noexcept
{
  return m_forward;
}

template<Arithmetic T>
constexpr const Matrix4<T>& Transform<T>::getInv() const noexcept
{
  return m_inverse;
}

template<Arithmetic T>
constexpr bool Transform<T>::swapsHandedness() const noexcept
{
  const auto& mat{ this->m_forward };

  const T a{ mat[0, 0] }; const T b{ mat[0, 1] }; const T c{ mat[0, 2] };
  const T d{ mat[1, 0] }; const T e{ mat[1, 1] }; const T f{ mat[1, 2] };
  const T g{ mat[2, 0] }; const T h{ mat[2, 1] }; const T i{ mat[2, 2] };

  const T ei_fh{ differenceOfProducts(e, i, f, h) };
  const T di_fg{ differenceOfProducts(d, i, f, g) };
  const T dh_eg{ differenceOfProducts(d, h, e, g) };

  const T det{ differenceOfProducts(a, ei_fh, b, di_fg) + c * dh_eg };
  
  return det < T(0);
}

template<Arithmetic T>
Transform<T> Transform<T>::lookAt(const Point<T, 3>& eye, const Point<T, 3>& target, const Vector<T, 3>& upHint)
{
  Vector<T, 3> f{ normalize(target - eye) };
  Vector<T, 3> up{ normalize(upHint) };
  if (std::abs(computeDot(f, up)) > T{ 0.999 }) up = { T{ 0 }, T{ 1 }, T{ 0 } };
  Vector<T, 3> r{ normalize(computeCross(up, f)) };
  Vector<T, 3> u{ computeCross(f, r) };

  Matrix4<T> camToWorld
  {
    Vector<T, 4>{ r[0], r[1], r[2], T{ 0 } },
    Vector<T, 4>{ u[0], u[1], u[2], T{ 0 } },
    Vector<T, 4>{ f[0], f[1], f[2], T{ 0 } },
    Vector<T, 4>{ eye[0], eye[1], eye[2], T{ 1 } }
  };
  
  return Transform<T>{ camToWorld };
}

template<Arithmetic T>
Transform<T> Transform<T>::translate(const Vector<T, 3>& v)
{
  Matrix4<T> m
  {
    Vector<T, 4>{1, 0, 0, 0},
    Vector<T, 4>{0, 1, 0, 0},
    Vector<T, 4>{0, 0, 1, 0},
    Vector<T, 4>{v[0], v[1], v[2], 1}
  };


  Matrix4<T> mInv
  {
    Vector<T, 4>{1, 0, 0, 0},
    Vector<T, 4>{0, 1, 0, 0},
    Vector<T, 4>{0, 0, 1, 0},
    Vector<T, 4>{-v[0], -v[1], -v[2], 1}
  };

  return { m, mInv };
}

template<Arithmetic T>
Transform<T> Transform<T>::scale(const Vector<T, 3>& v)
{
  Matrix4<T> m
  {
    Vector<T, 4>{v[0], 0, 0, 0},
    Vector<T, 4>{0, v[1], 0, 0},
    Vector<T, 4>{0, 0, v[2], 0},
    Vector<T, 4>{0, 0, 0, 1}
  };

  Matrix4<T> mInv
  {
    Vector<T, 4>{1 / v[0], 0, 0, 0},
    Vector<T, 4>{0, 1 / v[1], 0, 0},
    Vector<T, 4>{0, 0, 1 / v[2], 0},
    Vector<T, 4>{0, 0, 0, 1}
  };

  return { m, mInv };
}

template<Arithmetic T>
Transform<T> Transform<T>::perspective(T fovDegrees, T nearPlane, T farPlane)
{
  T fovRadians{ degreesToRadians(fovDegrees) };
  T invTan{ T{1} / std::tan(fovRadians / T{ 2 }) };

  const Matrix4<T> perspective
  {
    Vector<T, 4>{1, 0, 0, 0},
    Vector<T, 4>{0, 1, 0, 0},
    Vector<T, 4>{0, 0, farPlane / (farPlane - nearPlane), 1},
    Vector<T, 4>{0, 0, -farPlane * nearPlane / (farPlane - nearPlane), 0}
  };

  return Transform<T>::scale({ invTan, invTan, 1 }) * Transform<T>{ perspective };
}

template<Arithmetic T>
constexpr bool Transform<T>::hasScale() const noexcept
{
  const Vector<T, 3> ex{ (*this)(Vector<T, 3>{T{ 1 }, T{ 0 }, T{ 0 }}) };
  const Vector<T, 3> ey{ (*this)(Vector<T, 3>{T{ 0 }, T{ 1 }, T{ 0 }}) };
  const Vector<T, 3> ez{ (*this)(Vector<T, 3>{T{ 0 }, T{ 0 }, T{ 1 }}) };

  constexpr T one{ 1 };
  constexpr T eps{ kSafeNormalizeLen<T> };
  
  const auto dev = [&](T s) { return std::abs(s - one) > eps; };
  
  return dev(euclideanLengthSq(ex)) || dev(euclideanLengthSq(ey)) || dev(euclideanLengthSq(ez));
}