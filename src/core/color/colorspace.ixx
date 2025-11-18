export module indus.core.colorspace;

import indus.core.types;
import indus.core.geom.squarematrix;
import indus.core.math.algebra.iv;

export
{
  struct RGBCIE1931Vertices
  {
    Vec2f xRyR{};
    Vec2f xGyG{};
    Vec2f xByB{};
    Vec2f xWyW{};
  };

  struct RGBColorSpace
  {
    RGBCIE1931Vertices RGBVertices{};

    Mat3d XYZFromRGB{};
    Mat3d RGBFromXYZ{};

    Vec3d luminanceFromRGB{};
  };
};

export class ColorRegistry
{
public:
  static const RGBColorSpace& sRGB() noexcept
  {
    // Define the RGBW vertices on the CIE1931 chromaticity grid
    static RGBCIE1931Vertices vertices{};
    vertices.xRyR = { 0.64, 0.33 };
    vertices.xGyG = { 0.30, 0.60 };
    vertices.xByB = { 0.15, 0.06 };
    vertices.xWyW = { 0.3127, 0.3290 };

    // Isolate to the Y = 1 cross-section to form the matrix M. M gets used to convert an l-RGB to XYZ
    static const Vec3d PR{ X_Y1_Z(vertices.xRyR) };
    static const Vec3d PG{ X_Y1_Z(vertices.xGyG) };
    static const Vec3d PB{ X_Y1_Z(vertices.xByB) };
    static const Vec3d PW{ X_Y1_Z(vertices.xWyW) };

    static const Mat3d M{ { PR, PG, PB } };
    
    // With Cramers Rule, compute the determinant to get the volume of the p-piped and use the volumes to find the scalar coefficient vector S
    static const double D{ computeDeterminant(PR, PG, PB) };

    // Now use PW diagonally to get the correct scalars and guarantee that the resulting XYZ ~~ D65
    static const Vec3d S
    {
      computeDeterminant(PW, PG, PB) / D,
      computeDeterminant(PR, PW, PB) / D,
      computeDeterminant(PR, PG, PW) / D
    };
    
    // M^{-1} = adj(M) / det(M) and we have our inverse matrix
    static const Vec3d invFirstRowVecRed{ computeCross(PG, PB) / D };
    static const Vec3d invSecRowVecGreen{ computeCross(PB, PR) / D };
    static const Vec3d invThirdRowVecBlue{ computeCross(PR, PG) / D };


    static const Mat3d inverseMatrix
    { {
      Vec3d{ invFirstRowVecRed[0], invSecRowVecGreen[0], invThirdRowVecBlue[0] },
      Vec3d{ invFirstRowVecRed[1], invSecRowVecGreen[1], invThirdRowVecBlue[1] },
      Vec3d{ invFirstRowVecRed[2], invSecRowVecGreen[2], invThirdRowVecBlue[2] }
    } };

    // Then, XYZ<-RGB = M * diag(S)
    static const Mat3d XYZFromRGB
    { {
      PR * S[0],
      PG * S[1],
      PB * S[2]
    } };

    // And its inverse being RGB<-XYZ = diag(S)^{-1} * M^{-1}
    static const Mat3d RGBFromXYZ
    { {
      Vec3d{ inverseMatrix[0,0] / S[0], inverseMatrix[1,0] / S[1], inverseMatrix[2,0] / S[2] },
      Vec3d{ inverseMatrix[0,1] / S[0], inverseMatrix[1,1] / S[1], inverseMatrix[2,1] / S[2] },
      Vec3d{ inverseMatrix[0,2] / S[0], inverseMatrix[1,2] / S[1], inverseMatrix[2,2] / S[2] }
    } };

    // Extract the Y (second) row from XYZ<-RGB to get their contributions 
    static const Vec3d XYZFromRGB_RowY
    { {
      XYZFromRGB[1,0],
      XYZFromRGB[1,1],
      XYZFromRGB[1,2]
    } };

    static const RGBColorSpace cs{ vertices, XYZFromRGB, RGBFromXYZ, XYZFromRGB_RowY };

    return cs;
  }

private:
  static Vec3d X_Y1_Z(Vec2f XY)
  {
    double x{ XY[0] };
    double y{ XY[1] };
    return { x / y, 1.0, (1.0 - x - y) / y };
  }

  static double computeDeterminant(const Vec3d& a, const Vec3d& b, const Vec3d& c) noexcept
  {
    return computeDot(a, computeCross(b, c));
  }
};


