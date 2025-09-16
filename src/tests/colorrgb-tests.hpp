#pragma once

#include "vec3-tests.hpp"
#include "../dep/ut.hpp"

import <cstdint>;

import std;
import vector;
import types;
import point;
import ray;
import normal;
import core_util;
import colorrgb;
import colorrgbd;


void colorRGBTests() {
  using C = ColorRGB;
  using Cd = ColorRGBd;
  using V = Vec3f;
  using F = Float;
  constexpr Idx R{ 0 }, G{ 1 }, B{ 2 };

  "color/ctor_index_types"_test = [=] {
    C g{ F(0.25f) };
    expect(g[R] == F(0.25f) && g[G] == F(0.25f) && g[B] == F(0.25f));

    C c{ F(1.0f), F(-2.0f), F(3.5f) };
    expect(c[R] == F(1.0f) && c[G] == F(-2.0f) && c[B] == F(3.5f));

    V v{ F(0.1f), F(0.2f), F(0.3f) };
    C cv{ v };
    expect(cv[R] == v[0] && cv[G] == v[1] && cv[B] == v[2]);

    Cd cd{ 1.25, -0.5, 10.0 };
    C  cf{ cd };
    expect(cf[R] == F(1.25f) && cf[G] == F(-0.5f) && cf[B] == F(10.0f));

    static_assert(std::same_as<decltype(std::declval<C&>()[Idx{ 0 }]), F& > );
    static_assert(std::same_as<decltype(std::declval<const C&>()[Idx{ 0 }]), const F& > );
    };

  "color/unary_minus_signed_zero"_test = [=] {
    C z{ F(0.0f), F(-0.0f), F(0.0f) };
    C n = -z;
    expect(n[R] == F(0.0f));
    expect(n[G] == F(0.0f));
    expect(n[B] == F(0.0f));

    C a{ F(1.0f), F(-2.0f), F(3.0f) };
    C b = -a;
    expect(b[R] == F(-1.0f) && b[G] == F(2.0f) && b[B] == F(-3.0f));
    C c = -b;
    expect(c[R] == a[R] && c[G] == a[G] && c[B] == a[B]);
    };

  "color/compound_chain_return"_test = [=] {
    C c{ F(1), F(2), F(3) }, d{ F(4), F(5), F(6) };
    F s = F(2);
    auto* p = &c;
    auto& r1 = (c += d); expect(&r1 == p);
    auto& r2 = (c -= d); expect(&r2 == p);
    auto& r3 = (c *= s); expect(&r3 == p);
    auto& r4 = (c /= s); expect(&r4 == p);
    auto& r5 = (c *= d); expect(&r5 == p);
    };

  "color/algebra_identities"_test = [=] {
    C a{ F(1.25f), F(-2.5f), F(3.75f) };
    C b{ F(-0.5f), F(4.0f),  F(1.0f) };
    F s = F(2.5f);

    C z{ F(0), F(0), F(0) };
    expect(((a + z)[R] == a[R] && (a + z)[G] == a[G] && (a + z)[B] == a[B]));
    expect(((a - z)[R] == a[R] && (a - z)[G] == a[G] && (a - z)[B] == a[B]));

    C t = (a + b) - b;
    expect(t[R] == a[R] && t[G] == a[G] && t[B] == a[B]);

    C u = (a * s) / s;
    expect(u[R] == a[R] && u[G] == a[G] && u[B] == a[B]);

    C h1 = a * b;
    C h2 = b * a;
    expect(h1[R] == h2[R] && h1[G] == h2[G] && h1[B] == h2[B]);

    C r1 = a * s;
    C r2 = s * a;
    expect(r1[R] == r2[R] && r1[G] == r2[G] && r1[B] == r2[B]);
    };

  "color/randomized_componentwise_match"_test = [=] {
    std::mt19937_64 eng{ 0xA11CEB00ULL };
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    Tol t{ 1e-6, 1e-6, 6 };

    auto chk = [&](const C& x, F xr, F xg, F xb) {
      expect(almostEqual(x[R], xr, t));
      expect(almostEqual(x[G], xg, t));
      expect(almostEqual(x[B], xb, t));
      };

    for (int it = 0; it < 20000; ++it) {
      C a{ F(df(eng)), F(df(eng)), F(df(eng)) };
      C b{ F(df(eng)), F(df(eng)), F(df(eng)) };
      F s = F(df(eng));

      chk(a + b, F(a[R] + b[R]), F(a[G] + b[G]), F(a[B] + b[B]));
      chk(a - b, F(a[R] - b[R]), F(a[G] - b[G]), F(a[B] - b[B]));
      chk(a * s, F(a[R] * s), F(a[G] * s), F(a[B] * s));

      if (s != F(0)) chk(a / s, F(a[R] / s), F(a[G] / s), F(a[B] / s));

      chk(a * b, F(a[R] * b[R]), F(a[G] * b[G]), F(a[B] * b[B]));
      chk(s * a, F(s * a[R]), F(s * a[G]), F(s * a[B]));
    }
    };

  "color/compound_equivalence_and_aliasing"_test = [] {
    using F = Float; using C = ColorRGB;
    constexpr Idx R{ 0 }, G{ 1 }, B{ 2 };
    std::mt19937_64 eng{ 0xBADA5501ULL };
    std::uniform_real_distribution<float> df(-1e2f, 1e2f);
    const F eps = std::numeric_limits<F>::epsilon();

    auto tol_of = [&](F a, F b) {
      F scale = std::max(F(1), std::max(std::abs(a), std::abs(b)));
      return Tol{ double(32 * eps * scale), double(8 * eps), 6 };
      };

    for (int it = 0; it < 20000; ++it) {
      C a{ F(df(eng)), F(df(eng)), F(df(eng)) };
      C b{ F(df(eng)), F(df(eng)), F(df(eng)) };
      F s = F(df(eng));

      // +=
      {
        C u = a;
        u += b;
        F rR = F(a[R] + b[R]), rG = F(a[G] + b[G]), rB = F(a[B] + b[B]);
        expect(almostEqual(u[R], rR, tol_of(u[R], rR)));
        expect(almostEqual(u[G], rG, tol_of(u[G], rG)));
        expect(almostEqual(u[B], rB, tol_of(u[B], rB)));
      }

      // -=
      {
        C u = a;
        u -= b;
        F rR = F(a[R] - b[R]), rG = F(a[G] - b[G]), rB = F(a[B] - b[B]);
        expect(almostEqual(u[R], rR, tol_of(u[R], rR)));
        expect(almostEqual(u[G], rG, tol_of(u[G], rG)));
        expect(almostEqual(u[B], rB, tol_of(u[B], rB)));
      }

      // *= scalar
      {
        C u = a;
        u *= s;
        F rR = F(a[R] * s), rG = F(a[G] * s), rB = F(a[B] * s);
        expect(almostEqual(u[R], rR, tol_of(u[R], rR)));
        expect(almostEqual(u[G], rG, tol_of(u[G], rG)));
        expect(almostEqual(u[B], rB, tol_of(u[B], rB)));
      }

      // /= scalar
      if (s != F(0)) {
        C u = a;
        u /= s;
        F rR = F(a[R] / s), rG = F(a[G] / s), rB = F(a[B] / s);
        expect(almostEqual(u[R], rR, tol_of(u[R], rR)));
        expect(almostEqual(u[G], rG, tol_of(u[G], rG)));
        expect(almostEqual(u[B], rB, tol_of(u[B], rB)));
      }

      // *= color (Hadamard)
      {
        C u = a;
        u *= b;
        F rR = F(a[R] * b[R]), rG = F(a[G] * b[G]), rB = F(a[B] * b[B]);
        expect(almostEqual(u[R], rR, tol_of(u[R], rR)));
        expect(almostEqual(u[G], rG, tol_of(u[G], rG)));
        expect(almostEqual(u[B], rB, tol_of(u[B], rB)));
      }

      // Self-alias sanity: a + a == 2*a (within FP tolerance)
      {
        C w = a; w += w;
        F rR = F(2) * a[R], rG = F(2) * a[G], rB = F(2) * a[B];
        expect(almostEqual(w[R], rR, tol_of(w[R], rR)));
        expect(almostEqual(w[G], rG, tol_of(w[G], rG)));
        expect(almostEqual(w[B], rB, tol_of(w[B], rB)));
      }
    }
    };

  "color/nonmutation_binary"_test = [=] {
    C a{ F(1), F(2), F(3) }, b{ F(4), F(5), F(6) };
    C ac = a, bc = b;
    (void)(a + b);
    (void)(a - b);
    (void)(a * F(2));
    (void)(a * b);
    (void)(a / F(2));
    expect(a[R] == ac[R] && a[G] == ac[G] && a[B] == ac[B]);
    expect(b[R] == bc[R] && b[G] == bc[G] && b[B] == bc[B]);
    };

  "color/ieee_pathologies"_test = [=] {
    auto isNaN = [](F x) { return std::isnan(x); };
    auto isInf = [](F x) { return std::isinf(x); };

    C a{ F(1), F(-2), F(0) };
    C dpos = a / F(0.0f);
    C dneg = a / F(-0.0f);
    expect(isInf(dpos[R]) && isInf(dneg[R]));
    expect(isNaN(dpos[B]) && isNaN(dneg[B]));

    C infC{ std::numeric_limits<F>::infinity(), F(1), F(-1) };
    C nanC{ std::numeric_limits<F>::quiet_NaN(), F(2), F(3) };
    C h1 = a * infC; expect(isInf(h1[R]));
    C h2 = a * nanC; expect(isNaN(h2[R]));
    };

  //"color/linear_form_sanity"_test = [=] {
  "color/linear_form_sanity"_test = [] {
    using F = Float; using C = ColorRGB;
    constexpr Idx R{ 0 }, G{ 1 }, B{ 2 };
    const F wr = F(0.2126f), wg = F(0.7152f), wb = F(0.0722f);
    const F eps = std::numeric_limits<F>::epsilon();

    auto L = [&](const C& c) -> F { return wr * c[R] + wg * c[G] + wb * c[B]; };
    auto tol_of = [&](F a, F b) {
      F scale = std::max(F(1), std::max(std::abs(a), std::abs(b)));
      return Tol{ double(32 * eps * scale), double(8 * eps), 6 };
      };

    std::mt19937_64 eng{ 0xDEADBEEFCAFEBABEULL };
    std::uniform_real_distribution<float> df(-10.f, 10.f);

    for (int it = 0; it < 20000; ++it) {
      C a{ F(df(eng)), F(df(eng)), F(df(eng)) };
      C b{ F(df(eng)), F(df(eng)), F(df(eng)) };
      F s = F(df(eng));

      // Additivity: L(a + b) == L(a) + L(b)
      {
        F lhs = L(a + b);
        F rhs = F(L(a) + L(b));
        expect(almostEqual(lhs, rhs, tol_of(lhs, rhs)));
      }

      // Homogeneity: L(s * a) == s * L(a)
      {
        F lhs = L(a * s);
        F rhs = F(s * L(a));
        expect(almostEqual(lhs, rhs, tol_of(lhs, rhs)));
      }
    }
    };
}
