#pragma once

#include "../dep/ut.hpp"

#include "vec3-tests.hpp"
import <cstdint>;

import std;
import vector;
import types;
import point;

using namespace boost::ut;

void pointConstructionTests() {
  using P3f = Point3f; using P3d = Point3d;
  using V3f = Vector<float, 3>; using V3d = Vector<double, 3>;

  "point/ctor_edgecases"_test = [] {
    P3f pf0{ 0.0f,0.0f,0.0f }; P3f pfm0{ -0.0f,-0.0f,-0.0f };
    for (int i = 0; i < 3; ++i) { expect(pf0[i] == 0.0f); expect(std::signbit(pfm0[i])); }
    P3d pd0{ 0.0,0.0,0.0 }; P3d pdm0{ -0.0,-0.0,-0.0 };
    for (int i = 0; i < 3; ++i) { expect(pd0[i] == 0.0); expect(std::signbit(pdm0[i])); }
    std::mt19937_64 eng{ 0xC0FFEEULL };
    for (int t = 0; t < 20000; ++t) {
      std::array<float, 3> af{ extremeRand<float>(eng), extremeRand<float>(eng), extremeRand<float>(eng) };
      P3f pf{ af }; for (int i = 0; i < 3; ++i) { if (isNaN(af[i])) expect(isNaN(pf[i])); else expect(pf[i] == af[i]); }
      std::array<double, 3> ad{ extremeRand<double>(eng), extremeRand<double>(eng), extremeRand<double>(eng) };
      P3d pd{ ad }; for (int i = 0; i < 3; ++i) { if (isNaN(ad[i])) expect(isNaN(pd[i])); else expect(pd[i] == ad[i]); }
    }
    };

  "point/equality_reflexivity_symmetry_and_ulpmutation"_test = [] {
    std::mt19937_64 eng{ 0xE11ECA5EULL };
    std::uniform_real_distribution<float> df(-1e5f, 1e5f);
    for (int t = 0; t < 20000; ++t) {
      P3f a{ df(eng),df(eng),df(eng) }; P3f b = a; expect(a == b);
      int k = int(std::uniform_int_distribution<int>(0, 2)(eng));
      float x = b[k]; float toward = std::signbit(x) ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
      b[k] = (x == 0.0f) ? std::numeric_limits<float>::denorm_min() : std::nextafter(x, toward);
      expect(!(a == b));
    }
    std::uniform_real_distribution<double> dd(-1e8, 1e8);
    for (int t = 0; t < 20000; ++t) {
      P3d A{ dd(eng),dd(eng),dd(eng) }; P3d B = A; expect(A == B);
      int k = int(std::uniform_int_distribution<int>(0, 2)(eng));
      double x = B[k]; double toward = std::signbit(x) ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
      B[k] = (x == 0.0) ? std::numeric_limits<double>::denorm_min() : std::nextafter(x, toward);
      expect(!(A == B));
    }
    };

  "point/index_types_and_contiguity"_test = [] {
    static_assert(std::same_as<decltype(std::declval<P3f&>()[0]), float&>);
    static_assert(std::same_as<decltype(std::declval<const P3f&>()[0]), const float&>);
    static_assert(std::same_as<decltype(std::declval<P3d&>()[0]), double&>);
    static_assert(std::same_as<decltype(std::declval<const P3d&>()[0]), const double&>);
    P3f pf{ 2.f,-3.f,4.f }; const P3f& cpf = pf; expect(cpf[0] == 2.f && cpf[1] == -3.f && cpf[2] == 4.f);
    auto* p0 = &pf[0]; auto* p1 = &pf[1]; auto* p2 = &pf[2];
    expect(p0 + 1 == p1 && p1 + 1 == p2);
    P3d pd{ -5.0,6.0,7.5 }; const P3d& cpd = pd; expect(cpd[0] == -5.0 && cpd[1] == 6.0 && cpd[2] == 7.5);
    auto* q0 = &pd[0]; auto* q1 = &pd[1]; auto* q2 = &pd[2];
    expect(q0 + 1 == q1 && q1 + 1 == q2);
    };

  "point/plus_minus_vector_identities"_test = [] {
    P3f p{ 1.25f,-2.5f,3.75f }; V3f z{ 0.f,0.f,0.f };
    expect((p + z) == p); expect((p - z) == p);
    V3f v{ 3.f,-4.f,5.f }; expect(((p + v) - v) == p); expect(((p - v) + v) == p);
    P3d P{ 1.25,-2.5,3.75 }; V3d Z{ 0.0,0.0,0.0 }; V3d V{ 3.0,-4.0,5.0 };
    expect((P + Z) == P); expect((P - Z) == P); expect(((P + V) - V) == P); expect(((P - V) + V) == P);
    };

  "point/distance_translation_invariance"_test = [] {
    std::mt19937_64 eng{ 0xBADA5501ULL };
    std::uniform_real_distribution<float> df(-1e4f, 1e4f);
    Tol tf{ 1e-5,1e-5,6 };
    for (int t = 0; t < 30000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }, q{ df(eng),df(eng),df(eng) };
      V3f v{ df(eng),df(eng),df(eng) };
      float d0 = euclideanLength(p - q);
      float d1 = euclideanLength((p + v) - (q + v));
      expect(almostEqual(d0, d1, tf));
    }
    std::uniform_real_distribution<double> dd(-1e6, 1e6);
    Tol td{ 1e-12,1e-10,8 };
    for (int t = 0; t < 20000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, q{ dd(eng),dd(eng),dd(eng) };
      V3d v{ dd(eng),dd(eng),dd(eng) };
      double d0 = euclideanLength(p - q);
      double d1 = euclideanLength((p + v) - (q + v));
      expect(almostEqual(d0, d1, td));
    }
    };

  // --- FIX: numerical tolerance scaled to data magnitude; subtraction + add-back is not perfectly reversible in FP ---
  "point/minus_point_vector_semantics_and_anchor"_test = [] {
    std::mt19937_64 eng{ 0xA11C3D00ULL };
    constexpr float feps = std::numeric_limits<float>::epsilon();
    constexpr double deps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);

    for (int t = 0; t < 30000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }, q{ df(eng),df(eng),df(eng) };
      V3f v = p - q; P3f q2 = q + v; P3f p2 = p - v;
      for (int i = 0; i < 3; ++i) {
        float scale = std::max({ std::abs(p[i]), std::abs(q[i]), std::abs(v[i]), 1.0f });
        Tol tf{ double(64.f * feps * scale), double(16.f * feps), 6 };
        expect(almostEqual(q2[i], p[i], tf));
        expect(almostEqual(p2[i], q[i], tf));
      }
    }
    for (int t = 0; t < 30000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, q{ dd(eng),dd(eng),dd(eng) };
      V3d v = p - q; P3d q2 = q + v; P3d p2 = p - v;
      for (int i = 0; i < 3; ++i) {
        double scale = std::max({ std::abs(p[i]), std::abs(q[i]), std::abs(v[i]), 1.0 });
        Tol td{ 64.0 * deps * scale, 16.0 * deps, 6 };
        expect(almostEqual(q2[i], p[i], td));
        expect(almostEqual(p2[i], q[i], td));
      }
    }
    };

  // --- FIX: IEEE-754 permits either sign for zero in exact-sum results; assert zero-ness, not sign ---
  "point/signed_zero_and_ieee_propagation"_test = [] {
    P3f pz{ 0.f,0.f,0.f }; V3f mz{ -0.0f,-0.0f,-0.0f }; P3f r = pz + mz;
    expect(r[0] == 0.f && r[1] == 0.f && r[2] == 0.f);            // sign may be +0 or -0
    V3f diff = r - pz; expect(euclideanLengthSq(diff) == 0.0f);

    P3d pinf{ std::numeric_limits<double>::infinity(), 0.0, -0.0 };
    V3d v{ 1.0, -2.0, 3.0 }; P3d s = pinf + v;
    expect(isInf(s[0]));

    P3f pn{ std::numeric_limits<float>::quiet_NaN(), 1.f, 2.f };
    P3f t = pn + V3f{ 0.f,0.f,0.f };
    expect(isNaN(t[0]));
    };
}

