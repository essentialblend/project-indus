export module rayutil;

import types;
import mathfp;
import mathalgebra;

export [[nodiscard]] Point3f offsetRayOrigin(const Point3f& p, const Vec3f& pError, const Normal3f& n, const Vec3f& w)
{
  const Normal3f na{ Normal3f{std::abs(n[0]), std::abs(n[1]), std::abs(n[2])} };
  const Float d{ computeDot(Vec3f{na[0], na[1], na[2]}, pError) };

  Vec3f off{ d * Vec3f{n[0], n[1], n[2]} };

  if (computeDot(w, n) < Float{}) off = -off;

  Point3f po{ p + off };

  bool bumped{};

  for (Int i{}; i < 3; ++i)
  {
    if (off[i] > 0)
    {
      po[i] = nextFloatUp(po[i]);
      bumped = true;
    }
    else if (off[i] < 0)
    {
      po[i] = nextFloatDown(po[i]);
      bumped = true;
    }
  }

  if (!bumped)
  {
    for (Int i{}; i < 3; ++i)
    {
      if (n[i] > 0) po[i] = nextFloatUp(po[i]);
      else if (n[i] < 0) po[i] = nextFloatDown(po[i]);
    }
  }

  return po;
}