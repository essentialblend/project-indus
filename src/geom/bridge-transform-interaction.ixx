export module indus.geom.bridgetransforminteraction;

import std;

import indus.core.types;
import indus.core.geom.transform;
import indus.core.math.algebra.iv;

import indus.geom.interaction;
import indus.geom.surfaceinteraction;


export
{
  Interaction applyTransformToInteraction(const Transform4f& renderFromLocal, const Interaction& localInteraction) noexcept
  {
    const Point3f transformedWorldPosition{ renderFromLocal(localInteraction.getWorldPosition()) };

    const Vec3f transformedErrorRaw{ renderFromLocal(localInteraction.getWorldError()) };
    const Vec3f transformedWorldError
    {
      std::abs(transformedErrorRaw[0]),
      std::abs(transformedErrorRaw[1]),
      std::abs(transformedErrorRaw[2])
    };

    if (!localInteraction.hasNormal())
    {
      return Interaction{ transformedWorldPosition, localInteraction.getTimeSeconds(), transformedWorldError };
    }

    const Normal3f transformedWorldNormal{ normalize(renderFromLocal(*localInteraction.tryGetWorldNormal())) };

    return Interaction{ transformedWorldPosition, localInteraction.getTimeSeconds(), transformedWorldError, transformedWorldNormal };
  }

  SurfaceInteraction applyTransformToSurfaceInteraction(const Transform4f& renderFromLocal, const SurfaceInteraction& localSurfaceInteraction) noexcept
  {
    const Point3f transformedWorldPosition{ renderFromLocal(localSurfaceInteraction.getWorldPosition()) };

    const Vec3f transformedErrorRaw{ renderFromLocal(localSurfaceInteraction.getWorldError()) };
    const Vec3f transformedWorldError
    {
      std::abs(transformedErrorRaw[0]),
      std::abs(transformedErrorRaw[1]),
      std::abs(transformedErrorRaw[2])
    };

    const Normal3f transformedGeometricNormal
    {
      normalize(renderFromLocal(localSurfaceInteraction.getWorldGeometricNormal()))
    };

    SurfaceInteraction transformedSurfaceInteraction
    {
      transformedWorldPosition,
      localSurfaceInteraction.getTimeSeconds(),
      localSurfaceInteraction.getTHit(),
      transformedWorldError,
      transformedGeometricNormal
    };

    transformedSurfaceInteraction.setUV(localSurfaceInteraction.getUV());
    transformedSurfaceInteraction.setMaterial(localSurfaceInteraction.getMaterial());
    transformedSurfaceInteraction.setShadingBasis(localSurfaceInteraction.getShadingBasis());

    return transformedSurfaceInteraction;
  }
}