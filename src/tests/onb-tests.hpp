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

using namespace boost::ut;

void onbTests() {
  using V3f = Vec3f;
  using N3f = Normal3f;

  // Basic machine tolerances for float math
  constexpr float feps = std::numeric_limits<float>::epsilon();
  Tol tf_len{ 4e-7, 4e-6, 6 };     // length/unit tests
  Tol tf_dot{ 8e-7, 8e-6, 8 };     // orthogonality/dot≈0 tests
  Tol tf_cmp{ 1e-6, 1e-5, 8 };     // componentwise comparisons

  // Helpers
  auto len = [](const V3f& v) { return euclideanLength(v); };
  auto near_unit = [&](const V3f& v) {
    return almostEqual(len(v), 1.0f, tf_len);
    };
  auto ortho = [&](const V3f& a, const V3f& b) {
    return almostEqual(computeDot(a, b), 0.0f, tf_dot);
    };
  auto right_handed = [&](const V3f& t, const V3f& b, const V3f& n) {
    V3f c = computeCross(t, b);
    return almostEqual(c[0], n[0], tf_cmp) &&
      almostEqual(c[1], n[1], tf_cmp) &&
      almostEqual(c[2], n[2], tf_cmp);
    };
  auto toLocal = [](const OrthonormalBasis& B, const V3f& v) {
    return B.worldToLocal(v);
    };
  auto toWorld = [](const OrthonormalBasis& B, const V3f& v) {
    return B.localToWorld(v);
    };
  auto finite3 = [](const V3f& v) {
    return isFinite(v[0]) && isFinite(v[1]) && isFinite(v[2]);
    };

  // (1) Exact axes: verify orthonormal triad and handedness
  "onb/axes_orthonormal_and_handedness"_test = [&] {
    V3f axes[6] = {
      {1,0,0},{0,1,0},{0,0,1},
      {-1,0,0},{0,-1,0},{0,0,-1}
    };
    for (auto n : axes) {
      OrthonormalBasis B = OrthonormalBasis::fromPBRT(N3f{ n[0], n[1], n[2] });
      V3f t = B.getTangent();
      V3f b = B.getBitangent();
      V3f u = V3f{ B.getNormal()[0], B.getNormal()[1], B.getNormal()[2] };

      expect(near_unit(t));
      expect(near_unit(b));
      expect(near_unit(u));
      expect(ortho(t, u));
      expect(ortho(b, u));
      expect(ortho(t, b));
      expect(right_handed(t, b, u));

      // Basis maps local axes to world axes
      V3f ex{ 1,0,0 }, ey{ 0,1,0 }, ez{ 0,0,1 };
      V3f Tw = toWorld(B, ex);
      V3f Bw = toWorld(B, ey);
      V3f Nw = toWorld(B, ez);
      expect(almostEqual(Tw[0], t[0], tf_cmp) && almostEqual(Tw[1], t[1], tf_cmp) && almostEqual(Tw[2], t[2], tf_cmp));
      expect(almostEqual(Bw[0], b[0], tf_cmp) && almostEqual(Bw[1], b[1], tf_cmp) && almostEqual(Bw[2], b[2], tf_cmp));
      expect(almostEqual(Nw[0], u[0], tf_cmp) && almostEqual(Nw[1], u[1], tf_cmp) && almostEqual(Nw[2], u[2], tf_cmp));
    }
    };

  // (2) Roundtrip world<->local and component semantics
  "onb/local_world_inverse_and_component_semantics"_test = [&] {
    std::mt19937_64 eng{ 0xA11C3E0ULL };
    std::uniform_real_distribution<float> d(-1.0f, 1.0f);

    for (int it = 0; it < 40000; ++it) {
      // Random non-degenerate normal; avoid zero-length
      V3f rn{ d(eng), d(eng), d(eng) };
      if (euclideanLengthSq(rn) < 1e-8f) continue;

      OrthonormalBasis B = OrthonormalBasis::fromPBRT(N3f{ rn[0], rn[1], rn[2] });
      V3f t = B.getTangent();
      V3f b = B.getBitangent();
      V3f n = V3f{ B.getNormal()[0], B.getNormal()[1], B.getNormal()[2] };

      // Orthonormal + right-handed
      expect(near_unit(t) && near_unit(b) && near_unit(n));
      expect(ortho(t, n) && ortho(b, n) && ortho(t, b));
      expect(right_handed(t, b, n));

      // World->Local recovers dot components against basis
      V3f w{ d(eng) * 10, d(eng) * 10, d(eng) * 10 };
      V3f q = toLocal(B, w);
      float refX = computeDot(w, t);
      float refY = computeDot(w, b);
      float refZ = computeDot(w, n);
      expect(almostEqual(q[0], refX, tf_cmp));
      expect(almostEqual(q[1], refY, tf_cmp));
      expect(almostEqual(q[2], refZ, tf_cmp));

      // Local->World followed by World->Local is identity
      V3f vL{ d(eng) * 3, d(eng) * 3, d(eng) * 3 };
      V3f vW = toWorld(B, vL);
      V3f vL2 = toLocal(B, vW);
      expect(almostEqual(vL2[0], vL[0], tf_cmp));
      expect(almostEqual(vL2[1], vL[1], tf_cmp));
      expect(almostEqual(vL2[2], vL[2], tf_cmp));
    }
    };

  // (3) Normalization invariance: scaling input normal must not change the basis
  "onb/normalization_invariance"_test = [&] {
    std::mt19937_64 eng{ 0xBADA550ULL };
    std::uniform_real_distribution<float> d(-1.0f, 1.0f);
    std::uniform_real_distribution<float> s(0.1f, 10.0f);

    for (int it = 0; it < 30000; ++it) {
      V3f rn{ d(eng), d(eng), d(eng) };
      if (euclideanLengthSq(rn) < 1e-10f) continue;

      float k = s(eng);
      V3f sk = { rn[0] * k, rn[1] * k, rn[2] * k };

      OrthonormalBasis B1 = OrthonormalBasis::fromPBRT(N3f{ rn[0], rn[1], rn[2] });
      OrthonormalBasis B2 = OrthonormalBasis::fromPBRT(N3f{ sk[0], sk[1], sk[2] });

      V3f t1 = B1.getTangent(), b1 = B1.getBitangent(), n1{ B1.getNormal()[0], B1.getNormal()[1], B1.getNormal()[2] };
      V3f t2 = B2.getTangent(), b2 = B2.getBitangent(), n2{ B2.getNormal()[0], B2.getNormal()[1], B2.getNormal()[2] };

      // Allow possible sign flips on t/b if your construction can differ by a consistent 180° around n.
      auto same_or_neg = [&](float a, float b) { return almostEqual(a, b, tf_cmp) || almostEqual(a, -b, tf_cmp); };

      expect(same_or_neg(n1[0], n2[0]) && same_or_neg(n1[1], n2[1]) && same_or_neg(n1[2], n2[2]));
      expect(same_or_neg(t1[0], t2[0]) && same_or_neg(t1[1], t2[1]) && same_or_neg(t1[2], t2[2]));
      expect(same_or_neg(b1[0], b2[0]) && same_or_neg(b1[1], b2[1]) && same_or_neg(b1[2], b2[2]));
    }
    };

  // (4) Near-axis stress: normals extremely close to coordinate axes should remain finite & orthonormal
  "onb/near_axis_stability"_test = [&] {
    float e = 1e-6f;
    V3f tests[] = {
      { 1.0f,  e,    e    }, { -1.0f, e,    e    },
      {  e,   1.0f,  e    }, {  e,   -1.0f, e    },
      {  e,    e,   1.0f  }, {  e,    e,   -1.0f },
      { 0.0f, 1.0f,  e    }, { 1.0f, 0.0f,  e    }, { e, 0.0f, 1.0f }
    };
    for (auto n : tests) {
      OrthonormalBasis B = OrthonormalBasis::fromPBRT(N3f{ n[0], n[1], n[2] });
      V3f t = B.getTangent();
      V3f b = B.getBitangent();
      V3f u = V3f{ B.getNormal()[0], B.getNormal()[1], B.getNormal()[2] };

      expect(finite3(t) && finite3(b) && finite3(u));
      expect(near_unit(t) && near_unit(b) && near_unit(u));
      expect(ortho(t, u) && ortho(b, u) && ortho(t, b));
      expect(right_handed(t, b, u));
    }
    };

  // (5) Mapping sanity: local unit axes map to basis, and linearity holds
  "onb/mapping_axes_and_linearity"_test = [&] {
    std::mt19937_64 eng{ 0xC0FFEE44ULL };
    std::uniform_real_distribution<float> d(-1.0f, 1.0f);
    for (int it = 0; it < 30000; ++it) {
      V3f rn{ d(eng), d(eng), d(eng) };
      if (euclideanLengthSq(rn) < 1e-10f) continue;
      OrthonormalBasis B = OrthonormalBasis::fromPBRT(N3f{ rn[0], rn[1], rn[2] });

      V3f ex{ 1,0,0 }, ey{ 0,1,0 }, ez{ 0,0,1 };
      V3f Tw = toWorld(B, ex), Bw = toWorld(B, ey), Nw = toWorld(B, ez);
      V3f t = B.getTangent(), b = B.getBitangent(); V3f n{ B.getNormal()[0],B.getNormal()[1],B.getNormal()[2] };
      expect(almostEqual(Tw[0], t[0], tf_cmp) && almostEqual(Tw[1], t[1], tf_cmp) && almostEqual(Tw[2], t[2], tf_cmp));
      expect(almostEqual(Bw[0], b[0], tf_cmp) && almostEqual(Bw[1], b[1], tf_cmp) && almostEqual(Bw[2], b[2], tf_cmp));
      expect(almostEqual(Nw[0], n[0], tf_cmp) && almostEqual(Nw[1], n[1], tf_cmp) && almostEqual(Nw[2], n[2], tf_cmp));

      // Linearity: B(a*u + b*v) = a*B(u) + b*B(v)
      float a = d(eng) * 5.0f, bsc = d(eng) * 5.0f;
      V3f u{ d(eng), d(eng), d(eng) }, v{ d(eng), d(eng), d(eng) };
      V3f lhs = toWorld(B, u * a + v * bsc);
      V3f rhs = toWorld(B, u) * a + toWorld(B, v) * bsc;
      expect(almostEqual(lhs[0], rhs[0], tf_cmp));
      expect(almostEqual(lhs[1], rhs[1], tf_cmp));
      expect(almostEqual(lhs[2], rhs[2], tf_cmp));
    }
    };

  // (6) Local/global Z alignment: local (0,0,1) points along the provided normal direction
  "onb/local_z_aligns_with_input_normal"_test = [&] {
    std::mt19937_64 eng{ 0xDEADBEEF55ULL };
    std::uniform_real_distribution<float> d(-1.0f, 1.0f);
    for (int it = 0; it < 25000; ++it) {
      V3f rn{ d(eng), d(eng), d(eng) };
      if (euclideanLengthSq(rn) < 1e-10f) continue;
      OrthonormalBasis B = OrthonormalBasis::fromPBRT(N3f{ rn[0], rn[1], rn[2] });

      V3f n{ B.getNormal()[0], B.getNormal()[1], B.getNormal()[2] };
      V3f zW = B.localToWorld(V3f{ 0,0,1 });
      // They should match (both unit). Use a strict dot close to 1.
      float dzn = computeDot(zW, n);
      expect(almostEqual(dzn, 1.0f, Tol{ 2e-6, 2e-6, 6 }));
    }
    };

  // (7) Pathology guard: skip zero normal; ensure no NaN/Inf for small but non-zero normals
  "onb/degenerate_guard_nonzero_small"_test = [&] {
    float s = 1e-12f;
    V3f tiny[6] = {
      { s, 0, 0 }, { 0, s, 0 }, { 0, 0, s },
      { s, s, 0 }, { 0, s, s }, { s, 0, s }
    };
    for (auto n : tiny) {
      OrthonormalBasis B = OrthonormalBasis::fromPBRT(N3f{ n[0], n[1], n[2] });
      V3f t = B.getTangent();
      V3f b = B.getBitangent();
      V3f u = V3f{ B.getNormal()[0], B.getNormal()[1], B.getNormal()[2] };
      expect(finite3(t) && finite3(b) && finite3(u));
      // We only demand orthogonality and unit-ish (looser bound due to extreme conditioning)
      expect(ortho(t, u) && ortho(b, u));
      expect(almostEqual(len(t), 1.0f, Tol{ 1e-4,1e-4,6 }));
      expect(almostEqual(len(b), 1.0f, Tol{ 1e-4,1e-4,6 }));
      expect(almostEqual(len(u), 1.0f, Tol{ 1e-4,1e-4,6 }));
    }
    };
}
