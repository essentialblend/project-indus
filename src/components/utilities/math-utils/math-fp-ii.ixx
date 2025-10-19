export module mathfp;

import std;
import concepts;
import mathconstants;
//import vector;

export
{
  template <Arithmetic T, Arithmetic U, Arithmetic V>
  [[nodiscard]] constexpr T clamp(T val, U low, V high) noexcept
  {
    if (val < T{ low }) return T{ low };
    if (val > T{ high }) return T{ high };

    return val;
  }

  template<Arithmetic T>
  [[nodiscard]] bool isZero(T val)
  {
    return (val == T{});
  }
  
  template<Arithmetic T>
  [[nodiscard]] constexpr bool isFinite(T toCheck) noexcept
  {
    return std::isfinite(toCheck);
  }

  template<Vector3Like V>
  [[nodiscard]] constexpr bool isFinite(const V& v) noexcept
  {
    return (isFinite(v[0]) && isFinite(v[1]) && isFinite(v[2]));
  }

  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T fusedMultiplyAdd(T x, T y, T z) noexcept
  {
    return std::fma(x, y, z);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] T nextFloatUp(T x) noexcept
  {
    if (std::isnan(x)) return x;

    if (x == infinity<T>) return x;

    if (std::signbit(x) && x == T{}) x = T{};

    return std::nextafter(x, infinity<T>);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] T nextFloatDown(T x) noexcept
  {
    if (std::isnan(x)) return x;

    if (x == -infinity<T>) return x;

    if (!std::signbit(x) && x == T{}) x = -T{};

    return std::nextafter(x, -infinity<T>);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] T addRoundUp(T a, T b) noexcept
  {
    return nextFloatUp(a + b);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] T addRoundDown(T a, T b) noexcept
  {
    return nextFloatDown(a + b);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] T subRoundUp(T a, T b) noexcept
  {
    return nextFloatUp(a - b);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] T subRoundDown(T a, T b) noexcept
  {
    return nextFloatDown(a - b);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] T mulRoundUp(T a, T b) noexcept
  {
    return nextFloatUp(a * b);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] T mulRoundDown(T a, T b) noexcept
  {
    return nextFloatDown(a * b);
  }

  template <FloatingArithmetic T>
  [[nodiscard]] T divRoundUp(T a, T b) noexcept
  {
    return nextFloatUp(a / b);
  }

  template <FloatingArithmetic T>
  [[nodiscard]] T divRoundDown(T a, T b) noexcept
  {
    return nextFloatDown(a / b);
  }

  template <FloatingArithmetic T>
  [[nodiscard]] T FMARoundUp(T a, T b, T c) noexcept
  {
    return nextFloatUp(fusedMultiplyAdd(a, b, c));
  }

  template <FloatingArithmetic T>
  [[nodiscard]] T FMARoundDown(T a, T b, T c) noexcept
  {
    return nextFloatDown(fusedMultiplyAdd(a, b, c));
  }

  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T safeSqrt(T x) noexcept
  {
    if (!isFinite(x)) return T{};
    return (x > T{}) ? T{ std::sqrt(clamp(x, T{}, infinity<T>)) } : T{};
  }

  template <Arithmetic T>
  [[nodiscard]] constexpr T sqr(T x) noexcept
  {
    return (x * x);
  }

  template <FloatingArithmetic T>
  [[nodiscard]] T sqrtRoundUp(T x) noexcept
  {
    return nextFloatUp(safeSqrt(x));
  }

  template <FloatingArithmetic T>
  [[nodiscard]] T sqrtRoundDown(T x) noexcept
  {
    return nextFloatDown(safeSqrt(x));
  }

  template<typename A, typename B, typename... Rest> requires (FPCommon<A, B, Rest...> && (sizeof...(Rest) % 2 == 0)) [[nodiscard]] constexpr auto sumOfProducts(A a, B b, Rest... rest) noexcept
  {
    using R = std::common_type_t<A, B, Rest...>;
    if constexpr (sizeof...(Rest) == 0) return R{ a } * R{ b };
    else return fusedMultiplyAdd(R{ a }, R{ b }, sumOfProducts(R{ rest }...));
  }

  template<typename A, typename B, typename... Rest> requires (FPCommon<A, B, Rest...> && (sizeof...(Rest) % 2 == 0)) [[nodiscard]] constexpr auto differenceOfProducts(A a, B b, Rest... rest) noexcept
  {
    using R = std::common_type_t<A, B, Rest...>;
    if constexpr (sizeof...(Rest) == 0) return R{ a } * R{ b };
    else return fusedMultiplyAdd(R{ a }, R{ b }, -sumOfProducts(R{ rest }...));
  }

  // Unused
  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T modPos(T a, T b) noexcept
  {
    return a - b * std::floor(a / b);
  }

  // Unused
  template <IntegralArithmetic T>
  [[nodiscard]] constexpr T modPos(T a, T b) noexcept
  {
    return ((a % b) + b) % b;
  }

  // Unused
  template<UnsignedIntegralArithmetic U>
  [[nodiscard]] constexpr int floorLog2(U n) noexcept
  {
    return n ? static_cast<int>(std::bit_width(n) - 1) : -1;
  }

  // Unused
  template <UnsignedIntegralArithmetic U>
  [[nodiscard]] constexpr int log2Int(U n) noexcept
  {
    return floorLog2(n);
  }

  // Unused
  template<UnsignedIntegralArithmetic U>
  [[nodiscard]] constexpr int log4Int(U n) noexcept
  {
    const int l2{ floorLog2(n) };

    return (l2 < 0) ? -1 : (l2 >> 1);
  }

  // Unused
  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T log2v(T x) noexcept
  {
    const T invLog2{ T{ 1.4426950408889633870 } };
    return std::log(x) * invLog2;
  }

  // Unused
  template <UnsignedIntegralArithmetic U>
  [[nodiscard]] constexpr bool isPowerOf2(U v) noexcept
  {
    return v && ((v & (v - 1)) == 0);
  }

  // Unused
  template <int N, Arithmetic T>
  [[nodiscard]] constexpr T powi(T x) noexcept
  {
    if constexpr (N == 0) return T(1);
    if constexpr (N == 1) return x;

    const T half{ powi<N / 2>(x) };

    return (N % 2 == 0) ? sqr(half) : (sqr(half) * x);
  }

  template <FloatingArithmetic T, class It>
  [[nodiscard]] constexpr T evaluatePolynomial(T x, It first, It last) noexcept
  {
    T r{ T(0) };

    for (auto it{ last }; it != first;)
    {
      --it;
      r = fusedMultiplyAdd(x, r, *it);
    }

    return r;
  }

  // Unused
  template <UnsignedIntegralArithmetic U>
  [[nodiscard]] constexpr U roundUpPow2(U v) noexcept
  {
    if (v <= U{ 1 }) return v;

    --v;

    v |= (v >> 1);
    v |= (v >> 2);

    if constexpr (sizeof(U) * 8 >= 8)   v |= (v >> 4);
    if constexpr (sizeof(U) * 8 >= 16)  v |= (v >> 8);
    if constexpr (sizeof(U) * 8 >= 32)  v |= (v >> 16);
    if constexpr (sizeof(U) * 8 >= 64)  v |= (v >> 32);

    return v + U{ 1 };
  }

  // Unused
  template <UnsignedIntegralArithmetic U>
  [[nodiscard]] constexpr U roundDownPow2(U v) noexcept
  {
    if (v == 0) return 0;
    while (v & (v - 1)) v &= (v - 1);

    return v;
  }

  template <FloatingArithmetic T>
  [[nodiscard]] constexpr T lerp(T t, T a, T b) noexcept
  {
    return std::fma(t, b - a, a);
  }

  // Kahan's algorithm. Pending deeper understanding
  template<FloatingArithmetic T, class It>
  [[nodiscard]] constexpr T compensatedSum(It first, It last) noexcept
  {
    T sum{ 0 }; T c{ 0 };

    for (; first != last; ++first)
    {
      const T y{ *first - c };
      const T t{ sum + y };
      c = (t - sum) - y;
      sum = t;
    }

    return sum;
  }

  // Unused
  template<FloatingArithmetic T>
  [[nodiscard]] constexpr auto floatToBits(T v) noexcept
  {
    using U = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;

    return std::bit_cast<U>(v);
  }

  // Unused
  template<FloatingArithmetic T, UnsignedIntegralArithmetic U>
  [[nodiscard]] constexpr T bitsToFloat(U b) noexcept
  {
    return std::bit_cast<T>(b);
  }

  // Unused
  [[nodiscard]] constexpr std::uint32_t reverseBits32(std::uint32_t n) noexcept
  {
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ffu) << 8) | ((n & 0xff00ff00u) >> 8);
    n = ((n & 0x0f0f0f0fu) << 4) | ((n & 0xf0f0f0f0u) >> 4);
    n = ((n & 0x33333333u) << 2) | ((n & 0xccccccccu) >> 2);
    n = ((n & 0x55555555u) << 1) | ((n & 0xaaaaaaaau) >> 1);

    return n;
  }

  // Unused
  [[nodiscard]] constexpr std::uint64_t reverseBits64(std::uint64_t n) noexcept
  {
    const std::uint64_t n0{ reverseBits32(static_cast<std::uint32_t>(n)) };
    const std::uint64_t n1{ reverseBits32(static_cast<std::uint32_t>(n >> 32)) };

    return (n0 << 32) | n1;
  }

  // Unused
  template<FloatingArithmetic T>
  [[nodiscard]] constexpr T smoothStep(T x, T a, T b) noexcept
  {
    if (a == b) return (x < a) ? T{ 0 } : T{ 1 };

    const T t{ clamp((x - a) / (b - a), T{}, T{ 1 }) };

    return t * t * (T{ 3 } - T{ 2 } *t);
  }

  // Unused
  [[nodiscard]] int exponentBits(std::uint32_t b) noexcept
  {
    return static_cast<int>((b >> 23) & 0xFF) - 127;
  }

  // Unused
  [[nodiscard]] std::uint32_t significandBits(std::uint32_t b) noexcept
  {
    return b & 0x007FFFFFu;
  }

  // Unused
  template<FloatingArithmetic T>
  [[nodiscard]] int log2Int(T v) noexcept
  {
    if (!isFinite(v) || v <= T{}) return 0;

    if (v < T{ 1.0 }) return -log2Int(T{ 1.0 } / v);

    constexpr std::uint32_t midSignif{ 0x003504F3u };
    const std::uint32_t bits{ floatToBits<T>(v) };
    const int e{ exponentBits(bits) };
    const std::uint32_t s{ significandBits(bits) };

    return e + ((s >= midSignif) ? 1 : 0);
  }

  // Unused 
  template<FloatingArithmetic T>
  [[nodiscard]] T fastExp(T x) noexcept
  {
    if (!isFinite(x)) return std::signbit(x) ? T{} : infinity<T>;

    const T xp{ x * 1.442695041f };
    const T fxp{ std::floor(xp) };

    const int i{ static_cast<int>(fxp) };
    const T f{ xp - fxp };

    constexpr std::array<T, 4> c{ 1.0f, 0.695556856f, 0.226173572f, 0.0781455737f };
    const T twoToF{ evaluatePolynomial(f, c.begin(), c.end()) };

    const std::uint32_t tb{ floatToBits<T>(twoToF) };

    int exponent{ static_cast<int>((tb >> 23) & 0xFF) - 127 + i };

    if (exponent < -126) return T{};
    if (exponent > 127)  return infinity<T>;

    std::uint32_t bits{ tb & 0x807FFFFFu };
    bits |= static_cast<std::uint32_t>(exponent + 127) << 23;

    return bitsToFloat<T>(bits);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] constexpr std::optional<std::pair<T, T>> evaluateQuadratic(T a, T b, T c)
  {
    //Float discr{ sqr(b) - (Float(4.0) * a * c) };
    T discr{ differenceOfProducts(b, b, 4 * a, c) };

    if (discr < 0.0) return std::nullopt;

    T sqrtDiscr{ safeSqrt(discr) };

    T q{ (b < 0) ? T{ -0.5 } * (b - sqrtDiscr) : T{ -0.5 } * (b + sqrtDiscr) };

    // Product of roots with Viete's gets us the ratios (i.e. t0 = q/a, t1 = (c/a)/t0 => t1 = c/q) 
    T t0{ q / a }; T t1{ c / q };

    if (t0 > t1) std::swap(t0, t1);

    return std::make_pair(t0, t1);
  }

  template<FloatingArithmetic T>
  [[nodiscard]] constexpr T gamma(std::int32_t n) noexcept
  {
    return (n * epsilonMachine<T>) / (T{ 1 } - (n * epsilonMachine<T>));
  }


  template<FloatingArithmetic T>
  [[nodiscard]] T clampUnit(T x) noexcept
  {
    return clamp(x, T{ -1 }, T{ 1 });
  }

  template<Arithmetic T>
  constexpr bool nearZero(T x, T eps) noexcept 
  {
    return abs(x) <= eps;
  }

  //template<Arithmetic T, std::size_t N>
  //constexpr bool nearZero(const Vector<T, N>& v, T eps) noexcept 
  //{
  //  return computeDot(v, v) <= sqr(eps);
  //}
}