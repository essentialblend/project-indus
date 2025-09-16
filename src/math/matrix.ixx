export module matrix;

import std;
import vector;
import point;
import concepts;

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
  Matrix4<T> resultMatrix{};

  for (std::size_t r{}; r < 4; ++r)
  {
    for (std::size_t c{}; c < 4; ++c)
    {
      resultMatrix[r, c] = T{ 0 };

      for (std::size_t k{}; k < 4; ++k)
      {
        resultMatrix[r, c] += (*this)[r, k] * mat[k, c];
      }
    }
  }
  return resultMatrix;
}

template<Arithmetic T>
constexpr Vector<T, 4> Matrix4<T>::operator*(const Vector<T, 4>& v) const noexcept
{
  Vector<T, 4> result{};

  for (std::size_t i{}; i < 4; ++i)
  {
    result[i] = (*this)[i, 0] * v[0] + (*this)[i, 1] * v[1] + (*this)[i, 2] * v[2] + (*this)[i, 3] * v[3];
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

  return m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) - m[9] * (m[6] * m[15] - m[7] * m[14]) + m[13] * (m[6] * m[11] - m[7] * m[10])) - m[1] * (m[4] * (m[10] * m[15] - m[11] * m[14]) - m[8] * (m[6] * m[15] - m[7] * m[14]) + m[12] * (m[6] * m[11] - m[7] * m[10])) + m[2] * (m[4] * (m[9] * m[15] - m[11] * m[13]) - m[8] * (m[5] * m[15] - m[7] * m[13]) + m[12] * (m[5] * m[11] - m[7] * m[9])) - m[3] * (m[4] * (m[9] * m[14] - m[10] * m[13]) - m[8] * (m[5] * m[14] - m[6] * m[13]) + m[12] * (m[5] * m[10] - m[6] * m[9]));
}



