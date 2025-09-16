#pragma once

#include "../dep/ut.hpp"

#include "vec3-tests.hpp"
import <cstdint>;

import std;
import vector;
import types;
import point;
import matrix;

using namespace boost::ut;

void mat4ConstructionAndLayoutTests() {
  using M4f = Matrix4<float>;  using M4d = Matrix4<double>;
  using V4f = Vector<float, 4>; using V4d = Vector<double, 4>;

  "mat4/ctor_default_is_identity"_test = [] {
    M4f a;
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
      expect(a[r, c] == (r == c ? 1.0f : 0.0f));
    M4d b;
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
      expect(b[r, c] == (r == c ? 1.0 : 0.0));
    };

  "mat4/static_identity_and_zero"_test = [] {
    M4f I = M4f::identity(); M4f Z = M4f::zero();
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) {
      expect(I[r, c] == (r == c ? 1.0f : 0.0f));
      expect(Z[r, c] == 0.0f);
    }
    M4d Id = M4d::identity(); M4d Zd = M4d::zero();
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) {
      expect(Id[r, c] == (r == c ? 1.0 : 0.0));
      expect(Zd[r, c] == 0.0);
    }
    };

  "mat4/column_constructor_rowmajor_mapping"_test = [] {
    V4f c0{ 1.f, 2.f, 3.f, 4.f };
    V4f c1{ 5.f, 6.f, 7.f, 8.f };
    V4f c2{ 9.f,10.f,11.f,12.f };
    V4f c3{ 13.f,14.f,15.f,16.f };
    M4f A{ c0,c1,c2,c3 };
    // Expect A[r,c] == c_c[r]
    float ref[4][4] = {
      {1,5,9,13},
      {2,6,10,14},
      {3,7,11,15},
      {4,8,12,16}
    };
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
      expect(A[r, c] == ref[r][c]);
    };

  "mat4/index_contiguity_rowmajor"_test = [] {
    M4f A;
    // Addresses within a row must be contiguous and row stride must be 4
    auto* p00 = &A[0, 0];
    auto* p01 = &A[0, 1];
    auto* p02 = &A[0, 2];
    auto* p03 = &A[0, 3];
    auto* p10 = &A[1, 0];
    expect(p00 + 1 == p01);
    expect(p01 + 1 == p02);
    expect(p02 + 1 == p03);
    expect(p00 + 4 == p10);
    };

  "mat4/transpose_basic_and_involution"_test = [] {
    V4d c0{ 1,2,3,4 }, c1{ 0,1,0,1 }, c2{ -1,5,9,-2 }, c3{ 7,0,3,8 };
    M4d A{ c0,c1,c2,c3 };
    M4d AT = A.transpose();
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
      expect(AT[r, c] == A[c, r]);
    M4d ATT = AT.transpose();
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
      expect(ATT[r, c] == A[r, c]);
    };

  "mat4/matvec_and_matmul_smoke"_test = [] {
    V4f c0{ 1,0,0,0 }, c1{ 0,2,0,0 }, c2{ 0,0,3,0 }, c3{ 4,5,6,1 };
    M4f T{ c0,c1,c2,c3 };           // affine: scale(1,2,3)+translate(4,5,6)
    V4f x{ 1,2,3,1 };
    V4f y = T * x;
    expect(y[0] == 1.f * 1 + 0 * 2 + 0 * 3 + 4 * 1);
    expect(y[1] == 0 * 1 + 2.f * 2 + 0 * 3 + 5 * 1);
    expect(y[2] == 0 * 1 + 0 * 2 + 3.f * 3 + 6 * 1);
    expect(y[3] == 1.f);

    // Mat*Mat with identity
    M4f I = M4f::identity();
    M4f L = T * I, R = I * T;
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) {
      expect(L[r, c] == T[r, c]);
      expect(R[r, c] == T[r, c]);
    }
    };

  "mat4/determinant_sanity_on_known_cases"_test = [] {
    M4d I = M4d::identity();
    expect(almostEqual(I.determinant(), 1.0));

    // Upper triangular: det = product of diagonal
    V4d c0{ 2,0,0,0 }, c1{ 1,3,0,0 }, c2{ 4,5,5,0 }, c3{ 7,8,9,7 };
    M4d U{ c0,c1,c2,c3 }; // still upper-triangular (since stored as columns)
    double detU = U.determinant();
    expect(almostEqual(detU, static_cast<double>(2 * 3 * 5 * 7), Tol{ 1e-12,1e-10,6 }));

    // Singular (two equal rows) → det = 0
    V4d s0{ 1,2,3,4 }, s1{ 5,6,7,8 }, s2{ 1,2,3,4 }, s3{ 0,0,0,1 };
    M4d S{ s0,s1,s2,s3 };
    expect(almostEqual(S.determinant(), 0.0, Tol{ 1e-12,1e-10,6 }));
    };
}

