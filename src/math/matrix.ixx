export module matrix;

import std;
import vector;
import point;
import concepts;
import types;
import mathfp;

export template<Arithmetic T>
class Matrix4 final
{
public:
  constexpr Matrix4() noexcept;
  constexpr Matrix4(const Vector<T, 4>&, const Vector<T, 4>&, const Vector<T, 4>&, const Vector<T, 4>&) noexcept;

  constexpr Matrix4(const Matrix4&) noexcept = default;
  constexpr Matrix4(Matrix4&&) noexcept = default;

  constexpr Matrix4& operator=(const Matrix4&) noexcept = default;
  constexpr Matrix4& operator=(Matrix4&&) noexcept = default;

  constexpr auto operator<=>(const Matrix4&) noexcept = delete;

  constexpr const T& operator[](std::size_t, std::size_t) const & noexcept;
  constexpr T& operator[](std::size_t, std::size_t) & noexcept;

  constexpr Matrix4<T> operator*(const Matrix4<T>&) const noexcept;
  constexpr Vector<T, 4> operator*(const Vector<T, 4>&) const noexcept;

  static constexpr Matrix4 zero() noexcept;
  static constexpr Matrix4 identity() noexcept;
  constexpr Matrix4<T> transpose() const noexcept;
  constexpr Matrix4<T> inverse() const;
  constexpr T determinant() const noexcept;

  ~Matrix4() noexcept = default;

private:
  std::array<T, 16> m_elements{};
};

// Type alias for Matrix resides here to break cyclicity
export using Mat4f = Matrix4<Float>;

// Implementation

template<Arithmetic T>
constexpr Matrix4<T>::Matrix4() noexcept
{
  m_elements[0] = 1;
  m_elements[5] = 1;
  m_elements[10] = 1;
  m_elements[15] = 1;
}

template<Arithmetic T>
constexpr Matrix4<T>::Matrix4(const Vector<T, 4>& first, const Vector<T, 4>& second, const Vector<T, 4>& third, const Vector<T, 4>& fourth) noexcept
{
  m_elements =
  {
    first[0], second[0], third[0], fourth[0],
    first[1], second[1], third[1], fourth[1],
    first[2], second[2], third[2], fourth[2],
    first[3], second[3], third[3], fourth[3]
  };
}

template<Arithmetic T>
constexpr const T& Matrix4<T>::operator[](std::size_t row, std::size_t col) const & noexcept
{
  return m_elements[row * 4 + col];
}

template<Arithmetic T>
constexpr T& Matrix4<T>::operator[](std::size_t row, std::size_t col) & noexcept
{
  return const_cast<T&>(std::as_const(*this)[row, col]);
}

template<Arithmetic T>
constexpr Matrix4<T> Matrix4<T>::operator*(const Matrix4<T>& mat) const noexcept
{
  Matrix4<T> r{};

  for (std::size_t i{}; i < 4; ++i)
  {
    for (std::size_t j{}; j < 4; ++j)
    {
      const T a0{ (*this)[i, 0] }; const T a1{ (*this)[i, 1] };
      const T a2{ (*this)[i, 2] }; const T a3{ (*this)[i, 3] };
      const T b0{ mat[0, j] }; const T b1{ mat[1, j] };
      const T b2{ mat[2, j] }; const T b3{ mat[3, j] };

      T t{ a0 * b0 };

      t = fusedMultiplyAdd(a1, b1, t);
      t = fusedMultiplyAdd(a2, b2, t);
      t = fusedMultiplyAdd(a3, b3, t);
      
      r[i, j] = t;
    }
  }
  return r;
}

template<Arithmetic T>
constexpr Vector<T, 4> Matrix4<T>::operator*(const Vector<T, 4>& v) const noexcept
{
  Vector<T, 4> result{};

  for (std::size_t i{}; i < 4; ++i)
  {
    T t{ (*this)[i, 0] * v[0] };

    t = fusedMultiplyAdd((*this)[i, 1], v[1], t);
    t = fusedMultiplyAdd((*this)[i, 2], v[2], t);
    t = fusedMultiplyAdd((*this)[i, 3], v[3], t);
    
    result[i] = t;
  }

  return result;
}

