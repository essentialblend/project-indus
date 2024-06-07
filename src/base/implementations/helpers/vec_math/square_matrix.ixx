export module square_matrix;

import <span>;
import <string>;
import <compare>;
import <type_traits>;
import <intrin.h>;
import <array>;
import <sstream>;
import <optional>;
import <cmath>;

import h_miscutilities;
import h_mathutilities;

export template <typename T, unsigned int N>
class alignas(32) SquareMatrix {
public:
    constexpr explicit SquareMatrix() noexcept;
    constexpr explicit SquareMatrix(const T(&mat)[N][N]) noexcept;
    constexpr explicit SquareMatrix(const std::span<const T>&) noexcept;
    constexpr SquareMatrix(std::initializer_list<std::initializer_list<T>>) noexcept;
    constexpr SquareMatrix(const SquareMatrix&) noexcept = default;
    constexpr SquareMatrix(SquareMatrix&&) noexcept = default;
    constexpr SquareMatrix& operator=(const SquareMatrix&) noexcept = default;
    constexpr SquareMatrix& operator=(SquareMatrix&&) noexcept = default;

    constexpr std::span<const T> operator[](const unsigned int) const noexcept;
    constexpr std::span<T> operator[](const unsigned int) noexcept;
    constexpr std::partial_ordering operator<=>(const SquareMatrix&) const noexcept = default;

    constexpr SquareMatrix operator+(const SquareMatrix&) const noexcept;
    constexpr SquareMatrix operator-(const SquareMatrix&) const noexcept;
    constexpr SquareMatrix operator*(const T&) const noexcept;
    constexpr SquareMatrix operator/(const T&) const noexcept;
    constexpr SquareMatrix& operator+=(const SquareMatrix&) noexcept;
    constexpr SquareMatrix& operator-=(const SquareMatrix&) noexcept;
    constexpr SquareMatrix& operator*=(const T&) noexcept;
    constexpr SquareMatrix& operator/=(const T&) noexcept;

    constexpr bool isIdentityMatrix() const noexcept;
    std::string toString() const;

    constexpr void prefetchForCacheUtilization() const noexcept;

    ~SquareMatrix() noexcept = default;

private:
    T m_memberMatrix[N][N];
};

// Non-member function prototypes.
export template <typename T, unsigned int N>
constexpr SquareMatrix<T, N> operator*(const T&, const SquareMatrix<T, N>&) noexcept;

export template <typename ResultType, typename T, unsigned int N>
constexpr ResultType multiplyMatrixByVector(const SquareMatrix<T, N>&, const ResultType&) noexcept;

export template <typename T, unsigned int N>
constexpr T computeDeterminant(const SquareMatrix<T, N>& matrix) noexcept;

export template <typename T, unsigned int N>
constexpr SquareMatrix<T, N> transposeMatrix(const SquareMatrix<T, N>&) noexcept;

export template <typename T, unsigned int N>
constexpr std::optional<SquareMatrix<T, N>> getInverseMatrix(const SquareMatrix<T, N>& matrix) noexcept;

export template <typename T, unsigned int N>
constexpr SquareMatrix<T, N> invertOrExit(const SquareMatrix<T, N>&) noexcept;

export template<typename T, unsigned int N>
constexpr SquareMatrix<T, N> operator*(const SquareMatrix<T, N>&, const SquareMatrix<T, N>&) noexcept;


// SquareMatrix member function implementations. Need to be in the interface as its templated.
template <typename T, unsigned int N>
constexpr SquareMatrix<T, N>::SquareMatrix() noexcept {
    for (unsigned int i = 0; i < N; ++i) {
        for (unsigned int j = 0; j < N; ++j) {
            m_memberMatrix[i][j] = (i == j) ? 1 : 0;
        }
    }
}

