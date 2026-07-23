export module indus.core.geom.transform;

import indus.core.types;
import indus.core.concepts;
import indus.core.geom.squarematrix;
import indus.core.geom.vector;
import indus.core.geom.point;
import indus.core.geom.ray;
import indus.core.geom.normal;
import indus.core.geom.point;
import indus.core.math.constants.i;
import indus.core.math.fp.ii;
import indus.core.math.trig.iii;
import indus.core.math.algebra.iv;
import indus.core.math.interval;
import indus.core.geom.quaternion;
import indus.core.geom.bounds;

export struct DecomposedTRS final
{
  Vec3f translation{};
  Quaternion rotation{};
  Mat3f scale{};
};

export template<FloatingArithmetic T>
class Transform final 
{
public:

  constexpr Transform() noexcept = default;
  constexpr Transform(const SquareMatrix<T, 4>& m, const SquareMatrix<T, 4>& mInv) noexcept;

  constexpr Transform(const Transform&) noexcept = default;
  constexpr Transform(Transform&&) noexcept = default;
  constexpr Transform& operator=(const Transform&) noexcept = default;
  constexpr Transform& operator=(Transform&&) noexcept = default;

  constexpr Vector<T, 3> operator()(const Vector<T, 3>&) const noexcept;
  constexpr Normal<T> operator()(const Normal<T>&) const noexcept;
  constexpr Point<T, 3> operator()(const Point<T, 3>&) const noexcept;
  constexpr Ray operator()(const Ray&) const noexcept;

  constexpr Vector<Interval<T>, 3> operator()(const Vector<Interval<T>, 3>&) const noexcept;
  constexpr Point<Interval<T>, 3> operator()(const Point<Interval<T>, 3>&) const noexcept;
  constexpr Bounds<T, 3> operator()(const Bounds<T, 3>& b) const noexcept;

  constexpr Vector<T, 3> applyInverse(const Vector<T, 3>&) const noexcept;
  constexpr Normal<T> applyInverse(const Normal<T>&) const noexcept;
  constexpr Point<T, 3> applyInverse(const Point<T, 3>&) const noexcept;
  constexpr Ray applyInverse(const Ray&) const noexcept;
  constexpr Bounds<T, 3> applyInverse(const Bounds<T, 3>& b) const noexcept;

  constexpr Transform getInverseTransform() const noexcept;
  constexpr bool hasScale() const noexcept;
  constexpr bool swapsHandedness() const noexcept;

  constexpr Transform operator*(const Transform&) const noexcept;

  constexpr const SquareMatrix<T, 4>& get() const noexcept;
  constexpr const SquareMatrix<T, 4>& getInv() const noexcept;

  [[nodiscard]] DecomposedTRS decomposeTRS() const noexcept;

  static Transform lookAt(const Point<T, 3>&, const Point<T, 3>&, const Vector<T, 3>&);
  static Transform translate(const Vector<T, 3>&);
  static Transform scale(const Vector<T, 3>&);
  static Transform perspective(T fovDegrees, T n, T f);

private:
  SquareMatrix<T, 4> m_forward{};
  SquareMatrix<T, 4> m_inverse{};
};

export using Transform4f = Transform<Float>;

// Implementation
template<FloatingArithmetic T>
constexpr Transform<T>::Transform(const SquareMatrix<T, 4>& forward, const SquareMatrix<T, 4>& inverse) noexcept : m_forward{ forward }, m_inverse{ inverse } {}

template<FloatingArithmetic T>
constexpr Vector<T, 3> Transform<T>::operator()(const Vector<T, 3>& v) const noexcept
{
  const Vector<T, 4> hv{ v[0], v[1], v[2], T{ 0 } };
  const Vector<T, 4> res{ m_forward * hv };

  return { res[0], res[1], res[2] };
}

// Uses inverse transpose to transform normals
template<FloatingArithmetic T>
constexpr Normal<T> Transform<T>::operator()(const Normal<T>& n) const noexcept
{
  const T x{ n[0] }; const T y{ n[1] }; const T z{ n[2] };

  const T nx{ m_inverse[0, 0] * x + m_inverse[1, 0] * y + m_inverse[2, 0] * z };
  const T ny{ m_inverse[0, 1] * x + m_inverse[1, 1] * y + m_inverse[2, 1] * z };
  const T nz{ m_inverse[0, 2] * x + m_inverse[1, 2] * y + m_inverse[2, 2] * z };

  return Normal<T>{ nx, ny, nz };
}

template<FloatingArithmetic T>
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

