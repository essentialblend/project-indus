export module h_ray;

import <any>;
import <cmath>;

import point3;
import vector;

import h_mathutilities;

export class HRay
{
//public:
//    constexpr explicit HRay() noexcept = default;
//    constexpr HRay(const HRay&) = default;
//    constexpr HRay& operator=(const HRay&) = default;
//    constexpr HRay(HRay&&) noexcept = default;
//    constexpr HRay& operator=(HRay&&) noexcept = default;
//
//    constexpr explicit HRay(const Point3c& rayOrigin, const Vec3c& rayDir, Float rayTime = 0.f) noexcept
//        : m_rayOrigin{ rayOrigin }, m_rayDirection{ rayDir }, m_rayTime{ rayTime } {}
//
//    [[nodiscard]] constexpr Point3c getPointOnRayAt(const Float t) const noexcept
//    {
//        return m_rayOrigin + (m_rayDirection * t);
//    }
//    [[nodiscard]] constexpr Point3c getOrigin() const noexcept
//    {
//        return m_rayOrigin;
//    }
//    [[nodiscard]] constexpr Vec3c getDirection() const noexcept
//    {
//        return m_rayDirection;
//    }
//
//    [[nodiscard]] constexpr double getTime() const noexcept
//    {
//        return m_rayTime;
//    }
//
//    bool hasNaN() const
//    {
//        auto isnanVector = [](const auto& vec) {
//            return std::isnan(vec[0]) || std::isnan(vec[1]) || std::isnan(vec[2]);
//            };
//
//        return isnanVector(m_rayOrigin) || isnanVector(m_rayDirection);
//    }
//
//    ~HRay() noexcept = default;
//
//protected:
//	Point3c m_rayOrigin{};
//    Vec3c m_rayDirection{};
//    Float m_rayTime{};
};