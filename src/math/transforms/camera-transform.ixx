export module cameratransform;

import std;
import transform;
import types;
import ray;
import matrix;

export class CameraTransform final
{
public:
  CameraTransform() noexcept = default;
  explicit CameraTransform(const Transform4f& worldFromRender, /*const AnimatedTransform& renderFromCamera,*/ bool swapsHandedness) noexcept;

  const Transform4f& getWorldFromRender() const noexcept;
  //const AnimatedTransform& renderFromCamera() const noexcept;

  Transform4f getRenderFromWorld() const noexcept;
  Transform4f getCameraFromRender() const noexcept;

  Point3f applyRenderFromWorld(Point3f worldPoint3f) const;
  Vec3f applyRenderFromWorld(Vec3f worldVec3f) const;
  Normal3f applyRenderFromWorld(Normal3f worldNormal3f) const;
  Ray applyRenderFromWorld(const Ray& worldRay3f) const;

  Point3f applyWorldFromRender(Point3f renderPoint3f) const;
  Vec3f applyWorldFromRender(Vec3f renderVec3f) const;
  Normal3f applyWorldFromRender(Normal3f renderNormal3f) const;
  Ray applyWorldFromRender(const Ray& renderRay3f) const;

  // Currently render space = world space, hence Identity transform
  Point3f applyRenderFromCamera(Point3f cameraPoint3f, [[maybe_unused]] Float time = 0) const;
  Vec3f applyRenderFromCamera(Vec3f cameraVec3f, [[maybe_unused]] Float time = 0) const;
  Normal3f applyRenderFromCamera(Normal3f cameraNormal3f, [[maybe_unused]] Float time = 0) const;
  Ray applyRenderFromCamera(const Ray& cameraRay3f, [[maybe_unused]] Float time = 0) const;

  Point3f applyCameraFromRender(Point3f renderPoint3f, [[maybe_unused]] Float time = 0) const;
  Vec3f applyCameraFromRender(Vec3f renderVec3f, [[maybe_unused]] Float time = 0) const;
  Normal3f applyCameraFromRender(Normal3f renderNormal3f, [[maybe_unused]] Float time = 0) const;
  Ray applyCameraFromRender(const Ray& renderRay3f, [[maybe_unused]] Float time = 0) const;

  bool swapsHandedness() const noexcept;

private:
  Transform4f m_worldFromRender{};
  //AnimatedTransform m_renderFromCamera{};
  bool m_swapsHandedness{};
};

CameraTransform::CameraTransform(const Transform4f& worldFromRender, /*const AnimatedTransform& renderFromCamera,*/ bool swapsHandedness) noexcept : m_worldFromRender{ worldFromRender }, m_swapsHandedness{ swapsHandedness } {}

const Transform4f& CameraTransform::getWorldFromRender() const noexcept 
{
  return m_worldFromRender;
}

Transform4f CameraTransform::getRenderFromWorld() const noexcept 
{
  return m_worldFromRender.getInv();
}

Transform4f CameraTransform::getCameraFromRender() const noexcept 
{
  const auto I{ Mat4f::identity() };
  return Transform4f{ I, I };
}

Point3f CameraTransform::applyRenderFromWorld(Point3f worldPoint3f) const
{ 
  return getRenderFromWorld()(worldPoint3f);
}

Vec3f CameraTransform::applyRenderFromWorld(Vec3f worldVec3f) const
{ 
  return getRenderFromWorld()(worldVec3f);
}

Normal3f CameraTransform::applyRenderFromWorld(Normal3f worldNormal3f) const
{ 
  return getRenderFromWorld()(worldNormal3f);
}

Ray CameraTransform::applyRenderFromWorld(const Ray& worldRay3f) const
{ 
  return getRenderFromWorld()(worldRay3f);
}

Point3f CameraTransform::applyWorldFromRender(Point3f renderPoint3f) const
{ 
  return m_worldFromRender(renderPoint3f);
}

Vec3f CameraTransform::applyWorldFromRender(Vec3f renderVec3f) const
{ 
  return m_worldFromRender(renderVec3f);
}

Normal3f CameraTransform::applyWorldFromRender(Normal3f renderNormal3f) const
{ 
  return m_worldFromRender(renderNormal3f);
}

Ray CameraTransform::applyWorldFromRender(const Ray& renderRay3f) const
{ 
  return m_worldFromRender(renderRay3f);
}

// Stubs, TBD
Point3f CameraTransform::applyRenderFromCamera(Point3f cameraPoint3f, [[maybe_unused]] Float) const
{ 
  return cameraPoint3f;
}

Vec3f CameraTransform::applyRenderFromCamera(Vec3f cameraVec3f, [[maybe_unused]] Float) const
{ 
  return cameraVec3f;
}

Normal3f CameraTransform::applyRenderFromCamera(Normal3f cameraNormal3f, Float) const
{ 
  return cameraNormal3f;
}

Ray CameraTransform::applyRenderFromCamera(const Ray& cameraRay3f, [[maybe_unused]] Float) const
{ 
  return cameraRay3f;
}

Point3f CameraTransform::applyCameraFromRender(Point3f renderPoint3f, [[maybe_unused]] Float time) const
{
  return renderPoint3f;
}

Vec3f CameraTransform::applyCameraFromRender(Vec3f renderVec3f, [[maybe_unused]] Float time) const
{
  return renderVec3f;
}

Normal3f CameraTransform::applyCameraFromRender(Normal3f renderNormal3f, [[maybe_unused]] Float time) const
{
  return renderNormal3f;
}

Ray CameraTransform::applyCameraFromRender(const Ray& renderRay3f, [[maybe_unused]] Float time) const
{
  return renderRay3f;
}

bool CameraTransform::swapsHandedness() const noexcept 
{
  return m_swapsHandedness;
}