template<FloatingArithmetic T>
constexpr Ray Transform<T>::operator()(const Ray& r) const noexcept
{
  Point3fi oI{ (*this)(Point3fi{ r.getOrigin() }) };
  Vector<T, 3> d2{ (*this)(r.getDirection()) };
  Float tMax{ r.getTMax() };

  const Float lenSq{ computeDot(d2, d2) };
  
  if (lenSq > Float{}) 
  {
    const Vector<T, 3> absd{ std::abs(d2[0]), std::abs(d2[1]), std::abs(d2[2]) };
    const Float dt{ computeDot(absd, oI.getError<Float>()) / lenSq };
    
    oI = oI + Vector<Interval<T>, 3>{ d2 } * Interval<T>{ dt };
    
    tMax -= dt;
  }

  Ray out{ Point3f{ oI }, d2, r.getTimeSeconds() };
  out.setTMax(tMax);

  return out;
}

template<FloatingArithmetic T>
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

template<FloatingArithmetic T>
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

template<FloatingArithmetic T>
constexpr Bounds<T, 3> Transform<T>::operator()(const Bounds<T, 3>& b) const noexcept
{
  Bounds<T, 3> out{};

  for (Int i{}; i < 8; ++i) out = Bounds<T, 3>::getUnion(out, (*this)(b.getCorner(i)));

  return out;
}

template<FloatingArithmetic T>
constexpr Point<T, 3> Transform<T>::applyInverse(const Point<T, 3>& p) const noexcept
{
  return Transform{ m_inverse, m_forward }(p);
}

template<FloatingArithmetic T>
constexpr Vector<T, 3> Transform<T>::applyInverse(const Vector<T, 3>& v) const noexcept
{
  return Transform{ m_inverse, m_forward }(v);
}

template<FloatingArithmetic T>
constexpr Normal<T> Transform<T>::applyInverse(const Normal<T>& n) const noexcept
{
  return Transform{ m_inverse, m_forward }(n);
}

template<FloatingArithmetic T>
constexpr Ray Transform<T>::applyInverse(const Ray& r) const noexcept
{
  return Transform{ m_inverse, m_forward }(r);
}

template<FloatingArithmetic T>
constexpr Bounds<T, 3> Transform<T>::applyInverse(const Bounds<T, 3>& b) const noexcept
{
  return Transform{ m_inverse, m_forward }(b);
}

template<FloatingArithmetic T>
constexpr Transform<T> Transform<T>::getInverseTransform() const noexcept
{
  return Transform{ m_inverse, m_forward };
}

template<FloatingArithmetic T>
constexpr Transform<T> Transform<T>::operator*(const Transform<T>& other) const noexcept
{
  return Transform<T>{ m_forward * other.m_forward, other.m_inverse * m_inverse };
}

template<FloatingArithmetic T>
constexpr const SquareMatrix<T, 4>& Transform<T>::get() const noexcept
{
  return m_forward;
}

template<FloatingArithmetic T>
constexpr const SquareMatrix<T, 4>& Transform<T>::getInv() const noexcept
{
  return m_inverse;
}

template<FloatingArithmetic T>
constexpr bool Transform<T>::swapsHandedness() const noexcept
{
  const auto& mat{ this->m_forward };

  const T A{ mat[0, 0] }; const T B{ mat[0, 1] }; const T C{ mat[0, 2] };

  const T D{ mat[1, 0] }; const T E{ mat[1, 1] }; const T F{ mat[1, 2] };
  
  const T G{ mat[2, 0] }; const T H{ mat[2, 1] }; const T I{ mat[2, 2] };

  const T EIFH{ differenceOfProducts(E, I, F, H) };
  const T DIFG{ differenceOfProducts(D, I, F, G) };
  const T DHEG{ differenceOfProducts(D, H, E, G) };

  const T det{ differenceOfProducts(A, EIFH, B, DIFG) + (C * DHEG) };

  return det < T(0);
}