void mat4AlgebraPropsTests() {
  using M4f = Matrix4<float>;   using M4d = Matrix4<double>;
  using V4f = Vector<float, 4>;  using V4d = Vector<double, 4>;

  // ---------- Identity & basic algebra ----------
  "mat4/identity_and_basic_linearity"_test = [] {
    M4f I = M4f::identity();
    V4f x{ 1,2,3,1 }, y{ -4,5,-6,0 }; float s = 3.f;
    expect((I * x)[0] == x[0] && (I * x)[1] == x[1] && (I * x)[2] == x[2] && (I * x)[3] == x[3]);
    V4f Ax = I * x, Ay = I * y;
    V4f Axy = I * (x + y), sum = Ax + Ay;
    for (int i = 0; i < 4; ++i) expect(Axy[i] == sum[i]);
    V4f Asx = I * (x * s), sAx = (I * x) * s;
    for (int i = 0; i < 4; ++i) expect(Asx[i] == sAx[i]);
    };

  // ---------- Distributivity (matrix–matrix and matrix–vector) ----------
  "mat4/distributivity_mm_and_mv"_test = [] {
    Tol tf{ 1e-5,1e-5,6 };
    V4f c0{ 1,0,0,0 }, c1{ 0,2,0,0 }, c2{ 0,0,3,0 }, c3{ 4,5,6,1 };
    M4f A{ c0,c1,c2,c3 };

    V4f b0{ 2,0,0,0 }, b1{ 0,1,0,0 }, b2{ 0,0,1,0 }, b3{ 0,0,0,1 };
    M4f B{ b0,b1,b2,b3 };

    V4f d0{ 1,0,0,0 }, d1{ 0,1,0,0 }, d2{ 0,0,1,0 }, d3{ 1,1,1,1 };
    M4f C{ d0,d1,d2,d3 };

    // Form S = B + C via column-wise vector sums (no Matrix4 +)
    M4f S{ b0 + d0, b1 + d1, b2 + d2, b3 + d3 };

    M4f ABS = A * S;
    M4f AB = A * B;
    M4f AC = A * C;
    for (int r = 0; r < 4; ++r)
      for (int k = 0; k < 4; ++k)
        expect(almostEqual(ABS[r, k], AB[r, k] + AC[r, k], tf));

    // Matrix–vector distributivity uses Vector +
    V4f v{ 7,-3,2,1 }, w{ 1,1,1,0 };
    V4f lv = A * (v + w), rv = (A * v) + (A * w);
    for (int i = 0; i < 4; ++i) expect(almostEqual(lv[i], rv[i], tf));
    };

  // ---------- Transpose rules ----------
  "mat4/transpose_rules"_test = [] {
    Tol td{ 1e-12,1e-10,8 };
    V4d a0{ 1,2,3,4 }, a1{ 0,1,1,0 }, a2{ -2,5,0,1 }, a3{ 3,0,7,1 };
    V4d b0{ 2,0,0,0 }, b1{ 1,3,0,0 }, b2{ 4,5,5,0 }, b3{ 7,8,9,1 };
    M4d A{ a0,a1,a2,a3 }, B{ b0,b1,b2,b3 };
    M4d AB = A * B, AT = A.transpose(), BT = B.transpose();
    M4d ABt = AB.transpose(), BTAT = BT * AT;
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) expect(almostEqual(ABt[r, c], BTAT[r, c], td));
    expect(almostEqual(A.determinant(), AT.determinant(), td));
    };

  // ---------- Determinant multiplicativity ----------
  "mat4/determinant_multiplicativity"_test = [] {
    Tol td{ 1e-12,1e-10,8 };
    // Lower * upper (well-conditioned, exact diag products)
    V4d l0{ 1,0,0,0 }, l1{ 2,3,0,0 }, l2{ 4,5,4,0 }, l3{ 6,7,8,2 };
    V4d u0{ 2,1,4,7 }, u1{ 0,3,5,8 }, u2{ 0,0,5,9 }, u3{ 0,0,0,7 };
    M4d L{ l0,l1,l2,l3 }, U{ u0,u1,u2,u3 };
    double detL = 1 * 3 * 4 * 2, detU = 2 * 3 * 5 * 7;
    double detLU = (L * U).determinant();
    expect(almostEqual(detLU, (double)(detL * detU), td));
    };

  // ---------- Inverse correctness & identities (skip singular) ----------
  "mat4/inverse_correctness_and_identities"_test = [] {
    Tol td{ 1e-12,1e-10,10 };
    // Diagonal+translate (affine); inv known in closed form
    V4d c0{ 2,0,0,0 }, c1{ 0,3,0,0 }, c2{ 0,0,5,0 }, c3{ 10,20,30,1 };
    M4d T{ c0,c1,c2,c3 };
    M4d Ti = T.inverse();
    // Check T*Ti ≈ I and Ti*T ≈ I
    M4d L = T * Ti, R = Ti * T, I = M4d::identity();
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) expect(almostEqual(L[r, c], I[r, c], td));
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) expect(almostEqual(R[r, c], I[r, c], td));
    // inv(AB) ≈ B^{-1} A^{-1}
    V4d s0{ 1,0,0,0 }, s1{ 0,2,0,0 }, s2{ 0,0,4,0 }, s3{ 0,0,0,1 };
    M4d S{ s0,s1,s2,s3 };
    M4d P = S * T;
    M4d Pi = P.inverse(), rhs = Ti * S.inverse(); // (ST)^{-1} = T^{-1} S^{-1}
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) expect(almostEqual(Pi[r, c], rhs[r, c], td));
    // (A^{-1})^T ≈ (A^T)^{-1}
    M4d ATi = T.transpose().inverse(), TiT = Ti.transpose();
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) expect(almostEqual(ATi[r, c], TiT[r, c], td));
    };

  // ---------- Affine block properties ----------
  "mat4/affine_block_det_and_inv"_test = [] {
    Tol td{ 1e-12,1e-10,10 };
    // R = diag(sx,sy,sz)
    double sx = 2, sy = 3, sz = 4; double tx = 5, ty = -6, tz = 7;
    V4d c0{ sx,0,0,0 }, c1{ 0,sy,0,0 }, c2{ 0,0,sz,0 }, c3{ tx,ty,tz,1 };
    M4d A{ c0,c1,c2,c3 };
    expect(almostEqual(A.determinant(), sx * sy * sz, td));
    M4d Ai = A.inverse();
    // Expected inverse
    V4d ec0{ 1 / sx,0,0,0 }, ec1{ 0,1 / sy,0,0 }, ec2{ 0,0,1 / sz,0 };
    V4d Rt_t{ -tx / sx, -ty / sy, -tz / sz, 1 };
    M4d E{ ec0,ec1,ec2,Rt_t };
    // Verify Ai equals E
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) expect(almostEqual(Ai[r, c], E[r, c], td));
    };

  // ---------- Homogeneous behavior: point vs direction ----------
  "mat4/homogeneous_point_vs_direction"_test = [] {
    V4f c0{ 1,0,0,0 }, c1{ 0,2,0,0 }, c2{ 0,0,3,0 }, c3{ 4,5,6,1 };
    M4f T{ c0,c1,c2,c3 };
    V4f dir{ 7,-3,2,0 }, pt{ 7,-3,2,1 };
    V4f Td = T * dir, Tp = T * pt;
    expect(Td[0] == 7 && Td[1] == -6 && Td[2] == 6 && Td[3] == 0);  // scale only
    expect(Tp[0] == 7 + 4 && Tp[1] == -6 + 5 && Tp[2] == 6 + 6 && Tp[3] == 1); // plus translate
    };

  // ---------- Associativity & conditioning stress (toleranced) ----------
  "mat4/associativity_stress_and_conditioning"_test = [] {
    Tol tf{ 1e-5,1e-5,8 };
    std::mt19937_64 eng{ 0xC0FFEE123ULL };
    std::uniform_real_distribution<float> d(-3.f, 3.f), sdist(0.5f, 3.f);
    for (int t = 0; t < 5000; ++t) {
      // Build three random invertible affines as Scale*Shear with nonzero diag
      V4f a0{ sdist(eng), d(eng) * 0.2f, d(eng) * 0.2f, 0 };
      V4f a1{ 0, sdist(eng), d(eng) * 0.2f, 0 };
      V4f a2{ 0, 0, sdist(eng), 0 };
      V4f a3{ d(eng), d(eng), d(eng), 1 };
      M4f A{ a0,a1,a2,a3 };

      V4f b0{ sdist(eng), d(eng) * 0.2f, 0, 0 };
      V4f b1{ 0, sdist(eng), 0, 0 };
      V4f b2{ 0, 0, sdist(eng), 0 };
      V4f b3{ d(eng), d(eng), d(eng), 1 };
      M4f B{ b0,b1,b2,b3 };

      V4f c0{ sdist(eng), 0, 0, 0 };
      V4f c1{ d(eng) * 0.2f, sdist(eng), 0, 0 };
      V4f c2{ d(eng) * 0.2f, d(eng) * 0.2f, sdist(eng), 0 };
      V4f c3{ d(eng), d(eng), d(eng), 1 };
      M4f C{ c0,c1,c2,c3 };

      M4f L = (A * B) * C, R = A * (B * C);
      for (int r = 0; r < 4; ++r) for (int k = 0; k < 4; ++k) expect(almostEqual(L[r, k], R[r, k], tf));
    }
    };
}

