export module es_denselysampledspectrum;

import <span>;

import es_spectrum;
import ec_sampledspectrum;
import h_mathutilities;

export class ESDenselySampledSpectrum : public ESSpectrum
{
public:
	constexpr explicit ESDenselySampledSpectrum() noexcept = default;
	//constexpr explicit ESDenselySampledSpectrum(const Float, const Float) noexcept;
	//constexpr explicit ESDenselySampledSpectrum(const ESSpectrum&);
	//constexpr explicit ESDenselySampledSpectrum(const ESDenselySampledSpectrum&);

	//constexpr ESDenselySampledSpectrum(ESDenselySampledSpectrum&) = default;
	//constexpr ESDenselySampledSpectrum(ESDenselySampledSpectrum&&) noexcept = default;
	//constexpr ESDenselySampledSpectrum& operator=(const ESDenselySampledSpectrum&) = default;
	//constexpr ESDenselySampledSpectrum& operator=(ESDenselySampledSpectrum&&) noexcept = default;

	//constexpr std::strong_ordering operator<=>(const ESDenselySampledSpectrum&) const = default;

	//ECSampledSpectrum sampleFromSpectrum(const ECSampledWavelengths&) const override;
	//Float getMaxComponentValue() const override;
	//Float operator()(const Float) const override;

	//constexpr void scaleSpectrum(const Float);

	~ESDenselySampledSpectrum() noexcept = default;

private:
	Float m_lambdaMin{}, m_lambdaMax{};
	std::vector<Float> m_sampleValues{};
};