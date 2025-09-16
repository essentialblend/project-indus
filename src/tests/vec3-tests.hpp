#pragma once

#include "../dep/ut.hpp"
import <cstdint>;

import std;
import vector;
import types;

using namespace boost::ut;

using Vf = Vec3f;
using Vd = Vec3d;
using V = Vf;

constexpr struct Tol
{ 
  Float64 abs{ 1e-7 };
  Float64 rel{ 1e-6 };
  uint64_t maxULPS{ 2 };
};

template<Arithmetic T>
constexpr bool isNaN(T x) noexcept
{
  return std::isnan(x);
}

template<Arithmetic T>
constexpr bool isInf(T x) noexcept 
{ 
  return std::isinf(x); 
}

template<Arithmetic T>
constexpr bool isFinite(T x) noexcept
{
  return std::isfinite(x);
}

template<Arithmetic T>
constexpr bool isNegZero(T x) noexcept
{
  return std::signbit(x) && (x == T{ 0 });
}

template<Arithmetic T>
inline uint64_t ULPDistance(T a, T b) noexcept
{
  // Constrain the type based on T. If T = float, U = uint32_t, else uint64_t. This is to obtain the correct bucket to contain the floats
  using U = std::conditional_t<std::is_same_v<T, float>, uint32_t, uint64_t>;
  
  // Bitshift and obtain a negative magnitude. i.e. MSB = 1 means neg
  constexpr U SIGN = U(1) << (sizeof(U) * 8 - 1);

  // Now cast the floats to their underlying integral bitstream
  U ua{ std::bit_cast<U>(a) };
  U ub{ std::bit_cast<U>(b) };
  
  // Order to restore monotonicity. If a number is negative, we flip the entire stream, else if it's non-negative, we flip only the MSB from 0 -> 1
  auto order = [](U x) { return (x & SIGN) ? ~x : (x | SIGN); };
  
  U oa{ order(ua) };
  U ob{ order(ub) };
  
  // Subtract to evaluate the ULP distance and return 
  U diff = (oa >= ob) ? (oa - ob) : (ob - oa);
  
  return diff;
}

template<Arithmetic T>
bool almostEqual(T a, T b, Tol t = {}) 
{
  if (a == b) return true;
  if (!std::isfinite(a) || !std::isfinite(b)) return false;

  T diff = std::abs(a - b);
  T scale = std::max(std::abs(a), std::abs(b));
  if (diff <= T(t.abs)) return true;
  if (diff <= T(t.rel) * scale) return true;

  // Sentinel: ULP distance <= maxULPS
  return ULPDistance(a, b) <= (t.maxULPS);
}

template<typename T>
T extremeRand(std::mt19937_64& eng)
{
  std::uniform_int_distribution<int> pick(0, 7);
  switch (pick(eng))
  {
    case 0: return T{ 0 };
    case 1: return -T{ 0 };
    case 2: return std::numeric_limits<T>::denorm_min();
    case 3: return std::numeric_limits<T>::min();
    case 4: return std::numeric_limits<T>::max();
    case 5: return std::numeric_limits<T>::infinity();
    case 6: return -std::numeric_limits<T>::infinity();
    default:
    {
      std::uniform_real_distribution<T> d(T(-1e6), T(1e6));
      return d(eng);
    }
  }
}

void vecConstructionTests() 
{
  // These tests verify that vectors construct correctly from scalars,
  // arrays, and edge cases like +0 and -0. Critical for ensuring stable
  // semantics at the base of the class.

  "ctor/edgecases"_test = [] {
    Vf vf{ 0.0f }; Vd vd{ 0.0 };
    for (int i = 0; i < 3; ++i) {
      expect(vf[i] == 0.0f);
      expect(vd[i] == 0.0);
    }

    Vf vf2{ -0.0f }; Vd vd2{ -0.0 };
    for (int i = 0; i < 3; ++i) {
      expect(std::signbit(vf2[i]));
      expect(std::signbit(vd2[i]));
    }
    };

  // Randomized round-trip check: values passed into constructors must
  // round-trip unchanged, except NaNs which remain NaNs. Important for
  // catching mis-specified constructors.
  "ctor/randomized_roundtrip"_test = [] {
    std::mt19937_64 eng{ 0xC0FFEEULL };
    for (int trial = 0; trial < 20000; ++trial) {
      std::array<float, 3> af{ extremeRand<float>(eng), extremeRand<float>(eng), extremeRand<float>(eng) };
      Vf vf{ af };
      for (int i = 0; i < 3; ++i) {
        if (std::isnan(af[i])) expect(std::isnan(vf[i]));
        else expect(vf[i] == af[i]);
      }

      std::array<double, 3> ad{ extremeRand<double>(eng), extremeRand<double>(eng), extremeRand<double>(eng) };
      Vd vd{ ad };
      for (int i = 0; i < 3; ++i) {
        if (std::isnan(ad[i])) expect(std::isnan(vd[i]));
        else expect(vd[i] == ad[i]);
      }
    }
    };

  std::println("Construction tests passed...");
}

