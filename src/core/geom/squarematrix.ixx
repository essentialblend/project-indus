export module indus.core.geom.squarematrix;

import indus.core.concepts;
import indus.core.types;
import indus.core.math.fp.ii;
import indus.core.geom.vector;

export template<Arithmetic T, std::size_t N> requires Arity234<N>
class SquareMatrix final
{
public:
  constexpr SquareMatrix() noexcept;
  constexpr SquareMatrix(const std::array<Vector<T, N>, N>& columns) noexcept;

  constexpr auto operator<=>(const SquareMatrix&) noexcept = delete;

  template <class Self>
  constexpr decltype(auto) operator[](this Self&& self, std::size_t row, std::size_t col) noexcept;

  constexpr SquareMatrix& operator+=(const SquareMatrix& rhs) noexcept;
  constexpr SquareMatrix& operator-=(const SquareMatrix& rhs) noexcept;

  constexpr SquareMatrix& operator*=(T scalar) noexcept;
  constexpr SquareMatrix& operator/=(T scalar) noexcept requires FloatingArithmetic<T>;

  constexpr SquareMatrix operator*(const SquareMatrix& rhs) const noexcept;
  constexpr Vector<T, N> operator*(const Vector<T, N>& v) const noexcept;

  static constexpr SquareMatrix zero() noexcept;
  static constexpr SquareMatrix identity() noexcept;

  constexpr SquareMatrix transpose() const noexcept;
  constexpr T determinant() const noexcept requires FloatingArithmetic<T>;
  constexpr SquareMatrix inverse() const noexcept requires FloatingArithmetic<T>;

private:
  std::array<T, N* N> m_elements{};
};


export using Mat2f = SquareMatrix<Float, 2>;
export using Mat3f = SquareMatrix<Float, 3>;
export using Mat4f = SquareMatrix<Float, 4>;

export using Mat2d = SquareMatrix<Float64, 2>;
export using Mat3d = SquareMatrix<Float64, 3>;
export using Mat4d = SquareMatrix<Float64, 4>;

