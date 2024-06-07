export module eu_interaction;

import <compare>;

import h_rays;
import i_engineutility;
import h_mathutilities;
import point3;
import vector;

//export class EUInteraction : public IEngineUtility
//{
//public:
//	constexpr EUInteraction() noexcept = default;
//	constexpr EUInteraction(Point3c pi, Normal3c n, Vec2c uv, Vec3c wo, Float time) noexcept
//		: pi(pi), n(n), uv(uv), wo(Normalize(wo)), time(time) {}
//	constexpr EUInteraction(Point3c p, Vec3c wo, Float time, HMedium medium) noexcept
//		: pi(p), time(time), wo(wo), medium(medium) {}
//	constexpr EUInteraction(Point3c p, Normal3c n, Float time, HMedium medium) noexcept
//		: pi(p), n(n), time(time), medium(medium) {}
//	constexpr EUInteraction(Point3c p, Vec2c uv) noexcept : pi(p), uv(uv) {}
//	constexpr EUInteraction(const Point3c& pi, Normal3c n, Float time = 0, Vec2c uv = {}) noexcept
//		: pi(pi), n(n), uv(uv), time(time) {}
//	constexpr EUInteraction(const Point3c& pi, Normal3c n, Vec2c uv) noexcept : pi(pi), n(n), uv(uv) {}
//	constexpr EUInteraction(Point3c p, Float time, HMedium medium) noexcept
//		: pi(p), time(time), medium(medium) {}
//	constexpr EUInteraction(Point3c p, const HMediumInterface* mediumInterface) noexcept
//		: pi(p), mediumInterface(mediumInterface) {}
//	constexpr EUInteraction(Point3c p, Float time, const HMediumInterface* mediumInterface) noexcept
//		: pi(p), time(time), mediumInterface(mediumInterface) {}
//
//	constexpr bool isSurfaceInteraction() const {
//		return computeMagnitude(hitNormal) > 0.0;
//	}
//
//	Point3c hitPoint{};
//	Vec3c hitNormal{};
//	Float hitTime{};
//	Vec3c hitRayOutDir{};
//	//Vec2c uv; TBD
//	//const Medium* hitMedium{}; TBD
//	// HMediumInterface TBD
//	
//	~EUInteraction() noexcept = default;
//};
//
//export class EUSurfaceInteraction : public EUInteraction
//{
//public:
//	constexpr explicit EUSurfaceInteraction() noexcept = default;
//	constexpr explicit EUSurfaceInteraction(const Point3c& point, const Vec3c& normal, const double time, const Vec3c& out) : EUInteraction(point, normal, time, out), finalHitShadingNormal{ normal } {}
//	~EUSurfaceInteraction() noexcept = default;	
//
//	constexpr void setShadingGeometry(const Vec3c& outNormal, const Vec3c& faceNormal)
//	{
//		hitNormal = faceNormal; 
//		bool frontFace{ computeDot(hitRayOutDir, outNormal) < 0.0 };
//		finalHitShadingNormal = frontFace ? outNormal : -(outNormal);
//	}
//
//	Vec3c finalHitShadingNormal{};
//};