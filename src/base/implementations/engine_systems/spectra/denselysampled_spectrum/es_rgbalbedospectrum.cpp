//module es_rgbalbedospectrum;
//
//import h_spectraconstants;
//import ec_sampledspectrum;
//
//constexpr ESRGBAlbedoSpectrum::ESRGBAlbedoSpectrum(const HRGBSigmoidPolynomial& rsp) noexcept : m_rsp(rsp) {}
//
//Float ESRGBAlbedoSpectrum::operator()(const Float lambda) const
//{
//	return m_rsp(lambda);
//}
//
//ECSampledSpectrum ESRGBAlbedoSpectrum::sampleFromSpectrum(const ECSampledWavelengths& swl) const
//{
//    ECSampledSpectrum sampledSpectrum{};
//    for (std::size_t i = 0; i < HSpectraConstants::numSpectrumSamples; ++i) {
//        sampledSpectrum[i] = m_rsp(swl[static_cast<int>(i)]);
//    }
//    return sampledSpectrum;
//}
//
//Float ESRGBAlbedoSpectrum::getMaxComponentValue() const
//{
//	return 0.0;
//}