template <typename T, unsigned int N>
constexpr SquareMatrix<T, N>::SquareMatrix(const T(&mat)[N][N]) noexcept {
    for (unsigned int i = 0; i < N; ++i) {
        for (unsigned int j = 0; j < N; ++j) {
            m_memberMatrix[i][j] = mat[i][j];
        }
    }
}

template <typename T, unsigned int N>
constexpr SquareMatrix<T, N>::SquareMatrix(const std::span<const T>& t) noexcept {
    for (unsigned int i = 0; i < N * N; ++i) {
        m_memberMatrix[i / N][i % N] = t[i];
    }
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N>::SquareMatrix(std::initializer_list<std::initializer_list<T>> init) noexcept
{
    size_t row = 0;
    for (const auto& r : init) {
        std::copy(r.begin(), r.end(), m_memberMatrix[row]);
        ++row;
    }
}

template <typename T, unsigned int N>
constexpr std::span<const T> SquareMatrix<T, N>::operator[](const unsigned int i) const noexcept {
    return std::span<const T>(m_memberMatrix[i], N);
}

template <typename T, unsigned int N>
constexpr std::span<T> SquareMatrix<T, N>::operator[](const unsigned int i) noexcept {
    return std::span<T>(m_memberMatrix[i], N);
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T,N> SquareMatrix<T, N>::operator+(const SquareMatrix& other) const noexcept
{
    SquareMatrix<T, N> result{ *this };
    result += other;
    return result;
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N> SquareMatrix<T, N>::operator-(const SquareMatrix& other) const noexcept
{
    SquareMatrix<T, N> result{ *this };
    result -= other;
    return result;
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N> SquareMatrix<T, N>::operator*(const T& scalar) const noexcept
{
    SquareMatrix<T, N> result{ *this };
    result *= scalar;
    return result;
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N> SquareMatrix<T, N>::operator/(const T& scalar) const noexcept
{
    SquareMatrix<T, N> result{ *this };
    result /= scalar;
    return result;
}

template <typename T, unsigned int N>
constexpr SquareMatrix<T, N>& SquareMatrix<T, N>::operator+=(const SquareMatrix& other) noexcept 
{
    if constexpr (N > 4) {
        prefetchForCacheUtilization();
        other.prefetchForCacheUtilization();
    }

    if constexpr (std::is_same_v<T, float> && N == 4) {
        if (HIsAVX2Enabled()) {
            for (unsigned int i = 0; i < N; ++i) {
                __m128 row1 = _mm_load_ps(&m_memberMatrix[i][0]);
                __m128 row2 = _mm_load_ps(&other.m_memberMatrix[i][0]);
                row1 = _mm_add_ps(row1, row2);
                _mm_store_ps(&m_memberMatrix[i][0], row1);
            }
            return *this;
        }
    }
    if constexpr (std::is_same_v<T, double> && N == 4) {
        if (HIsAVX2Enabled()) {
            for (unsigned int i = 0; i < N; ++i) {
                __m256d row1 = _mm256_load_pd(&m_memberMatrix[i][0]);
                __m256d row2 = _mm256_load_pd(&other.m_memberMatrix[i][0]);
                row1 = _mm256_add_pd(row1, row2);
                _mm256_store_pd(&m_memberMatrix[i][0], row1);
            }
            return *this;
        }
    }

    for (unsigned int i = 0; i < N; ++i) {
        for (unsigned int j = 0; j < N; ++j) {
            m_memberMatrix[i][j] += other.m_memberMatrix[i][j];
        }
    }
    return *this;
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N>& SquareMatrix<T, N>::operator-=(const SquareMatrix& other) noexcept
{
    if constexpr (N > 4) {
        prefetchForCacheUtilization();
        other.prefetchForCacheUtilization();
    }

    if constexpr (std::is_same_v<T, float> && N == 4) {
        if (HIsAVX2Enabled()) {
            for (unsigned int i = 0; i < N; ++i) {
                __m128 row1 = _mm_load_ps(&m_memberMatrix[i][0]);
                __m128 row2 = _mm_load_ps(&other.m_memberMatrix[i][0]);
                row1 = _mm_sub_ps(row1, row2);
                _mm_store_ps(&m_memberMatrix[i][0], row1);
            }
            return *this;
        }
    }
    if constexpr (std::is_same_v<T, double> && N == 4) {
        if (HIsAVX2Enabled()) {
            for (unsigned int i = 0; i < N; ++i) {
                __m256d row1 = _mm256_load_pd(&m_memberMatrix[i][0]);
                __m256d row2 = _mm256_load_pd(&other.m_memberMatrix[i][0]);
                row1 = _mm256_sub_pd(row1, row2);
                _mm256_store_pd(&m_memberMatrix[i][0], row1);
            }
            return *this;
        }
    }

    for (unsigned int i = 0; i < N; ++i) {
        for (unsigned int j = 0; j < N; ++j) {
            m_memberMatrix[i][j] -= other.m_memberMatrix[i][j];
        }
    }
    return *this;
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N>& SquareMatrix<T, N>::operator*=(const T& scalar) noexcept
{
    if constexpr (std::is_same_v<T, float> && N == 4) {
        if (HIsAVX2Enabled()) {
            __m128 scalarVector = _mm_set1_ps(scalar);
            for (unsigned int i = 0; i < N; ++i) {
                __m128 row = _mm_load_ps(&m_memberMatrix[i][0]);
                row = _mm_mul_ps(row, scalarVector);
                _mm_store_ps(&m_memberMatrix[i][0], row);
            }
            return *this;
        }
    }

    if constexpr (std::is_same_v<T, double> && N == 4) {
        if (HIsAVX2Enabled()) {
            __m256d scalarVector = _mm256_set1_pd(scalar);
            for (unsigned int i = 0; i < N; ++i) {
                __m256d row = _mm256_load_pd(&m_memberMatrix[i][0]);
                row = _mm256_mul_pd(row, scalarVector);
                _mm256_store_pd(&m_memberMatrix[i][0], row);
            }
            return *this;
        }
    }

    for (unsigned int i = 0; i < N; ++i) {
        for (unsigned int j = 0; j < N; ++j) {
            m_memberMatrix[i][j] *= scalar;
        }
    }
    return *this;
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N>& SquareMatrix<T, N>::operator/=(const T& scalar) noexcept
{
    // Returning 0 for graceful handling.
    if (scalar == 0) {

        return SquareMatrix<T>{};
    }

    if constexpr (N > 4) {
        prefetchForCacheUtilization();
    }

    if constexpr (std::is_same_v<T, float> && N == 4) {
        if (HIsAVX2Enabled()) {
            __m128 scalarVector = _mm_set1_ps(scalar);
            for (unsigned int i = 0; i < N; ++i) {
                __m128 row = _mm_load_ps(&m_memberMatrix[i][0]);
                row = _mm_div_ps(row, scalarVector);
                _mm_store_ps(&m_memberMatrix[i][0], row);
            }
            return *this;
        }
    }

    if constexpr (std::is_same_v<T, double> && N == 4) {
        if (HIsAVX2Enabled()) {
            __m256d scalarVector = _mm256_set1_pd(scalar);
            for (unsigned int i = 0; i < N; ++i) {
                __m256d row = _mm256_load_pd(&m_memberMatrix[i][0]);
                row = _mm256_div_pd(row, scalarVector);
                _mm256_store_pd(&m_memberMatrix[i][0], row);
            }
            return *this;
        }
    }

    for (unsigned int i = 0; i < N; ++i) {
        for (unsigned int j = 0; j < N; ++j) {
            m_memberMatrix[i][j] /= scalar;
        }
    }
    return *this;
}

template<typename T, unsigned int N>
constexpr bool SquareMatrix<T, N>::isIdentityMatrix() const noexcept
{
    for (unsigned int i = 0; i < N; ++i) 
    {
        for (unsigned int j = 0; j < N; ++j) 
        {
            if (i == j) 
            {
                if (m_memberMatrix[i][j] != 1) 
                {
                    return false;
                }
            }
            else 
            {
                if (m_memberMatrix[i][j] != 0) 
                {
                    return false;
                }
            }
        }
    }
    return true;
}

template<typename T, unsigned int N>
std::string SquareMatrix<T, N>::toString() const
{
    std::ostringstream oss{};
    oss << "[";
    for (unsigned int i{}; i < N; ++i)
    {
        if (i > 0) 
        {
            oss << " ";
        }
        oss << "[";
        for (unsigned int j = 0; j < N; ++j) 
        {
            oss << m_memberMatrix[i][j];
            if (j < N - 1) 
            {
                oss << ", ";
            }
        }
        oss << "]";
        if (i < N - 1) 
        {
            oss << ",\n";
        }
    }
    oss << "]";

    return oss.str();
}

template<typename T, unsigned int N>
constexpr void SquareMatrix<T, N>::prefetchForCacheUtilization() const noexcept
{
    if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
    {
        for (unsigned int i = 0; i < N; ++i) 
        {
            _mm_prefetch(reinterpret_cast<const char*>(m_memberMatrix[i]), _MM_HINT_T0);
        }
    }
}

// Non-member implementations.
template<typename T, unsigned int N>
constexpr SquareMatrix<T, N> operator*(const T& scalar, const SquareMatrix<T, N>& matrix) noexcept
{
    SquareMatrix<T, N> result{ matrix };
    result *= scalar;
    return result;
}

template<typename ResultType, typename T, unsigned int N>
constexpr ResultType multiplyMatrixByVector(const SquareMatrix<T, N>& matrix, const ResultType& vector) noexcept
{
    ResultType result{};
    for (unsigned int i = 0; i < N; ++i) 
    {
        result[i] = 0;
        for (unsigned int j{}; j < N; ++j) 
        {
            result[i] += matrix[i][j] * vector[j];
        }
    }
    return result;
}

template <typename T, unsigned int N>
constexpr T computeDeterminant(const SquareMatrix<T, N>& matrix) noexcept {
    if constexpr (N == 1) {
        return matrix[0][0];
    }
    else if constexpr (N == 2) {
        return HDifferenceOfProducts(matrix[0][0], matrix[1][1], matrix[0][1], matrix[1][0]);
    }
    else if constexpr (N == 3) {
        T minor12 = HDifferenceOfProducts(matrix[1][1], matrix[2][2], matrix[1][2], matrix[2][1]);
        T minor02 = HDifferenceOfProducts(matrix[1][0], matrix[2][2], matrix[1][2], matrix[2][0]);
        T minor01 = HDifferenceOfProducts(matrix[1][0], matrix[2][1], matrix[1][1], matrix[2][0]);
        return HFusedMultiplyAdd(matrix[0][2], minor01, HDifferenceOfProducts(matrix[0][0], minor12, matrix[0][1], minor02));
    }
    else if constexpr (N == 4) {
        T s0 = HDifferenceOfProducts(matrix[0][0], matrix[1][1], matrix[1][0], matrix[0][1]);
        T s1 = HDifferenceOfProducts(matrix[0][0], matrix[1][2], matrix[1][0], matrix[0][2]);
        T s2 = HDifferenceOfProducts(matrix[0][0], matrix[1][3], matrix[1][0], matrix[0][3]);

        T s3 = HDifferenceOfProducts(matrix[0][1], matrix[1][2], matrix[1][1], matrix[0][2]);
        T s4 = HDifferenceOfProducts(matrix[0][1], matrix[1][3], matrix[1][1], matrix[0][3]);
        T s5 = HDifferenceOfProducts(matrix[0][2], matrix[1][3], matrix[1][2], matrix[0][3]);

        T c0 = HDifferenceOfProducts(matrix[2][0], matrix[3][1], matrix[3][0], matrix[2][1]);
        T c1 = HDifferenceOfProducts(matrix[2][0], matrix[3][2], matrix[3][0], matrix[2][2]);
        T c2 = HDifferenceOfProducts(matrix[2][0], matrix[3][3], matrix[3][0], matrix[2][3]);

        T c3 = HDifferenceOfProducts(matrix[2][1], matrix[3][2], matrix[3][1], matrix[2][2]);
        T c4 = HDifferenceOfProducts(matrix[2][1], matrix[3][3], matrix[3][1], matrix[2][3]);
        T c5 = HDifferenceOfProducts(matrix[2][2], matrix[3][3], matrix[3][2], matrix[2][3]);

        return HInnerProduct(s0, c5, -s1, c4, s2, c3, s3, c2, s5, c0, -s4, c1);
    }
    else {
        // General case for NxN matrix
        SquareMatrix<T, N - 1> sub;
        T det = 0;
        for (unsigned int i = 0; i < N; ++i) {
            for (unsigned int j = 0; j < N - 1; ++j) {
                for (unsigned int k = 0; k < N - 1; ++k) {
                    sub[j][k] = matrix[j + 1][k < i ? k : k + 1];
                }
            }
            T sign = (i & 1) ? -1 : 1;
            det += sign * matrix[0][i] * computeDeterminant(sub);
        }
        return det;
    }

}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N> transposeMatrix(const SquareMatrix<T, N>& matrix) noexcept
{
    SquareMatrix<T, N> result;
    for (unsigned int i = 0; i < N; ++i) 
    {
        for (unsigned int j = 0; j < N; ++j) 
        {
            result[j][i] = matrix[i][j];
        }
    }

    return result;
}

template <typename T, unsigned int N>
constexpr std::optional<SquareMatrix<T, N>> getInverseMatrix(const SquareMatrix<T, N>& matrix) noexcept {
    // For 1x1 matrix
    if constexpr (N == 1) {
        SquareMatrix<T, N> invMatrix;
        invMatrix[0][0] = 1 / matrix[0][0];
        return invMatrix;
    }

    // For 2x2 matrix
    if constexpr (N == 2) {
        T det = HDifferenceOfProducts(matrix[0][0], matrix[1][1], matrix[0][1], matrix[1][0]);
        if (det == 0) {
            return std::nullopt;
        }
        T invDet = 1 / det;
        SquareMatrix<T, N> invMatrix;
        invMatrix[0][0] = invDet * matrix[1][1];
        invMatrix[0][1] = -invDet * matrix[0][1];
        invMatrix[1][0] = -invDet * matrix[1][0];
        invMatrix[1][1] = invDet * matrix[0][0];
        return invMatrix;
    }

    // For 3x3 matrix
    if constexpr (N == 3) {
        T det = computeDeterminant(matrix);
        if (det == 0) {
            return std::nullopt;
        }
        T invDet = 1 / det;
        SquareMatrix<T, N> invMatrix;
        invMatrix[0][0] = invDet * HDifferenceOfProducts(matrix[1][1], matrix[2][2], matrix[1][2], matrix[2][1]);
        invMatrix[1][0] = invDet * HDifferenceOfProducts(matrix[1][2], matrix[2][0], matrix[1][0], matrix[2][2]);
        invMatrix[2][0] = invDet * HDifferenceOfProducts(matrix[1][0], matrix[2][1], matrix[1][1], matrix[2][0]);
        invMatrix[0][1] = invDet * HDifferenceOfProducts(matrix[0][2], matrix[2][1], matrix[0][1], matrix[2][2]);
        invMatrix[1][1] = invDet * HDifferenceOfProducts(matrix[0][0], matrix[2][2], matrix[0][2], matrix[2][0]);
        invMatrix[2][1] = invDet * HDifferenceOfProducts(matrix[0][1], matrix[2][0], matrix[0][0], matrix[2][1]);
        invMatrix[0][2] = invDet * HDifferenceOfProducts(matrix[0][1], matrix[1][2], matrix[0][2], matrix[1][1]);
        invMatrix[1][2] = invDet * HDifferenceOfProducts(matrix[0][2], matrix[1][0], matrix[0][0], matrix[1][2]);
        invMatrix[2][2] = invDet * HDifferenceOfProducts(matrix[0][0], matrix[1][1], matrix[0][1], matrix[1][0]);
        return invMatrix;
    }

    // For 4x4 matrix
    if constexpr (N == 4) {
        T s0 = HDifferenceOfProducts(matrix[0][0], matrix[1][1], matrix[1][0], matrix[0][1]);
        T s1 = HDifferenceOfProducts(matrix[0][0], matrix[1][2], matrix[1][0], matrix[0][2]);
        T s2 = HDifferenceOfProducts(matrix[0][0], matrix[1][3], matrix[1][0], matrix[0][3]);

        T s3 = HDifferenceOfProducts(matrix[0][1], matrix[1][2], matrix[1][1], matrix[0][2]);
        T s4 = HDifferenceOfProducts(matrix[0][1], matrix[1][3], matrix[1][1], matrix[0][3]);
        T s5 = HDifferenceOfProducts(matrix[0][2], matrix[1][3], matrix[1][2], matrix[0][3]);

        T c0 = HDifferenceOfProducts(matrix[2][0], matrix[3][1], matrix[3][0], matrix[2][1]);
        T c1 = HDifferenceOfProducts(matrix[2][0], matrix[3][2], matrix[3][0], matrix[2][2]);
        T c2 = HDifferenceOfProducts(matrix[2][0], matrix[3][3], matrix[3][0], matrix[2][3]);

        T c3 = HDifferenceOfProducts(matrix[2][1], matrix[3][2], matrix[3][1], matrix[2][2]);
        T c4 = HDifferenceOfProducts(matrix[2][1], matrix[3][3], matrix[3][1], matrix[2][3]);
        T c5 = HDifferenceOfProducts(matrix[2][2], matrix[3][3], matrix[3][2], matrix[2][3]);

        T determinant = HInnerProduct(s0, c5, -s1, c4, s2, c3, s3, c2, s5, c0, -s4, c1);
        if (determinant == 0) {
            return std::nullopt;
        }
        T invDet = 1 / determinant;

        T inv[4][4] = {
            {invDet * HInnerProduct(matrix[1][1], c5, matrix[1][3], c3, -matrix[1][2], c4),
             invDet * HInnerProduct(-matrix[0][1], c5, matrix[0][2], c4, -matrix[0][3], c3),
             invDet * HInnerProduct(matrix[3][1], s5, matrix[3][3], s3, -matrix[3][2], s4),
             invDet * HInnerProduct(-matrix[2][1], s5, matrix[2][2], s4, -matrix[2][3], s3)},

            {invDet * HInnerProduct(-matrix[1][0], c5, matrix[1][2], c2, -matrix[1][3], c1),
             invDet * HInnerProduct(matrix[0][0], c5, matrix[0][3], c1, -matrix[0][2], c2),
             invDet * HInnerProduct(-matrix[3][0], s5, matrix[3][2], s2, -matrix[3][3], s1),
             invDet * HInnerProduct(matrix[2][0], s5, matrix[2][3], s1, -matrix[2][2], s2)},

            {invDet * HInnerProduct(matrix[1][0], c4, matrix[1][3], c0, -matrix[1][1], c2),
             invDet * HInnerProduct(-matrix[0][0], c4, matrix[0][1], c2, -matrix[0][3], c0),
             invDet * HInnerProduct(matrix[3][0], s4, matrix[3][3], s0, -matrix[3][1], s2),
             invDet * HInnerProduct(-matrix[2][0], s4, matrix[2][1], s2, -matrix[2][3], s0)},

            {invDet * HInnerProduct(-matrix[1][0], c3, matrix[1][1], c1, -matrix[1][2], c0),
             invDet * HInnerProduct(matrix[0][0], c3, matrix[0][2], c0, -matrix[0][1], c1),
             invDet * HInnerProduct(-matrix[3][0], s3, matrix[3][1], s1, -matrix[3][2], s0),
             invDet * HInnerProduct(matrix[2][0], s3, matrix[2][2], s0, -matrix[2][1], s1)}
        };

        return SquareMatrix<T, 4>(inv);
    }

    // General case for NxN matrix
    if constexpr (N > 4) {
        int indxc[N], indxr[N], ipiv[N] = { 0 };
        SquareMatrix<T, N> minv = matrix;
        for (int i = 0; i < N; i++) {
            int irow = 0, icol = 0;
            T big = 0.0;
            for (int j = 0; j < N; j++) {
                if (ipiv[j] != 1) {
                    for (int k = 0; k < N; k++) {
                        if (ipiv[k] == 0) {
                            if (std::abs(minv[j][k]) >= big) {
                                big = std::abs(minv[j][k]);
                                irow = j;
                                icol = k;
                            }
                        }
                        else if (ipiv[k] > 1) {
                            return std::nullopt;  // Singular
                        }
                    }
                }
            }
            ++ipiv[icol];
            if (irow != icol) {
                for (int k = 0; k < N; ++k) {
                    std::swap(minv[irow][k], minv[icol][k]);
                }
            }
            indxr[i] = irow;
            indxc[i] = icol;
            if (minv[icol][icol] == 0) {
                return std::nullopt;  // Singular
            }

            T pivinv = 1 / minv[icol][icol];
            minv[icol][icol] = 1;
            for (int j = 0; j < N; j++) {
                minv[icol][j] *= pivinv;
            }

            for (int j = 0; j < N; j++) {
                if (j != icol) {
                    T save = minv[j][icol];
                    minv[j][icol] = 0;
                    for (int k = 0; k < N; k++) {
                        minv[j][k] -= minv[icol][k] * save;
                    }
                }
            }
        }
        for (int j = N - 1; j >= 0; j--) {
            if (indxr[j] != indxc[j]) {
                for (int k = 0; k < N; k++) {
                    std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
                }
            }
        }
        return minv;
    }
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N> invertOrExit(const SquareMatrix<T, N>& matrix) noexcept
{
    auto inv{ getInverseMatrix(matrix) };
	if (!inv) 
    {
		std::terminate();
	}
	
    return *inv;
}

template<typename T, unsigned int N>
constexpr SquareMatrix<T, N> operator*(const SquareMatrix<T, N>& m1, const SquareMatrix<T, N>& m2) noexcept {
    SquareMatrix<T, N> result{};
    for (unsigned int i = 0; i < N; ++i) {
        for (unsigned int j = 0; j < N; ++j) {
            result[i][j] = 0;
            for (unsigned int k = 0; k < N; ++k) {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return result;
}

export
{
    using Matrix2x2f = SquareMatrix<float, 2>;
    using Matrix3x3f = SquareMatrix<float, 3>;
    using Matrix4x4f = SquareMatrix<float, 4>;

    using Matrix2x2d = SquareMatrix<double, 2>;
    using Matrix3x3d = SquareMatrix<double, 3>;
    using Matrix4x4d = SquareMatrix<double, 4>;

    using Matrix2x2i = SquareMatrix<int, 2>;
    using Matrix3x3i = SquareMatrix<int, 3>;
    using Matrix4x4i = SquareMatrix<int, 4>;

    using Mat4d = Matrix4x4d;
    using Mat3d = Matrix3x3d;
    using Mat2d = Matrix2x2d;

    using Mat4f = Matrix4x4f;
    using Mat3f = Matrix3x3f;
    using Mat2f = Matrix2x2f;

    using Mat4c = Mat4d;
    using Mat3c = Mat3d;
    using Mat2c = Mat2d;
};