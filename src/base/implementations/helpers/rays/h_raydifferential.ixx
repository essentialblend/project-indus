export module h_raydifferential;

import <cmath>;
import <algorithm>;

import h_ray;
import vector;
import point3;
import h_mathutilities;

export class HRayDifferential : public HRay
{
//public:
//    constexpr explicit HRayDifferential() noexcept = default;
//    constexpr HRayDifferential(const HRayDifferential&) = default;
//    constexpr HRayDifferential& operator=(const HRayDifferential&) = default;
//    constexpr HRayDifferential(HRayDifferential&&) noexcept = default;
//    constexpr HRayDifferential& operator=(HRayDifferential&&) noexcept = default;
//
//    constexpr explicit HRayDifferential(const Point3c& rayOrigin, const Vec3c& rayDir, Float rayTime = 0.f) noexcept
//        : HRay{ rayOrigin, rayDir, rayTime } {}
//
//    constexpr void setRXOrigin(const Point3c& rxOrigin) noexcept
//    {
//        m_rxOrigin = rxOrigin;
//    }
//
//    constexpr void setRYOrigin(const Point3c& ryOrigin) noexcept
//    {
//        m_ryOrigin = ryOrigin;
//    }
//
//    constexpr void setRXDirection(const Vec3c& rxDir) noexcept
//    {
//        m_rxDirection = rxDir;
//    }
//
//    constexpr void setRYDirection(const Vec3c& ryDir) noexcept
//    {
//        m_ryDirection = ryDir;
//    }
//
//    const Point3c& getRXOrigin() const noexcept
//    {
//        return m_rxOrigin;
//    }
//
//    const Point3c& getRYOrigin() const noexcept
//    {
//        return m_ryOrigin;
//    }
//    
//    const Vec3c& getRXDirection() const noexcept
//    {
//        return m_rxDirection;
//    }
//
//    const Vec3c& getRYDirection() const noexcept
//    {
//        return m_ryDirection;
//    }
//
//    bool hasNaN() const noexcept {
//        auto isnanVector = [](const auto& vec) constexpr {
//            return std::isnan(vec[0]) || std::isnan(vec[1]) || std::isnan(vec[2]);
//            };
//
//        return std::isnan(getOrigin()[0]) || std::isnan(getOrigin()[1]) || std::isnan(getOrigin()[2]) ||
//            std::isnan(getDirection()[0]) || std::isnan(getDirection()[1]) || std::isnan(getDirection()[2]) ||
//            (hasDifferentials() && (isnanVector(getRXOrigin()) || isnanVector(getRYOrigin()) ||
//                isnanVector(getRXDirection()) || isnanVector(getRYDirection())));
//    }
//
//    constexpr bool hasDifferentials() const noexcept
//    {
//        return m_hasDifferentials;
//    }
//
//    ~HRayDifferential() noexcept = default;
//
//private:
//    bool m_hasDifferentials{ false };
//    Point3c m_rxOrigin{}, m_ryOrigin{};
//    Vec3c m_rxDirection{}, m_ryDirection{};
};