void mat4OrthoAndAffineGeometryTests() 
{
  using M4f = Matrix4<float>;   using M4d = Matrix4<double>;
  using V4f = Vector<float, 4>;  using V4d = Vector<double, 4>;
  using V3f = Vector<float, 3>;  using V3d = Vector<double, 3>;

  auto make_rotation = [](V3d a, V3d b) {
    // Build right-handed orthonormal basis (x,y,z)
    V3d x = normalize(a);
    V3d y0 = b - computeDot(x, b) * x;
    if (euclideanLengthSq(y0) == 0.0) y0 = V3d{ 1,0,0 };
    V3d y = normalize(y0);
    V3d z = normalize(computeCross(x, y));
    // Re-orthogonalize y to guarantee right-handed triad
    y = normalize(computeCross(z, x));
    V4d c0{ x[0], x[1], x[2], 0 }, c1{ y[0], y[1], y[2], 0 }, c2{ z[0], z[1], z[2], 0 }, c3{ 0,0,0,1 };
    return M4d{ c0,c1,c2,c3 };
    };

  "mat4/rotation_orthonormal_properties"_test = [] {
    using M4d = Matrix4<double>;
    using V4d = Vector<double, 4>;
    using V3d = Vector<double, 3>;
    auto make_rotation = [](const V3d& a, const V3d& b) {
      V3d x = normalize(a);
      V3d y0 = b - computeDot(x, b) * x;
      if (euclideanLengthSq(y0) == 0.0) y0 = V3d{ 1,0,0 };
      V3d y = normalize(y0);
      V3d z = normalize(computeCross(x, y));
      y = normalize(computeCross(z, x));
      V4d c0{ x[0], x[1], x[2], 0 }, c1{ y[0], y[1], y[2], 0 }, c2{ z[0], z[1], z[2], 0 }, c3{ 0,0,0,1 };
      return M4d{ c0,c1,c2,c3 };
      };

    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0xDEADBEEFCAFEBABEULL };
    std::uniform_real_distribution<double> d(-5.0, 5.0);

    for (int t = 0; t < 8000; ++t) {
      V3d a{ d(eng),d(eng),d(eng) }, b{ d(eng),d(eng),d(eng) };
      if (euclideanLengthSq(a) < 1e-12 || euclideanLengthSq(b) < 1e-12) continue;
      M4d R = make_rotation(a, b);

      M4d RT = R.transpose(), Ri = R.inverse();
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) expect(almostEqual(RT[r, c], Ri[r, c], td));
      expect(almostEqual(R.determinant(), 1.0, td));

      V4d dir{ d(eng), d(eng), d(eng), 0.0 };
      double L0 = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
      V4d d1 = R * dir;
      double L1 = std::sqrt(d1[0] * d1[0] + d1[1] * d1[1] + d1[2] * d1[2]);
      Tol tlen{ 1e-12 * std::max(1.0, L0), 1e-10, 8 };
      expect(almostEqual(L0, L1, tlen));

      V4d dir2{ d(eng), d(eng), d(eng), 0.0 };
      V4d Rd1 = R * dir, Rd2 = R * dir2;
      double dot0 = dir[0] * dir2[0] + dir[1] * dir2[1] + dir[2] * dir2[2];
      double dot1 = Rd1[0] * Rd2[0] + Rd1[1] * Rd2[1] + Rd1[2] * Rd2[2];
      Tol tdot{ 1e-12 * (std::abs(dot0) + 1.0), 1e-10, 8 };
      expect(almostEqual(dot0, dot1, tdot));
    }
    };

  "mat4/rigid_transform_inverse_and_distance_invariance"_test = [] {
    using M4d = Matrix4<double>;
    using V4d = Vector<double, 4>;
    using V3d = Vector<double, 3>;
    auto make_rotation = [](const V3d& a, const V3d& b) {
      V3d x = normalize(a);
      V3d y0 = b - computeDot(x, b) * x;
      if (euclideanLengthSq(y0) == 0.0) y0 = V3d{ 1,0,0 };
      V3d y = normalize(y0);
      V3d z = normalize(computeCross(x, y));
      y = normalize(computeCross(z, x));
      V4d c0{ x[0], x[1], x[2], 0 }, c1{ y[0], y[1], y[2], 0 }, c2{ z[0], z[1], z[2], 0 }, c3{ 0,0,0,1 };
      return M4d{ c0,c1,c2,c3 }; // columns = x,y,z, [0,0,0,1]
      };

    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0xA11C3D42ULL };
    std::uniform_real_distribution<double> d(-10.0, 10.0), tdist(-50.0, 50.0);

    for (int t = 0; t < 6000; ++t) {
      M4d R = make_rotation(V3d{ d(eng),d(eng),d(eng) }, V3d{ d(eng),d(eng),d(eng) });
      V4d c3{ tdist(eng), tdist(eng), tdist(eng), 1.0 };

      // Build A with the 3x3 block = R^T (since columns are rows of R below)
      M4d A{
        V4d{R[0,0],R[0,1],R[0,2],0},
        V4d{R[1,0],R[1,1],R[1,2],0},
        V4d{R[2,0],R[2,1],R[2,2],0},
        c3
      };

      M4d Ai = A.inverse();

      // Expected inverse of A = [R^T t; 0 1] is [R  -R*t; 0 1]
      V3d tvec{ c3[0], c3[1], c3[2] };
      // Compute R * t (use ROWS of R)
      V3d Rt{
        R[0,0] * tvec[0] + R[0,1] * tvec[1] + R[0,2] * tvec[2],
        R[1,0] * tvec[0] + R[1,1] * tvec[1] + R[1,2] * tvec[2],
        R[2,0] * tvec[0] + R[2,1] * tvec[1] + R[2,2] * tvec[2]
      };
      M4d E{
        V4d{ R[0,0], R[1,0], R[2,0], 0 },
        V4d{ R[0,1], R[1,1], R[2,1], 0 },
        V4d{ R[0,2], R[1,2], R[2,2], 0 },
        V4d{ -Rt[0], -Rt[1], -Rt[2], 1 }
      };

      for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
          expect(almostEqual(Ai[r, c], E[r, c], td));

      // Distance invariance for points (w=1)
      V4d p{ d(eng), d(eng), d(eng), 1.0 }, q{ d(eng), d(eng), d(eng), 1.0 };
      auto dist3 = [](const V4d& u, const V4d& v) {
        double dx = u[0] - v[0], dy = u[1] - v[1], dz = u[2] - v[2];
        return std::sqrt(dx * dx + dy * dy + dz * dz);
        };
      V4d Ap = A * p, Aq = A * q;
      double d0 = dist3(p, q), d1 = dist3(Ap, Aq);
      Tol tdistol{ 1e-12 * std::max(1.0, d0), 1e-10, 8 };
      expect(almostEqual(d0, d1, tdistol));
    }
    };

  "mat4/column_mapping_property"_test = [] {
    Tol td{ 1e-12,1e-10,8 };
    // Random well-conditioned affine A and arbitrary B
    std::mt19937_64 eng{ 0xBADA5510ULL };
    std::uniform_real_distribution<double> ds(0.5, 3.0), d(-2.0, 2.0);
    for (int t = 0; t < 4000; ++t) {
      V4d a0{ ds(eng), d(eng) * 0.1, d(eng) * 0.1, 0 };
      V4d a1{ 0, ds(eng), d(eng) * 0.1, 0 };
      V4d a2{ 0, 0, ds(eng), 0 };
      V4d a3{ d(eng), d(eng), d(eng), 1 };
      M4d A{ a0,a1,a2,a3 };
      V4d b0{ d(eng),d(eng),d(eng),d(eng) }, b1{ d(eng),d(eng),d(eng),d(eng) },
        b2{ d(eng),d(eng),d(eng),d(eng) }, b3{ d(eng),d(eng),d(eng),d(eng) };
      M4d B{ b0,b1,b2,b3 };
      M4d AB = A * B;
      V4d Ab0 = A * b0, Ab1 = A * b1, Ab2 = A * b2, Ab3 = A * b3;
      for (int r = 0; r < 4; ++r) {
        expect(almostEqual(AB[r, 0], Ab0[r], td));
        expect(almostEqual(AB[r, 1], Ab1[r], td));
        expect(almostEqual(AB[r, 2], Ab2[r], td));
        expect(almostEqual(AB[r, 3], Ab3[r], td));
      }
    }
    };

  "mat4/zero_annihilator_and_translation_vs_direction"_test = [] {
    M4f Z = M4f::zero();
    V4f v{ 3,-2,5,1 }, d{ 3,-2,5,0 };
    V4f Zv = Z * v, Zd = Z * d;
    for (int i = 0; i < 4; ++i) { expect(Zv[i] == 0.0f); expect(Zd[i] == 0.0f); }
    V4f c0{ 1,0,0,0 }, c1{ 0,1,0,0 }, c2{ 0,0,1,0 }, c3{ 10,20,30,1 };
    M4f T{ c0,c1,c2,c3 };
    V4f Td = T * d, Tv = T * v;
    expect(Td[0] == d[0] && Td[1] == d[1] && Td[2] == d[2] && Td[3] == 0.0f);
    expect(Tv[0] == v[0] + 10 && Tv[1] == v[1] + 20 && Tv[2] == v[2] + 30 && Tv[3] == 1.0f);
    // Zero annihilates in mat-mat
    M4f A = T;
    M4f AZ = A * Z, ZA = Z * A;
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) { expect(AZ[r, c] == 0.0f); expect(ZA[r, c] == 0.0f); }
    };

  "mat4/reflection_properties"_test = [] {
    Tol td{ 1e-12,1e-10,10 };
    // Start with a pure rotation R, then flip x-axis to get reflection
    M4d R = M4d::identity();
    // Simple 90° rotation about Z in column-vector convention:
    V4d c0{ 0,1,0,0 }, c1{ -1,0,0,0 }, c2{ 0,0,1,0 }, c3{ 0,0,0,1 };
    R = M4d{ c0,c1,c2,c3 };
    // Reflection across YZ-plane: flip x (col0 *= -1)
    M4d F{ V4d{-c0[0],-c0[1],-c0[2],0}, c1, c2, c3 };
    // det(F) ≈ -1, and Fᵀ ≈ F⁻¹ (it’s still orthogonal)
    expect(almostEqual(F.determinant(), -1.0, td));
    M4d FT = F.transpose(), Fi = F.inverse();
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) expect(almostEqual(FT[r, c], Fi[r, c], td));
    };
}

