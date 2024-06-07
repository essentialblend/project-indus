//export module ec_camera;
//
//import <glm/glm.hpp>;
//import <optional>;
//
//import core_constructs;
//import i_enginecomponent;
//import es_pixelsampler;
//import ec_framebuffer;
//import h_cameratransform;
//import h_ray;
//import h_raydifferential;
//
//export class ECCamera abstract : public IEngineComponent {
//public:
//    ECCamera() noexcept = default;
//
//    virtual const ECFramebuffer& getFilmFramebuffer() const noexcept = 0;
//    virtual const HCameraTransform& getCameraTransform() const noexcept = 0;
//    virtual double sampleTime(const double) const noexcept = 0;
//    //virtual void approximateDpDxy(const Point3&, const glm::dvec3&, const double time, const int, glm::dvec3*, glm::dvec3* dpdy) const = 0;
//    virtual ~ECCamera() noexcept = default;
//
//protected:
//    virtual HRay renderFromCameraRay(const HRay&) const = 0;
//    virtual HRayDifferential renderFromCameraRayDiff(const HRayDifferential&) const = 0;
//    virtual glm::dvec3 renderFromCameraVector(const glm::dvec3&, const double) const = 0;
//    virtual glm::dvec3 renderFromCameraNormal(const glm::dvec3&, const double) const = 0;
//    virtual Point3 renderFromCameraPoint(const Point3&, const double) const = 0;
//
//    virtual glm::dvec3 cameraFromRenderVector(const glm::dvec3&, const double) const = 0;
//    virtual glm::dvec3 cameraFromRenderNormal(const glm::dvec3&, const double) const = 0;
//    virtual Point3 cameraFromRenderPoint(const Point3&, const double) const = 0;
//
//    virtual void findMinimumDifferentials(ECCamera) = 0;
//    virtual std::optional<HRayDifferential> generateRayDifferential(const ECCamera&) = 0;
//};
