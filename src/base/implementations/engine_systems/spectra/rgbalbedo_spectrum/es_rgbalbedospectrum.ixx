export module es_rgbalbedospectrum;

import es_spectrum;
import h_rgbsigmoidpolynomial;

import h_mathutilities;

export class ESRGBAlbedoSpectrum : public ESSpectrum
{
public:
	constexpr explicit ESRGBAlbedoSpectrum() noexcept = default;
	//constexpr explicit ESRGBAlbedoSpectrum(const HRGBSigmoidPolynomial&) noexcept;

	//constexpr ESRGBAlbedoSpectrum(const ESRGBAlbedoSpectrum&) = default;
	//constexpr ESRGBAlbedoSpectrum(ESRGBAlbedoSpectrum&&) noexcept = default;
	//constexpr ESRGBAlbedoSpectrum& operator=(const ESRGBAlbedoSpectrum&) = default;
	//constexpr ESRGBAlbedoSpectrum& operator=(ESRGBAlbedoSpectrum&&) noexcept = default;

	//Float operator()(const Float) const override;
	//ECSampledSpectrum sampleFromSpectrum(const ECSampledWavelengths&) const override;
	//Float getMaxComponentValue() const override;

	~ESRGBAlbedoSpectrum() noexcept = default;

private:
	HRGBSigmoidPolynomial m_rsp{};
};


