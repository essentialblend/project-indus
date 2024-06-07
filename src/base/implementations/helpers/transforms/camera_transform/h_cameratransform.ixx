//export module h_cameratransform;
//
//import h_transform;
//import h_ray;
//import h_raydifferential;
//
//export class HCameraTransform
//{
//public:
//	explicit HCameraTransform() noexcept = default;
//	explicit HCameraTransform(const HTransform&) noexcept;
//	HCameraTransform(const HCameraTransform&) = default;
//	HCameraTransform& operator=(const HCameraTransform&) = default;
//	HCameraTransform(HCameraTransform&&) noexcept = default;
//	HCameraTransform& operator=(HCameraTransform&&) noexcept = default;
//
//	Point3 renderFromCameraPoint(const Point3&) const;
//	glm::dvec3 renderFromCameraVector(const Point3&) const;
//	HRay renderFromCameraRay(const HRay&) const;
//	HRayDifferential renderFromCameraRayDiff(const HRayDifferential&) const;
//
//	Point3 cameraFromRenderPoint(const Point3&) const;
//	glm::dvec3 cameraFromRenderVector(const Point3&) const;
//	HRay cameraFromRenderRay(const HRay&) const;
//	HRayDifferential cameraFromRenderRayDiff(const HRayDifferential&) const;
//	HTransform cameraFromRenderTrans() const;
//
//	HTransform cameraFromWorldTrans() const;
//	Point3 renderFromWorldPoint(const Point3&) const;
//	bool cameraFromRenderHasScale() const;
//
//	const HTransform& getRenderFromCamera() const noexcept;
//	const HTransform& getWorldFromRender() const noexcept;
//
//	~HCameraTransform() noexcept = default;
//private:
//	HTransform m_renderFromCameraTrans{}, m_worldFromRenderTrans{};
//};
//