void mat4DetAdjugateCramersTests() {
  using M4d = Matrix4<double>;
  using V4d = Vector<double, 4>;

  // --- 1) Laplace expansion along first row equals determinant() ---
  "mat4/det_laplace_first_row"_test = [] {
    Tol td{ 1e-12,1e-10,10 };
    auto det3 = [](double a00, double a01, double a02,
      double a10, double a11, double a12,
      double a20, double a21, double a22) {
        return a00 * (a11 * a22 - a12 * a21)
          - a01 * (a10 * a22 - a12 * a20)
          + a02 * (a10 * a21 - a11 * a20);
      };

    std::mt19937_64 eng{ 0xC0FFEE42ULL };
    std::uniform_real_distribution<double> d(-5.0, 5.0);
    for (int t = 0; t < 8000; ++t) {
      // Random columns
      V4d c0{ d(eng),d(eng),d(eng),d(eng) };
      V4d c1{ d(eng),d(eng),d(eng),d(eng) };
      V4d c2{ d(eng),d(eng),d(eng),d(eng) };
      V4d c3{ d(eng),d(eng),d(eng),d(eng) };
      M4d A{ c0,c1,c2,c3 };

      // Elements a[r,c]
      auto a = [&](int r, int c) { return A[r, c]; };

      double M00 = det3(a(1, 1), a(1, 2), a(1, 3), a(2, 1), a(2, 2), a(2, 3), a(3, 1), a(3, 2), a(3, 3));
      double M01 = det3(a(1, 0), a(1, 2), a(1, 3), a(2, 0), a(2, 2), a(2, 3), a(3, 0), a(3, 2), a(3, 3));
      double M02 = det3(a(1, 0), a(1, 1), a(1, 3), a(2, 0), a(2, 1), a(2, 3), a(3, 0), a(3, 1), a(3, 3));
      double M03 = det3(a(1, 0), a(1, 1), a(1, 2), a(2, 0), a(2, 1), a(2, 2), a(3, 0), a(3, 1), a(3, 2));
      double laplace = a(0, 0) * M00 - a(0, 1) * M01 + a(0, 2) * M02 - a(0, 3) * M03;

      double detA = A.determinant();
      // Scale-aware tolerance
      double scale = std::abs(laplace) + std::abs(detA) + 1.0;
      Tol tol{ 1e-12 * scale, 1e-10, 12 };
      expect(almostEqual(laplace, detA, tol));
    }
    };

  // --- 2) det( A * diag(s1,s2,s3,s4) ) = det(A) * Π si (right scaling of columns) ---
  "mat4/det_right_diag_scaling"_test = [] {
    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0xBADA5511ULL };
    std::uniform_real_distribution<double> d(-3.0, 3.0), sdist(0.25, 3.0);
    for (int t = 0; t < 6000; ++t) {
      V4d c0{ d(eng),d(eng),d(eng),d(eng) };
      V4d c1{ d(eng),d(eng),d(eng),d(eng) };
      V4d c2{ d(eng),d(eng),d(eng),d(eng) };
      V4d c3{ d(eng),d(eng),d(eng),d(eng) };
      M4d A{ c0,c1,c2,c3 };

      double s1 = sdist(eng), s2 = sdist(eng), s3 = sdist(eng), s4 = sdist(eng);
      // D = diag(s1..s4) as columns
      M4d D{ V4d{s1,0,0,0}, V4d{0,s2,0,0}, V4d{0,0,s3,0}, V4d{0,0,0,s4} };

      M4d AD = A * D;
      double detAD = AD.determinant();
      double detA = A.determinant();
      double rhs = detA * (s1 * s2 * s3 * s4);
      double scale = std::abs(detAD) + std::abs(rhs) + 1.0;
      Tol tol{ 1e-12 * scale, 1e-10, 10 };
      expect(almostEqual(detAD, rhs, tol));
    }
    };

  // --- 3) det( diag(s)*A ) = (Π s) * det(A) (left scaling of rows) ---
  "mat4/det_left_diag_scaling"_test = [] {
    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0x55AA33CCULL };
    std::uniform_real_distribution<double> d(-3.0, 3.0), sdist(0.25, 3.0);
    for (int t = 0; t < 6000; ++t) {
      V4d c0{ d(eng),d(eng),d(eng),d(eng) };
      V4d c1{ d(eng),d(eng),d(eng),d(eng) };
      V4d c2{ d(eng),d(eng),d(eng),d(eng) };
      V4d c3{ d(eng),d(eng),d(eng),d(eng) };
      M4d A{ c0,c1,c2,c3 };

      double s1 = sdist(eng), s2 = sdist(eng), s3 = sdist(eng), s4 = sdist(eng);
      M4d L{ V4d{s1,0,0,0}, V4d{0,s2,0,0}, V4d{0,0,s3,0}, V4d{0,0,0,s4} };

      M4d LA = L * A;
      double detLA = LA.determinant();
      double detA = A.determinant();
      double rhs = (s1 * s2 * s3 * s4) * detA;
      double scale = std::abs(detLA) + std::abs(rhs) + 1.0;
      Tol tol{ 1e-12 * scale, 1e-10, 10 };
      expect(almostEqual(detLA, rhs, tol));
    }
    };

  // --- 4) Cramer's rule cross-check: solve A x = e_k via inverse and via det-ratios ---
  "mat4/cramers_rule_vs_inverse"_test = [] {
    Tol td{ 1e-12,1e-10,12 };
    auto column_of = [](const M4d& A, int j) { return V4d{ A[0,j], A[1,j], A[2,j], A[3,j] }; };
    auto with_column_replaced = [&](const M4d& A, int j, const V4d& b) {
      return M4d{ j == 0 ? b : column_of(A,0), j == 1 ? b : column_of(A,1), j == 2 ? b : column_of(A,2), j == 3 ? b : column_of(A,3) };
      };

    std::mt19937_64 eng{ 0x0BADF00DULL };
    std::uniform_real_distribution<double> d(-2.0, 2.0);
    for (int t = 0; t < 4000; ++t) {
      // Build a reasonably invertible matrix as product of two affines
      V4d a0{ 1.0 + std::copysign(0.5,d(eng)), d(eng) * 0.2, d(eng) * 0.2, 0 };
      V4d a1{ 0, 1.0 + std::copysign(0.6,d(eng)), d(eng) * 0.2, 0 };
      V4d a2{ 0, 0, 1.0 + std::copysign(0.7,d(eng)), 0 };
      V4d a3{ d(eng), d(eng), d(eng), 1 };
      M4d A{ a0,a1,a2,a3 };

      V4d b0{ 1,0,0,0 }, b1{ 0,1,0,0 }, b2{ 0,0,1,0 }, b3{ 0,0,0,1 };
      V4d bs[4] = { b0,b1,b2,b3 };

      double detA = A.determinant();
      if (!std::isfinite(detA) || std::abs(detA) < 1e-9) continue;

      M4d Ai = A.inverse();

      for (int k = 0; k < 4; ++k) {                // e_k
        V4d x = Ai * bs[k];                  // inverse-based solution
        // Cramer's: x_i = det(A with column i replaced by e_k)/det(A)
        for (int i = 0; i < 4; ++i) {
          M4d A_i = with_column_replaced(A, i, bs[k]);
          double detAi = A_i.determinant();
          double xi = detAi / detA;
          double scale = std::abs(xi) + std::abs(x[i]) + 1.0;
          Tol tol{ 1e-12 * scale, 1e-10, 12 };
          expect(almostEqual(x[i], xi, tol));
        }
      }
    }
    };

  // --- 5) det(A^{-1}) = 1/det(A) and A^{-1}A ≈ I under near-singular stress (scaled tol) ---
  "mat4/inverse_det_relation_and_residual_stress"_test = [] {
    Tol base{ 1e-12,1e-10,12 };
    std::mt19937_64 eng{ 0xA11CEBEEULL };
    std::uniform_real_distribution<double> d(-1.0, 1.0), sdist(1e-3, 3.0);
    for (int t = 0; t < 4000; ++t) {
      // Make A = L * R, with L diag entries spanning magnitudes to hit conditioning
      M4d L{
        V4d{sdist(eng), 0, 0, 0},
        V4d{0, sdist(eng), 0, 0},
        V4d{0, 0, sdist(eng), 0},
        V4d{d(eng), d(eng), d(eng), 1}
      };
      M4d R{
        V4d{1, d(eng) * 0.2, d(eng) * 0.2, 0},
        V4d{0, 1, d(eng) * 0.2, 0},
        V4d{0, 0, 1, 0},
        V4d{d(eng), d(eng), d(eng), 1}
      };
      M4d A = L * R;

      double detA = A.determinant();
      if (!std::isfinite(detA) || std::abs(detA) < 1e-12) continue;

      M4d Ai = A.inverse();
      double detAi = Ai.determinant();
      // det(Ai) ≈ 1/det(A)
      {
        double rhs = 1.0 / detA;
        double scale = std::abs(detAi) + std::abs(rhs) + 1.0;
        Tol td{ base.abs * scale, base.rel, 12 };
        expect(almostEqual(detAi, rhs, td));
      }
      // Residual ||A*Ai - I||∞ scaled by magnitudes
      M4d I = M4d::identity();
      M4d R1 = A * Ai, R2 = Ai * A;
      auto max_abs = [&](const M4d& M) {
        double m = 0.0;
        for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) m = std::max(m, std::abs(M[r, c]));
        return m;
        };
      double scaleA = max_abs(A) * max_abs(Ai) + 1.0;
      double bound = 1e-9 * scaleA; // loose but meaningful bound
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) {
        expect(std::abs(R1[r, c] - I[r, c]) <= bound);
        expect(std::abs(R2[r, c] - I[r, c]) <= bound);
      }
    }
    };
}