template<Arithmetic T>
constexpr Matrix4<T> Matrix4<T>::identity() noexcept
{
  Matrix4 m{};

  m.m_elements[0] = 1;
  m.m_elements[5] = 1;
  m.m_elements[10] = 1;
  m.m_elements[15] = 1;

  return m;
}

template<Arithmetic T>
constexpr Matrix4<T> Matrix4<T>::zero() noexcept
{
  Matrix4 m{};
  m.m_elements.fill(T{ 0 });
  return m;
}

template<Arithmetic T>
constexpr Matrix4<T> Matrix4<T>::transpose() const noexcept
{
  Matrix4<T> resultMatrix{};

  for (std::size_t r{}; r < 4; ++r)
  {
    for (std::size_t c{}; c < 4; ++c)
    {
      resultMatrix[r, c] = (*this)[c, r];
    }
  }

  return resultMatrix;
}

template<Arithmetic T>
constexpr Matrix4<T> Matrix4<T>::inverse() const
{
  Matrix4 inv{};

  inv[0, 0] = (*this)[1, 1] * (*this)[2, 2] * (*this)[3, 3]
    - (*this)[1, 1] * (*this)[2, 3] * (*this)[3, 2]
    - (*this)[2, 1] * (*this)[1, 2] * (*this)[3, 3]
    + (*this)[2, 1] * (*this)[1, 3] * (*this)[3, 2]
    + (*this)[3, 1] * (*this)[1, 2] * (*this)[2, 3]
    - (*this)[3, 1] * (*this)[1, 3] * (*this)[2, 2];

  inv[0, 1] = -(*this)[0, 1] * (*this)[2, 2] * (*this)[3, 3]
    + (*this)[0, 1] * (*this)[2, 3] * (*this)[3, 2]
    + (*this)[2, 1] * (*this)[0, 2] * (*this)[3, 3]
    - (*this)[2, 1] * (*this)[0, 3] * (*this)[3, 2]
    - (*this)[3, 1] * (*this)[0, 2] * (*this)[2, 3]
    + (*this)[3, 1] * (*this)[0, 3] * (*this)[2, 2];

  inv[0, 2] = (*this)[0, 1] * (*this)[1, 2] * (*this)[3, 3]
    - (*this)[0, 1] * (*this)[1, 3] * (*this)[3, 2]
    - (*this)[1, 1] * (*this)[0, 2] * (*this)[3, 3]
    + (*this)[1, 1] * (*this)[0, 3] * (*this)[3, 2]
    + (*this)[3, 1] * (*this)[0, 2] * (*this)[1, 3]
    - (*this)[3, 1] * (*this)[0, 3] * (*this)[1, 2];

  inv[0, 3] = -(*this)[0, 1] * (*this)[1, 2] * (*this)[2, 3]
    + (*this)[0, 1] * (*this)[1, 3] * (*this)[2, 2]
    + (*this)[1, 1] * (*this)[0, 2] * (*this)[2, 3]
    - (*this)[1, 1] * (*this)[0, 3] * (*this)[2, 2]
    - (*this)[2, 1] * (*this)[0, 2] * (*this)[1, 3]
    + (*this)[2, 1] * (*this)[0, 3] * (*this)[1, 2];

  inv[1, 0] = -(*this)[1, 0] * (*this)[2, 2] * (*this)[3, 3]
    + (*this)[1, 0] * (*this)[2, 3] * (*this)[3, 2]
    + (*this)[2, 0] * (*this)[1, 2] * (*this)[3, 3]
    - (*this)[2, 0] * (*this)[1, 3] * (*this)[3, 2]
    - (*this)[3, 0] * (*this)[1, 2] * (*this)[2, 3]
    + (*this)[3, 0] * (*this)[1, 3] * (*this)[2, 2];

  inv[1, 1] = (*this)[0, 0] * (*this)[2, 2] * (*this)[3, 3]
    - (*this)[0, 0] * (*this)[2, 3] * (*this)[3, 2]
    - (*this)[2, 0] * (*this)[0, 2] * (*this)[3, 3]
    + (*this)[2, 0] * (*this)[0, 3] * (*this)[3, 2]
    + (*this)[3, 0] * (*this)[0, 2] * (*this)[2, 3]
    - (*this)[3, 0] * (*this)[0, 3] * (*this)[2, 2];

  inv[1, 2] = -(*this)[0, 0] * (*this)[1, 2] * (*this)[3, 3]
    + (*this)[0, 0] * (*this)[1, 3] * (*this)[3, 2]
    + (*this)[1, 0] * (*this)[0, 2] * (*this)[3, 3]
    - (*this)[1, 0] * (*this)[0, 3] * (*this)[3, 2]
    - (*this)[3, 0] * (*this)[0, 2] * (*this)[1, 3]
    + (*this)[3, 0] * (*this)[0, 3] * (*this)[1, 2];

  inv[1, 3] = (*this)[0, 0] * (*this)[1, 2] * (*this)[2, 3]
    - (*this)[0, 0] * (*this)[1, 3] * (*this)[2, 2]
    - (*this)[1, 0] * (*this)[0, 2] * (*this)[2, 3]
    + (*this)[1, 0] * (*this)[0, 3] * (*this)[2, 2]
    + (*this)[2, 0] * (*this)[0, 2] * (*this)[1, 3]
    - (*this)[2, 0] * (*this)[0, 3] * (*this)[1, 2];

  inv[2, 0] = (*this)[1, 0] * (*this)[2, 1] * (*this)[3, 3]
    - (*this)[1, 0] * (*this)[2, 3] * (*this)[3, 1]
    - (*this)[2, 0] * (*this)[1, 1] * (*this)[3, 3]
    + (*this)[2, 0] * (*this)[1, 3] * (*this)[3, 1]
    + (*this)[3, 0] * (*this)[1, 1] * (*this)[2, 3]
    - (*this)[3, 0] * (*this)[1, 3] * (*this)[2, 1];

  inv[2, 1] = -(*this)[0, 0] * (*this)[2, 1] * (*this)[3, 3]
    + (*this)[0, 0] * (*this)[2, 3] * (*this)[3, 1]
    + (*this)[2, 0] * (*this)[0, 1] * (*this)[3, 3]
    - (*this)[2, 0] * (*this)[0, 3] * (*this)[3, 1]
    - (*this)[3, 0] * (*this)[0, 1] * (*this)[2, 3]
    + (*this)[3, 0] * (*this)[0, 3] * (*this)[2, 1];

  inv[2, 2] = (*this)[0, 0] * (*this)[1, 1] * (*this)[3, 3]
    - (*this)[0, 0] * (*this)[1, 3] * (*this)[3, 1]
    - (*this)[1, 0] * (*this)[0, 1] * (*this)[3, 3]
    + (*this)[1, 0] * (*this)[0, 3] * (*this)[3, 1]
    + (*this)[3, 0] * (*this)[0, 1] * (*this)[1, 3]
    - (*this)[3, 0] * (*this)[0, 3] * (*this)[1, 1];

  inv[2, 3] = -(*this)[0, 0] * (*this)[1, 1] * (*this)[2, 3]
    + (*this)[0, 0] * (*this)[1, 3] * (*this)[2, 1]
    + (*this)[1, 0] * (*this)[0, 1] * (*this)[2, 3]
    - (*this)[1, 0] * (*this)[0, 3] * (*this)[2, 1]
    - (*this)[2, 0] * (*this)[0, 1] * (*this)[1, 3]
    + (*this)[2, 0] * (*this)[0, 3] * (*this)[1, 1];

  inv[3, 0] = -(*this)[1, 0] * (*this)[2, 1] * (*this)[3, 2]
    + (*this)[1, 0] * (*this)[2, 2] * (*this)[3, 1]
    + (*this)[2, 0] * (*this)[1, 1] * (*this)[3, 2]
    - (*this)[2, 0] * (*this)[1, 2] * (*this)[3, 1]
    - (*this)[3, 0] * (*this)[1, 1] * (*this)[2, 2]
    + (*this)[3, 0] * (*this)[1, 2] * (*this)[2, 1];

  inv[3, 1] = (*this)[0, 0] * (*this)[2, 1] * (*this)[3, 2]
    - (*this)[0, 0] * (*this)[2, 2] * (*this)[3, 1]
    - (*this)[2, 0] * (*this)[0, 1] * (*this)[3, 2]
    + (*this)[2, 0] * (*this)[0, 2] * (*this)[3, 1]
    + (*this)[3, 0] * (*this)[0, 1] * (*this)[2, 2]
    - (*this)[3, 0] * (*this)[0, 2] * (*this)[2, 1];

  inv[3, 2] = -(*this)[0, 0] * (*this)[1, 1] * (*this)[3, 2]
    + (*this)[0, 0] * (*this)[1, 2] * (*this)[3, 1]
    + (*this)[1, 0] * (*this)[0, 1] * (*this)[3, 2]
    - (*this)[1, 0] * (*this)[0, 2] * (*this)[3, 1]
    - (*this)[3, 0] * (*this)[0, 1] * (*this)[1, 2]
    + (*this)[3, 0] * (*this)[0, 2] * (*this)[1, 1];

  inv[3, 3] = (*this)[0, 0] * (*this)[1, 1] * (*this)[2, 2]
    - (*this)[0, 0] * (*this)[1, 2] * (*this)[2, 1]
    - (*this)[1, 0] * (*this)[0, 1] * (*this)[2, 2]
    + (*this)[1, 0] * (*this)[0, 2] * (*this)[2, 1]
    + (*this)[2, 0] * (*this)[0, 1] * (*this)[1, 2]
    - (*this)[2, 0] * (*this)[0, 2] * (*this)[1, 1];

  T det = (*this)[0, 0] * inv[0, 0] + (*this)[0, 1] * inv[1, 0] + (*this)[0, 2] * inv[2, 0] + (*this)[0, 3] * inv[3, 0];

  if (det == T{ 0 })
  {
    #if defined(_MSC_VER)
      __assume(false);
    #else
      __builtin_unreachable();
    #endif
  }

  T invDet = T{ 1 } / det;
  for (auto& e : inv.m_elements) e *= invDet;

  return inv;
}