export
{
  template<Arithmetic T, std::size_t N> requires Arity234<N>
  constexpr SquareMatrix<T, N> operator+(SquareMatrix<T, N> lhs, const SquareMatrix<T, N>& rhs) noexcept;

  template<Arithmetic T, std::size_t N> requires Arity234<N>
  constexpr SquareMatrix<T, N> operator-(SquareMatrix<T, N> lhs, const SquareMatrix<T, N>& rhs) noexcept;

  template<Arithmetic T, std::size_t N> requires Arity234<N>
  constexpr SquareMatrix<T, N> operator-(SquareMatrix<T, N> m) noexcept;

  template<Arithmetic T, std::size_t N> requires Arity234<N>
  constexpr SquareMatrix<T, N> operator*(SquareMatrix<T, N> m, T scalar) noexcept;

  template<Arithmetic T, std::size_t N> requires Arity234<N>
  constexpr SquareMatrix<T, N> operator*(T scalar, SquareMatrix<T, N> m) noexcept;

  template<FloatingArithmetic T, std::size_t N> requires Arity234<N>
  constexpr SquareMatrix<T, N> operator/(SquareMatrix<T, N> m, T scalar) noexcept;

  template<Arithmetic T, std::size_t N> requires Arity234<N>
  constexpr T maxAbsElementDiff(const SquareMatrix<T, N>& a, const SquareMatrix<T, N>& b) noexcept;
};

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N>::SquareMatrix() noexcept 
{
  for (std::size_t k{}; k < N * N; ++k) m_elements[k] = T{};
  for (std::size_t i{}; i < N; ++i) m_elements[i * N + i] = T{ 1 };
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N>::SquareMatrix(const std::array<Vector<T, N>, N>& cols) noexcept
{
  for (std::size_t i{}; i < N; ++i) 
  {
    for (std::size_t j{}; j < N; ++j) 
    {
      m_elements[i * N + j] = cols[j][i];
    }
  }
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
template <class Self>
constexpr decltype(auto) SquareMatrix<T, N>::operator[](this Self&& self, std::size_t i, std::size_t j) noexcept 
{
  return std::forward_like<Self>(self.m_elements)[i * N + j];
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N>& SquareMatrix<T, N>::operator+=(const SquareMatrix& rhs) noexcept
{
  for (std::size_t row{}; row < N; ++row)
    for (std::size_t col{}; col < N; ++col)
      (*this)[row, col] = (*this)[row, col] + rhs[row, col];

  return *this;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N>& SquareMatrix<T, N>::operator-=(const SquareMatrix& rhs) noexcept
{
  for (std::size_t row{}; row < N; ++row)
    for (std::size_t col{}; col < N; ++col)
      (*this)[row, col] = (*this)[row, col] - rhs[row, col];

  return *this;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N>& SquareMatrix<T, N>::operator*=(T scalar) noexcept
{
  for (std::size_t row{}; row < N; ++row)
    for (std::size_t col{}; col < N; ++col)
      (*this)[row, col] = (*this)[row, col] * scalar;

  return *this;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N>& SquareMatrix<T, N>::operator/=(T scalar) noexcept requires FloatingArithmetic<T>
{
  const T invScalar{ T{ 1 } / scalar };

  return (*this *= invScalar);
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr Vector<T, N> SquareMatrix<T, N>::operator*(const Vector<T, N>& v) const noexcept
{
  Vector<T, N> r{};

  for (std::size_t i{}; i < N; ++i)
  {
    T sum{};

    for (std::size_t j{}; j < N; ++j)
    {
      sum += (*this)[i, j] * v[j];
    }

    r[i] = sum;
  }

  return r;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> SquareMatrix<T, N>::operator*(const SquareMatrix<T, N>& rhs) const noexcept
{
  SquareMatrix<T, N> r{};

  for (std::size_t i{}; i < N; ++i) 
  {
    for (std::size_t j{}; j < N; ++j) 
    {
      T sum{};

      for (std::size_t k{}; k < N; ++k) 
      {
        sum += (*this)[i, k] * rhs[k, j];
      }

      r[i, j] = sum;
    }
  }

  return r;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N>
SquareMatrix<T, N>::transpose() const noexcept 
{
  SquareMatrix<T, N> r{};

  for (std::size_t i{}; i < N; ++i)
  {
    for (std::size_t j{}; j < N; ++j)
    {
      r[i, j] = (*this)[j, i];
    }
  }
  
  return r;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> SquareMatrix<T, N>::zero() noexcept 
{
  SquareMatrix<T, N> m{};

  for (std::size_t k{}; k < N * N; ++k)
    m.m_elements[k] = T{};
  
  return m;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> SquareMatrix<T, N>::identity() noexcept 
{
  return SquareMatrix<T, N>{};
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr T SquareMatrix<T, N>::determinant() const noexcept requires FloatingArithmetic<T>
{
  if constexpr (N == 2) 
  {
    const T a{ (*this)[0, 0] };
    const T b{ (*this)[0, 1] };
    const T c{ (*this)[1, 0] };
    const T d{ (*this)[1, 1] };

    return differenceOfProducts(a, d, b, c);
  }
  else if constexpr (N == 3)
  {
    const T a{ (*this)[0, 0] }; const T b{ (*this)[0, 1] };
    const T c{ (*this)[0, 2] }; const T d{ (*this)[1, 0] };
    const T e{ (*this)[1, 1] }; const T f{ (*this)[1, 2] };
    const T g{ (*this)[2, 0] }; const T h{ (*this)[2, 1] };
    const T i{ (*this)[2, 2] };
    
    const T M0{ differenceOfProducts(e, i, f, h) };
    const T M1{ differenceOfProducts(d, i, f, g) };
    const T M2{ differenceOfProducts(d, h, e, g) };
    
    T det{ fusedMultiplyAdd(a, M0, T{}) };
    
    det = fusedMultiplyAdd(-b, M1, det);
    det = fusedMultiplyAdd(c, M2, det);
    
    return det;
  }
  else
  {
    static_assert(N == 4);

    const T m00{ (*this)[0, 0] }; const T m01{ (*this)[0, 1] };
    const T m02{ (*this)[0, 2] }; const T m03{ (*this)[0, 3] };
    const T m10{ (*this)[1, 0] }; const T m11{ (*this)[1, 1] };
    const T m12{ (*this)[1, 2] }; const T m13{ (*this)[1, 3] };
    const T m20{ (*this)[2, 0] }; const T m21{ (*this)[2, 1] };
    const T m22{ (*this)[2, 2] }; const T m23{ (*this)[2, 3] };
    const T m30{ (*this)[3, 0] }; const T m31{ (*this)[3, 1] };
    const T m32{ (*this)[3, 2] }; const T m33{ (*this)[3, 3] };

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
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> SquareMatrix<T, N>::inverse() const noexcept requires FloatingArithmetic<T>
{
  if constexpr (N == 2)
  {
    const T det{ determinant() };
    const T invDet{ T{ 1 } / det };

    const T a{ (*this)[0, 0] }, b{ (*this)[0, 1] };
    const T c{ (*this)[1, 0] }, d{ (*this)[1, 1] };

    return SquareMatrix{ std::array<Vector<T, 2>, 2>{
      Vector<T, 2>{  d * invDet, -c * invDet },
      Vector<T, 2>{ -b * invDet,  a * invDet }
    } };
  }
  else if constexpr (N == 3)
  {
    const T det{ determinant() };
    const T invDet{ T{ 1 } / det };

    const T a00{ (*this)[0, 0] }, a01{ (*this)[0, 1] }, a02{ (*this)[0, 2] };
    const T a10{ (*this)[1, 0] }, a11{ (*this)[1, 1] }, a12{ (*this)[1, 2] };
    const T a20{ (*this)[2, 0] }, a21{ (*this)[2, 1] }, a22{ (*this)[2, 2] };

    const T c00{ differenceOfProducts(a11, a22, a12, a21) };
    const T c01{ differenceOfProducts(a02, a21, a01, a22) };
    const T c02{ differenceOfProducts(a01, a12, a02, a11) };

    const T c10{ differenceOfProducts(a12, a20, a10, a22) };
    const T c11{ differenceOfProducts(a00, a22, a02, a20) };
    const T c12{ differenceOfProducts(a02, a10, a00, a12) };

    const T c20{ differenceOfProducts(a10, a21, a11, a20) };
    const T c21{ differenceOfProducts(a01, a20, a00, a21) };
    const T c22{ differenceOfProducts(a00, a11, a01, a10) };

    return SquareMatrix{ std::array<Vector<T, 3>, 3>{
      Vector<T, 3>{ c00 * invDet, c10 * invDet, c20 * invDet },
      Vector<T, 3>{ c01 * invDet, c11 * invDet, c21 * invDet },
      Vector<T, 3>{ c02 * invDet, c12 * invDet, c22 * invDet }
    } };
  }
  else
  {
    static_assert(N == 4);

    const T det{ determinant() };
    const T invDet{ T{ 1 } / det };

    SquareMatrix adj{ SquareMatrix::zero() };

    for (std::size_t row{}; row < 4; ++row)
    {
      for (std::size_t col{}; col < 4; ++col)
      {
        SquareMatrix<T, 3> minor{ SquareMatrix<T, 3>::zero() };

        std::size_t minorRow{};
        for (std::size_t r{}; r < 4; ++r)
        {
          if (r == row) continue;

          std::size_t minorCol{};
          for (std::size_t c{}; c < 4; ++c)
          {
            if (c == col) continue;
            minor[minorRow, minorCol] = (*this)[r, c];
            ++minorCol;
          }

          ++minorRow;
        }

        const T cofactor{ ((row + col) & 1) ? -minor.determinant() : minor.determinant() };
        adj[col, row] = cofactor;
      }
    }

    adj *= invDet;

    return adj;
  }
}

// Free functions

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> operator+(SquareMatrix<T, N> lhs, const SquareMatrix<T, N>& rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> operator-(SquareMatrix<T, N> lhs, const SquareMatrix<T, N>& rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> operator-(SquareMatrix<T, N> m) noexcept
{
  m *= T{ -1 };
  return m;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> operator*(SquareMatrix<T, N> m, T scalar) noexcept
{
  m *= scalar;
  return m;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> operator*(T scalar, SquareMatrix<T, N> m) noexcept
{
  m *= scalar;
  return m;
}

template<FloatingArithmetic T, std::size_t N> requires Arity234<N>
constexpr SquareMatrix<T, N> operator/(SquareMatrix<T, N> m, T scalar) noexcept
{
  m /= scalar;
  return m;
}

template<Arithmetic T, std::size_t N> requires Arity234<N>
constexpr T maxAbsElementDiff(const SquareMatrix<T, N>& a, const SquareMatrix<T, N>& b) noexcept
{
  T maxDelta{};

  for (std::size_t row{}; row < N; ++row)
  {
    for (std::size_t col{}; col < N; ++col)
    {
      const T delta{ std::abs(a[row, col] - b[row, col]) };
      maxDelta = std::max(maxDelta, delta);
    }
  }

  return maxDelta;
}