template<FloatingArithmetic T>
Transform<T> Transform<T>::lookAt(const Point<T, 3>& eye, const Point<T, 3>& target, const Vector<T, 3>& upHint)
{
  // Decide the camera's forward direction based on the provided target and eye points, then choose a candidate up vector
  Vector<T, 3> f{ normalize(target - eye) };
  Vector<T, 3> up{ normalize(upHint) };

  // Test if forward and up cancel, in which case we flip and choose a different candidate up axis
  if (std::abs(computeDot(f, up)) > T{ 0.999 }) up = { T{0}, T{1}, T{0} };

  // Use the forward and up to decide the right axis, and then use the right and forward to get the actual up axis 
  Vector<T, 3> r{ normalize(computeCross(up, f)) };
  Vector<T, 3> u{ computeCross(f, r) };

  // Now we construct the camToWorld/worldFromCam matrix where each column encodes one of the three (r, u, f) coordinate axes
  SquareMatrix<T, 4> camToWorld
  { { 
      Vector<T,4>{ r[0], r[1], r[2], T{ 0 } }, 
      Vector<T,4>{ u[0], u[1], u[2], T{ 0 } }, 
      Vector<T,4>{ f[0], f[1], f[2], T{ 0 } }, 
      Vector<T,4>{ eye[0], eye[1], eye[2], T{ 1 } } 
  } };

  // Setup the dot product for the last column of the inverse matrix
  const auto& dotREye{ computeDot(r, eye) };
  const auto& dotUEye{ computeDot(u, eye) };
  const auto& dotFEye{ computeDot(f, eye) };

  // Since our coordinate transform is a rotation at heart, M^{-1} = M^T, the last column is simply -dot(R^t, e) giving us the inverse matrix
  SquareMatrix<T, 4> worldToCam
  { {
    Vector<T,4>{ r[0],  u[0],  f[0],  T{ 0 } },
    Vector<T,4>{ r[1],  u[1],  f[1],  T{ 0 } },
    Vector<T,4>{ r[2],  u[2],  f[2],  T{ 0 } },
    Vector<T,4>{ -dotREye, -dotUEye, -dotFEye, T{ 1 } }
  } };

  return Transform<T>{ camToWorld, worldToCam };
}

template<FloatingArithmetic T>
Transform<T> Transform<T>::translate(const Vector<T, 3>& v)
{
  SquareMatrix<T, 4> m{ { Vector<T, 4>{ 1, 0, 0, 0 }, Vector<T, 4>{ 0, 1, 0, 0 }, Vector<T, 4>{ 0, 0, 1, 0 }, Vector<T, 4>{ v[0], v[1], v[2], 1 } } };

  SquareMatrix<T, 4> mInv{ { Vector<T, 4>{ 1, 0, 0, 0 }, Vector<T, 4>{ 0, 1, 0, 0 }, Vector<T, 4>{ 0, 0, 1, 0 }, Vector<T, 4>{ -v[0], -v[1], -v[2], 1 } } };

  return { m, mInv };
}

template<FloatingArithmetic T>
Transform<T> Transform<T>::scale(const Vector<T, 3>& v)
{
  SquareMatrix<T, 4> m{ { Vector<T, 4>{ v[0], 0, 0, 0 }, Vector<T, 4>{ 0, v[1], 0, 0 }, Vector<T, 4>{ 0, 0, v[2], 0 }, Vector<T, 4>{ 0, 0, 0, 1 } } };

  SquareMatrix<T, 4> mInv{ { Vector<T, 4>{ 1 / v[0], 0, 0, 0 }, Vector<T, 4>{ 0, 1 / v[1], 0, 0 }, Vector<T, 4>{ 0, 0, 1 / v[2], 0 }, Vector<T, 4>{ 0, 0, 0, 1 } } };

  return { m, mInv };
}

template<FloatingArithmetic T>
Transform<T> Transform<T>::perspective(T fovDegrees, T nearPlane, T farPlane)
{
  const T s{ T{ 1 } / std::tan(degreesToRadians(fovDegrees) / T{ 2 }) };
  const T a{ farPlane / (farPlane - nearPlane) };
  const T b{ -(farPlane * nearPlane) / (farPlane - nearPlane) };

  const SquareMatrix<T, 4> perspectiveMat
  { { 
    Vector<T,4>{ 1, 0, 0, 0 }, 
    Vector<T,4>{ 0, 1, 0, 0 }, 
    Vector<T,4>{ 0, 0, a, 1 }, 
    Vector<T,4>{ 0, 0, b, 0 } 
  } };

  const SquareMatrix<T, 4> invPerspectiveMat
  { {
    Vector<T,4>{ 1, 0, 0, 0},
    Vector<T,4>{ 0, 1, 0, 0},
    Vector<T,4>{ 0, 0, 0, T{1} / b},
    Vector<T,4>{ 0, 0, T{1}, -a / b}
  } };
  
  return Transform<T>::scale({ s, s, 1 }) * Transform<T>{ perspectiveMat, invPerspectiveMat };
}

template<FloatingArithmetic T>
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

