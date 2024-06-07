export module vector;

import <compare>;
import <type_traits>;
import <intrin.h>;
import <array>;
import <cmath>;

import h_miscutilities;
import h_mathutilities;

export template<typename T, std::size_t N>
class alignas(32) Vector
{
public:
	constexpr explicit Vector() noexcept = default;
	constexpr explicit Vector(const T value) noexcept;
	template<typename... Args, std::enable_if_t<sizeof...(Args) == N, int> = 0>
	constexpr explicit Vector(const Args&... args) noexcept;

	constexpr Vector(const Vector&) = default;
	constexpr Vector& operator=(const Vector&) = default;
	constexpr Vector(Vector&&) noexcept = default;
	constexpr Vector& operator=(Vector&&) noexcept = default;

	constexpr auto operator<=>(const Vector&) const = default;
	constexpr bool operator==(const Vector&) const = default;
	constexpr bool operator!=(const Vector&) const = default;
	constexpr Vector operator-() const noexcept; /*{
		Vector result;
		for (std::size_t i = 0; i < N; ++i) {
			result.m_data[i] = -m_data[i];
		}
		return result;
	}*/

	[[nodiscard]] constexpr T operator[](const std::size_t index) const noexcept;
	[[nodiscard]] constexpr T& operator[](const std::size_t index) noexcept;

	[[nodiscard]] constexpr Vector operator+(const Vector&) const;
	[[nodiscard]] constexpr Vector operator-(const Vector&) const;
	[[nodiscard]] constexpr Vector operator*(const T) const;
	[[nodiscard]] constexpr Vector operator/(const T) const;
	constexpr Vector& operator+=(const Vector&);
	constexpr Vector& operator-=(const Vector&);
	constexpr Vector& operator*=(const T);
	constexpr Vector& operator/=(const T);

	constexpr void prefetchForCacheUtilization() const noexcept;

	~Vector() noexcept = default;

private:
	std::array<T, N> m_data{};
};

// Non-members.

export template<typename T, std::size_t N>
[[nodiscard]] constexpr T computeDot(const Vector<T, N>&, const Vector<T, N>&);

export template<typename T, std::size_t N>
[[nodiscard]] constexpr Vector<T, N> normalize(const Vector<T, N>&);

export template<typename T, std::size_t N>
[[nodiscard]] constexpr T computeMagnitudeSq(const Vector<T, N>&);

export template<typename T, std::size_t N>
[[nodiscard]] constexpr T computeMagnitude(const Vector<T, N>&);

export template<typename T>
[[nodiscard]] constexpr Vector<T, 3> computeCross(const Vector<T, 3>&, const Vector<T, 3>&);

export
{
	using Vec2f = Vector<float, 2>;
	using Vec2d = Vector<double, 2>;
	using Vec2i = Vector<int, 2>;

	using Vec3f = Vector<float, 3>;
	using Vec3d = Vector<double, 3>;
	using Vec3i = Vector<int, 3>;

	using Vec4f = Vector<float, 4>;
	using Vec4d = Vector<double, 4>;
	using Vec4i = Vector<int, 4>;

	using Vec2c = Vec2d;
	using Vec3c = Vec3d;
	using Vec4c = Vec4d;
};
// ------------------------------------------------------------------------//

