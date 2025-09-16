#pragma once

#include "vec3-tests.hpp"
#include "../dep/ut.hpp"

import <cstdint>;

import std;
import vector;
import types;
import point;
import ray;

void rayTests() {
  using P = Point3f;
  using V = Vec3f;

  // Accessors and construction sanity
  "ray/ctor_and_accessors"_test = [] {
    P o{ 1.25f, -2.5f, 3.75f };
    V d{ -4.0f, 5.0f, -6.0f };
    Ray r{ o, d };
    expect(r.getOrigin()[0] == o[0] && r.getOrigin()[1] == o[1] && r.getOrigin()[2] == o[2]);
    expect(r.getDirection()[0] == d[0] && r.getDirection()[1] == d[1] && r.getDirection()[2] == d[2]);

    // t=0 -> origin, t=1 -> origin + d
    P p0 = r.getPointAt(0.0f);
    P p1 = r.getPointAt(1.0f);
    expect(p0 == o);
    expect(p1 == (o + d));
    };

  // getPointAt(t) == o + t d (randomized, tolerance-guarded)
  "ray/getPointAt_affine_correctness"_test = [] {
    std::mt19937_64 eng{ 0xC0FFEEULL };
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    std::uniform_real_distribution<float> dt(-100.f, 100.f);
    constexpr float eps = std::numeric_limits<float>::epsilon();

    for (int it = 0; it < 30000; ++it) {
      P o{ df(eng), df(eng), df(eng) };
      V d{ df(eng), df(eng), df(eng) };
      float t = dt(eng);
      Ray r{ o, d };

      P q = r.getPointAt(t);
      P ref = o + (d * t);

      for (int i = 0; i < 3; ++i) {
        float scale = std::max({ 1.0f, std::abs(o[i]), std::abs(d[i] * t), std::abs(ref[i]) });
        Tol tol{ double(8.0f * eps * scale), double(4.0f * eps), 4 };
        expect(almostEqual(q[i], ref[i], tol));
      }
    }
    };

  // Parameter arithmetic: r(a) + (b-a) d == r(b)
  "ray/param_increment_consistency"_test = [] {
    using P = Point3f; using V = Vec3f;
    std::mt19937_64 eng{ 0xA11CEB00ULL };
    std::uniform_real_distribution<float> df(-100.f, 100.f);
    std::uniform_real_distribution<float> dparam(-25.f, 25.f);
    constexpr float eps = std::numeric_limits<float>::epsilon();

    for (int iter = 0; iter < 20000; ++iter) {
      P o{ df(eng), df(eng), df(eng) };
      V d{ df(eng), df(eng), df(eng) };
      Ray r{ o, d };

      float a = dparam(eng), b = dparam(eng);
      P ra = r.getPointAt(a);
      P rb = r.getPointAt(b);
      P rb_ref = ra + d * (b - a);

      for (int i = 0; i < 3; ++i) {
        // Bound error by magnitude of operands participating in the final sum
        float mag = std::abs(ra[i]) + std::abs(d[i]) * std::abs(b - a) + 1.0f;
        // Generous absolute tol; relative tol keeps it sane when values grow
        Tol tol{ double(128.0f * eps * mag), 1e-5, 8 };
        expect(almostEqual(rb[i], rb_ref[i], tol));
      }
    }
    };

  //// Distance along the ray: |b-a| * ||d||
  "ray/parametric_distance_law"_test = [] {
    using P = Point3f; using V = Vec3f;
    std::mt19937_64 eng{ 0x1A2B3C4DULL };
    std::uniform_real_distribution<float> df(-200.f, 200.f);
    std::uniform_real_distribution<float> dparam(-50.f, 50.f);
    constexpr float eps = std::numeric_limits<float>::epsilon();

    for (int iter = 0; iter < 20000; ++iter) {
      P o{ df(eng), df(eng), df(eng) };
      V d{ df(eng), df(eng), df(eng) };
      Ray r{ o, d };

      float a = dparam(eng), b = dparam(eng);
      P pa = r.getPointAt(a);
      P pb = r.getPointAt(b);

      V diff = pb - pa;
      float lhs = euclideanLength(diff);
      float rhs = std::abs(b - a) * euclideanLength(d);

      // Big guard for catastrophic cancellation when |b-a| is tiny or |d| is large
      float scale = std::max({ 1.0f, lhs, rhs, std::abs(b - a) * (std::abs(d[0]) + std::abs(d[1]) + std::abs(d[2])) });
      Tol tol{ double(4096.0f * eps * scale), 5e-5, 16 };
      expect(almostEqual(lhs, rhs, tol));
    }
    };

  // Signed monotonicity per component when direction component ≠ 0
  "ray/monotonicity_componentwise"_test = [] {
    std::mt19937_64 eng{ 0xFEEDFACEULL };
    std::uniform_real_distribution<float> df(-1e2f, 1e2f);
    std::uniform_real_distribution<float> dt(-10.f, 10.f);

    for (int it = 0; it < 20000; ++it) {
      P o{ df(eng), df(eng), df(eng) };
      V d{ df(eng), df(eng), df(eng) };
      Ray r{ o, d };

      float t1 = dt(eng), t2 = dt(eng);
      if (t1 == t2) continue;
      if (t1 > t2) std::swap(t1, t2);

      P p1 = r.getPointAt(t1);
      P p2 = r.getPointAt(t2);

      for (int i = 0; i < 3; ++i) {
        if (d[i] > 0) expect(p2[i] >= p1[i]);
        else if (d[i] < 0) expect(p2[i] <= p1[i]);
        else expect(p2[i] == p1[i] || (p2[i] - p1[i] == 0.0f));
      }
    }
    };

  // Zero direction: ray is constant in t
  "ray/zero_direction_is_constant"_test = [] {
    P o{ 3.0f, -4.0f, 5.0f };
    V d{ 0.0f, 0.0f, 0.0f };
    Ray r{ o, d };

    float ts[] = { -1000.f, -1.f, 0.f, 1.f, 1000.f };
    for (float t : ts) {
      P q = r.getPointAt(t);
      expect(q == o);
    }
    };

  //// Parameter shift invariance: (o,d) vs (o + s d, d) with t' = t - s
  "ray/parameter_shift_invariance"_test = [] {
    using P = Point3f; using V = Vec3f;
    std::mt19937_64 eng{ 0xBADA5511ULL };
    std::uniform_real_distribution<float> df(-150.f, 150.f);
    std::uniform_real_distribution<float> dshift(-20.f, 20.f);
    constexpr float eps = std::numeric_limits<float>::epsilon();

    for (int iter = 0; iter < 20000; ++iter) {
      P o{ df(eng), df(eng), df(eng) };
      V d{ df(eng), df(eng), df(eng) };
      Ray r{ o, d };

      float s = dshift(eng);
      Ray r2{ o + d * s, d };

      float tval = dshift(eng);
      P q1 = r.getPointAt(tval);
      P q2 = r2.getPointAt(tval - s);

      // Compare in norm with a scale that reflects magnitudes of o, d, s, t
      V e = q2 - q1;
      float err = euclideanLength(e);
      float mag_o = std::abs(o[0]) + std::abs(o[1]) + std::abs(o[2]);
      float mag_d = std::abs(d[0]) + std::abs(d[1]) + std::abs(d[2]);
      float scale = std::max(1.0f, mag_o + mag_d * (std::abs(s) + std::abs(tval)));
      float bound = 4096.0f * eps * scale + 1e-5f; // generous absolute floor
      expect(err <= bound);
    }
    };

  // Reverse-direction equivalence: r(t) == r'(−t) for r' with -d
  "ray/reverse_direction_equivalence"_test = [] {
    std::mt19937_64 eng{ 0xC0DECAFEULL };
    std::uniform_real_distribution<float> df(-1e3f, 1e3f);
    std::uniform_real_distribution<float> dt(-100.f, 100.f);
    constexpr float eps = std::numeric_limits<float>::epsilon();

    for (int it = 0; it < 20000; ++it) {
      P o{ df(eng), df(eng), df(eng) };
      V d{ df(eng), df(eng), df(eng) };
      Ray r{ o, d };
      Ray rp{ o, V{-d[0], -d[1], -d[2]} };

      float t = dt(eng);
      P q = r.getPointAt(t);
      P qr = rp.getPointAt(-t);

      for (int i = 0; i < 3; ++i) {
        float scale = std::max({ 1.0f, std::abs(q[i]), std::abs(qr[i]) });
        Tol tol{ double(8.0f * eps * scale), double(4.0f * eps), 4 };
        expect(almostEqual(q[i], qr[i], tol));
      }
    }
    };

  // NaN/Inf propagation behavior (sanity, not prescriptive)
  "ray/ieee_propagation"_test = [] {
    float inf = std::numeric_limits<float>::infinity();
    float qn = std::numeric_limits<float>::quiet_NaN();

    // NaN in origin or direction should propagate to result in corresponding components
    {
      Ray r{ P{qn, 1.f, 2.f}, V{3.f, 4.f, 5.f} };
      P q = r.getPointAt(2.0f);
      expect(isNaN(q[0]) && isFinite(q[1]) && isFinite(q[2]));
    }
    {
      Ray r{ P{1.f, 2.f, 3.f}, V{qn, 4.f, 5.f} };
      P q = r.getPointAt(2.0f);
      expect(isNaN(q[0]) && isFinite(q[1]) && isFinite(q[2]));
    }

    // Infinities propagate when arithmetic overflows or operands are infinite
    {
      Ray r{ P{inf, 0.f, 0.f}, V{1.f, 2.f, 3.f} };
      P q = r.getPointAt(1.0f);
      expect(isInf(q[0]));
    }
    {
      Ray r{ P{0.f, 0.f, 0.f}, V{inf, 1.f, 1.f} };
      P q = r.getPointAt(0.5f);
      expect(isInf(q[0]));
    }
    };

  // Large-but-finite magnitudes remain finite when we stay within safe range
  "ray/finite_guard_large_mags"_test = [] {
    // Choose values that won't overflow float when combined
    P o{ 1e10f, -2e10f, 3e10f };
    V d{ 1e5f, 2e5f, -3e5f };
    float t = 1e3f; // o + t*d stays ~1e10 .. 1e11 (finite in float)
    Ray r{ o, d };
    P q = r.getPointAt(t);
    expect(isFinite(q[0]) && isFinite(q[1]) && isFinite(q[2]));
    };
}