template<FloatingArithmetic T>
DecomposedTRS Transform<T>::decomposeTRS() const noexcept
{
  const Mat4f& matrix4x4{ get() };

  const Vec3f translation{
    matrix4x4[0, 3],
    matrix4x4[1, 3],
    matrix4x4[2, 3]
  };

  const Vec3f linearCol0{ matrix4x4[0, 0], matrix4x4[1, 0], matrix4x4[2, 0] };
  const Vec3f linearCol1{ matrix4x4[0, 1], matrix4x4[1, 1], matrix4x4[2, 1] };
  const Vec3f linearCol2{ matrix4x4[0, 2], matrix4x4[1, 2], matrix4x4[2, 2] };

  const Mat3f linear3x3{ { linearCol0, linearCol1, linearCol2 } };

  const Float eps{ kSafeNormalizeLen<Float> };

  Vec3f basisX{ normalizeSafe(linearCol0, eps) };
  if (isZero(euclideanLengthSq(basisX))) basisX = Vec3f{ Float{1}, Float{0}, Float{0} };

  const Float col1DotX{ computeDot(linearCol1, basisX) };
  Vec3f col1Orthogonal{ linearCol1 - (basisX * col1DotX) };

  Vec3f basisY{ normalizeSafe(col1Orthogonal, eps) };
  if (isZero(euclideanLengthSq(basisY))) basisY = Vec3f{ Float{0}, Float{1}, Float{0} };

  Vec3f basisZ{ computeCross(basisX, basisY) };
  if (isZero(euclideanLengthSq(basisZ))) basisZ = Vec3f{ Float{0}, Float{0}, Float{1} };

  const Mat3f rotation3x3{ { basisX, basisY, basisZ } };

  const Mat3f scale3x3{ rotation3x3.transpose() * linear3x3 };

  const auto quaternionFromRotation3x3 = [&](const Mat3f& rotation) noexcept -> Quaternion
    {
      const Float m00{ rotation[0, 0] }, m01{ rotation[0, 1] }, m02{ rotation[0, 2] };
      const Float m10{ rotation[1, 0] }, m11{ rotation[1, 1] }, m12{ rotation[1, 2] };
      const Float m20{ rotation[2, 0] }, m21{ rotation[2, 1] }, m22{ rotation[2, 2] };

      const Float trace{ m00 + m11 + m22 };

      Vec3f vectorPart{};
      Float scalarPart{};

      if (trace > Float{ 0 })
      {
        const Float fourW{ Float{2} * safeSqrt(trace + Float{1}) };
        scalarPart = fourW * Float{ 0.25 };

        const Float invFourW{ (fourW > Float{0}) ? (Float{1} / fourW) : Float{0} };
        vectorPart = Vec3f{
          (m21 - m12) * invFourW,
          (m02 - m20) * invFourW,
          (m10 - m01) * invFourW
        };
      }
      else if (m00 > m11 && m00 > m22)
      {
        const Float fourX{ Float{2} * safeSqrt(Float{1} + m00 - m11 - m22) };
        vectorPart[0] = fourX * Float{ 0.25 };

        const Float invFourX{ (fourX > Float{0}) ? (Float{1} / fourX) : Float{0} };
        vectorPart[1] = (m01 + m10) * invFourX;
        vectorPart[2] = (m02 + m20) * invFourX;
        scalarPart = (m21 - m12) * invFourX;
      }
      else if (m11 > m22)
      {
        const Float fourY{ Float{2} * safeSqrt(Float{1} + m11 - m00 - m22) };
        vectorPart[1] = fourY * Float{ 0.25 };

        const Float invFourY{ (fourY > Float{0}) ? (Float{1} / fourY) : Float{0} };
        vectorPart[0] = (m01 + m10) * invFourY;
        vectorPart[2] = (m12 + m21) * invFourY;
        scalarPart = (m02 - m20) * invFourY;
      }
      else
      {
        const Float fourZ{ Float{2} * safeSqrt(Float{1} + m22 - m00 - m11) };
        vectorPart[2] = fourZ * Float{ 0.25 };

        const Float invFourZ{ (fourZ > Float{0}) ? (Float{1} / fourZ) : Float{0} };
        vectorPart[0] = (m02 + m20) * invFourZ;
        vectorPart[1] = (m12 + m21) * invFourZ;
        scalarPart = (m10 - m01) * invFourZ;
      }

      return normalize(Quaternion{ vectorPart, scalarPart });
    };

  const Quaternion rotationQuat{ quaternionFromRotation3x3(rotation3x3) };

  return DecomposedTRS{ translation, rotationQuat, scale3x3 };
}