void pointAffineTests() 
{
  using P3f = Point3f; using P3d = Point3d;
  using V3f = Vector<float, 3>; using V3d = Vector<double, 3>;

  "point/anchor_equivalence_randomized"_test = [] {
    std::mt19937_64 eng{ 0xA11CEB01ULL }; Tol tf{ 1e-6,1e-5,6 }, td{ 1e-12,1e-10,8 };
    std::uniform_real_distribution<float> df(-1e4f, 1e4f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);
    for (int t = 0; t < 30000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }, q{ df(eng),df(eng),df(eng) };
      V3f v = q - p; P3f q2 = p + v; for (int i = 0; i < 3; ++i) expect(almostEqual(q2[i], q[i], tf));
      P3f p2 = q - v; for (int i = 0; i < 3; ++i) expect(almostEqual(p2[i], p[i], tf));
    }
    for (int t = 0; t < 20000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, q{ dd(eng),dd(eng),dd(eng) };
      V3d v = q - p; P3d q2 = p + v; for (int i = 0; i < 3; ++i) expect(almostEqual(q2[i], q[i], td));
      P3d p2 = q - v; for (int i = 0; i < 3; ++i) expect(almostEqual(p2[i], p[i], td));
    }
    };

  "point/translation_associativity"_test = [] {
    std::mt19937_64 eng{ 0xBADA5502ULL };
    constexpr float feps = std::numeric_limits<float>::epsilon();
    constexpr double deps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);

    for (int t = 0; t < 25000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) };
      V3f a{ df(eng),df(eng),df(eng) }, b{ df(eng),df(eng),df(eng) };
      P3f lhs = (p + a) + b, rhs = p + (a + b);
      for (int i = 0; i < 3; ++i) {
        float scale = std::max({ 1.0f,std::abs(p[i]) + std::abs(a[i]) + std::abs(b[i]), std::abs(lhs[i]), std::abs(rhs[i]) });
        Tol tf{ double(64.f * feps * scale), double(16.f * feps), 6 };
        expect(almostEqual(lhs[i], rhs[i], tf));
      }
    }
    for (int t = 0; t < 20000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) };
      V3d a{ dd(eng),dd(eng),dd(eng) }, b{ dd(eng),dd(eng),dd(eng) };
      P3d lhs = (p + a) + b, rhs = p + (a + b);
      for (int i = 0; i < 3; ++i) {
        double scale = std::max({ 1.0, std::abs(p[i]) + std::abs(a[i]) + std::abs(b[i]), std::abs(lhs[i]), std::abs(rhs[i]) });
        Tol td{ 64.0 * deps * scale, 16.0 * deps, 6 };
        expect(almostEqual(lhs[i], rhs[i], td));
      }
    }
    };

  "point/lerp_equivalence"_test = [] {
    std::mt19937_64 eng{ 0x0BADF00DULL };
    constexpr float feps = std::numeric_limits<float>::epsilon();
    constexpr double deps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float> df(-1e3f, 1e3f), sf(-10.f, 10.f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6), sd(-10.0, 10.0);

    for (int t = 0; t < 25000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }, q{ df(eng),df(eng),df(eng) }; float s = sf(eng);
      V3f qp = q - p; P3f r = p + qp * s; V3f v = r - p; V3f ref = qp * s;
      for (int i = 0; i < 3; ++i) {
        float scale = std::abs(p[i]) + std::abs(qp[i] * s) + 1.0f;
        Tol tf{ double(128.f * feps * scale), double(8.f * feps), 8 };
        expect(almostEqual(v[i], ref[i], tf));
      }
      if (s == 0.f || s == 1.f) {
        P3f target = (s == 0.f ? p : q);
        for (int i = 0; i < 3; ++i) {
          float scale = std::abs(target[i]) + 1.0f;
          Tol tfz{ double(64.f * feps * scale), double(8.f * feps), 6 };
          expect(almostEqual(r[i], target[i], tfz));
        }
      }
    }

    for (int t = 0; t < 20000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, q{ dd(eng),dd(eng),dd(eng) }; double s = sd(eng);
      V3d qp = q - p; P3d r = p + qp * s; V3d v = r - p; V3d ref = qp * s;
      for (int i = 0; i < 3; ++i) {
        double scale = std::abs(p[i]) + std::abs(qp[i] * s) + 1.0;
        Tol td{ 128.0 * deps * scale, 8.0 * deps, 8 };
        expect(almostEqual(v[i], ref[i], td));
      }
    }
    };

  "point/difference_chaining"_test = [] {
    std::mt19937_64 eng{ 0xC0DECAFEULL };
    constexpr float feps = std::numeric_limits<float>::epsilon();
    constexpr double deps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float> df(-1e4f, 1e4f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);

    for (int t = 0; t < 30000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }, q{ df(eng),df(eng),df(eng) }, r{ df(eng),df(eng),df(eng) };
      V3f lhs = (p - q) + (q - r), rhs = (p - r);
      for (int i = 0; i < 3; ++i) {
        float scale = std::max({ 1.0f, std::abs(p[i]) + std::abs(q[i]) + std::abs(r[i]), std::abs(lhs[i]), std::abs(rhs[i]) });
        Tol tf{ double(64.f * feps * scale), double(16.f * feps), 6 };
        expect(almostEqual(lhs[i], rhs[i], tf));
      }
    }
    for (int t = 0; t < 20000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, q{ dd(eng),dd(eng),dd(eng) }, r{ dd(eng),dd(eng),dd(eng) };
      V3d lhs = (p - q) + (q - r), rhs = (p - r);
      for (int i = 0; i < 3; ++i) {
        double scale = std::max({ 1.0, std::abs(p[i]) + std::abs(q[i]) + std::abs(r[i]), std::abs(lhs[i]), std::abs(rhs[i]) });
        Tol td{ 64.0 * deps * scale, 16.0 * deps, 6 };
        expect(almostEqual(lhs[i], rhs[i], td));
      }
    }
    };

  "point/non_mutation_semantics"_test = [] {
    P3f p{ 1,2,3 }; V3f v{ 4,5,6 }; P3f copy = p; auto x = p + v; auto y = p - v;
    expect(p == copy); (void)x; (void)y;
    P3d P{ 1,2,3 }; V3d V{ 4,5,6 }; P3d Copy = P; auto X = P + V; auto Y = P - V;
    expect(P == Copy); (void)X; (void)Y;
    };
}

