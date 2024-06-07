export module h_transform;

import <compare>;
import <optional>;

import h_ray;
import h_raydifferential;
import h_mathutilities;
import square_matrix;
import vector;
import point3;
import eu_interaction;

export class HTransform final
{
public:
    constexpr explicit HTransform() noexcept;
    //constexpr explicit HTransform(const Mat4c&, const Mat4c&) noexcept;
    //constexpr HTransform(const HTransform&) = default;
    //constexpr HTransform& operator=(const HTransform&) = default;
    //constexpr HTransform(HTransform&&) noexcept = default;
    //constexpr HTransform& operator=(HTransform&&) noexcept = default;

    //constexpr auto operator<=>(const HTransform&) const = default;

    //constexpr bool isIdentity() const;
    //constexpr bool hasScale() const;

    //template<typename T>
    //constexpr Point3<T> operator()(const Point3<T>&) const;
    //template<typename T>
    //constexpr Vector3<T> operator()(const Vector3<T>&) const;
    //// Normals remain.

    //constexpr HRay operator()(const HRay&, std::optional<Float&> tMax = std::nullopt) const;
    //constexpr HRayDifferential operator()(const HRayDifferential&, std::optional<Float&> tMax = std::nullopt) const;
    //constexpr EUInteraction operator()(const EUInteraction&) const;
    //constexpr EUInteraction applyInverse(const EUInteraction&) const;
    //EUSurfaceInteraction operator()(const EUSurfaceInteraction&) const;
    //EUSurfaceInteraction applyInverse(const EUSurfaceInteraction&) const;

    //const Mat4c& getTransformMatrix() const;
    //const Mat4c& getInverseTransformMatrix() const;

    ~HTransform() noexcept = default;

private:
    Mat4c m_transformMatrix{};
    Mat4c m_inverseTransformMatrix{};
};

// Non-member functions.
//constexpr HTransform inverse(const HTransform&);
//constexpr HTransform transpose(const HTransform&);
//constexpr HTransform translate(const Vec3c&);
//constexpr HTransform scale(const Vec3c&);
//constexpr HTransform rotateX(const Float);
//constexpr HTransform rotateY(const Float);
//constexpr HTransform rotateZ(const Float);
//constexpr HTransform rotate(const Float, const Vec3c&);
//constexpr HTransform lookAt(const Point3c&, const Point3c&, const Vec3c&);
//constexpr HTransform orthographic(const Float, const Float);
//constexpr HTransform perspective(const Float, const Float, const Float);