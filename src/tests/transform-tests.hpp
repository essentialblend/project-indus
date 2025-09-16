#pragma once

#include "../dep/ut.hpp"

import <cstdint>;

import std;
import vector;
import types;
import point;
import matrix;
import transform;

using namespace boost::ut;

void transformConformanceTests()
{
  using V3d = Vector<double, 3>;
  using V4d = Vector<double, 4>;
  using P3d = Point<double, 3>;
  using M4d = Matrix4<double>;
  using Txd = Transform<double>;

  // --- translate & scale semantics on points/vectors; inverse round-trip; composition order ---
  "transform/translate_scale_basic_inverse_and_order"_test = [] {
    // Helpers (local to avoid capture issues)
    auto apply_point = [](const M4d& M, const P3d& p) {
      V4d hp{ p[0], p[1], p[2], 1.0 };
      V4d hq = M * hp;
      if (hq[3] != 0.0 && hq[3] != 1.0) {
        double iw = 1.0 / hq[3];
        return P3d{ hq[0] * iw, hq[1] * iw, hq[2] * iw };
      }
      return P3d{ hq[0], hq[1], hq[2] };
      };
    auto apply_vec = [](const M4d& M, const V3d& v) {
      V4d hv{ v[0], v[1], v[2], 0.0 };
      V4d hq = M * hv;
      return V3d{ hq[0], hq[1], hq[2] };
      };

    Tol td{ 1e-12,1e-10,8 };

    P3d p{ 1.5, -2.0, 3.25 };
    V3d v{ -0.75, 4.0, 1.0 };

    // translate
    Txd Ts = Txd::translate(V3d{ 2.0, -3.0, 5.0 });
    P3d pt = Ts(p);
    expect(almostEqual(pt[0], p[0] + 2.0, td));
    expect(almostEqual(pt[1], p[1] - 3.0, td));
    expect(almostEqual(pt[2], p[2] + 5.0, td));
    // vectors are invariant to translation
    V3d vt = Ts(v);
    expect(almostEqual(vt[0], v[0], td));
    expect(almostEqual(vt[1], v[1], td));
    expect(almostEqual(vt[2], v[2], td));
    // inverse round-trip
    P3d p0 = apply_point(Ts.getInv(), pt);
    expect(almostEqual(p0[0], p[0], td) && almostEqual(p0[1], p[1], td) && almostEqual(p0[2], p[2], td));

    // scale (non-zero)
    Txd Sc = Txd::scale(V3d{ 2.0, -3.0, 4.0 });
    P3d ps = Sc(p);
    expect(almostEqual(ps[0], p[0] * 2.0, td));
    expect(almostEqual(ps[1], p[1] * -3.0, td));
    expect(almostEqual(ps[2], p[2] * 4.0, td));
    V3d vs = Sc(v);
    expect(almostEqual(vs[0], v[0] * 2.0, td));
    expect(almostEqual(vs[1], v[1] * -3.0, td));
    expect(almostEqual(vs[2], v[2] * 4.0, td));
    // inverse round-trip
    P3d p1 = apply_point(Sc.getInv(), ps);
    expect(almostEqual(p1[0], p[0], td) && almostEqual(p1[1], p[1], td) && almostEqual(p1[2], p[2], td));

    // composition order: (A*B)(p) == A(B(p)); rightmost applies first
    Txd A = Txd::translate(V3d{ 1.0, 2.0, 3.0 });
    Txd B = Txd::scale(V3d{ 2.0, 3.0, 4.0 });
    Txd AB = A * B;
    P3d rL = AB(p);
    P3d rR = A(B(p));
    expect(almostEqual(rL[0], rR[0], td));
    expect(almostEqual(rL[1], rR[1], td));
    expect(almostEqual(rL[2], rR[2], td));
    };

  // --- perspective: XY back-solve using actual matrix coefficients from your builder (w' = z path) ---
  "transform/perspective_xy_backsolve"_test = [] {
    Tol td{ 1e-12,1e-10,10 };

    auto ndc_div = [](const V4d& h) {
      const double iw = 1.0 / h[3];
      return V4d{ h[0] * iw, h[1] * iw, h[2] * iw, 1.0 };
      };

    std::mt19937_64 eng{ 0xC0FFEEAAULL };
    std::uniform_real_distribution<double> fov_deg(35.0, 95.0);
    std::uniform_real_distribution<double> znear(0.05, 3.0);
    std::uniform_real_distribution<double> zfar(5.0, 1000.0);
    std::uniform_real_distribution<double> u01(-1.0, 1.0);

    for (int t = 0; t < 4000; ++t) {
      double fovy = fov_deg(eng);
      double n = znear(eng);
      double f = zfar(eng);
      if (f <= n) std::swap(f, n);

      // Build your perspective transform and read the actual matrix
      Txd P = Txd::perspective(fovy, n, f);
      const M4d& M = P.get();

      // Extract the effective fx, fy actually present in the matrix
      const double fx = M[0, 0];
      const double fy = M[1, 1];

      // (1) Random interior samples: choose (u,v) and z∈[n,f]; back-solve x,y so NDC->(u,v)
      for (int k = 0; k < 64; ++k) {
        const double u = u01(eng);
        const double v = u01(eng);
        const double z = std::uniform_real_distribution<double>(n, f)(eng);

        P3d eyePt{ u * (z / fx), v * (z / fy), z };
        V4d h{ eyePt[0], eyePt[1], eyePt[2], 1.0 };
        V4d q = ndc_div(M * h);

        expect(almostEqual(q[0], u, td));
        expect(almostEqual(q[1], v, td));
        expect(q[0] <= 1.000001 && q[0] >= -1.000001 && q[1] <= 1.000001 && q[1] >= -1.000001);
      }

      // (2) Near/far XY corners
      {
        const double zN = n, zF = f;
        const double xsN[2] = { +(zN / fx), -(zN / fx) };
        const double ysN[2] = { +(zN / fy), -(zN / fy) };
        const double xsF[2] = { +(zF / fx), -(zF / fx) };
        const double ysF[2] = { +(zF / fy), -(zF / fy) };
        for (double xN : xsN) for (double yN : ysN) {
          V4d qN = ndc_div(M * V4d{ xN, yN, zN, 1.0 });
          expect(almostEqual(std::abs(qN[0]), 1.0, td));
          expect(almostEqual(std::abs(qN[1]), 1.0, td));
        }
        for (double xF : xsF) for (double yF : ysF) {
          V4d qF = ndc_div(M * V4d{ xF, yF, zF, 1.0 });
          expect(almostEqual(std::abs(qF[0]), 1.0, td));
          expect(almostEqual(std::abs(qF[1]), 1.0, td));
        }
      }
    }
    };

  // --- lookAt: world->camera mapping, cam axes, origin, determinant ≈ +1 for rotation ---
  "transform/lookAt_world_to_cam_basics"_test = [] {
    Tol td{ 1e-12,1e-10,10 };

    // Local helpers
    auto apply_point = [](const M4d& M, const P3d& p) {
      V4d hp{ p[0], p[1], p[2], 1.0 };
      V4d hq = M * hp;
      if (hq[3] != 0.0 && hq[3] != 1.0) {
        double iw = 1.0 / hq[3];
        return P3d{ hq[0] * iw, hq[1] * iw, hq[2] * iw };
      }
      return P3d{ hq[0], hq[1], hq[2] };
      };
    auto len3 = [](const V3d& v) { return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); };

    // Eye looks down +Z toward target; your lookAt builds +Z-forward camera
    P3d eye{ 1.0, 2.0, -3.0 };
    P3d tgt{ 1.0, 2.0,  7.0 };
    V3d up{ 0.0, 1.0,  0.0 };

    Txd CtoW = Txd::lookAt(eye, tgt, up);
    const M4d& WtoC = CtoW.getInv();  // world->camera

    // Eye maps to camera origin
    P3d cEye = apply_point(WtoC, eye);
    expect(almostEqual(cEye[0], 0.0, td));
    expect(almostEqual(cEye[1], 0.0, td));
    expect(almostEqual(cEye[2], 0.0, td));

    // Target lies on +Z axis (x≈0,y≈0,z>0)
    P3d cTgt = apply_point(WtoC, tgt);
    expect(std::abs(cTgt[0]) < 1e-9 && std::abs(cTgt[1]) < 1e-9);
    expect(cTgt[2] > 0.0);

    // Rotation block is orthonormal (columns of CtoW are right, up, forward); det ≈ +1
    double det = CtoW.get().determinant();
    expect(almostEqual(det, 1.0, td));

    // Round-trip: CtoW followed by WtoC is identity on a few random points
    std::mt19937_64 eng{ 0xBADA5531ULL };
    std::uniform_real_distribution<double> d(-100.0, 100.0);
    for (int i = 0; i < 256; ++i) {
      P3d p{ d(eng), d(eng), d(eng) };
      P3d pc = apply_point(WtoC, p);
      P3d pw = apply_point(CtoW.get(), pc);
      expect(almostEqual(pw[0], p[0], td));
      expect(almostEqual(pw[1], p[1], td));
      expect(almostEqual(pw[2], p[2], td));
    }
    };

  // --- inverse round-trip for affine compositions (stable, no perspective divide branch) ---
  "transform/affine_inverse_roundtrip_randomized"_test = [] {
    Tol td{ 1e-12,1e-10,10 };

    auto apply_point = [](const M4d& M, const P3d& p) {
      V4d hp{ p[0], p[1], p[2], 1.0 };
      V4d hq = M * hp;
      return P3d{ hq[0], hq[1], hq[2] }; // affine: w' == 1
      };

    std::mt19937_64 eng{ 0xCAFEF00DULL };
    std::uniform_real_distribution<double> tdist(-50.0, 50.0);
    std::uniform_real_distribution<double> sdist(0.1, 10.0);
    std::uniform_real_distribution<double> pdist(-100.0, 100.0);

    for (int i = 0; i < 2000; ++i) {
      V3d t{ tdist(eng), tdist(eng), tdist(eng) };
      V3d s{ sdist(eng), sdist(eng), sdist(eng) };

      Txd T = Txd::translate(t);
      Txd S = Txd::scale(s);
      Txd A = T * S;              // apply S then T

      // Build inverse transform for operator() by swapping (getInv(), get())
      Txd Ainv{ A.getInv(), A.get() };

      for (int k = 0; k < 8; ++k) {
        P3d p{ pdist(eng), pdist(eng), pdist(eng) };
        P3d q = A(p);
        P3d r = Ainv(q);
        expect(almostEqual(r[0], p[0], td));
        expect(almostEqual(r[1], p[1], td));
        expect(almostEqual(r[2], p[2], td));
      }
    }
    };

  // --- composition equals matrix product (forward) / inverse equals reverse product ---
  "transform/compose_matrix_agrees"_test = [] {
    Tol td{ 1e-12,1e-10,10 };

    std::mt19937_64 eng{ 0xABCD1234ULL };
    std::uniform_real_distribution<double> tdist(-10.0, 10.0);
    std::uniform_real_distribution<double> sdist(0.2, 5.0);

    for (int i = 0; i < 2048; ++i) {
      V3d t1{ tdist(eng), tdist(eng), tdist(eng) };
      V3d t2{ tdist(eng), tdist(eng), tdist(eng) };
      V3d s1{ sdist(eng), sdist(eng), sdist(eng) };

      Txd A = Txd::translate(t1);
      Txd B = Txd::scale(s1);
      Txd C = Txd::translate(t2);

      Txd ABC = A * B * C;

      M4d Fref = A.get() * (B.get() * C.get());
      M4d Iref = C.getInv() * (B.getInv() * A.getInv());

      // Compare a handful of entries to avoid over-asserting tiny FP diffs
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) {
        expect(almostEqual(ABC.get()[r, c], Fref[r, c], td));
        expect(almostEqual(ABC.getInv()[r, c], Iref[r, c], td));
      }
    }
    };
}