void pointGeometricRelationsTests() 
{
  using P3f = Point3f; using P3d = Point3d;
  using V3f = Vector<float, 3>; using V3d = Vector<double, 3>;

  "point/midpoint_symmetry"_test = [] {
    std::mt19937_64 eng{ 0x515151ULL }; Tol tf{ 1e-6,1e-5,6 }, td{ 1e-12,1e-10,8 };
    std::uniform_real_distribution<float> df(-1e4f, 1e4f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);
    for (int t = 0; t < 30000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }, q{ df(eng),df(eng),df(eng) };
      P3f m = p + (q - p) * 0.5f;
      expect(almostEqual(euclideanLength(m - p), euclideanLength(q - m), tf));
      expect(almostEqual(euclideanLength(q - p), 2.0f * euclideanLength(m - p), tf));
    }
    for (int t = 0; t < 20000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, q{ dd(eng),dd(eng),dd(eng) };
      P3d m = p + (q - p) * 0.5;
      expect(almostEqual(euclideanLength(m - p), euclideanLength(q - m), td));
      expect(almostEqual(euclideanLength(q - p), 2.0 * euclideanLength(m - p), td));
    }
    };

  // Translation invariance of barycentric evaluation, with norm-based FP bounds.
  "point/barycentric_translation_invariance"_test = [] {
    std::mt19937_64 eng{ 0xBABAFADEULL };
    constexpr float feps = std::numeric_limits<float>::epsilon();
    constexpr double deps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float> df(-1e3f, 1e3f), sf(-5.f, 5.f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6), sd(-5.0, 5.0);

    for (int t = 0; t < 25000; ++t) {
      P3f a{ df(eng),df(eng),df(eng) }, b{ df(eng),df(eng),df(eng) }, c{ df(eng),df(eng),df(eng) };
      float u = sf(eng), v = sf(eng); V3f w{ df(eng),df(eng),df(eng) };
      P3f r1 = a + (b - a) * u + (c - a) * v;
      P3f r2 = (a + w) + ((b + w) - (a + w)) * u + ((c + w) - (a + w)) * v;
      V3f resid = (r1 + w) - r2;
      float scale = euclideanLength(a - P3f{ 0,0,0 }) + euclideanLength(b - P3f{ 0,0,0 }) +
        euclideanLength(c - P3f{ 0,0,0 }) + euclideanLength(w);
      float magUV = std::abs(u) + std::abs(v) + 1.0f;
      float bound = 256.f * feps * std::max(1.0f, scale * magUV);
      expect(euclideanLength(resid) <= bound);
    }

    for (int t = 0; t < 20000; ++t) {
      P3d a{ dd(eng),dd(eng),dd(eng) }, b{ dd(eng),dd(eng),dd(eng) }, c{ dd(eng),dd(eng),dd(eng) };
      double u = sd(eng), v = sd(eng); V3d w{ dd(eng),dd(eng),dd(eng) };
      P3d r1 = a + (b - a) * u + (c - a) * v;
      P3d r2 = (a + w) + ((b + w) - (a + w)) * u + ((c + w) - (a + w)) * v;
      V3d resid = (r1 + w) - r2;
      double scale = euclideanLength(a - P3d{ 0,0,0 }) + euclideanLength(b - P3d{ 0,0,0 }) +
        euclideanLength(c - P3d{ 0,0,0 }) + euclideanLength(w);
      double magUV = std::abs(u) + std::abs(v) + 1.0;
      double bound = 256.0 * deps * std::max(1.0, scale * magUV);
      expect(euclideanLength(resid) <= bound);
    }
    };

  // FIX: bound residual against magnitudes of a,b,c and both constructions (a/u/v vs edge form).
  "point/barycentric_edge_uplusv_eq_1"_test = [] {
    std::mt19937_64 eng{ 0xC001D00DULL };
    constexpr float  feps = std::numeric_limits<float>::epsilon();
    constexpr double deps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float>  df(-1e3f, 1e3f), tfp(0.f, 1.f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6), tdp(0.0, 1.0);

    for (int t = 0; t < 25000; ++t) {
      P3f a{ df(eng),df(eng),df(eng) }, b{ df(eng),df(eng),df(eng) }, c{ df(eng),df(eng),df(eng) };
      float s = tfp(eng); float u = s, v = 1.f - s;
      P3f r1 = a + (b - a) * u + (c - a) * v; // barycentric (two subs, two scales, two adds)
      P3f r2 = c + (b - c) * s;           // edge form (one sub, scale, add)
      V3f resid = r1 - r2;
      for (int i = 0; i < 3; ++i) {
        float scale = std::abs(a[i]) + std::abs(b[i]) + std::abs(c[i])
          + std::abs((b[i] - a[i]) * u) + std::abs((c[i] - a[i]) * v)
          + std::abs((b[i] - c[i]) * s) + 1.0f;
        float bound = 1024.f * feps * scale;
        expect(std::abs(resid[i]) <= bound);
      }
    }

    for (int t = 0; t < 20000; ++t) {
      P3d a{ dd(eng),dd(eng),dd(eng) }, b{ dd(eng),dd(eng),dd(eng) }, c{ dd(eng),dd(eng),dd(eng) };
      double s = tdp(eng); double u = s, v = 1.0 - s;
      P3d r1 = a + (b - a) * u + (c - a) * v;
      P3d r2 = c + (b - c) * s;
      V3d resid = r1 - r2;
      for (int i = 0; i < 3; ++i) {
        double scale = std::abs(a[i]) + std::abs(b[i]) + std::abs(c[i])
          + std::abs((b[i] - a[i]) * u) + std::abs((c[i] - a[i]) * v)
          + std::abs((b[i] - c[i]) * s) + 1.0;
        double bound = 1024.0 * deps * scale;
        expect(std::abs(resid[i]) <= bound);
      }
    }
    };

  // FIX: ratio-based collinearity (use the dominant component of a = r-p to estimate s and check others).
  "point/collinearity_characterization"_test = [] {
    std::mt19937_64 eng{ 0x55AA77CCULL };
    constexpr float  feps = std::numeric_limits<float>::epsilon();
    constexpr double deps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float>  df(-1e3f, 1e3f), sf(-5.f, 5.f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6), sd(-5.0, 5.0);

    for (int t = 0; t < 30000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }, r{ df(eng),df(eng),df(eng) }; float s = sf(eng);
      V3f a = r - p; float an = euclideanLength(a); if (an < 1e-6f) continue;
      P3f q = p + a * s; V3f qp = q - p;
      int k = 0; if (std::abs(a[1]) > std::abs(a[k])) k = 1; if (std::abs(a[2]) > std::abs(a[k])) k = 2;
      if (a[k] == 0.f) continue;
      float s_est = qp[k] / a[k];
      for (int i = 0; i < 3; ++i) {
        float ref = a[i] * s_est;
        float scale = std::abs(ref) + std::abs(qp[i]) + std::abs(p[i]) + 1.0f;
        float bound = 256.f * feps * scale;
        expect(std::abs(qp[i] - ref) <= bound);
      }
    }

    for (int t = 0; t < 20000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, r{ dd(eng),dd(eng),dd(eng) }; double s = sd(eng);
      V3d a = r - p; double an = euclideanLength(a); if (an < 1e-12) continue;
      P3d q = p + a * s; V3d qp = q - p;
      int k = 0; if (std::abs(a[1]) > std::abs(a[k])) k = 1; if (std::abs(a[2]) > std::abs(a[k])) k = 2;
      if (a[k] == 0.0) continue;
      double s_est = qp[k] / a[k];
      for (int i = 0; i < 3; ++i) {
        double ref = a[i] * s_est;
        double scale = std::abs(ref) + std::abs(qp[i]) + std::abs(p[i]) + 1.0;
        double bound = 256.0 * deps * scale;
        expect(std::abs(qp[i] - ref) <= bound);
      }
    }
    };

  // Centroid translation equivalence; compare residual with scale-aware bound.
  "point/centroid_equivalence_and_translation"_test = [] {
    std::mt19937_64 eng{ 0x0DEFACEDULL };
    constexpr float feps = std::numeric_limits<float>::epsilon();
    constexpr double deps = std::numeric_limits<double>::epsilon();
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);

    for (int t = 0; t < 25000; ++t) {
      P3f a{ df(eng),df(eng),df(eng) }, b{ df(eng),df(eng),df(eng) }, c{ df(eng),df(eng),df(eng) };
      P3f g1 = a + ((b - a) + (c - a)) / 3.0f;
      V3f w{ df(eng),df(eng),df(eng) };
      P3f g2 = (a + w) + (((b + w) - (a + w)) + ((c + w) - (a + w))) / 3.0f;
      V3f resid = (g1 + w) - g2;
      float scale = euclideanLength(a - P3f{ 0,0,0 }) + euclideanLength(b - P3f{ 0,0,0 }) +
        euclideanLength(c - P3f{ 0,0,0 }) + euclideanLength(w);
      float bound = 256.f * feps * std::max(1.0f, scale);
      expect(euclideanLength(resid) <= bound);
    }

    for (int t = 0; t < 20000; ++t) {
      P3d a{ dd(eng),dd(eng),dd(eng) }, b{ dd(eng),dd(eng),dd(eng) }, c{ dd(eng),dd(eng),dd(eng) };
      P3d g1 = a + ((b - a) + (c - a)) / 3.0;
      V3d w{ dd(eng),dd(eng),dd(eng) };
      P3d g2 = (a + w) + (((b + w) - (a + w)) + ((c + w) - (a + w))) / 3.0;
      V3d resid = (g1 + w) - g2;
      double scale = euclideanLength(a - P3d{ 0,0,0 }) + euclideanLength(b - P3d{ 0,0,0 }) +
        euclideanLength(c - P3d{ 0,0,0 }) + euclideanLength(w);
      double bound = 256.0 * deps * std::max(1.0, scale);
      expect(euclideanLength(resid) <= bound);
    }
    };

  "point/triangle_inequality_distances"_test = [] {
    std::mt19937_64 eng{ 0xFACEB00CULL };
    std::uniform_real_distribution<float> df(-1e4f, 1e4f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);
    for (int t = 0; t < 30000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }, q{ df(eng),df(eng),df(eng) }, r{ df(eng),df(eng),df(eng) };
      float pq = euclideanLength(p - q), qr = euclideanLength(q - r), pr = euclideanLength(p - r);
      expect(pq + qr + 1e-5f >= pr);
    }
    for (int t = 0; t < 20000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, q{ dd(eng),dd(eng),dd(eng) }, r{ dd(eng),dd(eng),dd(eng) };
      double pq = euclideanLength(p - q), qr = euclideanLength(q - r), pr = euclideanLength(p - r);
      expect(pq + qr + 1e-10 >= pr);
    }
    };

  "point/extreme_magnitude_stability"_test = [] {
    std::mt19937_64 eng{ 0xDEADC0DEULL };
    for (int t = 0; t < 20000; ++t) {
      P3f p{ 1e30f, -1e30f, 5e29f }, q{ -1e30f, 1e30f, -5e29f };
      auto v = p - q; expect(isFinite(v[0]) && isFinite(v[1]) && isFinite(v[2]));
    }
    for (int t = 0; t < 20000; ++t) {
      P3d p{ 1e300, -1e300, 5e299 }, q{ -1e300, 1e300, -5e299 };
      auto v = p - q; expect(isFinite(v[0]) && isFinite(v[1]) && isFinite(v[2]));
    }
    };
}

