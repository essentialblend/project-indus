//module ec_sampledwavelengths;
//
//import <cmath>;
//
//import ec_sampledspectrum;
//import h_spectraconstants;
//import h_mathutilities;
//import h_samplingutils;
//
//ECSampledWavelengths::ECSampledWavelengths(const double u, const double minLambda = HSpectraConstants::lambdaMIN, const double maxLambda = HSpectraConstants::lambdaMAX) noexcept {
//    m_sampledWavelengthsVec.resize(HSpectraConstants::numSpectrumSamples);
//    m_wavelengthsPDFsVec.resize(HSpectraConstants::numSpectrumSamples);
//
//    // Sample first wavelength using _u_
//    m_sampledWavelengthsVec[0] = HLerp<double>(u, minLambda, maxLambda);
//
//    // Initialize _lambda_ for remaining wavelengths
//    double delta = (maxLambda - minLambda) / HSpectraConstants::numSpectrumSamples;
//    for (std::size_t i = 1; i < HSpectraConstants::numSpectrumSamples; ++i) {
//        m_sampledWavelengthsVec[i] = m_sampledWavelengthsVec[i - 1] + delta;
//        if (m_sampledWavelengthsVec[i] > maxLambda) {
//            m_sampledWavelengthsVec[i] = minLambda + (m_sampledWavelengthsVec[i] - maxLambda);
//        }
//    }
//
//    // Compute PDF for sampled wavelengths
//    for (std::size_t i = 0; i < HSpectraConstants::numSpectrumSamples; ++i) {
//        m_wavelengthsPDFsVec[i] = 1.0 / (maxLambda - minLambda);
//    }
//}
//
//double ECSampledWavelengths::operator[](const int i) const noexcept
//{
//    return m_sampledWavelengthsVec[i];
//}
//
//double& ECSampledWavelengths::operator[](const int i) noexcept
//{
//    return m_sampledWavelengthsVec[i];
//}
//
//ECSampledWavelengths ECSampledWavelengths::sampleUniform(const double u, const double minLambda = HSpectraConstants::lambdaMIN, const double maxLambda = HSpectraConstants::lambdaMAX)
//{
//    ECSampledWavelengths swl{};
//
//    swl.m_sampledWavelengthsVec[0] = HLerp<double>(u, minLambda, maxLambda);
//
//    double delta{ (maxLambda - minLambda) / HSpectraConstants::numSpectrumSamples };
//    for (std::size_t i{ 1 }; i < HSpectraConstants::numSpectrumSamples; ++i) {
//        swl.m_sampledWavelengthsVec[i] = swl.m_sampledWavelengthsVec[static_cast<long long>(i - 1)] + delta;
//        if (swl.m_sampledWavelengthsVec[i] > maxLambda) {
//            swl.m_sampledWavelengthsVec[i] = minLambda + (swl.m_sampledWavelengthsVec[i] - maxLambda);
//        }
//    }
//
//    for (int i = 0; i < HSpectraConstants::numSpectrumSamples; ++i)
//    {
//        swl.m_wavelengthsPDFsVec[i] = 1.0 / (maxLambda - minLambda);
//    }
//
//    return swl;
//}
//
//ECSampledWavelengths ECSampledWavelengths::sampleVisible(const double u)
//{
//    ECSampledWavelengths swl{};
//
//    for (std::size_t i{}; i < HSpectraConstants::numSpectrumSamples; ++i)
//    {
//        double up{ u + (double(i) / HSpectraConstants::numSpectrumSamples) };
//    
//        if (up > 1.0) up -= 1.0;
//
//        swl.m_sampledWavelengthsVec[i] = sampleVisibleWavelengths(up);
//        swl.m_wavelengthsPDFsVec[i] = visibleWavelengthsPDF(swl.m_sampledWavelengthsVec[i]);
//    }
//
//    return swl;
//}
//
//ECSampledSpectrum ECSampledWavelengths::getSpectralPDFs() const
//{
//    return ECSampledSpectrum(m_wavelengthsPDFsVec);
//}
//
//const std::vector<double>& ECSampledWavelengths::getWavelengthsPDFVec() const noexcept
//{
//    return m_wavelengthsPDFsVec;
//}
//
//std::string ECSampledWavelengths::toString() const
//{
//    std::string result = "Lambda: ";
//    for (const auto& l : m_sampledWavelengthsVec) {
//        result += std::to_string(l) + " ";
//    }
//    result += "\nPDF: ";
//    for (const auto& p : m_wavelengthsPDFsVec) {
//        result += std::to_string(p) + " ";
//    }
//    return result;
//}
//
//void ECSampledWavelengths::terminateSecondary()
//{
//    if (secondaryTerminated())
//        return;
//    for (std::size_t i{ 1 }; i < HSpectraConstants::numSpectrumSamples; ++i)
//        m_wavelengthsPDFsVec[i] = 0;
//    m_wavelengthsPDFsVec[0] /= HSpectraConstants::numSpectrumSamples;
//}
//
//bool ECSampledWavelengths::secondaryTerminated() const
//{
//    for (std::size_t i{ 1 }; i < HSpectraConstants::numSpectrumSamples; ++i)
//        if (m_wavelengthsPDFsVec[i] != 0)
//            return false;
//    return true;
//}
