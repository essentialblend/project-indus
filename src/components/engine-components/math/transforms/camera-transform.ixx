export module cameratransform;

import std;
import transform;
import types;
import ray;
import squarematrix;
import concepts;
import miscconstructs;
import samplingconstructs;
import animatedtransform;
import cameraconstructs;

export class CameraTransform final
{
public:
  CameraTransform() = default;
  explicit CameraTransform(const AnimatedTransform& worldFromCamera, const RenderingSpace& renderingSpace) noexcept;

  const Transform4f& getWorldFromRender() const noexcept;
  const AnimatedTransform& getRenderFromCamera() const noexcept;

  Point3f applyRenderFromCamera(Point3f p, Float t) const noexcept;
  Point3f applyCameraFromRender(Point3f p, Float t) const noexcept;
  Point3f applyRenderFromWorld(Point3f p) const noexcept;

  Ray applyRenderFromCamera(const Ray& ray) const noexcept;
  // RayDiff pending

  template<FloatOnlyVector3Like V>
  V applyRenderFromCamera(V v, Float t) const noexcept;

  template<FloatOnlyVector3Like V>
  V applyCameraFromRender(V v, Float t) const noexcept;

  template<FloatOnlyVector3Like V>
  V applyRenderFromWorld(V v) const noexcept;

  template<FloatOnlyVector3Like V>
  V applyWorldFromRender(V v) const noexcept;

  Transform4f getRenderFromWorld() const noexcept;
  Transform4f applyCameraFromRender(Float t) const noexcept;
  Transform4f applyCameraFromWorld(Float t) const noexcept;

private:
  AnimatedTransform m_renderFromCamera{};
  Transform4f m_worldFromRender{};
};

CameraTransform::CameraTransform(const AnimatedTransform& worldFromCamera, const RenderingSpace& renderingSpace) noexcept
{
  const Float startTime{ worldFromCamera.getStartTime() };
  const Float endTime{ worldFromCamera.getEndTime() };
  const Float tMid{ (startTime + endTime) * Float { 0.5 } };

  switch (renderingSpace) 
  {
    case RenderingSpace::Camera:
    {
      m_worldFromRender = worldFromCamera.interpolate(tMid);
    }
    break;
    case RenderingSpace::CameraWorld:
    {
      const Point3f pCamWorldSpaceOrigin{ worldFromCamera(Point3f{}, tMid) };
      m_worldFromRender = Transform4f::translate(Vec3f{ pCamWorldSpaceOrigin[0], pCamWorldSpaceOrigin[1], pCamWorldSpaceOrigin[2] });
    }
    break;
    case RenderingSpace::World:
    {
      m_worldFromRender = Transform4f{}; 
    }
    break;
  }

  const Transform4f renderFromWorld{ m_worldFromRender.getInverseTransform() };
  const Transform4f startRFC{ renderFromWorld * worldFromCamera.interpolate(startTime) };
  const Transform4f endRFC{ renderFromWorld * worldFromCamera.interpolate(endTime) };
  
  m_renderFromCamera = AnimatedTransform{ startRFC, startTime, endRFC, endTime };
}

const Transform4f& CameraTransform::getWorldFromRender() const noexcept 
{ 
  return m_worldFromRender; 
}

const AnimatedTransform& CameraTransform::getRenderFromCamera() const noexcept 
{ 
  return m_renderFromCamera; 
}

Point3f CameraTransform::applyRenderFromCamera(Point3f p, Float t) const noexcept 
{ 
  return m_renderFromCamera(p, t); 
}

Point3f CameraTransform::applyCameraFromRender(Point3f p, Float t) const noexcept 
{ 
  return m_renderFromCamera.applyInverse(p, t); 
}

Point3f CameraTransform::applyRenderFromWorld(Point3f p) const noexcept 
{ 
  return m_worldFromRender.applyInverse(p); 
}

Ray CameraTransform::applyRenderFromCamera(const Ray& ray) const noexcept 
{ 
  return m_renderFromCamera(ray); 
}

template<FloatOnlyVector3Like V>
V CameraTransform::applyRenderFromCamera(V v, Float t) const noexcept 
{ 
  return m_renderFromCamera(v, t); 
}

template<FloatOnlyVector3Like V>
V CameraTransform::applyCameraFromRender(V v, Float t) const noexcept 
{ 
  return m_renderFromCamera.applyInverse(v, t);
}

template<FloatOnlyVector3Like V>
V CameraTransform::applyRenderFromWorld(V v) const noexcept 
{ 
  return m_worldFromRender.applyInverse(v); 
}

template<FloatOnlyVector3Like V>
V CameraTransform::applyWorldFromRender(V v) const noexcept 
{ 
  return m_worldFromRender(v); 
}

Transform4f CameraTransform::getRenderFromWorld() const noexcept 
{ 
  return m_worldFromRender.getInverseTransform();
}

Transform4f CameraTransform::applyCameraFromRender(Float t) const noexcept 
{ 
  return m_renderFromCamera.interpolate(t).getInverseTransform(); 
}

Transform4f CameraTransform::applyCameraFromWorld(Float t) const noexcept 
{ 
  return applyCameraFromRender(t) * m_worldFromRender.getInverseTransform(); 
}