export module transform;

import std;
import concepts;
import matrix;
import vector;
import point;

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

  constexpr Vector<T, 3> operator()(const Vector<T, 3>&) const;
  constexpr Point<T, 3> operator()(const Point<T, 3>&) const;

  constexpr Transform operator*(const Transform&) const noexcept;

  constexpr const Matrix4<T>& get() const noexcept;
  constexpr const Matrix4<T>& getInv() const noexcept;

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

// Implementation
template<Arithmetic T>
constexpr Transform<T>::Transform(const Matrix4<T>& forward, const Matrix4<T>& inverse) noexcept : m_forward{ forward }, m_inverse{ inverse } {}

template<Arithmetic T>
constexpr Transform<T>::Transform(const Matrix4<T>& forward) noexcept : m_forward{ forward }, m_inverse{ forward.inverse() } {}

template<Arithmetic T>
constexpr Vector<T, 3> Transform<T>::operator()(const Vector<T, 3>& v) const
{
  Vector<T, 4> hv(v[0], v[1], v[2], T{ 0 });
  Vector<T, 4> res = m_forward * hv;
  return { res[0], res[1], res[2] };
}

template<Arithmetic T>
constexpr Point<T, 3> Transform<T>::operator()(const Point<T, 3>& p) const
{
  Vector<T, 4> hp(p[0], p[1], p[2], T{ 1 });
  Vector<T, 4> res{ m_forward * hp };

  if (res[3] != T{ 0 } && res[3] != T{ 1 })
  {
    T invW = T{ 1 } / res[3];
    return { res[0] * invW, res[1] * invW, res[2] * invW };
  }

  return { res[0], res[1], res[2] };
}

template<Arithmetic T>
constexpr Transform<T> Transform<T>::operator*(const Transform<T>& other) const noexcept
{
  return Transform<T>{ m_forward* other.m_forward, other.m_inverse* m_inverse };
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
Transform<T> Transform<T>::lookAt(const Point<T, 3>& eye, const Point<T, 3>& target, const Vector<T, 3>& upHint)
{
  // Create an ONB around the camera
  Vector<T, 3> forward = normalize(target - eye);
  Vector<T, 3> up = normalize(upHint);

  if (std::abs(computeDot(forward, up)) > T{ 0.999 }) up = Vector<T, 3>{ 0,1,0 };

  Vector<T, 3> right = normalize(computeCross(up, forward));
  up = computeCross(forward, right);

  Vector<T, 3> eyeVec = eye - Point<T, 3>{0, 0, 0};

  Matrix4<T> worldToCam(
    Vector<T, 4>{ right[0], up[0], forward[0], 0 },
    Vector<T, 4>{ right[1], up[1], forward[1], 0 },
    Vector<T, 4>{ right[2], up[2], forward[2], 0 },
    Vector<T, 4>{ -computeDot(right, eyeVec), -computeDot(up, eyeVec), -computeDot(forward, eyeVec), 1 }
  );


  Matrix4<T> camToWorld = worldToCam.inverse();
  return Transform<T>(camToWorld, worldToCam);
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
  T fovRadians{ fovDegrees * (std::numbers::pi_v<T> / 180) };
  T invTan{ T{1} / std::tan(fovRadians / T{2}) };

  const Matrix4<T> perspective
  {
    Vector<T, 4>{1, 0, 0, 0},
    Vector<T, 4>{0, 1, 0, 0},
    Vector<T, 4>{0, 0, farPlane / (farPlane - nearPlane), 1},
    Vector<T, 4>{0, 0, -farPlane * nearPlane / (farPlane - nearPlane), 0}
  };

  return Transform<T>::scale({ invTan, invTan, 1 }) * Transform<T>{ perspective };
}
