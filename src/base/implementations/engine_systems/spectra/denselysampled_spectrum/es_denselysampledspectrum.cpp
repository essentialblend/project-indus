//module es_denselysampledspectrum;
//
//import <cmath>;
//
//import h_spectraconstants;
//
//
//constexpr ESDenselySampledSpectrum::ESDenselySampledSpectrum(const Float lambda_min = HSpectraConstants::lambdaMIN , const Float lambda_max = HSpectraConstants::lambdaMAX) noexcept
//    : m_lambdaMin(lambda_min), m_lambdaMax(lambda_max), m_sampleValues(static_cast<int>(m_lambdaMax - m_lambdaMin + 1), 0.0f) {}
//
//constexpr ESDenselySampledSpectrum::ESDenselySampledSpectrum(const ESSpectrum& ess) : ESDenselySampledSpectrum(HSpectraConstants::lambdaMIN, HSpectraConstants::lambdaMAX)
//{
//    for (int lambda{ static_cast<int>(m_lambdaMin) }; lambda <= m_lambdaMax; ++lambda) 
//    {
//        m_sampleValues[static_cast<long long>(lambda - m_lambdaMin)] = ess(lambda);
//    }
//}
//
//constexpr ESDenselySampledSpectrum::ESDenselySampledSpectrum(const ESDenselySampledSpectrum& dss)
//    : m_lambdaMin(dss.m_lambdaMin), m_lambdaMax(dss.m_lambdaMax), m_sampleValues(dss.m_sampleValues) {}
//
//ECSampledSpectrum ESDenselySampledSpectrum::sampleFromSpectrum(const ECSampledWavelengths& swl) const
//{
//    ECSampledSpectrum sampled{};
//    for (int i{}; i < HSpectraConstants::numSpectrumSamples; ++i) {
//        int offset{ static_cast<int>(std::lround(swl[i]) - m_lambdaMin) };
//        sampled[i] = (offset < 0 || offset >= static_cast<int>(m_sampleValues.size())) ? 0.0 : m_sampleValues[offset];
//    }
//    return sampled;
//}
//
//Float ESDenselySampledSpectrum::getMaxComponentValue() const
//{
//    return *std::max_element(m_sampleValues.begin(), m_sampleValues.end());
//}
//
//Float ESDenselySampledSpectrum::operator()(const Float lambda) const
//{
//    int offset{ static_cast<int>(std::lround(lambda) - m_lambdaMin) };
//    if (offset < 0 || offset >= m_sampleValues.size())
//        return 0.0f;
//    return m_sampleValues[offset];
//}
//
//constexpr void ESDenselySampledSpectrum::scaleSpectrum(const Float s)
//{
//    for (Float& value : m_sampleValues) {
//        value *= s;
//    }
//}
//
