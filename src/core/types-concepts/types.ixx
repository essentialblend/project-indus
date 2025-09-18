export module types;

import std;
import vector;
import point;
import matrix;
import bounds2;
import normal;

export using Float = float;
export using Float64 = double;
export using Int = int;
export using Idx = std::size_t;

export using Vec2f = Vector<Float, 2>;
export using Vec3f = Vector<Float, 3>;
export using Vec3d = Vector<Float64, 3>;
export using Vec4f = Vector<Float, 4>;
export using Vec4d = Vector<Float64, 4>;

export using Point2i = Point<Int, 2>;
export using Point2f = Point<Float, 2>;
export using Point3f = Point<Float, 3>;
export using Point3d = Point<Float64, 3>;

export using Point4f = Point<Float, 4>;

export using Mat4f = Matrix4<Float>;

export using Bounds2f = Bounds2<Float>;

export using Normal3f = Normal<Float>;
export using Normal3d = Normal<Float64>;




