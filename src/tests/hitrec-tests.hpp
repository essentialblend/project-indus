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
import onb;
import hit_record;

void hitRecordTests() {
  using V = Vec3f;
  using P = Point3f;
  using N = Normal3f;

  // (1) Front/back orientation: oriented shading normal opposes the ray direction
  "hit/orientation_front_back"_test = [] {
    Tol t{ 1e-6, 1e-6, 6 };

    // Front face: dir • outward < 0
    {
      Ray r{ P{0,0,0}, V{0,0,-1} };
      HitRecord h{};
      h.setFaceNormal(r, N{ 0,0,1 });
      expect(h.hitFrontFace);
      auto n = h.shadingBasis.getNormal();
      V nv{ n[0], n[1], n[2] };
      expect(almostEqual(computeDot(r.getDirection(), nv), 0.0f, t) || computeDot(r.getDirection(), nv) < 0.0f);
    }

    // Back face: dir • outward ≥ 0 -> flip
    {
      Ray r{ P{0,0,0}, V{0,0,-1} };
      HitRecord h{};
      h.setFaceNormal(r, N{ 0,0,-1 });
      expect(!h.hitFrontFace);
      auto n = h.shadingBasis.getNormal();
      V nv{ n[0], n[1], n[2] };
      // Should have been flipped to +Z, still opposing the ray
      expect(almostEqual(computeDot(r.getDirection(), nv), 0.0f, t) || computeDot(r.getDirection(), nv) < 0.0f);
    }

    // Non-axis case, scaled outward (sign preserved)
    {
      Ray r{ P{1,2,3}, V{0.3f, -0.1f, 0.9f} };
      V out{ -0.3f, 0.1f, -0.9f }; // opposite direction; scale it
      out = out * 3.0f;
      HitRecord h{};
      h.setFaceNormal(r, N{ out[0], out[1], out[2] });
      auto n = h.shadingBasis.getNormal();
      V nv{ n[0], n[1], n[2] };
      expect(computeDot(r.getDirection(), nv) <= 0.0f);
    }
    };

  // (2) Basis is orthonormal and right-handed; normal aligns with ONB Z
  "hit/basis_orthonormality"_test = [] {
    Tol t{ 1e-5, 1e-5, 8 };

    auto check_basis = [&](const V& dir, const N& outward) {
      Ray r{ P{0,0,0}, dir };
      HitRecord h{};
      h.setFaceNormal(r, outward);

      auto n = h.shadingBasis.getNormal();
      V nv{ n[0], n[1], n[2] };
      V tng = h.shadingBasis.getTangent();
      V btg = h.shadingBasis.getBitangent();

      auto norm = [](const V& a) { return std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]); };

      // unit lengths (within tolerance)
      expect(almostEqual(norm(nv), 1.0f, t));
      expect(almostEqual(norm(tng), 1.0f, t));
      expect(almostEqual(norm(btg), 1.0f, t));

      // orthogonality
      expect(almostEqual(computeDot(nv, tng), 0.0f, t));
      expect(almostEqual(computeDot(nv, btg), 0.0f, t));
      expect(almostEqual(computeDot(tng, btg), 0.0f, t));

      // right-handed: t × b ≈ n (positive alignment)
      V cx = computeCross(tng, btg);
      float align = computeDot(cx, nv);
      expect(align > 0.0f || almostEqual(align, 0.0f, t));

      // Oriented against incoming ray
      expect(computeDot(dir, nv) <= 0.0f);
      };

    check_basis(V{ 0,0,-1 }, N{ 0,0,1 });
    check_basis(V{ 0.2f, 0.9f, -0.1f }, N{ 0.3f, -0.5f, 0.8f });
    check_basis(V{ -0.7f, 0.1f, 0.6f }, N{ 0.4f, 0.2f, -0.9f });
    };

  // (3) World<->local round-trip consistency for the ONB stored in HitRecord
  "hit/local_world_roundtrip"_test = [] {
    Tol t{ 1e-5, 1e-5, 8 };

    auto roundtrip = [&](const V& dir, const N& outward, const V& local) {
      Ray r{ P{0,0,0}, dir };
      HitRecord h{}; h.setFaceNormal(r, outward);

      V w = h.shadingBasis.localToWorld(local);
      V back = h.shadingBasis.worldToLocal(w);

      for (int i = 0; i < 3; ++i) {
        float scale = std::max(1.0f, std::max(std::abs(local[i]), std::abs(back[i])));
        Tol tt{ double(64.0f * std::numeric_limits<float>::epsilon() * scale), 1e-5, 8 };
        expect(almostEqual(back[i], local[i], tt));
      }
      };

    roundtrip(V{ 0,0,-1 }, N{ 0,0,1 }, V{ 0.3f, -0.2f, 0.9f });
    roundtrip(V{ 0.8f, -0.1f, -0.6f }, N{ 0.1f, 0.7f, 0.7f }, V{ -0.5f, 0.4f, -0.1f });
    roundtrip(V{ -0.2f, 0.4f, -0.9f }, N{ 0.6f, -0.3f, 0.74f }, V{ 0.0f, 1.0f, 0.0f });
    };

  // (4) Normal mapped to local space is (0,0,1) and remains unit; dir·n ≤ 0 always
  "hit/normal_is_local_z"_test = [] {
    Tol t{ 1e-5, 1e-5, 8 };

    auto check = [&](const V& dir, const N& outward) {
      Ray r{ P{0,0,0}, dir };
      HitRecord h{}; h.setFaceNormal(r, outward);

      auto n = h.shadingBasis.getNormal();
      V nv{ n[0], n[1], n[2] };
      V loc = h.shadingBasis.worldToLocal(nv);

      expect(almostEqual(loc[0], 0.0f, t));
      expect(almostEqual(loc[1], 0.0f, t));
      expect(almostEqual(loc[2], 1.0f, t));
      expect(computeDot(dir, nv) <= 0.0f);
      };

    check(V{ 0,0,-1 }, N{ 0,0,1 });
    check(V{ 0.5f, 0.2f, -0.8f }, N{ -0.1f, 0.4f, 0.91f });
    };

  // (5) Stress: random finite cases keep invariants (no NaNs; orientation; orthonormality)
  "hit/random_stress_invariants"_test = [] {
    std::mt19937_64 eng{ 0xC0FFEE1234ULL };
    std::uniform_real_distribution<float> d(-1.0f, 1.0f);
    Tol t{ 1e-4, 1e-4, 8 };

    auto norm = [](const V& a) { return std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]); };

    for (int it = 0; it < 20000; ++it) {
      V dir{ d(eng), d(eng), d(eng) };
      if (norm(dir) < 1e-6f) continue;
      V out{ d(eng), d(eng), d(eng) };
      if (norm(out) < 1e-6f) continue;

      Ray r{ P{0,0,0}, dir };
      HitRecord h{};
      h.setFaceNormal(r, N{ out[0], out[1], out[2] });

      auto n = h.shadingBasis.getNormal();
      V nv{ n[0], n[1], n[2] };
      V tng = h.shadingBasis.getTangent();
      V btg = h.shadingBasis.getBitangent();

      // Orientation
      expect(computeDot(dir, nv) <= 0.0f);

      // Orthonormal (looser tolerances for random inputs)
      expect(almostEqual(norm(nv), 1.0f, t));
      expect(almostEqual(norm(tng), 1.0f, t));
      expect(almostEqual(norm(btg), 1.0f, t));
      expect(almostEqual(computeDot(nv, tng), 0.0f, t));
      expect(almostEqual(computeDot(nv, btg), 0.0f, t));
      expect(almostEqual(computeDot(tng, btg), 0.0f, t));

      // Right-handed check
      V cx = computeCross(tng, btg);
      float align = computeDot(cx, nv);
      expect(align > -1e-4f); // should be ≥ 0, loose guard against FP noise
    }
    };
}
