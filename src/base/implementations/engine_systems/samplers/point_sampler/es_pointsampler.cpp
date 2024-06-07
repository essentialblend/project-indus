//module es_pointsampler;
//
//import core_util;
//import core_constructs;
//
//void ESPointSampler::initializeEntity([[maybe_unused]] const std::vector<std::any>& args)
//{
//    for(auto& arg : args)
//	{
//		if (arg.type() == typeid(PixelDimension))
//		{
//            m_pixelDimensionObj = std::any_cast<PixelDimension>(arg);
//		}
//        else
//        {
//            m_cameraCenter = std::any_cast<Point3>(arg);
//        }
//	}
//}
//
//HRay ESPointSampler::getRayForPixelSample([[maybe_unused]] const std::vector<std::any>& args)
//{
//    int rowCount{ std::any_cast<int>(args[0]) }; int columnCount{ std::any_cast<int>(args[1]) };
//
//    const auto sampleOffset{ glm::dvec3(UGenRNG<double>() - 0.5, UGenRNG<double>() - 0.5, 0) };
//    m_tempSamplePoint = m_pixelDimensionObj.topLeftPixCenter + ((columnCount + sampleOffset.x) * m_pixelDimensionObj.lateralSpanAbsVal) + ((rowCount + sampleOffset.y) * m_pixelDimensionObj.vertSpanAbsVal);
//    
//    const Point3 rayOrigin{m_cameraCenter};
//    const glm::dvec3 rayDirection{ m_tempSamplePoint - rayOrigin };
//    
//    return HRay(rayOrigin, rayDirection);
//}