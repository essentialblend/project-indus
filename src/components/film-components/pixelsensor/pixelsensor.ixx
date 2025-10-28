export module pixelsensor;

import types;
import cameraconstructs;
import squarematrix;
import colorrgb;

export class PixelSensor final
{
public:

  explicit PixelSensor(const Mat3d& XYZFromSensorRGB, const RGBColorSpace& outputCS, Float imagingRatio) noexcept;

  [[nodiscard]] ColorRGB toOutputRGB(const ColorRGB& sensorLinearRGB) const noexcept;

  [[nodiscard]] Float getImagingRatio() const noexcept;
  [[nodiscard]] const RGBColorSpace& getOutputColorSpace() const noexcept;

  [[nodiscard]] const Mat3d& getXYZFromSensorRGB() const noexcept;
  [[nodiscard]] const Mat3d& getOutputRGBFromXYZ() const noexcept;
  [[nodiscard]] const Mat3d& getOutputRGBFromSensorRGB() const noexcept;
  
private:
  const RGBColorSpace& m_outputColorSpace;

  Float m_imagingRatio{};

  Mat3d m_XYZFromSensorRGB{};
  Mat3d m_outRGBFromXYZ{};
  Mat3d m_outRGBFromSensorRGB{};

  static Mat3d whiteBalance(const Vec2f& sourceWhiteXY, const Vec2f& targetWhiteXY) noexcept;

  static Vec3d xyToXYZUnitY(const Vec2f& xy);
};

PixelSensor::PixelSensor(const Mat3d& XYZFromSensorRGB, const RGBColorSpace& outputCS, Float imagingRatio) noexcept : m_outputColorSpace{ outputCS }, m_imagingRatio{ imagingRatio }, m_XYZFromSensorRGB{ XYZFromSensorRGB }, m_outRGBFromXYZ{ outputCS.RGBFromXYZ }, m_outRGBFromSensorRGB{ m_outRGBFromXYZ * m_XYZFromSensorRGB } {}

ColorRGB PixelSensor::toOutputRGB(const ColorRGB& sensorLinearRGB) const noexcept
{
  const Float s0{ m_imagingRatio * sensorLinearRGB[0] };
  const Float s1{ m_imagingRatio * sensorLinearRGB[1] };
  const Float s2{ m_imagingRatio * sensorLinearRGB[2] };
  
  ColorRGB out{};
  
  out[0] = static_cast<Float>(m_outRGBFromSensorRGB[0, 0] * s0 + m_outRGBFromSensorRGB[0, 1] * s1 + m_outRGBFromSensorRGB[0, 2] * s2);

  out[1] = static_cast<Float>(m_outRGBFromSensorRGB[1, 0] * s0 + m_outRGBFromSensorRGB[1, 1] * s1 + m_outRGBFromSensorRGB[1, 2] * s2);

  out[2] = static_cast<Float>(m_outRGBFromSensorRGB[2, 0] * s0 + m_outRGBFromSensorRGB[2, 1] * s1 + m_outRGBFromSensorRGB[2, 2] * s2);

  return out;
}

Float PixelSensor::getImagingRatio() const noexcept 
{ 
  return m_imagingRatio; 
}

const RGBColorSpace& PixelSensor::getOutputColorSpace() const noexcept 
{ 
  return m_outputColorSpace; 
}

const Mat3d& PixelSensor::getXYZFromSensorRGB() const noexcept 
{ 
  return m_XYZFromSensorRGB; 
}

const Mat3d& PixelSensor::getOutputRGBFromXYZ() const noexcept
{
  return m_outRGBFromXYZ;
}

const Mat3d& PixelSensor::getOutputRGBFromSensorRGB() const noexcept 
{ 
  return m_outRGBFromSensorRGB; 
}

// Unused, deferred deeper understanding
Mat3d PixelSensor::whiteBalance(const Vec2f& sourceWhiteXY, const Vec2f& targetWhiteXY) noexcept
{
  const Mat3d LMSFromXYZ
  { { 
    Vec3d{ 0.4002,  0.7075, -0.0807 },
    Vec3d{ -0.2263, 1.1653,  0.0457 },
    Vec3d{ 0.0000,  0.0000,  0.9182 } 
  } };

  const Mat3d XYZFromLMS
  { { 
    Vec3d{ 1.8599364, -1.1293816, 0.2198974 },
    Vec3d{ 0.3611914,  0.6388125, 0.0000064 },
    Vec3d{ 0.0000000,  0.0000000, 1.0890636 } 
  } };

  const Vec3d srcXYZ{ xyToXYZUnitY(sourceWhiteXY) };
  const Vec3d dstXYZ{ xyToXYZUnitY(targetWhiteXY) };

  const Vec3d srcLMS{ LMSFromXYZ * srcXYZ };
  const Vec3d dstLMS{ LMSFromXYZ * dstXYZ };

  const double a{ (srcLMS[0] != 0.0) ? dstLMS[0] / srcLMS[0] : 1.0 };
  const double b{ (srcLMS[1] != 0.0) ? dstLMS[1] / srcLMS[1] : 1.0 };
  const double c{ (srcLMS[2] != 0.0) ? dstLMS[2] / srcLMS[2] : 1.0 };
  
  const Mat3d D
  { { 
    Vec3d{ a, 0, 0 }, 
    Vec3d{ 0, b, 0 }, 
    Vec3d{ 0, 0, c } 
  } };
  
  return XYZFromLMS * (D * LMSFromXYZ);
}

Vec3d PixelSensor::xyToXYZUnitY(const Vec2f& xy)
{
  const double x{ static_cast<double>(xy[0]) };
  const double y{ static_cast<double>(xy[1]) };

  const double Y{ 1.0 };
  const double X{ (y != 0.0) ? x * Y / y : 0.0 };
  const double Z{ (y != 0.0) ? (1.0 - x - y) * Y / y : 0.0 };
  
  return Vec3d{ X, Y, Z };
}