//// Implementation must be in the interface because it is an exported template.
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N>::Vector(const T value) noexcept {
//	for (std::size_t i = 0; i < N; ++i) {
//		m_data[i] = value;
//	}
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N> Vector<T, N>::operator-() const noexcept
//{
//	Vector result{};
//	for (std::size_t i = 0; i < N; ++i) {
//		result.m_data[i] = -m_data[i];
//	}
//	return result;
//}
//
//template<typename T, std::size_t N>
//template<typename... Args, std::enable_if_t<sizeof...(Args) == N, int>>
//constexpr Vector<T, N>::Vector(const Args&... args) noexcept : m_data{ args... } {}
//
//template<typename T, std::size_t N>
//constexpr T Vector<T, N>::operator[](const std::size_t index) const noexcept
//{
//	return m_data[index];
//}
//
//template<typename T, std::size_t N>
//constexpr T& Vector<T, N>::operator[](const std::size_t index) noexcept
//{
//	return const_cast<T>(std::as_const<*this>[index]);
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N> Vector<T, N>::operator+(const Vector<T, N>& vec) const {
//	Vector<T, N> result{ *this };
//	result += vec;
//	return result;
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N> Vector<T, N>::operator-(const Vector<T, N>& vec) const {
//	Vector<T, N> result{ *this };
//	result -= vec;
//	return result;
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N> Vector<T, N>::operator*(const T scalar) const {
//	Vector<T, N> result{ *this };
//	result *= scalar;
//	return result;
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N> Vector<T, N>::operator/(const T scalar) const {
//	Vector<T, N> result{ *this };
//	result /= scalar;
//	return result;
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N>& Vector<T, N>::operator+=(const Vector<T, N>& vec) {
//	bool needFallback{ true };
//
//	this->prefetchForCacheUtilization();
//	vec.prefetchForCacheUtilization();
//
//	if constexpr (std::is_same_v<T, float>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m128 first = _mm_set_ps(0.0f, 0.0f, m_data[1], m_data[0]);
//				__m128 second = _mm_set_ps(0.0f, 0.0f, vec.m_data[1], vec.m_data[0]);
//				__m128 add = _mm_add_ps(first, second);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm_store_ps(result.data(), add);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//			}
//			else if constexpr (N == 3)
//			{
//				__m128 first = _mm_set_ps(0.0f, m_data[2], m_data[1], m_data[0]);
//				__m128 second = _mm_set_ps(0.0f, vec.m_data[2], vec.m_data[1], vec.m_data[0]);
//				__m128 add = _mm_add_ps(first, second);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm_store_ps(result.data(), add);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//				m_data[2] = result[2];
//			}
//			else if constexpr (N == 4)
//			{
//				__m128 first = _mm_load_ps(m_data.data());
//				__m128 second = _mm_load_ps(vec.m_data.data());
//				__m128 add = _mm_add_ps(first, second);
//				_mm_store_ps(m_data.data(), add);
//			}
//
//			needFallback = false;
//		}
//	}
//	else if constexpr (std::is_same_v<T, double>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m256d first = _mm256_set_pd(0.0, 0.0, m_data[1], m_data[0]);
//				__m256d second = _mm256_set_pd(0.0, 0.0, vec.m_data[1], vec.m_data[0]);
//				__m256d add = _mm256_add_pd(first, second);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm256_store_pd(result.data(), add);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//			}
//			else if constexpr (N == 3)
//			{
//				__m256d first = _mm256_set_pd(0.0, m_data[2], m_data[1], m_data[0]);
//				__m256d second = _mm256_set_pd(0.0, vec.m_data[2], vec.m_data[1], vec.m_data[0]);
//				__m256d add = _mm256_add_pd(first, second);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm256_store_pd(result.data(), add);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//				m_data[2] = result[2];
//			}
//			else if constexpr (N == 4)
//			{
//				__m256d first = _mm256_load_pd(m_data.data());
//				__m256d second = _mm256_load_pd(vec.m_data.data());
//				__m256d add = _mm256_add_pd(first, second);
//				_mm256_store_pd(m_data.data(), add);
//			}
//
//			needFallback = false;
//		}
//	}
//
//	if (needFallback)
//	{
//		for (std::size_t i = 0; i < N; ++i)
//		{
//			m_data[i] += vec.m_data[i];
//		}
//	}
//
//	return *this;
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N>& Vector<T, N>::operator-=(const Vector<T, N>& vec) {
//	bool needFallback{ true };
//
//	this->prefetchForCacheUtilization();
//	vec.prefetchForCacheUtilization();
//
//	if constexpr (std::is_same_v<T, float>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m128 first = _mm_set_ps(0.0f, 0.0f, m_data[1], m_data[0]);
//				__m128 second = _mm_set_ps(0.0f, 0.0f, vec.m_data[1], vec.m_data[0]);
//				__m128 sub = _mm_sub_ps(first, second);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm_store_ps(result.data(), sub);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//			}
//			else if constexpr (N == 3)
//			{
//				__m128 first = _mm_set_ps(0.0f, m_data[2], m_data[1], m_data[0]);
//				__m128 second = _mm_set_ps(0.0f, vec.m_data[2], vec.m_data[1], vec.m_data[0]);
//				__m128 sub = _mm_sub_ps(first, second);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm_store_ps(result.data(), sub);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//				m_data[2] = result[2];
//			}
//			else if constexpr (N == 4)
//			{
//				__m128 first = _mm_load_ps(m_data.data());
//				__m128 second = _mm_load_ps(vec.m_data.data());
//				__m128 sub = _mm_sub_ps(first, second);
//				_mm_store_ps(m_data.data(), sub);
//			}
//
//			needFallback = false;
//		}
//	}
//	else if constexpr (std::is_same_v<T, double>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m256d first = _mm256_set_pd(0.0, 0.0, m_data[1], m_data[0]);
//				__m256d second = _mm256_set_pd(0.0, 0.0, vec.m_data[1], vec.m_data[0]);
//				__m256d sub = _mm256_sub_pd(first, second);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm256_store_pd(result.data(), sub);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//			}
//			else if constexpr (N == 3)
//			{
//				__m256d first = _mm256_set_pd(0.0, m_data[2], m_data[1], m_data[0]);
//				__m256d second = _mm256_set_pd(0.0, vec.m_data[2], vec.m_data[1], vec.m_data[0]);
//				__m256d sub = _mm256_sub_pd(first, second);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm256_store_pd(result.data(), sub);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//				m_data[2] = result[2];
//			}
//			else if constexpr (N == 4)
//			{
//				__m256d first = _mm256_load_pd(m_data.data());
//				__m256d second = _mm256_load_pd(vec.m_data.data());
//				__m256d sub = _mm256_sub_pd(first, second);
//				_mm256_store_pd(m_data.data(), sub);
//			}
//
//			needFallback = false;
//		}
//	}
//
//	if (needFallback)
//	{
//		for (std::size_t i = 0; i < N; ++i)
//		{
//			m_data[i] -= vec.m_data[i];
//		}
//	}
//
//	return *this;
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N>& Vector<T, N>::operator*=(const T scalar) {
//	bool needFallback{ true };
//
//	this->prefetchForCacheUtilization();
//
//	if constexpr (std::is_same_v<T, float>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m128 vec = _mm_set_ps(0.0f, 0.0f, m_data[1], m_data[0]);
//				__m128 sca = _mm_set1_ps(scalar);
//				__m128 mul = _mm_mul_ps(vec, sca);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm_store_ps(result.data(), mul);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//			}
//			else if constexpr (N == 3)
//			{
//				__m128 vec = _mm_set_ps(0.0f, m_data[2], m_data[1], m_data[0]);
//				__m128 sca = _mm_set1_ps(scalar);
//				__m128 mul = _mm_mul_ps(vec, sca);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm_store_ps(result.data(), mul);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//				m_data[2] = result[2];
//			}
//			else if constexpr (N == 4)
//			{
//				__m128 vec = _mm_load_ps(m_data.data());
//				__m128 sca = _mm_set1_ps(scalar);
//				__m128 mul = _mm_mul_ps(vec, sca);
//				_mm_store_ps(m_data.data(), mul);
//			}
//
//			needFallback = false;
//		}
//	}
//	else if constexpr (std::is_same_v<T, double>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m256d vec = _mm256_set_pd(0.0, 0.0, m_data[1], m_data[0]);
//				__m256d sca = _mm256_set1_pd(scalar);
//				__m256d mul = _mm256_mul_pd(vec, sca);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm256_store_pd(result.data(), mul);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//			}
//			else if constexpr (N == 3)
//			{
//				__m256d vec = _mm256_set_pd(0.0, m_data[2], m_data[1], m_data[0]);
//				__m256d sca = _mm256_set1_pd(scalar);
//				__m256d mul = _mm256_mul_pd(vec, sca);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm256_store_pd(result.data(), mul);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//				m_data[2] = result[2];
//			}
//			else if constexpr (N == 4)
//			{
//				__m256d vec = _mm256_load_pd(m_data.data());
//				__m256d sca = _mm256_set1_pd(scalar);
//				__m256d mul = _mm256_mul_pd(vec, sca);
//				_mm256_store_pd(m_data.data(), mul);
//			}
//
//			needFallback = false;
//		}
//	}
//
//	if (needFallback)
//	{
//		for (std::size_t i = 0; i < N; ++i)
//		{
//			m_data[i] *= scalar;
//		}
//	}
//
//	return *this;
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N>& Vector<T, N>::operator/=(const T scalar) {
//	// Returning Vec(0) for graceful handling.
//	if (scalar == 0) {
//		for (std::size_t i = 0; i < N; ++i) {
//			m_data[i] = 0;
//		}
//		return *this;
//	}
//
//	bool needFallback{ true };
//
//	this->prefetchForCacheUtilization();
//
//	if constexpr (std::is_same_v<T, float>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m128 vec = _mm_set_ps(0.0f, 0.0f, m_data[1], m_data[0]);
//				__m128 sca = _mm_set1_ps(scalar);
//				__m128 div = _mm_div_ps(vec, sca);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm_store_ps(result.data(), div);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//			}
//			else if constexpr (N == 3)
//			{
//				__m128 vec = _mm_set_ps(0.0f, m_data[2], m_data[1], m_data[0]);
//				__m128 sca = _mm_set1_ps(scalar);
//				__m128 div = _mm_div_ps(vec, sca);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm_store_ps(result.data(), div);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//				m_data[2] = result[2];
//			}
//			else if constexpr (N == 4)
//			{
//				__m128 vec = _mm_load_ps(m_data.data());
//				__m128 sca = _mm_set1_ps(scalar);
//				__m128 div = _mm_div_ps(vec, sca);
//				_mm_store_ps(m_data.data(), div);
//			}
//
//			needFallback = false;
//		}
//	}
//	else if constexpr (std::is_same_v<T, double>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m256d vec = _mm256_set_pd(0.0, 0.0, m_data[1], m_data[0]);
//				__m256d sca = _mm256_set1_pd(scalar);
//				__m256d div = _mm256_div_pd(vec, sca);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm256_store_pd(result.data(), div);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//			}
//			else if constexpr (N == 3)
//			{
//				__m256d vec = _mm256_set_pd(0.0, m_data[2], m_data[1], m_data[0]);
//				__m256d sca = _mm256_set1_pd(scalar);
//				__m256d div = _mm256_div_pd(vec, sca);
//
//				alignas(32) std::array<T, 4> result{};
//				_mm256_store_pd(result.data(), div);
//
//				m_data[0] = result[0];
//				m_data[1] = result[1];
//				m_data[2] = result[2];
//			}
//			else if constexpr (N == 4)
//			{
//				__m256d vec = _mm256_load_pd(m_data.data());
//				__m256d sca = _mm256_set1_pd(scalar);
//				__m256d div = _mm256_div_pd(vec, sca);
//				_mm256_store_pd(m_data.data(), div);
//			}
//
//			needFallback = false;
//		}
//	}
//
//	if (needFallback)
//	{
//		for (std::size_t i = 0; i < N; ++i)
//		{
//			m_data[i] /= scalar;
//		}
//	}
//
//	return *this;
//}
//
//
//template<typename T, std::size_t N>
//constexpr void Vector<T, N>::prefetchForCacheUtilization() const noexcept {
//	if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
//		_mm_prefetch(reinterpret_cast<const char*>(this), _MM_HINT_T0);
//	}
//}
//
////// Non-members.
//template<typename T, std::size_t N>
//constexpr T computeDot(const Vector<T, N>& first, const Vector<T, N>& second) {
//	first.prefetchForCacheUtilization();
//	second.prefetchForCacheUtilization();
//
//	if constexpr (std::is_same_v<T, float>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m128 vec1 = _mm_set_ps(0.0f, 0.0f, first[1], first[0]);
//				__m128 vec2 = _mm_set_ps(0.0f, 0.0f, second[1], second[0]);
//
//				__m128 mul = _mm_mul_ps(vec1, vec2);
//				__m128 hadd = _mm_hadd_ps(mul, mul);
//
//				return _mm_cvtss_f32(hadd);
//			}
//			else if constexpr (N == 3)
//			{
//				__m128 vec1 = _mm_set_ps(0.0f, first[2], first[1], first[0]);
//				__m128 vec2 = _mm_set_ps(0.0f, second[2], second[1], second[0]);
//
//				__m128 mul = _mm_mul_ps(vec1, vec2);
//				__m128 hadd = _mm_hadd_ps(mul, mul);
//				hadd = _mm_hadd_ps(hadd, hadd);
//
//				return _mm_cvtss_f32(hadd);
//			}
//			else if constexpr (N == 4)
//			{
//				__m128 vec1 = _mm_load_ps(first.m_data.data());
//				__m128 vec2 = _mm_load_ps(second.m_data.data());
//
//				__m128 mul = _mm_mul_ps(vec1, vec2);
//				__m128 hadd = _mm_hadd_ps(mul, mul);
//				hadd = _mm_hadd_ps(hadd, hadd);
//
//				return _mm_cvtss_f32(hadd);
//			}
//		}
//	}
//	else if constexpr (std::is_same_v<T, double>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			if constexpr (N == 2)
//			{
//				__m256d vec1 = _mm256_set_pd(0.0, 0.0, first[1], first[0]);
//				__m256d vec2 = _mm256_set_pd(0.0, 0.0, second[1], second[0]);
//				__m256d mul = _mm256_mul_pd(vec1, vec2);
//				__m256d hadd = _mm256_hadd_pd(mul, mul);
//				__m128d lower = _mm256_castpd256_pd128(hadd);
//				__m128d upper = _mm256_extractf128_pd(hadd, 1);
//				__m128d sum = _mm_add_pd(lower, upper);
//				return _mm_cvtsd_f64(sum);
//			}
//			else if constexpr (N == 3)
//			{
//				__m256d vec1 = _mm256_set_pd(0.0, first[2], first[1], first[0]);
//				__m256d vec2 = _mm256_set_pd(0.0, second[2], second[1], second[0]);
//				__m256d mul = _mm256_mul_pd(vec1, vec2);
//				__m256d hadd = _mm256_hadd_pd(mul, mul);
//				__m128d lower = _mm256_castpd256_pd128(hadd);
//				__m128d upper = _mm256_extractf128_pd(hadd, 1);
//				__m128d sum = _mm_add_pd(lower, upper);
//				return _mm_cvtsd_f64(sum);
//			}
//			else if constexpr (N == 4)
//			{
//				__m256d vec1 = _mm256_load_pd(first.m_data.data());
//				__m256d vec2 = _mm256_load_pd(second.m_data.data());
//				__m256d mul = _mm256_mul_pd(vec1, vec2);
//				__m256d hadd = _mm256_hadd_pd(mul, mul);
//				__m128d lower = _mm256_castpd256_pd128(hadd);
//				__m128d upper = _mm256_extractf128_pd(hadd, 1);
//				__m128d sum = _mm_add_pd(lower, upper);
//				return _mm_cvtsd_f64(sum);
//			}
//		}
//	}
//	T result = T(0);
//	for (std::size_t i = 0; i < N; ++i) {
//		result += first[i] * second[i];
//	}
//	return result;
//}
//
//template<typename T>
//constexpr Vector<T, 3> computeCross(const Vector<T, 3>& first, const Vector<T, 3>& second) {
//	first.prefetchForCacheUtilization();
//	second.prefetchForCacheUtilization();
//
//	if constexpr (std::is_same_v<T, float>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			__m128 vec1_yzx = _mm_set_ps(0.0f, first[0], first[2], first[1]);
//			__m128 vec1_zxy = _mm_set_ps(0.0f, first[1], first[0], first[2]);
//			__m128 vec2_yzx = _mm_set_ps(0.0f, second[0], second[2], second[1]);
//			__m128 vec2_zxy = _mm_set_ps(0.0f, second[1], second[0], second[2]);
//			__m128 mul1 = _mm_mul_ps(vec1_yzx, vec2_zxy);
//			__m128 mul2 = _mm_mul_ps(vec1_zxy, vec2_yzx);
//			__m128 result = _mm_sub_ps(mul1, mul2);
//
//			alignas(32) std::array<T, 4> resultArray{};
//			_mm_store_ps(resultArray.data(), result);
//
//			return Vector<T, 3>{ resultArray[2], resultArray[1], resultArray[0] };
//		}
//	}
//	else if constexpr (std::is_same_v<T, double>)
//	{
//		if (HIsAVX2Enabled())
//		{
//			__m256d vec1_yzx = _mm256_set_pd(0.0, first[0], first[2], first[1]);
//			__m256d vec1_zxy = _mm256_set_pd(0.0, first[1], first[0], first[2]);
//			__m256d vec2_yzx = _mm256_set_pd(0.0, second[0], second[2], second[1]);
//			__m256d vec2_zxy = _mm256_set_pd(0.0, second[1], second[0], second[2]);
//			__m256d mul1 = _mm256_mul_pd(vec1_yzx, vec2_zxy);
//			__m256d mul2 = _mm256_mul_pd(vec1_zxy, vec2_yzx);
//			__m256d result = _mm256_sub_pd(mul1, mul2);
//
//			alignas(32) std::array<T, 4> resultArray{};
//			_mm256_store_pd(resultArray.data(), result);
//
//			return Vector<T, 3>{ resultArray[2], resultArray[1], resultArray[0] };
//		}
//	}
//	else
//	{
//		return Vector<T, 3>
//		{
//			(first[1] * second[2]) - (first[2] * second[1]),
//				(first[2] * second[0]) - (first[0] * second[2]),
//				(first[0] * second[1]) - (first[1] * second[0])
//		};
//	}
//}
//
//template<typename T, std::size_t N>
//constexpr T computeMagnitudeSq(const Vector<T, N>& vec) {
//	return computeDot(vec, vec);
//}
//
//template<typename T, std::size_t N>
//constexpr T computeMagnitude(const Vector<T, N>& vec) {
//	return std::sqrt(computeMagnitudeSq(vec));
//}
//
//template<typename T, std::size_t N>
//constexpr Vector<T, N> normalize(const Vector<T, N>& vec) {
//	T mag{ computeMagnitude(vec) };
//
//	if (mag > T(0)) return (vec * (1 / mag));
//
//	return vec;
//}