void mat4TransposeProductIdAssocTests() {
  using M4d = Matrix4<double>;
  using V4d = Vector<double, 4>;

  // (A^T)^T = A  and  det(A^T) = det(A)
  "mat4/transpose_involution_and_det_invariance"_test = [] {
    auto rand_affine = [](std::mt19937_64& eng) {
      std::uniform_real_distribution<double> sdist(0.5, 3.0), off(-0.5, 0.5), tdist(-10.0, 10.0);
      double sx = sdist(eng), sy = sdist(eng), sz = sdist(eng);
      double a = off(eng), b = off(eng), c = off(eng);
      V4d c0{ sx,  0,  0, 0 };
      V4d c1{ a,  sy, 0, 0 };
      V4d c2{ b,  c,  sz,0 };
      V4d c3{ tdist(eng), tdist(eng), tdist(eng), 1 };
      return M4d{ c0,c1,c2,c3 }; // well-conditioned affine: upper-triangular-ish 3x3
      };

    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0xABCD1234EFULL };
    for (int t = 0; t < 6000; ++t) {
      M4d A = rand_affine(eng);
      M4d ATT = A.transpose().transpose();
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
        expect(almostEqual(ATT[r, c], A[r, c], td));
      double dAT = A.transpose().determinant();
      double dA = A.determinant();
      double scale = std::abs(dAT) + std::abs(dA) + 1.0;
      Tol tdet{ 1e-12 * scale,1e-10,10 };
      expect(almostEqual(dAT, dA, tdet));
    }
    };

  // (AB)^T = B^T A^T
  "mat4/transpose_product_rule"_test = [] {
    auto rand_affine = [](std::mt19937_64& eng) {
      std::uniform_real_distribution<double> sdist(0.5, 3.0), off(-0.5, 0.5), tdist(-10.0, 10.0);
      double sx = sdist(eng), sy = sdist(eng), sz = sdist(eng);
      double a = off(eng), b = off(eng), c = off(eng);
      V4d c0{ sx,  0,  0, 0 };
      V4d c1{ a,  sy, 0, 0 };
      V4d c2{ b,  c,  sz,0 };
      V4d c3{ tdist(eng), tdist(eng), tdist(eng), 1 };
      return M4d{ c0,c1,c2,c3 };
      };

    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0x55AA77CCEEULL };
    for (int t = 0; t < 5000; ++t) {
      M4d A = rand_affine(eng), B = rand_affine(eng);
      M4d AB = A * B;
      M4d L = AB.transpose();
      M4d R = B.transpose() * A.transpose();
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
        expect(almostEqual(L[r, c], R[r, c], td));
    }
    };

  // Identity and zero identities: A*I = I*A = A, det(I)=1, det(0)=0; A*0=0*A=0
  "mat4/identity_and_zero_properties"_test = [] {
    auto rand_affine = [](std::mt19937_64& eng) {
      std::uniform_real_distribution<double> sdist(0.5, 3.0), off(-0.5, 0.5), tdist(-10.0, 10.0);
      double sx = sdist(eng), sy = sdist(eng), sz = sdist(eng);
      double a = off(eng), b = off(eng), c = off(eng);
      V4d c0{ sx,  0,  0, 0 };
      V4d c1{ a,  sy, 0, 0 };
      V4d c2{ b,  c,  sz,0 };
      V4d c3{ tdist(eng), tdist(eng), tdist(eng), 1 };
      return M4d{ c0,c1,c2,c3 };
      };

    M4d I = M4d::identity();
    M4d Z = M4d::zero();
    Tol td{ 1e-12,1e-10,8 };
    expect(almostEqual(I.determinant(), 1.0, td));
    expect(almostEqual(Z.determinant(), 0.0, td));
    std::mt19937_64 eng{ 0xCAFEBABE42ULL };
    for (int t = 0; t < 3000; ++t) {
      M4d A = rand_affine(eng);
      M4d AI = A * I, IA = I * A, AZ = A * Z, ZA = Z * A;
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) {
        expect(almostEqual(AI[r, c], A[r, c], td));
        expect(almostEqual(IA[r, c], A[r, c], td));
        expect(almostEqual(AZ[r, c], 0.0, td));
        expect(almostEqual(ZA[r, c], 0.0, td));
      }
    }
    };

  // det(AB) = det(A)*det(B) and associativity (AB)C ≈ A(BC)
  "mat4/det_multiplicativity_and_associativity"_test = [] {
    auto rand_affine = [](std::mt19937_64& eng) {
      std::uniform_real_distribution<double> sdist(0.5, 3.0), off(-0.5, 0.5), tdist(-10.0, 10.0);
      double sx = sdist(eng), sy = sdist(eng), sz = sdist(eng);
      double a = off(eng), b = off(eng), c = off(eng);
      V4d c0{ sx,  0,  0, 0 };
      V4d c1{ a,  sy, 0, 0 };
      V4d c2{ b,  c,  sz,0 };
      V4d c3{ tdist(eng), tdist(eng), tdist(eng), 1 };
      return M4d{ c0,c1,c2,c3 };
      };

    Tol td{ 1e-12,1e-10,12 };
    std::mt19937_64 eng{ 0xB16B00B5ULL };
    for (int t = 0; t < 5000; ++t) {
      M4d A = rand_affine(eng), B = rand_affine(eng), C = rand_affine(eng);
      M4d AB = A * B, BC = B * C;
      // det multiplicativity
      double detAB = AB.determinant();
      double detA = A.determinant();
      double detB = B.determinant();
      double rhs = detA * detB;
      double scale = std::abs(detAB) + std::abs(rhs) + 1.0;
      Tol tdet{ 1e-12 * scale,1e-10,12 };
      expect(almostEqual(detAB, rhs, tdet));
      // associativity (entry-wise)
      M4d L = AB * C;
      M4d R = A * BC;
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
        expect(almostEqual(L[r, c], R[r, c], td));
    }
    };

  // Similarity invariance of det under orthogonal change of basis: det(R A R^T) = det(A)
  "mat4/det_similarity_invariance_under_rotation"_test = [] {
    Tol td{ 1e-12,1e-10,12 };
    std::mt19937_64 eng{ 0x0BADF00DULL };
    std::uniform_real_distribution<double> d(-1.0, 1.0);
    auto make_R = [&](const Vector<double, 3>& a, const Vector<double, 3>& b) {
      using V3d = Vector<double, 3>; using V4d = Vector<double, 4>;
      V3d x = normalize(a);
      V3d y0 = b - computeDot(x, b) * x; if (euclideanLengthSq(y0) == 0.0) y0 = V3d{ 1,0,0 };
      V3d y = normalize(y0);
      V3d z = normalize(computeCross(x, y)); y = normalize(computeCross(z, x));
      V4d c0{ x[0], x[1], x[2], 0 }, c1{ y[0], y[1], y[2], 0 }, c2{ z[0], z[1], z[2], 0 }, c3{ 0,0,0,1 };
      return M4d{ c0,c1,c2,c3 };
      };
    auto rand_affine = [](std::mt19937_64& eng) {
      std::uniform_real_distribution<double> sdist(0.5, 3.0), off(-0.5, 0.5), tdist(-10.0, 10.0);
      double sx = sdist(eng), sy = sdist(eng), sz = sdist(eng);
      double a = off(eng), b = off(eng), c = off(eng);
      V4d c0{ sx,  0,  0, 0 };
      V4d c1{ a,  sy, 0, 0 };
      V4d c2{ b,  c,  sz,0 };
      V4d c3{ tdist(eng), tdist(eng), tdist(eng), 1 };
      return M4d{ c0,c1,c2,c3 };
      };

    for (int t = 0; t < 4000; ++t) {
      M4d A = rand_affine(eng);
      Vector<double, 3> a{ d(eng), d(eng), d(eng) }, b{ d(eng), d(eng), d(eng) };
      if (euclideanLengthSq(a) < 1e-12 || euclideanLengthSq(b) < 1e-12) continue;
      M4d R = make_R(a, b);
      M4d RARt = R * A * R.transpose();
      double lhs = RARt.determinant(), rhs = A.determinant();
      double scale = std::abs(lhs) + std::abs(rhs) + 1.0;
      Tol tdet{ 1e-12 * scale,1e-10,12 };
      expect(almostEqual(lhs, rhs, tdet));
    }
    };

  // Composition order with points vs directions: M = T*R applies R then T to points; directions ignore T.
  "mat4/affine_order_points_vs_directions"_test = [] {
    Tol td{ 1e-12,1e-10,8 };
    std::mt19937_64 eng{ 0xF00DCAFEULL };
    std::uniform_real_distribution<double> ang(-3.1415, 3.1415), tr(-20.0, 20.0);
    for (int t = 0; t < 4000; ++t) {
      // Simple Z-rotation Rz (right-handed), then translation T
      double c = std::cos(ang(eng)), s = std::sin(ang(eng));
      V4d rc0{ c, s, 0, 0 }, rc1{ -s, c, 0, 0 }, rc2{ 0,0,1,0 }, rc3{ 0,0,0,1 };
      M4d R{ rc0,rc1,rc2,rc3 };
      V4d tc0{ 1,0,0,0 }, tc1{ 0,1,0,0 }, tc2{ 0,0,1,0 }, tc3{ tr(eng),tr(eng),tr(eng),1 };
      M4d T{ tc0,tc1,tc2,tc3 };
      M4d M = T * R;

      V4d p{ tr(eng), tr(eng), tr(eng), 1 }, d{ tr(eng), tr(eng), tr(eng), 0 };
      // Reference via sequential application
      V4d p_ref = T * (R * p);
      V4d d_ref = T * (R * d); // translation should not affect d_ref
      V4d Mp = M * p, Md = M * d;

      for (int i = 0; i < 4; ++i) expect(almostEqual(Mp[i], p_ref[i], td));
      // Check: Md equals R*d (since T shouldn't change directions)
      V4d Rd = R * d;
      for (int i = 0; i < 4; ++i) expect(almostEqual(Md[i], Rd[i], td));
      expect(almostEqual(Md[3], 0.0, td));
    }
    };
}