void pointAccumulationRoundtripTests() 
{
  using P3f = Point3f; using P3d = Point3d;
  using V3f = Vector<float, 3>; using V3d = Vector<double, 3>;

  "point/roundtrip_sum_diff_stability/float"_test = [] {
    std::mt19937_64 eng{ 0xACCF011ULL };
    std::uniform_real_distribution<float> df(-1e2f, 1e2f);
    constexpr float eps = std::numeric_limits<float>::epsilon();
    for (int t = 0; t < 4000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }; V3f S{ 0,0,0 };
      float mag = 0.f; int n = 64;
      for (int i = 0; i < n; ++i) { V3f v{ df(eng),df(eng),df(eng) }; S += v; mag += euclideanLength(v); }
      P3f r = (p + S) - S;
      for (int i = 0; i < 3; ++i) {
        Tol tol{ double(64.f * eps * std::max(1.f,mag)), double(16.f * eps), 6 };
        expect(almostEqual(r[i], p[i], tol));
      }
    }
    };

  "point/roundtrip_sum_diff_stability/double"_test = [] {
    std::mt19937_64 eng{ 0xACCF022ULL };
    std::uniform_real_distribution<double> dd(-1e4, 1e4);
    constexpr double eps = std::numeric_limits<double>::epsilon();
    for (int t = 0; t < 3000; ++t) {
      P3d p{ dd(eng),dd(eng),dd(eng) }; V3d S{ 0,0,0 };
      double mag = 0.0; int n = 96;
      for (int i = 0; i < n; ++i) { V3d v{ dd(eng),dd(eng),dd(eng) }; S += v; mag += euclideanLength(v); }
      P3d r = (p + S) - S;
      for (int i = 0; i < 3; ++i) {
        Tol tol{ 96.0 * eps * std::max(1.0,mag), 24.0 * eps, 8 };
        expect(almostEqual(r[i], p[i], tol));
      }
    }
    };

  "point/partition_invariance"_test = [] {
    std::mt19937_64 eng{ 0xB1A2C3D4ULL };
    std::uniform_real_distribution<float> df(-1e2f, 1e2f);
    P3f p{ df(eng),df(eng),df(eng) }; std::vector<V3f> vs(256);
    for (auto& v : vs) v = { df(eng),df(eng),df(eng) };
    V3f S{ 0,0,0 }; for (auto& v : vs) S += v;
    P3f a = p + S, b = p;
    for (size_t i = 0; i < vs.size(); i += 17) { V3f chunk{ 0,0,0 }; for (size_t j = i; j < std::min(vs.size(), i + 17ULL); ++j) chunk += vs[j]; b = b + chunk; }
    Tol tf{ 1e-5,1e-5,8 }; for (int i = 0; i < 3; ++i) expect(almostEqual(a[i], b[i], tf));
    };

  "point/parametric_distance_linearity"_test = [] {
    std::mt19937_64 eng{ 0x1A2B3C4DULL };
    std::uniform_real_distribution<double> dd(-1e3, 1e3);
    double ts[]{ -10,-2,-1,-0.5,0,0.25,0.5,1,2,10 };
    for (int k = 0; k < 15000; ++k) {
      P3d p{ dd(eng),dd(eng),dd(eng) }, q{ dd(eng),dd(eng),dd(eng) };
      V3d v = q - p; double L = euclideanLength(v); if (!isFinite(L)) continue;
      for (double s : ts) {
        P3d r = p + v * s; double d = euclideanLength(r - p);
        Tol td{ 1e-9 * std::max(1.0,std::abs(s) * L), 1e-9, 8 };
        expect(almostEqual(d, std::abs(s) * L, td));
      }
    }
    };

  "point/identical_subtract_zero_vector"_test = [] {
    std::mt19937_64 eng{ 0x0CEED1FFULL };
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    for (int t = 0; t < 30000; ++t) {
      P3f p{ df(eng),df(eng),df(eng) }; V3f z = p - p;
      expect((z[0] == 0.f || isNegZero(z[0])) && (z[1] == 0.f || isNegZero(z[1])) && (z[2] == 0.f || isNegZero(z[2])));
      expect(almostEqual(euclideanLengthSq(z), 0.0f));
    }
    };

}