template<Arithmetic T>
constexpr T Matrix4<T>::determinant() const noexcept
{
  const auto& m = m_elements;

  const T m00{ m[0] }; const T m01{ m[1] }; const T m02{ m[2] }; const T m03{ m[3] };
  const T m10{ m[4] }; const T m11{ m[5] }; const T m12{ m[6] }; const T m13{ m[7] };
  const T m20{ m[8] }; const T m21{ m[9] }; const T m22{ m[10] }; const T m23{ m[11] };
  const T m30{ m[12] }; const T m31{ m[13] }; const T m32{ m[14] }; const T m33{ m[15] };

  const T s0{ differenceOfProducts(m00, m11, m10, m01) };
  const T s1{ differenceOfProducts(m00, m12, m10, m02) };
  const T s2{ differenceOfProducts(m00, m13, m10, m03) };
  const T s3{ differenceOfProducts(m01, m12, m11, m02) };
  const T s4{ differenceOfProducts(m01, m13, m11, m03) };
  const T s5{ differenceOfProducts(m02, m13, m12, m03) };

  const T c0{ differenceOfProducts(m20, m31, m30, m21) };
  const T c1{ differenceOfProducts(m20, m32, m30, m22) };
  const T c2{ differenceOfProducts(m20, m33, m30, m23) };
  const T c3{ differenceOfProducts(m21, m32, m31, m22) };
  const T c4{ differenceOfProducts(m21, m33, m31, m23) };
  const T c5{ differenceOfProducts(m22, m33, m32, m23) };

  T det{ differenceOfProducts(s0, c5, s1, c4) };
  
  det = sumOfProducts(det, T{ 1 }, s2, c3);
  det = sumOfProducts(det, T{ 1 }, s3, c2);
  det = differenceOfProducts(det, T{ 1 }, s4, c1);
  det = sumOfProducts(det, T{ 1 }, s5, c0);
  
  return det;
}