void mat4HomogeneousAndPathologyTests() {
  using M4d = Matrix4<double>;
  using V4d = Vector<double, 4>;
  using V3d = Vector<double, 3>;

  // Affine matrices preserve w exactly for w ∈ {0,1} (directions vs points).
  "mat4/homogeneous_w_preservation_affine"_test = [] {
    auto rand_affine = [](std::mt19937_64& eng) {
      std::uniform_real_distribution<double> sdist(0.5, 3.0), off(-0.5, 0.5), tdist(-10.0, 10.0);
      double sx = sdist(eng), sy = sdist(eng), sz = sdist(eng);
      double a = off(eng), b = off(eng), c = off(eng);
      V4d c0{ sx,  0,  0, 0 };
      V4d c1{ a,  sy, 0, 0 };
      V4d c2{ b,  c,  sz,0 };
      V4d c3{ tdist(eng), tdist(eng), tdist(eng), 1 };
      return M4d{ c0,c1,c2,c3 };
      };
    Tol td{ 1e-12,1e-10,8 };
    std::mt19937_64 eng{ 0xDEADBEEFCAFELL };
    for (int t = 0; t < 6000; ++t) {
      M4d A = rand_affine(eng);
      V4d p{ 2.0, -3.0, 5.0, 1.0 };
      V4d d{ -1.0, 4.0, -2.0, 0.0 };
      V4d Ap = A * p;
      V4d Ad = A * d;
      expect(almostEqual(Ap[3], 1.0, td));
      expect(almostEqual(Ad[3], 0.0, td));
    }
    };

  // Translation affects points but not directions; linear 3x3 applies to both.
  "mat4/point_vs_direction_semantics"_test = [] {
    auto make_affine = [](double L[3][3], const V3d& t) {
      V4d c0{ L[0][0], L[1][0], L[2][0], 0 };
      V4d c1{ L[0][1], L[1][1], L[2][1], 0 };
      V4d c2{ L[0][2], L[1][2], L[2][2], 0 };
      V4d c3{ t[0], t[1], t[2], 1 };
      return M4d{ c0,c1,c2,c3 };
      };
    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0xA11CE00ULL };
    std::uniform_real_distribution<double> d(-5.0, 5.0), tr(-10.0, 10.0);
    for (int k = 0; k < 6000; ++k) {
      double L[3][3]{
        {1.0 + 0.3 * d(eng), 0.2 * d(eng),       0.1 * d(eng)},
        {0.1 * d(eng),       1.0 + 0.3 * d(eng), 0.2 * d(eng)},
        {0.2 * d(eng),       0.1 * d(eng),       1.0 + 0.3 * d(eng)}
      };
      V3d tvec{ tr(eng), tr(eng), tr(eng) };
      M4d A = make_affine(L, tvec);
      V4d p{ d(eng), d(eng), d(eng), 1.0 };
      V4d v{ d(eng), d(eng), d(eng), 0.0 };

      // Reference formulas
      V3d Lp{
        L[0][0] * p[0] + L[0][1] * p[1] + L[0][2] * p[2],
        L[1][0] * p[0] + L[1][1] * p[1] + L[1][2] * p[2],
        L[2][0] * p[0] + L[2][1] * p[1] + L[2][2] * p[2]
      };
      V3d Lv{
        L[0][0] * v[0] + L[0][1] * v[1] + L[0][2] * v[2],
        L[1][0] * v[0] + L[1][1] * v[1] + L[1][2] * v[2],
        L[2][0] * v[0] + L[2][1] * v[1] + L[2][2] * v[2]
      };

      V4d Ap = A * p;
      V4d Av = A * v;
      for (int i = 0; i < 3; ++i) {
        expect(almostEqual(Ap[i], Lp[i] + tvec[i], td));
        expect(almostEqual(Av[i], Lv[i], td));
      }
      expect(almostEqual(Ap[3], 1.0, td));
      expect(almostEqual(Av[3], 0.0, td));
    }
    };

  // Orthographic projection onto XY-plane: P^2 = P (idempotent), det(P)=0, z->0 while w preserved.
  "mat4/orthographic_xy_projection_idempotent"_test = [] {
    M4d P{
      V4d{1,0,0,0},
      V4d{0,1,0,0},
      V4d{0,0,0,0},
      V4d{0,0,0,1}
    };
    Tol td{ 1e-12,1e-10,8 };
    // Idempotency
    M4d PP = P * P;
    for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
      expect(almostEqual(PP[r, c], P[r, c], td));
    // Determinant zero
    expect(almostEqual(P.determinant(), 0.0, td));
    // Behavior on point and direction
    V4d p{ 2, -3, 7, 1 }, d{ 1, 2, 3, 0 };
    V4d Pp = P * p, Pd = P * d;
    expect(almostEqual(Pp[2], 0.0, td) && almostEqual(Pp[3], 1.0, td));
    expect(almostEqual(Pd[2], 0.0, td) && almostEqual(Pd[3], 0.0, td));
    };

  // Reflection across plane through origin with unit normal n: R = I - 2nn^T (involution, det = -1).
  "mat4/reflection_involution_and_det"_test = [] {
    auto make_reflect = [](const V3d& n) {
      V3d u = normalize(n);
      double nx = u[0], ny = u[1], nz = u[2];
      double R00 = 1 - 2 * nx * nx, R01 = -2 * nx * ny, R02 = -2 * nx * nz;
      double R10 = -2 * ny * nx, R11 = 1 - 2 * ny * ny, R12 = -2 * ny * nz;
      double R20 = -2 * nz * nx, R21 = -2 * nz * ny, R22 = 1 - 2 * nz * nz;
      V4d c0{ R00,R10,R20,0 }, c1{ R01,R11,R21,0 }, c2{ R02,R12,R22,0 }, c3{ 0,0,0,1 };
      return M4d{ c0,c1,c2,c3 };
      };
    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0xC001D00DULL };
    std::uniform_real_distribution<double> d(-1.0, 1.0);
    for (int t = 0; t < 6000; ++t) {
      V3d n{ d(eng), d(eng), d(eng) };
      if (euclideanLengthSq(n) < 1e-18) continue;
      M4d R = make_reflect(n);
      M4d RR = R * R;
      M4d I = M4d::identity();
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c)
        expect(almostEqual(RR[r, c], I[r, c], td));
      double detR = R.determinant();
      Tol tdet{ 1e-12 * (std::abs(detR) + 1.0),1e-10,10 };
      expect(almostEqual(detR, -1.0, tdet));
    }
    };

  // Pure shear has det = 1; inverse is shear with negative parameter.
  "mat4/shear_properties_det_and_inverse"_test = [] {
    auto make_shear_xy = [](double k) {
      // x' = x + k*y (shear X by Y)
      V4d c0{ 1,0,0,0 };
      V4d c1{ k,1,0,0 };
      V4d c2{ 0,0,1,0 };
      V4d c3{ 0,0,0,1 };
      return M4d{ c0,c1,c2,c3 };
      };
    Tol td{ 1e-12,1e-10,10 };
    std::mt19937_64 eng{ 0xBADC0FFEEULL };
    std::uniform_real_distribution<double> kdist(-5.0, 5.0);
    for (int t = 0; t < 6000; ++t) {
      double k = kdist(eng);
      M4d S = make_shear_xy(k);
      M4d Si = make_shear_xy(-k);
      double detS = S.determinant();
      expect(almostEqual(detS, 1.0, td));
      M4d I = M4d::identity();
      M4d SS = S * Si, SS2 = Si * S;
      for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) {
        expect(almostEqual(SS[r, c], I[r, c], td));
        expect(almostEqual(SS2[r, c], I[r, c], td));
      }
    }
    };

  // NaN/Inf propagation in multiplication (avoid 0*Inf ambiguity by picking nonzero components).
  "mat4/ieee754_nan_inf_propagation"_test = [] {
    Tol td{ 1e-12,1e-10,8 };
    // Start with identity
    M4d A = M4d::identity();
    // Inject a NaN in a position that will be hit by a non-zero vector component
    double qn = std::numeric_limits<double>::quiet_NaN();
    A[2, 1] = qn; // row 2, col 1
    V4d v{ 0.0, 1.0, 0.0, 0.0 }; // v[1] = 1 ensures NaN contributes
    V4d Av = A * v;
    expect(isNaN(Av[2]));

    // Reset and inject +Inf in A, multiply by nonzero vector -> some component Inf
    A = M4d::identity();
    double inf = std::numeric_limits<double>::infinity();
    A[0, 2] = inf;
    V4d w{ 0.0, 0.0, 2.0, 0.0 }; // non-zero in col 2
    V4d Aw = A * w;
    expect(isInf(Aw[0]));

    // Vector-side NaN/Inf with finite matrix
    A = M4d::identity();
    V4d vnan{ 1.0, qn, 0.0, 0.0 }, vinf{ 0.0, 0.0, inf, 0.0 };
    V4d rnan = A * vnan, rinf = A * vinf;
    expect(isNaN(rnan[1]));
    expect(isInf(rinf[2]));
    };

  // Overflow to Inf under extreme scaling; finite under subnormal perturbations.
  "mat4/overflow_and_subnormal_stability"_test = [] {
    Tol td{ 1e-12,1e-10,8 };
    // Huge scale on diagonal
    double M = 1e308;
    M4d S{
      V4d{M,0,0,0},
      V4d{0,M,0,0},
      V4d{0,0,M,0},
      V4d{0,0,0,1}
    };
    V4d v{ 1e308, 1e308, 1e308, 1.0 };
    V4d Sv = S * v;
    expect(isInf(Sv[0]) && isInf(Sv[1]) && isInf(Sv[2]));
    expect(almostEqual(Sv[3], 1.0, td));

    // Subnormals stay finite through identity
    double s = std::numeric_limits<double>::denorm_min();
    M4d I = M4d::identity();
    V4d u{ s, -s, s, 1.0 };
    V4d Iu = I * u;
    expect(isFinite(Iu[0]) && isFinite(Iu[1]) && isFinite(Iu[2]) && isFinite(Iu[3]));
    };

  // Large-chain associativity sanity: ((A*B)*C)*p ≈ A*(B*(C*p)) for a point p (w=1)
  "mat4/long_chain_point_associativity_sanity"_test = [] {
    auto rand_affine = [](std::mt19937_64& eng) {
      std::uniform_real_distribution<double> sdist(0.5, 3.0), off(-0.5, 0.5), tdist(-10.0, 10.0);
      double sx = sdist(eng), sy = sdist(eng), sz = sdist(eng);
      double a = off(eng), b = off(eng), c = off(eng);
      V4d c0{ sx,  0,  0, 0 };
      V4d c1{ a,  sy, 0, 0 };
      V4d c2{ b,  c,  sz,0 };
      V4d c3{ tdist(eng), tdist(eng), tdist(eng), 1 };
      return M4d{ c0,c1,c2,c3 };
      };
    Tol td{ 1e-12,1e-10,12 };
    std::mt19937_64 eng{ 0xA55E5511ULL };
    std::uniform_real_distribution<double> d(-5.0, 5.0);
    for (int t = 0; t < 2500; ++t) {
      M4d A = rand_affine(eng), B = rand_affine(eng), C = rand_affine(eng), D = rand_affine(eng);
      V4d p{ d(eng), d(eng), d(eng), 1.0 };
      V4d L = (A * B * C * D) * p;
      V4d R = A * (B * (C * (D * p)));
      for (int i = 0; i < 4; ++i) expect(almostEqual(L[i], R[i], td));
    }
    };
}

