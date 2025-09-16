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
import hit_record;

void coreUtilTests() {
  using F = Float;

  // ============================
  // fresnelDielectric() tests
  // ============================

  // (1) Bounds and basic invariants: R ∈ [0,1], monotone to grazing; normal-incidence = R0
  "fresnel/basic_bounds_and_normal_incidence"_test = [] {
    // Helper: exact normal-incidence reflectance R0 = ((etaI-etaT)/(etaI+etaT))^2
    auto R0 = [](F ei, F et) {
      F r = (ei - et) / (ei + et);
      return r * r;
      };

    // A few common IOR pairs (air↔glass, air↔water, glass↔air)
    struct Case { F ei, et; };
    Case cases[] = {
      { F(1.0), F(1.5) },
      { F(1.0), F(1.33) },
      { F(1.5), F(1.0) },
      { F(1.0), F(2.4) }
    };

    for (auto c : cases) {
      // (a) Normal incidence must equal R0 (entering)
      {
        F R = fresnelDielectric(F(1.0), c.ei, c.et);
        Tol t{ 1e-6, 1e-5, 6 };
        expect(almostEqual(R, R0(c.ei, c.et), t));
        expect(R >= F(0) && R <= F(1));
      }

      // (b) Normal incidence must also equal R0 when exiting (cos < 0)
      {
        F R = fresnelDielectric(F(-1.0), c.ei, c.et);
        Tol t{ 1e-6, 1e-5, 6 };
        expect(almostEqual(R, R0(c.ei, c.et), t));
        expect(R >= F(0) && R <= F(1));
      }

      // (c) As cos→0 (grazing), reflectance increases (R(1) <= R(0))
      {
        F Rn = fresnelDielectric(F(1.0), c.ei, c.et);
        F Rg = fresnelDielectric(F(0.0), c.ei, c.et);
        expect(Rg >= Rn);
        expect(Rg <= F(1));
      }

      // (d) Clamp defense for cos outside [-1,1] should just behave like edges
      {
        Tol t{ 1e-6, 1e-5, 6 };
        F R_hi = fresnelDielectric(F(2.0), c.ei, c.et);   // should clamp to +1
        F R_lo = fresnelDielectric(F(-2.0), c.ei, c.et);  // should clamp to -1
        expect(almostEqual(R_hi, fresnelDielectric(F(1.0), c.ei, c.et), t));
        expect(almostEqual(R_lo, fresnelDielectric(F(-1.0), c.ei, c.et), t));
      }
    }
    };

  //// (2) Total Internal Reflection (TIR) when going from higher to lower IOR past critical angle
  "fresnel/total_internal_reflection"_test = [] {
    // Choose etaI > etaT and sinθi strictly above the critical value so TIR must occur
    std::mt19937_64 eng{ 0xF1EE1D0CULL };
    std::uniform_real_distribution<F> dEtaHi(F(1.2), F(2.5));
    std::uniform_real_distribution<F> dEtaLo(F(1.0), F(1.2));

    for (int it = 0; it < 20000; ++it) {
      F etaI = dEtaHi(eng), etaT = dEtaLo(eng);

      // sin θ_crit = etaT / etaI, pick sinθi a hair above this but < 1
      F sCrit = etaT / etaI;
      F s = std::min(F(0.999999), sCrit + F(1e-3));
      if (s <= sCrit) s = std::min(F(0.999999), sCrit + F(5e-3)); // ensure strictly above

      // cosθi from sinθi
      F c2 = F(1) - s * s;
      if (c2 < F(0)) c2 = F(0);
      F cosI = std::sqrt(c2);

      // Entering case (cosI > 0, etaI > etaT) must give (nearly) total reflection
      F R_enter = fresnelDielectric(+cosI, etaI, etaT);
      expect(R_enter <= F(1));
      expect(R_enter >= F(1) - F(1e-6));  // tolerate tiny numeric slack

      // Exiting case (cosI < 0) is *not* necessarily TIR after the internal swap.
      // Just check it remains a valid probability.
      F R_exit = fresnelDielectric(-cosI, etaI, etaT);
      expect(R_exit >= F(0) && R_exit <= F(1));
    }
    };

  // (3) Smoothness & monotonic rise toward grazing for a fixed (etaI, etaT)
  "fresnel/monotone_to_grazing"_test = [] {
    struct Pair { F ei, et; };
    Pair pairs[] = { {F(1.0), F(1.5)}, {F(1.5), F(1.0)}, {F(1.0), F(2.4)} };

    for (auto p : pairs) {
      // Sample cos from 1 down to 0; reflectance should be non-decreasing
      F prev = fresnelDielectric(F(1.0), p.ei, p.et);
      for (int i = 1; i <= 50; ++i) {
        F c = F(1.0) - F(i) / F(50); // 1, 0.98, ..., 0
        F R = fresnelDielectric(c, p.ei, p.et);
        expect(R >= prev - F(1e-6)); // tiny slack
        expect(R <= F(1));
        prev = std::max(prev, R);
      }
    }
    };

  // ============================
  // evaluateQuadratic() tests
  // ============================

  // (4) Known small cases (distinct roots, repeated root, negative discriminant)
  "quad/known_small_cases"_test = [] {
    Tol t{ 1e-6, 1e-6, 6 };

    // Distinct roots: (t-2)(t-5) = t^2 -7t +10
    {
      auto r = evaluateQuadratic(F(1), F(-7), F(10));
      expect(!!r);
      auto [t0, t1] = *r;
      expect(almostEqual(t0, F(2), t));
      expect(almostEqual(t1, F(5), t));
      expect(t0 <= t1);
    }

    // Repeated root: (t-3)^2 = t^2 -6t +9
    {
      auto r = evaluateQuadratic(F(1), F(-6), F(9));
      expect(!!r);
      auto [t0, t1] = *r;
      expect(almostEqual(t0, F(3), t));
      expect(almostEqual(t1, F(3), t));
    }

    // Negative discriminant -> nullopt
    {
      auto r = evaluateQuadratic(F(1), F(0), F(1)); // t^2+1
      expect(!r);
    }
    };

  // (5) Root residual, Vieta identities, and scaling invariance
  "quad/handpicked_residual_and_vieta"_test = [] {
    using F = Float;

    struct Case { F r0, r1; };
    const Case roots[] = {
      {F(2),   F(5)},      // well-separated
      {F(-3),  F(7)},      // mixed signs
      {F(1),   F(1)},      // double root
      {F(1e-3),F(-2e-3)},  // tiny, opposite signs
      {F(100), F(-0.5)}    // different magnitudes
    };
    const F scales[] = { F(1), F(0.5), F(2), F(-3) };

    auto poly = [](F a, F b, F c, F t) { return (a * t + b) * t + c; };

    for (auto cs : roots) {
      // Build monic coefficients from chosen roots
      F a = F(1);
      F b = -(cs.r0 + cs.r1);
      F c = (cs.r0 * cs.r1);

      for (F s : scales) {
        F as = a * s, bs = b * s, cs_ = c * s;

        auto sol = evaluateQuadratic(as, bs, cs_);
        if (!sol) { continue; } // discriminant < 0 (shouldn't happen for these)
        auto [t0, t1] = *sol;

        // Residuals on the *scaled* polynomial
        F R0 = poly(as, bs, cs_, t0);
        F R1 = poly(as, bs, cs_, t1);

        auto mag_for = [&](F t) {
          // scale-aware guard; +1 keeps tolerance nonzero near 0
          return std::abs(as) * (t * t) + std::abs(bs) * std::abs(t) + std::abs(cs_) + F(1);
          };
        F m0 = mag_for(t0), m1 = mag_for(t1);

        // Loose but reliable tolerances (works for float/double Float typedefs)
        Tol tolR0{ double(5e-6 * m0), 5e-6, 8 };
        Tol tolR1{ double(5e-6 * m1), 5e-6, 8 };

        expect(almostEqual(R0, F(0), tolR0));
        expect(almostEqual(R1, F(0), tolR1));

        // Vieta on scaled polynomial (order-free, just sum/product)
        F sum_t = t0 + t1;
        F prod_t = t0 * t1;
        F sumRef = -bs / as;
        F prodRef = cs_ / as;

        F sumScale = std::abs(sumRef) + std::abs(sum_t) + F(1);
        F prodScale = std::abs(prodRef) + std::abs(prod_t) + F(1);

        Tol tolSum{ double(5e-6 * sumScale),  5e-6, 8 };
        Tol tolProd{ double(5e-6 * prodScale), 5e-6, 8 };

        expect(almostEqual(sum_t, sumRef, tolSum));
        expect(almostEqual(prod_t, prodRef, tolProd));
      }
    }
    };


  // (6) Degenerate c=0 case -> one root at 0, the other at -b/a (provided a!=0)
  "quad/degenerate_c_zero"_test = [] {
    std::mt19937_64 eng{ 0xBADA5512ULL };
    std::uniform_real_distribution<F> da(F(1e-4), F(1e4)); // keep a!=0
    std::uniform_real_distribution<F> db(F(-1e4), F(1e4));
    Tol t{ 1e-6, 1e-6, 6 };

    for (int it = 0; it < 30000; ++it) {
      F a = da(eng);
      F b = db(eng);
      F c = F(0);

      auto r = evaluateQuadratic(a, b, c);
      expect(!!r);
      auto [t0, t1] = *r;
      expect(t0 <= t1);

      // Known roots: 0 and -b/a
      F r0 = F(0);
      F r1 = -b / a;

      // Sort expected to match ordering
      if (r0 > r1) std::swap(r0, r1);

      F scale = F(1) + std::max(std::abs(r0), std::abs(r1));
      Tol tt{ double(1e-6 * scale), double(1e-6), 6 };
      expect(almostEqual(t0, r0, tt));
      expect(almostEqual(t1, r1, tt));
    }
    };
}
