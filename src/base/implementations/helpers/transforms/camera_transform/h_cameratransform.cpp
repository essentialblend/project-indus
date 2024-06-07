//module h_cameratransform;
//
//import h_transform;
//
//HCameraTransform::HCameraTransform(const HTransform& worldFromCamera) noexcept : m_renderFromCameraTrans{ worldFromCamera }, m_worldFromRenderTrans{ worldFromCamera.getInvertedTransformMatrix() } {}
//
//Point3 HCameraTransform::renderFromCameraPoint(const Point3& point) const
//{
//	return m_renderFromCameraTrans.transformPoint(point);
//}
//
//glm::dvec3 HCameraTransform::renderFromCameraVector(const Point3& point) const
//{
//	return m_renderFromCameraTrans.transformVector(point);
//}
//
//HRay HCameraTransform::renderFromCameraRay(const HRay& ray) const
//{
//	return m_renderFromCameraTrans.transformRay(ray);
//}
//
//HRayDifferential HCameraTransform::renderFromCameraRayDiff(const HRayDifferential& rayDiff) const
//{
//	return m_renderFromCameraTrans.transformRayDifferential(rayDiff);
//}
//
//Point3 HCameraTransform::cameraFromRenderPoint(const Point3& point) const
//{
//	return m_renderFromCameraTrans.inverseTransformPoint(point);
//}
//
//glm::dvec3 HCameraTransform::cameraFromRenderVector(const Point3& point) const
//{
//	return m_renderFromCameraTrans.inverseTransformVector(point);
//}
//
//HRay HCameraTransform::cameraFromRenderRay(const HRay& ray) const
//{
//	return m_renderFromCameraTrans.inverseTransformRay(ray);
//}
//
//HRayDifferential HCameraTransform::cameraFromRenderRayDiff(const HRayDifferential& rayDiff) const
//{
//	return m_renderFromCameraTrans.inverseTransformRayDifferential(rayDiff);
//}
//
//HTransform HCameraTransform::cameraFromRenderTrans() const
//{
//	return m_renderFromCameraTrans.getInvertedTransformMatrix();
//}
//
//HTransform HCameraTransform::cameraFromWorldTrans() const
//{
//	return m_worldFromRenderTrans.getInvertedTransformMatrix();
//}
//
//Point3 HCameraTransform::renderFromWorldPoint(const Point3& point) const
//{
//	return m_worldFromRenderTrans.inverseTransformPoint(point);
//}
//
//bool HCameraTransform::cameraFromRenderHasScale() const
//{
//	return m_renderFromCameraTrans.hasScale();
//}
//
//const HTransform& HCameraTransform::getRenderFromCamera() const noexcept
//{
//	return m_renderFromCameraTrans;
//}
//
//const HTransform& HCameraTransform::getWorldFromRender() const noexcept
//{
//	return m_worldFromRenderTrans;
//}