void pointIEEEPathologyTests() 
{
  using P3f = Point3f; using P3d = Point3d;
  using V3f = Vector<float, 3>; using V3d = Vector<double, 3>;

  "point/NaN_propagation/float"_test = [] {
    float qn = std::numeric_limits<float>::quiet_NaN();
    P3f p{ qn, 1.f, -2.f };
    V3f z{ 0.f,0.f,0.f };
    P3f r = p + z; expect(isNaN(r[0]) && r[1] == 1.f && r[2] == -2.f);
    V3f d1 = p - P3f{ 3.f,4.f,5.f }; expect(isNaN(d1[0]) && d1[1] == (1.f - 4.f) && d1[2] == (-2.f - 5.f));
    V3f d2 = p - p; expect(isNaN(d2[0]) && isNaN(d2[1]) == false && isNaN(d2[2]) == false);
    };

  "point/NaN_propagation/double"_test = [] {
    double qn = std::numeric_limits<double>::quiet_NaN();
    P3d p{ 1.0, qn, -2.0 }; V3d z{ 0.0,0.0,0.0 };
    P3d r = p + z; expect(r[0] == 1.0 && isNaN(r[1]) && r[2] == -2.0);
    V3d d1 = p - P3d{ 3.0,4.0,5.0 }; expect(d1[0] == -2.0 && isNaN(d1[1]) && d1[2] == -7.0);
    V3d d2 = p - p; expect(isNaN(d2[0]) == false && isNaN(d2[1]) && isNaN(d2[2]) == false);
    };

  "point/Inf_translation/float"_test = [] {
    float inf = std::numeric_limits<float>::infinity();
    P3f p{ inf, -inf, 1.f }; V3f v{ -5.f, 7.f, -3.f };
    P3f r = p + v; expect(isInf(r[0]) && isInf(r[1]) && r[2] == -2.f);
    V3f d = p - p; expect(isNaN(d[0]) && isNaN(d[1]) && d[2] == 0.f);
    };

  "point/Inf_translation/double"_test = [] {
    double inf = std::numeric_limits<double>::infinity();
    P3d p{ 1.0, -2.0, inf }; V3d v{ -1.0, 2.0, -5.0 };
    P3d r = p + v; expect(r[0] == 0.0 && r[1] == 0.0 && isInf(r[2]));
    V3d d = P3d{ inf, -inf, 0.0 } - P3d{ inf, -inf, 0.0 };
    expect(isNaN(d[0]) && isNaN(d[1]) && (d[2] == 0.0 || isNegZero(d[2])));
    };

  "point/overflow_to_inf_detection"_test = [] {
    float M = std::numeric_limits<float>::max();
    P3f p{ M, -M, 0.f }; V3f v{ M, M, 0.f };
    P3f r = p + v; expect(isInf(r[0]) && r[1] == 0.f && r[2] == 0.f);
    double Md = std::numeric_limits<double>::max();
    P3d P{ Md, -Md, 0.0 }; V3d V{ Md, Md, 0.0 };
    P3d R = P + V; expect(isInf(R[0]) && R[1] == 0.0 && R[2] == 0.0);
    };

  "point/subnormal_stability"_test = [] {
    float s = std::numeric_limits<float>::denorm_min();
    P3f p{ s, -s, 0.f }; V3f v{ -s, s, -s };
    P3f r = p + v; expect(isFinite(r[0]) && isFinite(r[1]) && isFinite(r[2]));
    double S = std::numeric_limits<double>::denorm_min();
    P3d P{ S, -S, 0.0 }; V3d V{ -S, S, -S };
    P3d R = P + V; expect(isFinite(R[0]) && isFinite(R[1]) && isFinite(R[2]));
    };

  "point/signed_zero_interactions"_test = [] {
    P3f p{ 0.f, -0.0f, 0.f }; V3f v{ -0.0f, 0.f, -0.0f }; P3f r = p + v;
    expect(r[0] == 0.f && r[1] == 0.f && r[2] == 0.f);
    V3f back = r - p; expect(euclideanLengthSq(back) == 0.0f);
    P3d P{ 0.0, -0.0, 0.0 }; V3d V{ -0.0, 0.0, -0.0 }; P3d R = P + V;
    expect(R[0] == 0.0 && R[1] == 0.0 && R[2] == 0.0);
    V3d Back = R - P; expect(euclideanLengthSq(Back) == 0.0);
    };
}

void point3Tests() {
  pointConstructionTests();
  pointAffineTests();
  pointGeometricRelationsTests();
  pointAccumulationRoundtripTests();
  pointIEEEPathologyTests();
}