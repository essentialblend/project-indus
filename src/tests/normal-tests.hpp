#pragma once

#include "vec3-tests.hpp"
#include "../dep/ut.hpp"

import <cstdint>;

import std;
import vector;
import types;
import point;
import normal;
import transform;
import onb;

void normalTests() {
  using Nf = Normal3f; using Nd = Normal3d;
  using Vf = Vec3f;    using Vd = Vec3d;

  // --- Construction & basic semantics -----------------------------------------------------------

  "normal/ctor_edgecases"_test = [] {
    Nf nz0{ 0.0f }; Nf nzm0{ -0.0f };
    for (int i = 0; i < 3; ++i) {
      expect(nz0[i] == 0.0f);
      expect(std::signbit(nzm0[i]));  // allow -0
    }
    Nd dz0{ 0.0 }; Nd dzm0{ -0.0 };
    for (int i = 0; i < 3; ++i) {
      expect(dz0[i] == 0.0);
      expect(std::signbit(dzm0[i]));
    }

    // From array
    {
      std::array<float, 3> a{ 1.25f,-2.5f,3.75f }; Nf n{ a };
      expect(n[0] == 1.25f && n[1] == -2.5f && n[2] == 3.75f);
    }
    {
      std::array<double, 3> a{ 1.25,-2.5,3.75 }; Nd n{ a };
      expect(n[0] == 1.25 && n[1] == -2.5 && n[2] == 3.75);
    }

    // From Vector
    {
      Vf v{ 2.f,-3.f,4.f }; Nf n{ v };
      expect(n[0] == 2.f && n[1] == -3.f && n[2] == 4.f);
    }
    {
      Vd v{ -5.0,6.0,7.5 }; Nd n{ v };
      expect(n[0] == -5.0 && n[1] == 6.0 && n[2] == 7.5);
    }
    };

  "normal/equality_reflexivity_symmetry_and_ulpmutation"_test = [] {
    // Float
    {
      std::mt19937_64 eng{ 0xA11CE001ULL };
      std::uniform_real_distribution<float> df(-1e5f, 1e5f);
      for (int t = 0; t < 30000; ++t) {
        Nf a{ df(eng), df(eng), df(eng) }; Nf b = a;
        expect(a == b);
        int k = int(std::uniform_int_distribution<int>(0, 2)(eng));
        float x = b[k];
        float toward = std::signbit(x) ? -std::numeric_limits<float>::infinity()
          : std::numeric_limits<float>::infinity();
        b = Nf{ b[0], b[1], b[2] };  // ensure lvalue container (no rvalue subscript)
        if (k == 0) b = Nf{ (x == 0.0f) ? std::numeric_limits<float>::denorm_min() : std::nextafter(x,toward), b[1], b[2] };
        if (k == 1) b = Nf{ b[0], (x == 0.0f) ? std::numeric_limits<float>::denorm_min() : std::nextafter(x,toward), b[2] };
        if (k == 2) b = Nf{ b[0], b[1], (x == 0.0f) ? std::numeric_limits<float>::denorm_min() : std::nextafter(x,toward) };
        expect(!(a == b));
      }
    }
    // Double
    {
      std::mt19937_64 eng{ 0xBADA5511ULL };
      std::uniform_real_distribution<double> dd(-1e8, 1e8);
      for (int t = 0; t < 30000; ++t) {
        Nd A{ dd(eng), dd(eng), dd(eng) }; Nd B = A;
        expect(A == B);
        int k = int(std::uniform_int_distribution<int>(0, 2)(eng));
        double x = B[k];
        double toward = std::signbit(x) ? -std::numeric_limits<double>::infinity()
          : std::numeric_limits<double>::infinity();
        B = Nd{ B[0], B[1], B[2] };
        if (k == 0) B = Nd{ (x == 0.0) ? std::numeric_limits<double>::denorm_min() : std::nextafter(x,toward), B[1], B[2] };
        if (k == 1) B = Nd{ B[0], (x == 0.0) ? std::numeric_limits<double>::denorm_min() : std::nextafter(x,toward), B[2] };
        if (k == 2) B = Nd{ B[0], B[1], (x == 0.0) ? std::numeric_limits<double>::denorm_min() : std::nextafter(x,toward) };
        expect(!(A == B));
      }
    }
    };

  // --- Arithmetic operators (no subscripting of temporaries!) -----------------------------------

  "normal/arith_identity_inverse_float_double"_test = [] {
    // Float
    {
      Nf a{ 1.25f,-2.5f,3.75f }, b{ -4.0f,5.0f,-6.0f }; float s = -2.0f;
      Nf z{ 0.0f };
      Nf t1 = a + z; expect(t1[0] == a[0] && t1[1] == a[1] && t1[2] == a[2]);
      Nf t2 = a - z; expect(t2[0] == a[0] && t2[1] == a[1] && t2[2] == a[2]);
      Nf t3 = a * 1.0f; expect(t3[0] == a[0] && t3[1] == a[1] && t3[2] == a[2]);
      Nf t4 = a / 1.0f; expect(t4[0] == a[0] && t4[1] == a[1] && t4[2] == a[2]);
      Nf t5 = (a + b) - b; expect(t5[0] == a[0] && t5[1] == a[1] && t5[2] == a[2]);
      Nf t6 = (a * s) / s; expect(t6[0] == a[0] && t6[1] == a[1] && t6[2] == a[2]);
    }
    // Double
    {
      Nd a{ 1.25,-2.5,3.75 }, b{ -4.0,5.0,-6.0 }; double s = 2.0;
      Nd z{ 0.0 };
      Nd t1 = a + z; expect(t1[0] == a[0] && t1[1] == a[1] && t1[2] == a[2]);
      Nd t2 = a - z; expect(t2[0] == a[0] && t2[1] == a[1] && t2[2] == a[2]);
      Nd t3 = a * 1.0; expect(t3[0] == a[0] && t3[1] == a[1] && t3[2] == a[2]);
      Nd t4 = a / 1.0; expect(t4[0] == a[0] && t4[1] == a[1] && t4[2] == a[2]);
      Nd t5 = (a + b) - b; expect(t5[0] == a[0] && t5[1] == a[1] && t5[2] == a[2]);
      Nd t6 = (a * s) / s; expect(t6[0] == a[0] && t6[1] == a[1] && t6[2] == a[2]);
    }
    };

  "normal/compound_assign_and_selfalias"_test = [] {
    // Float
    {
      Nf a{ 3.f,-4.f,5.f }, b{ -1.f,2.f,-3.f }; float s = -3.0f;
      Nf u = a; u += b; Nf ref1 = a + b; expect(u[0] == ref1[0] && u[1] == ref1[1] && u[2] == ref1[2]);
      u = a; u -= b; Nf ref2 = a - b; expect(u[0] == ref2[0] && u[1] == ref2[1] && u[2] == ref2[2]);
      u = a; u *= s; Nf ref3 = a * s; expect(u[0] == ref3[0] && u[1] == ref3[1] && u[2] == ref3[2]);
      u = a; u /= s; Nf ref4 = a / s; expect(u[0] == ref4[0] && u[1] == ref4[1] && u[2] == ref4[2]);

      u = a; u += u; Nf r1 = a + a; expect(u[0] == r1[0] && u[1] == r1[1] && u[2] == r1[2]);
      u = a; u -= u; expect(u[0] == 0.f && u[1] == 0.f && u[2] == 0.f);
    }
    // Double
    {
      Nd a{ 3.0,-4.0,5.0 }, b{ -1.0,2.0,-3.0 }; double s = 3.5;
      Nd u = a; u += b; Nd ref1 = a + b; expect(u[0] == ref1[0] && u[1] == ref1[1] && u[2] == ref1[2]);
      u = a; u -= b; Nd ref2 = a - b; expect(u[0] == ref2[0] && u[1] == ref2[1] && u[2] == ref2[2]);
      u = a; u *= s; Nd ref3 = a * s; expect(u[0] == ref3[0] && u[1] == ref3[1] && u[2] == ref3[2]);
      u = a; u /= s; Nd ref4 = a / s; expect(u[0] == ref4[0] && u[1] == ref4[1] && u[2] == ref4[2]);

      u = a; u += u; Nd r1 = a + a; expect(u[0] == r1[0] && u[1] == r1[1] && u[2] == r1[2]);
      u = a; u -= u; expect(u[0] == 0.0 && u[1] == 0.0 && u[2] == 0.0);
    }
    };

  "normal/unary_minus_and_signed_zero"_test = [] {
    // Float
    {
      Nf v{ 1.5f,-2.0f,0.0f };
      Nf u = -v; expect(u[0] == -1.5f && u[1] == 2.0f && (u[2] == -0.0f || u[2] == 0.0f));
      Nf w = -u; expect(w == v);

      Nf z{ -0.0f, 0.0f, -0.0f };
      Nf nz = -z;
      expect(nz[0] == 0.0f && !std::signbit(nz[0]));
      expect(nz[1] == -0.0f && std::signbit(nz[1]));
      expect(nz[2] == 0.0f && !std::signbit(nz[2]));
    }
    // Double
    {
      Nd v{ 1.5,-2.0,0.0 };
      Nd u = -v; expect(u[0] == -1.5 && u[1] == 2.0 && (u[2] == -0.0 || u[2] == 0.0));
      Nd w = -u; expect(w == v);

      Nd z{ -0.0, 0.0, -0.0 };
      Nd nz = -z;
      expect(nz[0] == 0.0 && !std::signbit(nz[0]));
      expect(nz[1] == -0.0 && std::signbit(nz[1]));
      expect(nz[2] == 0.0 && !std::signbit(nz[2]));
    }
    };

  // --- Dot products & metric identities ---------------------------------------------------------

  "normal/dot_variants_and_cauchy_schwarz"_test = [] {
    // Float
    {
      std::mt19937_64 eng{ 0x9E3779B97F4A7C15ULL };
      std::uniform_real_distribution<float> d(-1e3f, 1e3f);
      Tol tf{ 1e-5,1e-5,6 };

      for (int t = 0; t < 30000; ++t) {
        Nf n{ d(eng), d(eng), d(eng) };
        Vf v{ d(eng), d(eng), d(eng) };

        float nv = computeDot(n, v);
        float vn = computeDot(v, n);
        expect(almostEqual(nv, vn, tf));

        float nn = computeDot(n, n);
        float vv = computeDot(v, v);
        float lhs = std::abs(nv);
        float rhs = std::sqrt(std::max(0.0f, nn)) * std::sqrt(std::max(0.0f, vv));
        expect(lhs <= rhs || almostEqual(lhs, rhs, tf));

        float an = computeAbsDot(n, v);
        expect(almostEqual(an, std::abs(nv), tf));
      }
    }
    // Double
    {
      std::mt19937_64 eng{ 0xC6A4A7935BD1E995ULL };
      std::uniform_real_distribution<double> d(-1e6, 1e6);
      Tol td{ 1e-12,1e-10,8 };

      for (int t = 0; t < 30000; ++t) {
        Nd n{ d(eng), d(eng), d(eng) };
        Vd v{ d(eng), d(eng), d(eng) };

        double nv = computeDot(n, v);
        double vn = computeDot(v, n);
        expect(almostEqual(nv, vn, td));

        double nn = computeDot(n, n);
        double vv = computeDot(v, v);
        double lhs = std::abs(nv);
        double rhs = std::sqrt(std::max(0.0, nn)) * std::sqrt(std::max(0.0, vv));
        expect(lhs <= rhs || almostEqual(lhs, rhs, td));

        double an = computeAbsDot(n, v);
        expect(almostEqual(an, std::abs(nv), td));
      }
    }
    };

  "normal/normalize_unit_length_direction_and_zero_policy"_test = [] {
    // Float
    {
      std::mt19937_64 eng{ 0xDA942042E4DD58B5ULL };
      std::uniform_real_distribution<float> d(-1e3f, 1e3f);
      constexpr float eps = std::numeric_limits<float>::epsilon();

      for (int t = 0; t < 30000; ++t) {
        Nf n{ d(eng), d(eng), d(eng) };
        float lsq = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
        if (lsq <= 1e-20f) continue;

        Nf u = normalize(n);
        Tol tlen{ double(64.f * eps), double(32.f * eps), 8 };
        float un = std::sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
        expect(almostEqual(un, 1.0f, tlen));

        float nv = computeDot(u, Vf{ n[0], n[1], n[2] });
        float mag = std::sqrt(lsq);
        Tol tdir{ double(128.f * eps * std::max(1.0f,mag)), double(64.f * eps), 8 };
        expect(almostEqual(nv, mag, tdir));
      }

      Nf z{ 0.f,0.f,0.f };
      Nf nz = normalize(z);
      expect(nz[0] == 0.f && nz[1] == 0.f && nz[2] == 0.f);
    }
    // Double
    {
      std::mt19937_64 eng{ 0xC1357BDF8E9A2461ULL };
      std::uniform_real_distribution<double> d(-1e6, 1e6);
      constexpr double eps = std::numeric_limits<double>::epsilon();

      for (int t = 0; t < 30000; ++t) {
        Nd n{ d(eng), d(eng), d(eng) };
        double lsq = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
        if (lsq <= 1e-40) continue;

        Nd u = normalize(n);
        Tol tlen{ 64.0 * eps, 32.0 * eps, 8 };
        double un = std::sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
        expect(almostEqual(un, 1.0, tlen));

        double nv = computeDot(u, Vd{ n[0], n[1], n[2] });
        double mag = std::sqrt(lsq);
        Tol tdir{ 128.0 * eps * std::max(1.0,mag), 64.0 * eps, 8 };
        expect(almostEqual(nv, mag, tdir));
      }

      Nd z{ 0.0,0.0,0.0 };
      Nd nz = normalize(z);
      expect(nz[0] == 0.0 && nz[1] == 0.0 && nz[2] == 0.0);
    }
    };

  "normal/normalize_scale_invariance"_test = [] {
    using Nf = Normal3f; using Nd = Normal3d;

    // Float
    {
      std::mt19937_64 eng{ 0xD1B54A32D192ED03ULL };
      std::uniform_real_distribution<float> dv(-1e3f, 1e3f), ds(-1e3f, 1e3f);
      constexpr float eps = std::numeric_limits<float>::epsilon();

      for (int iter = 0; iter < 30000; ++iter) {
        Nf n{ dv(eng), dv(eng), dv(eng) }; float s = ds(eng);
        float lsq = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
        if (lsq <= 1e-20f || s == 0.0f) continue;

        Nf u1 = normalize(n);
        Nf sn{ n[0] * s, n[1] * s, n[2] * s };
        Nf u2 = normalize(sn);
        Nf ref = (s > 0.0f) ? u1 : Nf{ -u1[0], -u1[1], -u1[2] };

        Tol tol{ double(64.f * eps), double(64.f * eps), 8 };
        for (int i = 0; i < 3; ++i) expect(almostEqual(u2[i], ref[i], tol));
      }
    }

    // Double
    {
      std::mt19937_64 eng{ 0x9C0FFEE0DADA5533ULL };
      std::uniform_real_distribution<double> dv(-1e6, 1e6), ds(-1e6, 1e6);
      constexpr double eps = std::numeric_limits<double>::epsilon();

      for (int iter = 0; iter < 30000; ++iter) {
        Nd n{ dv(eng), dv(eng), dv(eng) }; double s = ds(eng);
        double lsq = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
        if (lsq <= 1e-40 || s == 0.0) continue;

        Nd u1 = normalize(n);
        Nd sn{ n[0] * s, n[1] * s, n[2] * s };
        Nd u2 = normalize(sn);
        Nd ref = (s > 0.0) ? u1 : Nd{ -u1[0], -u1[1], -u1[2] };

        Tol tol{ 64.0 * eps, 64.0 * eps, 8 };
        for (int i = 0; i < 3; ++i) expect(almostEqual(u2[i], ref[i], tol));
      }
    }
    };


  // --- Interactions with Vector (projection & rejection) ----------------------------------------

  "normal/projection_rejection_float_double"_test = [] {
    using Nf = Normal3f; using Nd = Normal3d;
    using Vf = Vec3f;     using Vd = Vec3d;

    // --- Float ---
    {
      std::mt19937_64 eng{ 0x517CC1B727220A95ULL };
      std::uniform_real_distribution<float> d(-1e3f, 1e3f);
      constexpr float eps = std::numeric_limits<float>::epsilon();

      for (int it = 0; it < 20000; ++it) {
        Nf n{ d(eng), d(eng), d(eng) };
        // Skip degenerate normals
        float lsq = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
        if (lsq <= 1e-20f) continue;

        Vf v{ d(eng), d(eng), d(eng) };

        // Project using unit normal to control conditioning
        Nf u = normalize(n);
        float nv = computeDot(u, v);
        Vf proj{ u[0] * nv, u[1] * nv, u[2] * nv };
        Vf rej{ v[0] - proj[0], v[1] - proj[1], v[2] - proj[2] };

        // 1) Orthogonality: u · rej ≈ 0 with a scale-aware bound
        float dot_u_rej = u[0] * rej[0] + u[1] * rej[1] + u[2] * rej[2];
        float v1 = std::abs(v[0]) + std::abs(v[1]) + std::abs(v[2]);
        float p1 = std::abs(proj[0]) + std::abs(proj[1]) + std::abs(proj[2]);
        float r1 = std::abs(rej[0]) + std::abs(rej[1]) + std::abs(rej[2]);
        float bound_ortho = 128.f * eps * (v1 + p1 + r1 + std::abs(nv) + 1.0f); // generous but principled
        expect(std::abs(dot_u_rej) <= bound_ortho);

        // 2) Decomposition: proj + rej ≈ v
        Vf chk{ proj[0] + rej[0] - v[0], proj[1] + rej[1] - v[1], proj[2] + rej[2] - v[2] };
        float chk_norm = std::sqrt(chk[0] * chk[0] + chk[1] * chk[1] + chk[2] * chk[2]);
        float v_norm = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        float pr_norm = std::sqrt(proj[0] * proj[0] + proj[1] * proj[1] + proj[2] * proj[2]) +
          std::sqrt(rej[0] * rej[0] + rej[1] * rej[1] + rej[2] * rej[2]);
        float bound_recomp = 128.f * eps * (v_norm + pr_norm + 1.0f);
        expect(chk_norm <= bound_recomp);
      }
    }

    // --- Double ---
    {
      std::mt19937_64 eng{ 0xA24F4F5A1B3C9D07ULL };
      std::uniform_real_distribution<double> d(-1e6, 1e6);
      constexpr double eps = std::numeric_limits<double>::epsilon();

      for (int it = 0; it < 20000; ++it) {
        Nd n{ d(eng), d(eng), d(eng) };
        double lsq = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
        if (lsq <= 1e-40) continue;

        Vd v{ d(eng), d(eng), d(eng) };

        Nd u = normalize(n);
        double nv = computeDot(u, v);
        Vd proj{ u[0] * nv, u[1] * nv, u[2] * nv };
        Vd rej{ v[0] - proj[0], v[1] - proj[1], v[2] - proj[2] };

        double dot_u_rej = u[0] * rej[0] + u[1] * rej[1] + u[2] * rej[2];
        double v1 = std::abs(v[0]) + std::abs(v[1]) + std::abs(v[2]);
        double p1 = std::abs(proj[0]) + std::abs(proj[1]) + std::abs(proj[2]);
        double r1 = std::abs(rej[0]) + std::abs(rej[1]) + std::abs(rej[2]);
        double bound_ortho = 256.0 * eps * (v1 + p1 + r1 + std::abs(nv) + 1.0);
        expect(std::abs(dot_u_rej) <= bound_ortho);

        Vd chk{ proj[0] + rej[0] - v[0], proj[1] + rej[1] - v[1], proj[2] + rej[2] - v[2] };
        double chk_norm = std::sqrt(chk[0] * chk[0] + chk[1] * chk[1] + chk[2] * chk[2]);
        double v_norm = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        double pr_norm = std::sqrt(proj[0] * proj[0] + proj[1] * proj[1] + proj[2] * proj[2]) +
          std::sqrt(rej[0] * rej[0] + rej[1] * rej[1] + rej[2] * rej[2]);
        double bound_recomp = 256.0 * eps * (v_norm + pr_norm + 1.0);
        expect(chk_norm <= bound_recomp);
      }
    }
    };


  // --- IEEE-754 pathologies ---------------------------------------------------------------------

  "normal/ieee_nan_inf_and_subnormal"_test = [] {
    // Float
    {
      float qn = std::numeric_limits<float>::quiet_NaN();
      float inf = std::numeric_limits<float>::infinity();
      Nf n{ qn, 1.f, -2.f }, m{ inf, -inf, 0.f };

      Nf s1 = n + Nf{ 0.f };
      expect(isNaN(s1[0]) && s1[1] == 1.f && s1[2] == -2.f);

      Nf d1 = n - Nf{ 3.f,4.f,5.f };
      expect(isNaN(d1[0]) && d1[1] == (1.f - 4.f) && d1[2] == (-2.f - 5.f));

      Nf s2 = m + Nf{ -5.f, 7.f, -3.f };
      expect(isInf(s2[0]) && isInf(s2[1]) && s2[2] == -3.f);

      // subnormal stability
      float s = std::numeric_limits<float>::denorm_min();
      Nf sn{ s, -s, 0.f }; Nf r = sn * 2.0f;
      expect(isFinite(r[0]) && isFinite(r[1]) && isFinite(r[2]));
    }
    // Double
    {
      double qn = std::numeric_limits<double>::quiet_NaN();
      double inf = std::numeric_limits<double>::infinity();
      Nd n{ 1.0, qn, -2.0 }, m{ inf, -inf, 0.0 };

      Nd s1 = n + Nd{ 0.0 };
      expect(s1[0] == 1.0 && isNaN(s1[1]) && s1[2] == -2.0);

      Nd d1 = n - Nd{ 3.0,4.0,5.0 };
      expect(d1[0] == -2.0 && isNaN(d1[1]) && d1[2] == -7.0);

      Nd s2 = m + Nd{ -5.0, 7.0, -3.0 };
      expect(isInf(s2[0]) && isInf(s2[1]) && s2[2] == -3.0);

      double S = std::numeric_limits<double>::denorm_min();
      Nd sn{ S, -S, 0.0 }; Nd r = sn * 2.0;
      expect(isFinite(r[0]) && isFinite(r[1]) && isFinite(r[2]));
    }
    };

  // --- Stress vs Vector algebra (consistency with Vec3) ----------------------------------------

  "normal/vector_consistency_randomized"_test = [] {
    std::mt19937_64 eng{ 0x2545F4914F6CDD1DULL };
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    std::uniform_real_distribution<double> dd(-1e6, 1e6);
    Tol tf{ 1e-5,1e-5,6 }, td{ 1e-12,1e-10,8 };

    // Float
    for (int t = 0; t < 25000; ++t) {
      Vf v{ df(eng), df(eng), df(eng) }; Nf n{ v };
      float dn = computeDot(n, v);
      float vv = computeDot(v, v);
      expect(almostEqual(dn, vv, tf)); // n == v => n·v == v·v
    }
    // Double
    for (int t = 0; t < 25000; ++t) {
      Vd v{ dd(eng), dd(eng), dd(eng) }; Nd n{ v };
      double dn = computeDot(n, v);
      double vv = computeDot(v, v);
      expect(almostEqual(dn, vv, td));
    }
    };
}