void vecArithmeticOpsTests() {
  // Verifies all arithmetic operators for float and double vectors across identities, algebraic laws, random stress, compound ops, and special IEEE-754 edge cases. Critical to ensure operator semantics are correct and numerically stable.

  // Addition, subtraction, scaling, and division by one preserve values exactly for float.
  "arith/identity_inverse/vf"_test = [] {
    Vf v{ 1.5f, -2.0f, 3.25f }, w{ -0.5f, 4.0f, 1.0f };
    Vf z{ 0.0f, 0.0f, 0.0f };
    float one = 1.0f, s = 2.5f;
    expect((v + z)[0] == v[0] && (v + z)[1] == v[1] && (v + z)[2] == v[2]);
    expect((v - z)[0] == v[0] && (v - z)[1] == v[1] && (v - z)[2] == v[2]);
    expect((v * one)[0] == v[0] && (v * one)[1] == v[1] && (v * one)[2] == v[2]);
    expect((v / one)[0] == v[0] && (v / one)[1] == v[1] && (v / one)[2] == v[2]);
    expect(((v + w) - w)[0] == v[0] && ((v + w) - w)[1] == v[1] && ((v + w) - w)[2] == v[2]);
    expect(((v * s) / s)[0] == v[0] && ((v * s) / s)[1] == v[1] && ((v * s) / s)[2] == v[2]);
    };

  // Addition, subtraction, scaling, and division by one preserve values exactly for double.
  "arith/identity_inverse/vd"_test = [] {
    Vd v{ 1.5, -2.0, 3.25 }, w{ -0.5, 4.0, 1.0 };
    Vd z{ 0.0, 0.0, 0.0 };
    double one = 1.0, s = 2.5;
    expect((v + z)[0] == v[0] && (v + z)[1] == v[1] && (v + z)[2] == v[2]);
    expect((v - z)[0] == v[0] && (v - z)[1] == v[1] && (v - z)[2] == v[2]);
    expect((v * one)[0] == v[0] && (v * one)[1] == v[1] && (v * one)[2] == v[2]);
    expect((v / one)[0] == v[0] && (v / one)[1] == v[1] && (v / one)[2] == v[2]);
    expect(((v + w) - w)[0] == v[0] && ((v + w) - w)[1] == v[1] && ((v + w) - w)[2] == v[2]);
    expect(((v * s) / s)[0] == v[0] && ((v * s) / s)[1] == v[1] && ((v * s) / s)[2] == v[2]);
    };

  // Division by signed zero yields infinities or NaNs per IEEE-754, ensuring defined edge-case behavior.
  "arith/div_zero_semantics"_test = [] {
    Vf v{ 1.0f,-2.0f,0.0f };
    Vd w{ 1.0,-2.0,0.0 };
    Vf vf_pos = v / 0.0f, vf_neg = v / -0.0f;
    Vd vd_pos = w / 0.0, vd_neg = w / -0.0;
    expect(std::isinf(vf_pos[0]) && std::isinf(vf_neg[0]));
    expect(std::isinf(vd_pos[0]) && std::isinf(vd_neg[0]));
    expect(isNaN(vf_pos[2]) && isNaN(vf_neg[2]));
    expect(isNaN(vd_pos[2]) && isNaN(vd_neg[2]));
    };

  // Scalar distributivity over vector addition holds numerically for both precisions under tolerance.
  "arith/distributive_sanity"_test = [] {
    Vf a{ 3.0f,-4.0f,5.0f }, b{ -2.0f,6.0f,1.0f }; float s{ 7.0f };
    Vd A{ 3.0,-4.0,5.0 }, B{ -2.0,6.0,1.0 }; double S{ 7.0 };
    Vf lhsf = (a + b) * s, rhsf = (a * s) + (b * s);
    Vd lhsd = (A + B) * S, rhsd = (A * S) + (B * S);
    for (int i = 0; i < 3; ++i) { expect(almostEqual(lhsf[i], rhsf[i])); expect(almostEqual(lhsd[i], rhsd[i])); }
    };

  // Random stress validates that each arithmetic operator matches its componentwise scalar counterpart for float.
  "arith/randomized_vf"_test = [] {
    std::mt19937_64 eng{ 0xA11CEB00ULL };
    for (int t = 0; t < 20000; ++t) {
      Vf x{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      Vf y{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      float s = std::uniform_real_distribution<float>(-1e3f, 1e3f)(eng);
      for (int i = 0; i < 3; ++i) {
        expect(almostEqual((x + y)[i], x[i] + y[i]));
        expect(almostEqual((x - y)[i], x[i] - y[i]));
        expect(almostEqual((x * s)[i], x[i] * s));
        if (s != 0.0f) expect(almostEqual((x / s)[i], x[i] / s));
      }
    }
    };

  // Random stress validates that each arithmetic operator matches its componentwise scalar counterpart for double.
  "arith/randomized_vd"_test = [] {
    std::mt19937_64 eng{ 0x00BADA55ULL };
    for (int t = 0; t < 20000; ++t) {
      Vd x{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      Vd y{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      double s{ std::uniform_real_distribution<double>(-1e6, 1e6)(eng) };
      for (int i = 0; i < 3; ++i) {
        expect(almostEqual((x + y)[i], x[i] + y[i]));
        expect(almostEqual((x - y)[i], x[i] - y[i]));
        expect(almostEqual((x * s)[i], x[i] * s));
        if (s != 0.0) expect(almostEqual((x / s)[i], x[i] / s));
      }
    }
    };

  // Compound assignment matches non-compound reference expressions for float, ensuring in-place ops are correct.
  "arith/compound_assign/vf"_test = [] {
    Vf a{ 1.25f,-2.5f,3.75f }, b{ -4.0f,5.0f,-6.0f }; float s = -2.0f;
    Vf u = a; u += b; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (a + b)[i]));
    u = a; u -= b; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (a - b)[i]));
    u = a; u *= s; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (a * s)[i]));
    u = a; u /= s; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (a / s)[i]));
    };

  // Compound assignment matches non-compound reference expressions for double, ensuring in-place ops are correct.
  "arith/compound_assign/vd"_test = [] {
    Vd a{ 1.25,-2.5,3.75 }, b{ -4.0,5.0,-6.0 }; double s = 2.0;
    Vd u = a; u += b; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (a + b)[i]));
    u = a; u -= b; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (a - b)[i]));
    u = a; u *= s; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (a * s)[i]));
    u = a; u /= s; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (a / s)[i]));
    };

  // Compound ops are robust to self-aliasing for float, validating u op= u behavior.
  "arith/compound_selfalias/vf"_test = [] {
    Vf v{ 3.0f,-4.0f,5.0f }; float s = -3.0f;
    Vf u = v; u += u; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (v + v)[i]));
    u = v; u -= u; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], 0.0f));
    u = v; u *= s; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (v * s)[i]));
    u = v; u /= s; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (v / s)[i]));
    };

  // Compound ops are robust to self-aliasing for double, validating u op= u behavior.
  "arith/compound_selfalias/vd"_test = [] {
    Vd v{ 3.0,-4.0,5.0 }; double s = 3.5;
    Vd u = v; u += u; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (v + v)[i]));
    u = v; u -= u; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], 0.0));
    u = v; u *= s; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (v * s)[i]));
    u = v; u /= s; for (int i = 0; i < 3; ++i) expect(almostEqual(u[i], (v / s)[i]));
    };

  // Compound ops return self reference for chaining in float, asserting fluent interface semantics.
  "arith/compound_chain_return/vf"_test = [] {
    Vf u{ 1,2,3 }, a{ 4,5,6 }; float s = 2.0f;
    auto* p = &u; auto& r1 = (u += a); expect(p == &r1);
    auto& r2 = (u -= a); expect(p == &r2);
    auto& r3 = (u *= s); expect(p == &r3);
    auto& r4 = (u /= s); expect(p == &r4);
    };

  // Compound ops return self reference for chaining in double, asserting fluent interface semantics.
  "arith/compound_chain_return/vd"_test = [] {
    Vd u{ 1,2,3 }, a{ 4,5,6 }; double s = 2.0;
    auto* p = &u; auto& r1 = (u += a); expect(p == &r1);
    auto& r2 = (u -= a); expect(p == &r2);
    auto& r3 = (u *= s); expect(p == &r3);
    auto& r4 = (u /= s); expect(p == &r4);
    };

  // Validates left-right scalar multiplication symmetry for float to ensure free operator*(scalar,vec) matches member.
  "arith/scalar_symmetry_left_right"_test = [] {
    std::mt19937_64 eng{ 0x51DEADBEEULL };
    for (int t = 0; t < 20000; ++t) {
      float s = std::uniform_real_distribution<float>(-1e3f, 1e3f)(eng);
      Vf v{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      for (int i = 0; i < 3; ++i) expect(almostEqual((v * s)[i], (s * v)[i]));
    }
    };

  // Validates commutativity of addition under floating point rounding using tolerance for double.
  "arith/plus_commutativity_randomized"_test = [] {
    std::mt19937_64 eng{ 0x00C0FFEEULL };
    for (int t = 0; t < 20000; ++t) {
      Vd a{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      Vd b{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      for (int i = 0; i < 3; ++i) expect(almostEqual((a + b)[i], (b + a)[i]));
    }
    };

  // Validates that subtraction is anti-commutative up to a sign under rounding for float.
  "arith/minus_anti_commutativity"_test = [] {
    std::mt19937_64 eng{ 0xFEEDFACEULL };
    for (int t = 0; t < 20000; ++t) {
      Vf a{ std::uniform_real_distribution<float>(-1e4f,1e4f)(eng),
            std::uniform_real_distribution<float>(-1e4f,1e4f)(eng),
            std::uniform_real_distribution<float>(-1e4f,1e4f)(eng) };
      Vf b{ std::uniform_real_distribution<float>(-1e4f,1e4f)(eng),
            std::uniform_real_distribution<float>(-1e4f,1e4f)(eng),
            std::uniform_real_distribution<float>(-1e4f,1e4f)(eng) };
      for (int i = 0; i < 3; ++i) expect(almostEqual((a - b)[i], (-(b - a))[i]));
    }
    };

  // Validates scalar distributivity over subtraction for double with random stress and tolerance.
  "arith/distributivity_subtraction_randomized"_test = [] {
    std::mt19937_64 eng{ 0xB16B00B5ULL };
    for (int t = 0; t < 20000; ++t) {
      double s = std::uniform_real_distribution<double>(-1e3, 1e3)(eng);
      Vd a{ std::uniform_real_distribution<double>(-1e3,1e3)(eng),
            std::uniform_real_distribution<double>(-1e3,1e3)(eng),
            std::uniform_real_distribution<double>(-1e3,1e3)(eng) };
      Vd b{ std::uniform_real_distribution<double>(-1e3,1e3)(eng),
            std::uniform_real_distribution<double>(-1e3,1e3)(eng),
            std::uniform_real_distribution<double>(-1e3,1e3)(eng) };
      for (int i = 0; i < 3; ++i) expect(almostEqual((s * (a - b))[i], ((s * a - s * b))[i]));
    }
    };

  // End-to-end compound sequence for float ensures cumulative rounding remains bounded and reversible.
  "arith/randomized_compound_consistency_vf"_test = [] {
    std::mt19937_64 eng{ 0xA11C3D00ULL };
    constexpr float eps = std::numeric_limits<float>::epsilon();
    for (int t = 0; t < 20000; ++t) {
      Vf a{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      Vf b{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
            std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      float s = std::uniform_real_distribution<float>(-1e3f, 1e3f)(eng);

      Vf u = a;
      u += b;
      for (int i = 0; i < 3; ++i) {
        float ref = (a + b)[i];
        float scale = std::max({ std::abs(a[i]), std::abs(b[i]), std::abs(ref) });
        Tol tol{ double(16.0f * eps * std::max(scale,1.0f)), double(8.0f * eps), 4 };
        expect(almostEqual(u[i], ref, tol));
      }

      u -= b;
      for (int i = 0; i < 3; ++i) {
        float scale = std::max(std::abs(a[i]), std::abs(b[i]));
        Tol tol{ double(32.0f * eps * std::max(scale,1.0f)), double(16.0f * eps), 6 };
        expect(almostEqual(u[i], a[i], tol));
      }

      u *= s;
      for (int i = 0; i < 3; ++i) {
        float ref = (a * s)[i];
        float scale = std::max(std::abs(ref), std::abs(u[i]));
        Tol tol{ double(48.0f * eps * std::max(scale,1.0f)), double(24.0f * eps), 6 };
        expect(almostEqual(u[i], ref, tol));
      }

      if (s != 0.0f) {
        u /= s;
        for (int i = 0; i < 3; ++i) {
          float scale = std::max(std::abs(a[i]), 1.0f);
          Tol tol{ double(64.0f * eps * scale), double(32.0f * eps), 8 };
          expect(almostEqual(u[i], a[i], tol));
        }
      }
    }
    };

  // End-to-end compound sequence for double ensures cumulative rounding remains bounded and reversible.
  "arith/randomized_compound_consistency_vd"_test = [] {
    std::mt19937_64 eng{ 0xBADA5500ULL };
    constexpr double eps = std::numeric_limits<double>::epsilon();
    for (int t = 0; t < 20000; ++t) {
      Vd a{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      Vd b{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng),
            std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      double s = std::uniform_real_distribution<double>(-1e6, 1e6)(eng);

      Vd u = a;
      u += b;
      for (int i = 0; i < 3; ++i) {
        double ref = (a + b)[i];
        double scale = std::max({ std::abs(a[i]), std::abs(b[i]), std::abs(ref) });
        Tol tol{ 16.0 * eps * std::max(1.0,scale), 8.0 * eps, 4 };
        expect(almostEqual(u[i], ref, tol));
      }

      u -= b;
      for (int i = 0; i < 3; ++i) {
        double scale = std::max(std::abs(a[i]), std::abs(b[i]));
        Tol tol{ 32.0 * eps * std::max(1.0,scale), 16.0 * eps, 6 };
        expect(almostEqual(u[i], a[i], tol));
      }

      u *= s;
      for (int i = 0; i < 3; ++i) {
        double ref = (a * s)[i];
        double scale = std::max(std::abs(ref), std::abs(u[i]));
        Tol tol{ 48.0 * eps * std::max(1.0,scale), 24.0 * eps, 6 };
        expect(almostEqual(u[i], ref, tol));
      }

      if (s != 0.0) {
        u /= s;
        for (int i = 0; i < 3; ++i) {
          double scale = std::max(std::abs(a[i]), 1.0);
          Tol tol{ 64.0 * eps * scale, 32.0 * eps, 8 };
          expect(almostEqual(u[i], a[i], tol));
        }
      }
    }
    };

  std::println("Arithmetic operations tests passed...");
}

// Equality, indexing, and unary-minus semantics on finite inputs; critical to ensure exact- or bitwise-equality invariants match operator== policy and that indexing returns proper references.
// These tests avoid NaNs and mixed +0/-0 comparisons to remain implementation-agnostic for floating equality that is exact/bitwise.

void vecEqualityIndexUnaryTests() 
{
  // Reflexivity and symmetry for ordinary finite values; critical property for any equality relation used in containers or caching.
  "eq/reflexivity_symmetry_basic/vf"_test = [] {
    Vf a{ 1.25f, -2.5f, 3.75f };
    Vf b = a;
    expect(a == a);
    expect(b == b);
    expect(a == b);
    expect(b == a);
    };

  // Reflexivity and symmetry for doubles; critical for algorithms relying on associative maps or memoization keyed by vectors.
  "eq/reflexivity_symmetry_basic/vd"_test = [] {
    Vd a{ 1.25, -2.5, 3.75 };
    Vd b = a;
    expect(a == a);
    expect(b == b);
    expect(a == b);
    expect(b == a);
    };

  // Inequality when a single component changes by an ordinary delta; critical to guarantee distinct states are not coalesced.
  "eq/inequality_single_component/vf"_test = [] {
    Vf a{ 3.0f, -4.0f, 5.0f };
    Vf b = a; b[1] = b[1] + 1.0f;
    Vf c = a; c[0] = c[0] - 2.0f;
    Vf d = a; d[2] = d[2] * 1.1f;
    expect(!(a == b));
    expect(!(a == c));
    expect(!(a == d));
    };

  // Inequality for doubles with deliberate per-component changes; critical for correctness of spatial hashing and BVH updates.
  "eq/inequality_single_component/vd"_test = [] {
    Vd a{ 3.0, -4.0, 5.0 };
    Vd b = a; b[1] = b[1] + 1.0;
    Vd c = a; c[0] = c[0] - 2.0;
    Vd d = a; d[2] = d[2] * 1.1;
    expect(!(a == b));
    expect(!(a == c));
    expect(!(a == d));
    };

  // Randomized single-ULP style mutation for floats using nextafter; critical to verify strict equality rejects minimally changed states.
  "eq/randomized_mutation/vf"_test = [] {
    std::mt19937_64 eng{ 0xE11ECA5E5EEDULL };
    std::uniform_real_distribution<float> dist(-1e5f, 1e5f);
    for (int t = 0; t < 30000; ++t) {
      Vf a{ dist(eng), dist(eng), dist(eng) };
      Vf b = a;
      expect(a == b);
      int idx = int(std::uniform_int_distribution<int>(0, 2)(eng));
      if (b[idx] == 0.0f) {
        b[idx] = std::numeric_limits<float>::denorm_min();
      }
      else {
        float toward = std::signbit(b[idx]) ? -std::numeric_limits<float>::infinity()
          : std::numeric_limits<float>::infinity();
        b[idx] = std::nextafter(b[idx], toward);
      }
      expect(!(a == b));
    }
    };

  // Randomized single-ULP style mutation for doubles using nextafter; critical to ensure exact equality is not tolerant by accident.
  "eq/randomized_mutation/vd"_test = [] {
    std::mt19937_64 eng{ 0xBEEF1234C0FFEE99ULL };
    std::uniform_real_distribution<double> dist(-1e8, 1e8);
    for (int t = 0; t < 30000; ++t) {
      Vd a{ dist(eng), dist(eng), dist(eng) };
      Vd b = a;
      expect(a == b);
      int idx = int(std::uniform_int_distribution<int>(0, 2)(eng));
      if (b[idx] == 0.0) {
        b[idx] = std::numeric_limits<double>::denorm_min();
      }
      else {
        double toward = std::signbit(b[idx]) ? -std::numeric_limits<double>::infinity()
          : std::numeric_limits<double>::infinity();
        b[idx] = std::nextafter(b[idx], toward);
      }
      expect(!(a == b));
    }
    };

  // Indexing must support read and chained write semantics; critical for ergonomic math code and in-place updates.
  "index/read_write_chained/vf"_test = [] {
    Vf v{ 0.0f, 0.0f, 0.0f };
    v[0] = 1.25f; v[1] = -4.5f; v[2] = 9.0f;
    expect(v[0] == 1.25f && v[1] == -4.5f && v[2] == 9.0f);
    v[0] = v[1] = v[2] = 3.5f;
    expect(v[0] == 3.5f && v[1] == 3.5f && v[2] == 3.5f);
    };

  // Double-precision indexing read and chained write; critical to mirror float behavior for consistency across T.
  "index/read_write_chained/vd"_test = [] {
    Vd v{ 0.0, 0.0, 0.0 };
    v[0] = 1.25; v[1] = -4.5; v[2] = 9.0;
    expect(v[0] == 1.25 && v[1] == -4.5 && v[2] == 9.0);
    v[0] = v[1] = v[2] = 3.5;
    expect(v[0] == 3.5 && v[1] == 3.5 && v[2] == 3.5);
    };

  // Const correctness and contiguous element layout; important for interop with SIMD and external libraries.
  "index/const_read_and_aliasing/vfvd"_test = [] {
    Vf v{ 2.0f, -3.0f, 4.0f };
    const Vf& cv = v;
    expect(cv[0] == 2.0f && cv[1] == -3.0f && cv[2] == 4.0f);
    auto* p0 = &v[0]; auto* p1 = &v[1]; auto* p2 = &v[2];
    expect(p0 != p1 && p1 != p2 && p0 != p2);
    expect(p0 + 1 == p1 && p1 + 1 == p2);

    Vd w{ -5.0, 6.0, 7.5 };
    const Vd& cw = w;
    expect(cw[0] == -5.0 && cw[1] == 6.0 && cw[2] == 7.5);
    auto* q0 = &w[0]; auto* q1 = &w[1]; auto* q2 = &w[2];
    expect(q0 != q1 && q1 != q2 && q0 != q2);
    expect(q0 + 1 == q1 && q1 + 1 == q2);
    };

  // Reference category of operator[] must be lvalue-ref for non-const and const-ref for const; critical for API correctness and move avoidance.
  "index/type_properties/vfvd"_test = [] {
    static_assert(std::same_as<decltype(std::declval<Vf&>()[0]), float&>);
    static_assert(std::same_as<decltype(std::declval<const Vf&>()[0]), const float&>);
    static_assert(std::same_as<decltype(std::declval<Vd&>()[0]), double&>);
    static_assert(std::same_as<decltype(std::declval<const Vd&>()[0]), const double&>);
    expect(true);
    };

  // Unary minus must invert signs elementwise and be an involution; critical for stable normal and direction manipulations.
  "unary/elementwise_and_double_neg/vf"_test = [] {
    Vf v{ 1.5f, -2.0f, 0.0f };
    Vf u = -v;
    expect(u[0] == -1.5f && u[1] == 2.0f && u[2] == -0.0f);
    Vf w = -u;
    expect(w == v);

    Vf z{ -0.0f, 0.0f, -0.0f };
    Vf nz = -z;
    expect(nz[0] == 0.0f && !isNegZero(nz[0]));
    expect(nz[1] == -0.0f && isNegZero(nz[1]));
    expect(nz[2] == 0.0f && !isNegZero(nz[2]));
    };

  // Double-precision unary minus involution and signed-zero handling; important for reproducibility across precisions.
  "unary/elementwise_and_double_neg/vd"_test = [] {
    Vd v{ 1.5, -2.0, 0.0 };
    Vd u = -v;
    expect(u[0] == -1.5 && u[1] == 2.0 && u[2] == -0.0);
    Vd w = -u;
    expect(w == v);

    Vd z{ -0.0, 0.0, -0.0 };
    Vd nz = -z;
    expect(nz[0] == 0.0 && !isNegZero(nz[0]));
    expect(nz[1] == -0.0 && isNegZero(nz[1]));
    expect(nz[2] == 0.0 && !isNegZero(nz[2]));
    };

  // Distributivity of unary minus over addition and scalar multiplication with tolerance; important algebraic sanity for transformations.
  "unary/distributivity_float"_test = [] {
    std::mt19937_64 eng{ 0xA11CE55ULL };
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    constexpr float eps = std::numeric_limits<float>::epsilon();
    for (int t = 0; t < 30000; ++t) {
      Vf a{ df(eng), df(eng), df(eng) }, b{ df(eng), df(eng), df(eng) };
      float s = df(eng);
      Vf lhs1 = -(a + b), rhs1 = (-a) + (-b);
      Vf lhs2 = -(s * a), rhs2 = (-s) * a;
      for (int i = 0; i < 3; ++i) {
        float scale1 = std::max(std::abs(lhs1[i]), std::abs(rhs1[i]));
        Tol t1{ double(8.0f * eps * std::max(1.0f, scale1)), double(4.0f * eps), 4 };
        expect(almostEqual(lhs1[i], rhs1[i], t1));
        float scale2 = std::max(std::abs(lhs2[i]), std::abs(rhs2[i]));
        Tol t2{ double(8.0f * eps * std::max(1.0f, scale2)), double(4.0f * eps), 4 };
        expect(almostEqual(lhs2[i], rhs2[i], t2));
      }
    }
    };

  // Double-precision distributivity of unary minus with tolerance; important for robust shading math and accumulation paths.
  "unary/distributivity_double"_test = [] {
    std::mt19937_64 eng{ 0xBADA5511ULL };
    std::uniform_real_distribution<double> dd(-1e6, 1e6);
    constexpr double eps = std::numeric_limits<double>::epsilon();
    for (int t = 0; t < 30000; ++t) {
      Vd a{ dd(eng), dd(eng), dd(eng) }, b{ dd(eng), dd(eng), dd(eng) };
      double s = dd(eng);
      Vd lhs1 = -(a + b), rhs1 = (-a) + (-b);
      Vd lhs2 = -(s * a), rhs2 = (-s) * a;
      for (int i = 0; i < 3; ++i) {
        double scale1 = std::max(std::abs(lhs1[i]), std::abs(rhs1[i]));
        Tol t1{ 8.0 * eps * std::max(1.0, scale1), 4.0 * eps, 4 };
        expect(almostEqual(lhs1[i], rhs1[i], t1));
        double scale2 = std::max(std::abs(lhs2[i]), std::abs(rhs2[i]));
        Tol t2{ 8.0 * eps * std::max(1.0, scale2), 4.0 * eps, 4 };
        expect(almostEqual(lhs2[i], rhs2[i], t2));
      }
    }
    };

  std::println("Equality, index, and unary tests passed...");
}

// Free-function properties for dot, length, normalize, and cross; critical to validate algebraic identities and numerical behavior relied upon by rendering kernels and geometry utilities.
void vecFreeFunctionTests() 
{
  // Dot is commutative and self-dot equals squared length; critical for inner-product correctness.
  "dot/commutativity_and_selfsq/vf"_test = [] {
    std::mt19937_64 eng{ 0x9E3779B97F4A7C15ULL };
    Tol tf{ 1e-6, 1e-5, 6 };
    for (int trial = 0; trial < 50000; ++trial) {
      Vf a{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      Vf b{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      float ab = computeDot(a, b);
      float ba = computeDot(b, a);
      float aa = computeDot(a, a);
      expect(almostEqual(ab, ba, tf));
      expect(almostEqual(aa, euclideanLengthSq(a), tf));
    }
    };

  // Dot is commutative and self-dot equals squared length in double; critical for high-dynamic-range inputs.
  "dot/commutativity_and_selfsq/vd"_test = [] {
    std::mt19937_64 eng{ 0xC6A4A7935BD1E995ULL };
    Tol td{ 1e-12, 1e-10, 8 };
    for (int trial = 0; trial < 50000; ++trial) {
      Vd a{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
           std::uniform_real_distribution<double>(-1e6,1e6)(eng),
           std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      Vd b{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
           std::uniform_real_distribution<double>(-1e6,1e6)(eng),
           std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      double ab = computeDot(a, b);
      double ba = computeDot(b, a);
      double aa = computeDot(a, a);
      expect(almostEqual(ab, ba, td));
      expect(almostEqual(aa, euclideanLengthSq(a), td));
    }
    };

  // Parallelogram law for squared lengths; critical as a structural identity of inner-product spaces.
  "length/parallelogram_law/vf"_test = [] {
    std::mt19937_64 eng{ 0x2545F4914F6CDD1DULL };
    Tol t{ 1e-5, 1e-5, 8 };
    for (int trial = 0; trial < 30000; ++trial) {
      Vf a{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      Vf b{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      float lhs = euclideanLengthSq(a + b) + euclideanLengthSq(a - b);
      float rhs = 2.0f * euclideanLengthSq(a) + 2.0f * euclideanLengthSq(b);
      expect(almostEqual(lhs, rhs, t));
    }
    };

  // Parallelogram law in double precision; critical for numerical stability in large-scale transforms.
  "length/parallelogram_law/vd"_test = [] {
    std::mt19937_64 eng{ 0x94D049BB133111EBULL };
    Tol t{ 1e-12, 1e-10, 8 };
    for (int trial = 0; trial < 30000; ++trial) {
      Vd a{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
           std::uniform_real_distribution<double>(-1e6,1e6)(eng),
           std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      Vd b{ std::uniform_real_distribution<double>(-1e6,1e6)(eng),
           std::uniform_real_distribution<double>(-1e6,1e6)(eng),
           std::uniform_real_distribution<double>(-1e6,1e6)(eng) };
      double lhs = euclideanLengthSq(a + b) + euclideanLengthSq(a - b);
      double rhs = 2.0 * euclideanLengthSq(a) + 2.0 * euclideanLengthSq(b);
      expect(almostEqual(lhs, rhs, t));
    }
    };

  // Cauchy–Schwarz inequality for floats; critical bound for angles and projections.
  "dot/cauchy_schwarz/vf"_test = [] {
    std::mt19937_64 eng{ 0x517CC1B727220A95ULL };
    Tol t{};
    for (int trial = 0; trial < 30000; ++trial) {
      Vf a{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      Vf b{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      float lhs = std::abs(computeDot(a, b));
      float rhs = euclideanLength(a) * euclideanLength(b);
      expect(lhs <= rhs || almostEqual(lhs, rhs, t));
    }
    };

  // Cauchy–Schwarz inequality for doubles; critical for robust cosine computations.
  "dot/cauchy_schwarz/vd"_test = [] {
    std::mt19937_64 eng{ 0xA24F4F5A1B3C9D07ULL };
    std::uniform_real_distribution<double> dist(-1e6, 1e6);
    Tol td{ 1e-12, 1e-10, 8 };
    for (int trial = 0; trial < 30000; ++trial) {
      Vd a{ dist(eng), dist(eng), dist(eng) };
      Vd b{ dist(eng), dist(eng), dist(eng) };
      double lhs = std::abs(computeDot(a, b));
      double rhs = euclideanLength(a) * euclideanLength(b);
      expect(lhs <= rhs || almostEqual(lhs, rhs, td));
    }
    };

  // Normalization produces unit length and preserves direction for floats; critical for normals and directions.
  "normalize/unit_length_and_direction/vf"_test = [] {
    std::mt19937_64 eng{ 0xDA942042E4DD58B5ULL };
    constexpr float eps = std::numeric_limits<float>::epsilon();
    for (int trial = 0; trial < 30000; ++trial) {
      Vf v{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      float lsq = euclideanLengthSq(v);
      if (lsq <= 1e-20f) continue;
      Vf n = normalize(v);
      Tol t_len{ double(64.0f * eps), double(32.0f * eps), 8 };
      expect(almostEqual(euclideanLength(n), 1.0f, t_len));
      float dv = computeDot(n, v);
      float mag = euclideanLength(v);
      Tol t_dir{ double(128.0f * eps * mag), double(64.0f * eps), 8 };
      expect(almostEqual(dv, mag, t_dir));
    }
    };

  // Zero vector normalization policy for floats; critical to avoid NaNs in downstream pipelines.
  "normalize/zero_policy/vf"_test = [] {
    Vf z{ 0.0f,0.0f,0.0f };
    Vf n = normalize(z);
    expect(n[0] == 0.0f && n[1] == 0.0f && n[2] == 0.0f);
    };

  // Normalization scale invariance for floats; critical when upstream scales vary.
  "normalize/scale_invariance/vf"_test = [] {
    std::mt19937_64 eng{ 0xD1B54A32D192ED03ULL };
    constexpr float eps = std::numeric_limits<float>::epsilon();
    for (int trial = 0; trial < 30000; ++trial) {
      Vf v{ std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng),
           std::uniform_real_distribution<float>(-1e3f,1e3f)(eng) };
      float s = std::uniform_real_distribution<float>(-1e3f, 1e3f)(eng);
      float lsq = euclideanLengthSq(v);
      if (lsq <= 1e-20f || s == 0.0f) continue;
      Vf n1 = normalize(v);
      Vf n2 = normalize(v * s);
      Vf ref = (s > 0.0f) ? n1 : (n1 * -1.0f);
      for (int i = 0; i < 3; ++i) {
        Tol t{ double(64.0f * eps), double(64.0f * eps), 8 };
        expect(almostEqual(n2[i], ref[i], t));
      }
    }
    };

  // Normalization produces unit length and preserves direction for doubles; critical for precision-sensitive tasks.
  "normalize/unit_length_and_direction/vd"_test = [] {
    std::mt19937_64 eng{ 0xC1357BDF8E9A2461ULL };
    constexpr double eps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<double> dist(-1e6, 1e6);
    for (int trial = 0; trial < 30000; ++trial) {
      Vd v{ dist(eng), dist(eng), dist(eng) };
      double lsq = euclideanLengthSq(v);
      if (lsq <= 1e-40) continue;
      Vd n = normalize(v);
      Tol t_len{ 64.0 * eps, 32.0 * eps, 8 };
      expect(almostEqual(euclideanLength(n), 1.0, t_len));
      double dv = computeDot(n, v);
      double mag = euclideanLength(v);
      Tol t_dir{ 128.0 * eps * std::max(1.0, mag), 64.0 * eps, 8 };
      expect(almostEqual(dv, mag, t_dir));
    }
    };

  // Zero vector normalization policy for doubles; critical for stable default behavior.
  "normalize/zero_policy/vd"_test = [] {
    Vd z{ 0.0, 0.0, 0.0 };
    Vd n = normalize(z);
    expect(n[0] == 0.0 && n[1] == 0.0 && n[2] == 0.0);
    };

  // Normalization scale invariance for doubles; critical when world units differ across assets.
  "normalize/scale_invariance/vd"_test = [] {
    std::mt19937_64 eng{ 0x9C0FFEE0DADA5533ULL };
    constexpr double eps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<double> dist(-1e6, 1e6);
    std::uniform_real_distribution<double> scal(-1e6, 1e6);
    for (int trial = 0; trial < 30000; ++trial) {
      Vd v{ dist(eng), dist(eng), dist(eng) };
      double s = scal(eng);
      double lsq = euclideanLengthSq(v);
      if (lsq <= 1e-40 || s == 0.0) continue;
      Vd n1 = normalize(v);
      Vd n2 = normalize(v * s);
      Vd ref = (s > 0.0) ? n1 : (n1 * -1.0);
      for (int i = 0; i < 3; ++i) {
        Tol t{ 64.0 * eps, 64.0 * eps, 8 };
        expect(almostEqual(n2[i], ref[i], t));
      }
    }
    };

  // Right-handed basis cross products i×j=k, j×k=i, k×i=j; critical for orientation conventions.
  "cross/basis_right_hand/vfvd"_test = [] {
    Vf i{ 1.f,0.f,0.f }, j{ 0.f,1.f,0.f }, k{ 0.f,0.f,1.f };
    Vd I{ 1.0,0.0,0.0 }, J{ 0.0,1.0,0.0 }, K{ 0.0,0.0,1.0 };
    Vf ij = computeCross(i, j); for (int c = 0; c < 3; ++c) expect(almostEqual(ij[c], k[c], Tol{}));
    Vf jk = computeCross(j, k); for (int c = 0; c < 3; ++c) expect(almostEqual(jk[c], i[c], Tol{}));
    Vf ki = computeCross(k, i); for (int c = 0; c < 3; ++c) expect(almostEqual(ki[c], j[c], Tol{}));
    Vd IJ = computeCross(I, J); for (int c = 0; c < 3; ++c) expect(almostEqual(IJ[c], K[c], Tol{}));
    Vd JK = computeCross(J, K); for (int c = 0; c < 3; ++c) expect(almostEqual(JK[c], I[c], Tol{}));
    Vd KI = computeCross(K, I); for (int c = 0; c < 3; ++c) expect(almostEqual(KI[c], J[c], Tol{}));
    };

  // Antisymmetry a×b = −(b×a) for floats; critical identity for torque and area computations.
  "cross/antisymmetry/vf"_test = [] {
    std::mt19937_64 eng{ 0xC0DECAFEULL };
    constexpr float eps = std::numeric_limits<float>::epsilon();
    std::uniform_real_distribution<float> dist(-1e3f, 1e3f);
    for (int t = 0; t < 30000; ++t) {
      Vf a{ dist(eng),dist(eng),dist(eng) };
      Vf b{ dist(eng),dist(eng),dist(eng) };
      Vf ab = computeCross(a, b);
      Vf ba = computeCross(b, a);
      for (int i = 0; i < 3; ++i) {
        float scale = std::max(std::abs(ab[i]), std::abs(ba[i]));
        Tol tol{ double(32.0f * eps * std::max(scale, 1.0f)), double(16.0f * eps), 6 };
        expect(almostEqual(ab[i], -ba[i], tol));
      }
    }
    };

  // Antisymmetry a×b = −(b×a) for doubles; critical for precision physics and constraints.
  "cross/antisymmetry/vd"_test = [] {
    std::mt19937_64 eng{ 0xD0EFAACEBEEFABCDULL };
    constexpr double eps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<double> dist(-1e6, 1e6);
    for (int t = 0; t < 30000; ++t) {
      Vd a{ dist(eng),dist(eng),dist(eng) };
      Vd b{ dist(eng),dist(eng),dist(eng) };
      Vd ab = computeCross(a, b);
      Vd ba = computeCross(b, a);
      for (int i = 0; i < 3; ++i) {
        double scale = std::max(std::abs(ab[i]), std::abs(ba[i]));
        Tol tol{ 32.0 * eps * std::max(1.0, scale), 16.0 * eps, 6 };
        expect(almostEqual(ab[i], -ba[i], tol));
      }
    }
    };

  // Perpendicularity a·(a×b)=0 and b·(a×b)=0 for floats with near-parallel pairs skipped; critical for normal orthogonality.
  "cross/perpendicularity/vf"_test = [] {
    std::mt19937_64 eng{ 0x5EEDFACEULL };
    constexpr float eps = std::numeric_limits<float>::epsilon();
    std::uniform_real_distribution<float> dist(-1e3f, 1e3f);
    for (int t = 0; t < 30000; ++t) {
      Vf a{ dist(eng),dist(eng),dist(eng) };
      Vf b{ dist(eng),dist(eng),dist(eng) };
      Vf c = computeCross(a, b);
      float na = euclideanLength(a), nb = euclideanLength(b), nc = euclideanLength(c);
      if (!(isFinite(na) && isFinite(nb) && isFinite(nc))) continue;
      if (nc < 1e-6f * (na * nb)) continue;
      float da = computeDot(a, c), db = computeDot(b, c);
      Tol tA{ double(64.0f * eps * na * nc), double(32.0f * eps), 8 };
      Tol tB{ double(64.0f * eps * nb * nc), double(32.0f * eps), 8 };
      expect(almostEqual(da, 0.0f, tA));
      expect(almostEqual(db, 0.0f, tB));
    }
    };

  // Perpendicularity a·(a×b)=0 and b·(a×b)=0 for doubles with near-parallel pairs skipped; critical for geometric predicates.
  "cross/perpendicularity/vd"_test = [] {
    std::mt19937_64 eng{ 0xAABBCCDD11223344ULL };
    constexpr double eps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<double> dist(-1e6, 1e6);
    for (int t = 0; t < 30000; ++t) {
      Vd a{ dist(eng),dist(eng),dist(eng) };
      Vd b{ dist(eng),dist(eng),dist(eng) };
      Vd c = computeCross(a, b);
      double na = euclideanLength(a), nb = euclideanLength(b), nc = euclideanLength(c);
      if (!(isFinite(na) && isFinite(nb) && isFinite(nc))) continue;
      if (nc < 1e-12 * (na * nb)) continue;
      double da = computeDot(a, c), db = computeDot(b, c);
      Tol tA{ 64.0 * eps * na * nc, 32.0 * eps, 8 };
      Tol tB{ 64.0 * eps * nb * nc, 32.0 * eps, 8 };
      expect(almostEqual(da, 0.0, tA));
      expect(almostEqual(db, 0.0, tB));
    }
    };

  // Lagrange identity ||a×b||^2 = ||a||^2||b||^2 − (a·b)^2 for doubles with cancellation guarded; critical for area computations.
  "cross/lagrange_identity/vd"_test = [] {
    std::mt19937_64 eng{ 0x1D1E7711ULL };
    constexpr double eps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<double> dist(-1e6, 1e6);
    for (int t = 0; t < 40000; ++t) {
      Vd a{ dist(eng),dist(eng),dist(eng) };
      Vd b{ dist(eng),dist(eng),dist(eng) };
      if (!(isFinite(a[0]) && isFinite(a[1]) && isFinite(a[2]) &&
        isFinite(b[0]) && isFinite(b[1]) && isFinite(b[2]))) continue;
      Vd c = computeCross(a, b);
      double lhs = euclideanLengthSq(c);
      double aa = euclideanLengthSq(a);
      double bb = euclideanLengthSq(b);
      double ab = computeDot(a, b);
      if (!(isFinite(lhs) && isFinite(aa) && isFinite(bb) && isFinite(ab))) continue;
      if (lhs < 1e-24 * std::max(1.0, aa * bb)) continue;
      double rhs = aa * bb - ab * ab;
      double scaleTerms = std::abs(aa * bb) + std::abs(ab * ab) + std::abs(lhs) + 1.0;
      Tol tol{ 2048.0 * eps * scaleTerms, 256.0 * eps, 10 };
      expect(almostEqual(lhs, rhs, tol));
    }
    };

  // Cross-product scaling and distributivity for floats with norm bounds; important for linear-algebraic consistency.
  "cross/scaling_and_distributivity/vf"_test = [] {
    std::mt19937_64 eng{ 0x5C4EAD15ULL };
    constexpr float eps = std::numeric_limits<float>::epsilon();
    std::uniform_real_distribution<float> dist(-1e3f, 1e3f);
    std::uniform_real_distribution<float> scal(-1e3f, 1e3f);
    for (int t = 0; t < 25000; ++t) {
      Vf a{ dist(eng),dist(eng),dist(eng) };
      Vf b{ dist(eng),dist(eng),dist(eng) };
      Vf c{ dist(eng),dist(eng),dist(eng) };
      float k = scal(eng);
      Vf lhs1 = computeCross(k * a, b);
      Vf rhs1 = k * computeCross(a, b);
      Vf d1 = lhs1 - rhs1;
      float bound1 = 128.0f * eps * (euclideanLength(lhs1) + euclideanLength(rhs1) + std::abs(k) * euclideanLength(a) * euclideanLength(b)) + 1e-6f;
      expect(euclideanLength(d1) <= bound1);
      Vf lhs2 = computeCross(a, k * b);
      Vf rhs2 = k * computeCross(a, b);
      Vf d2 = lhs2 - rhs2;
      float bound2 = 128.0f * eps * (euclideanLength(lhs2) + euclideanLength(rhs2) + std::abs(k) * euclideanLength(a) * euclideanLength(b)) + 1e-6f;
      expect(euclideanLength(d2) <= bound2);
      Vf lhs3 = computeCross(a, b + c);
      Vf rhs3 = computeCross(a, b) + computeCross(a, c);
      Vf d3 = lhs3 - rhs3;
      float bound3 = 192.0f * eps * (euclideanLength(lhs3) + euclideanLength(rhs3) + euclideanLength(a) * (euclideanLength(b) + euclideanLength(c))) + 1e-6f;
      expect(euclideanLength(d3) <= bound3);
      Vf lhs4 = computeCross(a + b, c);
      Vf rhs4 = computeCross(a, c) + computeCross(b, c);
      Vf d4 = lhs4 - rhs4;
      float bound4 = 192.0f * eps * (euclideanLength(lhs4) + euclideanLength(rhs4) + (euclideanLength(a) + euclideanLength(b)) * euclideanLength(c)) + 1e-6f;
      expect(euclideanLength(d4) <= bound4);
    }
    };

  // Cross-product scaling and distributivity for doubles with norm bounds; important for precise differential operations.
  "cross/scaling_and_distributivity/vd"_test = [] {
    std::mt19937_64 eng{ 0x55AA33CC77EE99FFULL };
    constexpr double eps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<double> dist(-1e6, 1e6);
    std::uniform_real_distribution<double> scal(-1e6, 1e6);
    for (int t = 0; t < 20000; ++t) {
      Vd a{ dist(eng),dist(eng),dist(eng) };
      Vd b{ dist(eng),dist(eng),dist(eng) };
      Vd c{ dist(eng),dist(eng),dist(eng) };
      double k = scal(eng);
      Vd lhs1 = computeCross(k * a, b), rhs1 = k * computeCross(a, b);
      Vd d1 = lhs1 - rhs1;
      double bound1 = 128.0 * eps * (euclideanLength(lhs1) + euclideanLength(rhs1) + std::abs(k) * euclideanLength(a) * euclideanLength(b)) + 1e-12;
      expect(euclideanLength(d1) <= bound1);
      Vd lhs2 = computeCross(a, k * b), rhs2 = k * computeCross(a, b);
      Vd d2 = lhs2 - rhs2;
      double bound2 = 128.0 * eps * (euclideanLength(lhs2) + euclideanLength(rhs2) + std::abs(k) * euclideanLength(a) * euclideanLength(b)) + 1e-12;
      expect(euclideanLength(d2) <= bound2);
      Vd lhs3 = computeCross(a, b + c), rhs3 = computeCross(a, b) + computeCross(a, c);
      Vd d3 = lhs3 - rhs3;
      double bound3 = 192.0 * eps * (euclideanLength(lhs3) + euclideanLength(rhs3) + euclideanLength(a) * (euclideanLength(b) + euclideanLength(c))) + 1e-12;
      expect(euclideanLength(d3) <= bound3);
      Vd lhs4 = computeCross(a + b, c), rhs4 = computeCross(a, c) + computeCross(b, c);
      Vd d4 = lhs4 - rhs4;
      double bound4 = 192.0 * eps * (euclideanLength(lhs4) + euclideanLength(rhs4) + (euclideanLength(a) + euclideanLength(b)) * euclideanLength(c)) + 1e-12;
      expect(euclideanLength(d4) <= bound4);
    }
    };

  // Parallel or anti-parallel vectors have zero cross product; critical for degeneracy handling in geometry kernels.
  "cross/parallel_zero/vfvd"_test = [] {
    std::mt19937_64 eng{ 0x7F4A7C15ULL };
    constexpr float epsf = std::numeric_limits<float>::epsilon();
    constexpr double epsd = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);
    std::uniform_real_distribution<float> tf(-1e3f, 1e3f);
    std::uniform_real_distribution<double> td(-1e6, 1e6);
    for (int t = 0; t < 20000; ++t) {
      Vf a{ df(eng),df(eng),df(eng) }; float sf = tf(eng);
      Vf c = computeCross(a, sf * a);
      float na = euclideanLength(a);
      float bound = 256.0f * epsf * na * std::abs(sf) * na + 1e-7f;
      for (int i = 0; i < 3; ++i) {
        Tol tol{ double(bound), double(64.0f * epsf), 12 };
        expect(almostEqual(c[i], 0.0f, tol));
      }
      Vd A{ dd(eng),dd(eng),dd(eng) }; double sd = td(eng);
      Vd C = computeCross(A, sd * A);
      double nA = euclideanLength(A);
      double boundD = 256.0 * epsd * nA * std::abs(sd) * nA + 1e-12;
      for (int i = 0; i < 3; ++i) {
        Tol tol{ boundD, 64.0 * epsd, 12 };
        expect(almostEqual(C[i], 0.0, tol));
      }
    }
    };

  // Jacobi identity a×(b×c) + b×(c×a) + c×(a×b)≈0 in double; important sanity check for vector triple products.
  "cross/jacobi_identity/vd"_test = [] {
    std::mt19937_64 eng{ 0x0BADF00DULL };
    constexpr double eps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<double> dist(-1e3, 1e3);
    for (int t = 0; t < 15000; ++t) {
      Vd a{ dist(eng),dist(eng),dist(eng) };
      Vd b{ dist(eng),dist(eng),dist(eng) };
      Vd c{ dist(eng),dist(eng),dist(eng) };
      if (!(isFinite(a[0]) && isFinite(a[1]) && isFinite(a[2]) &&
        isFinite(b[0]) && isFinite(b[1]) && isFinite(b[2]) &&
        isFinite(c[0]) && isFinite(c[1]) && isFinite(c[2]))) continue;
      Vd s = computeCross(a, computeCross(b, c))
        + computeCross(b, computeCross(c, a))
        + computeCross(c, computeCross(a, b));
      double na = euclideanLength(a), nb = euclideanLength(b), nc = euclideanLength(c);
      double scale = (na * nb + nb * nc + nc * na) * (na + nb + nc) + 1.0;
      double resid = std::sqrt(euclideanLengthSq(s));
      Tol tol{ 1024.0 * eps * scale, 512.0 * eps, 16 };
      expect(resid <= tol.abs);
    }
    };

  std::println("Free function tests passed...");
}

void vector3Tests()
{
  vecConstructionTests();
  vecArithmeticOpsTests();
  vecEqualityIndexUnaryTests();
  vecFreeFunctionTests();
}