void mat4ProjectionClipXYTests() {
  using M4d = Matrix4<double>;
  using V4d = Vector<double, 4>;

  "mat4/perspective_xy_mapping_backsolve_correct_w_row"_test = [] {
    // Build perspective with w' = z.
    // Columns (c0..c3), column vectors, v' = M * v:
    //   x' = fx*x
    //   y' = fy*y
    //   z' = A*z + B*w        (A,B don't matter for this XY-only test)
    //   w' = 1*z + 0*w
    auto make_persp_w_eq_z = [](double fovy_deg, double aspect, double n, double f) {
      const double rad = fovy_deg * 3.14159265358979323846 / 180.0;
      const double fy = 1.0 / std::tan(rad * 0.5);
      const double fx = fy / aspect;
      // Any finite A,B are fine for this test; pick something plausible.
      const double A = f / (f - n);
      const double B = (-f * n) / (f - n);

      // NOTE: c2[3] = 1.0 ensures w' = z (since w' = M[3,2]*z + M[3,3]*w).
      V4d c0{ fx, 0.0, 0.0, 0.0 };
      V4d c1{ 0.0, fy, 0.0, 0.0 };
      V4d c2{ 0.0, 0.0,  A,  1.0 }; // <-- row 3, col 2 = 1.0  (M[3,2])
      V4d c3{ 0.0, 0.0,  B,  0.0 };

      return M4d{ c0, c1, c2, c3 };
      };

    auto ndc_div = [](const V4d& h) {
      const double iw = 1.0 / h[3];  // w' = z > 0 in this test
      return V4d{ h[0] * iw, h[1] * iw, h[2] * iw, 1.0 };
      };

    auto inside_xy = [](const V4d& q) {
      return (q[0] <= 1.0000001 && q[0] >= -1.0000001) &&
        (q[1] <= 1.0000001 && q[1] >= -1.0000001);
      };

    Tol td{ 1e-12, 1e-10, 10 };

    std::mt19937_64 eng{ 0xC0FFEE55ULL };
    std::uniform_real_distribution<double> fovy_deg(35.0, 95.0);
    std::uniform_real_distribution<double> aspect(0.5, 3.0);
    std::uniform_real_distribution<double> znear(0.05, 3.0);
    std::uniform_real_distribution<double> zfar(5.0, 1000.0);
    std::uniform_real_distribution<double> u01(-1.0, 1.0);

    for (int t = 0; t < 4000; ++t) {
      double fovy = fovy_deg(eng);
      double asp = aspect(eng);
      double n = znear(eng);
      double f = zfar(eng);
      if (f <= n) std::swap(f, n);

      M4d P = make_persp_w_eq_z(fovy, asp, n, f);

      // Actual scales from THIS matrix
      const double fx = P[0, 0];
      const double fy = P[1, 1];

      // (1) Random interior samples: back-solve eye-space (x,y) so NDC becomes (u,v).
      for (int k = 0; k < 64; ++k) {
        const double u = u01(eng);
        const double v = u01(eng);
        const double z = std::uniform_real_distribution<double>(n, f)(eng);  // z>0

        V4d p_eye{ u * (z / fx), v * (z / fy), z, 1.0 };
        V4d q = ndc_div(P * p_eye);

        expect(almostEqual(q[0], u, td));
        expect(almostEqual(q[1], v, td));
        expect(inside_xy(q));
      }

      // (2) Near/far XY corners: u,v ∈ {±1}. Back-solve x = u*z/fx, y = v*z/fy.
      {
        const double zN = n, zF = f;
        const double xsN[2] = { +(zN / fx), -(zN / fx) };
        const double ysN[2] = { +(zN / fy), -(zN / fy) };
        const double xsF[2] = { +(zF / fx), -(zF / fx) };
        const double ysF[2] = { +(zF / fy), -(zF / fy) };

        for (double xN : xsN) for (double yN : ysN) {
          V4d qN = ndc_div(P * V4d{ xN, yN, zN, 1.0 });
          expect(inside_xy(qN));
          expect(almostEqual(std::abs(qN[0]), 1.0, td));
          expect(almostEqual(std::abs(qN[1]), 1.0, td));
        }
        for (double xF : xsF) for (double yF : ysF) {
          V4d qF = ndc_div(P * V4d{ xF, yF, zF, 1.0 });
          expect(inside_xy(qF));
          expect(almostEqual(std::abs(qF[0]), 1.0, td));
          expect(almostEqual(std::abs(qF[1]), 1.0, td));
        }
      }
    }
    };
}

void mat4Tests() 
{
  mat4ConstructionAndLayoutTests();
  mat4AlgebraPropsTests();
  mat4OrthoAndAffineGeometryTests();
  mat4DetAdjugateCramersTests();
  mat4TransposeProductIdAssocTests();
  mat4HomogeneousAndPathologyTests();
  mat4ProjectionClipXYTests();

}