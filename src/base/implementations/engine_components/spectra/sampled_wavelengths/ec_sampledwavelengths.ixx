export module ec_sampledwavelengths;

import <vector>;
import <compare>;
import <string>;

import i_enginecomponent;

export class ECSampledSpectrum;

export class ECSampledWavelengths : public IEngineComponent
{
public:
	explicit ECSampledWavelengths() noexcept = default;
	explicit ECSampledWavelengths(const double, const double, const double) noexcept;
	ECSampledWavelengths(const ECSampledWavelengths&) = default;
	ECSampledWavelengths& operator=(const ECSampledWavelengths&) = default;
	ECSampledWavelengths(ECSampledWavelengths&&) noexcept = default;
	ECSampledWavelengths& operator=(ECSampledWavelengths&&) noexcept = default;
	
	std::partial_ordering operator<=>(const ECSampledWavelengths&) const = default;
	double operator[](const int i) const noexcept;
	double& operator[](const int i) noexcept;

	static ECSampledWavelengths sampleUniform(const double, const double, const double);
	static ECSampledWavelengths sampleVisible(const double);

	ECSampledSpectrum getSpectralPDFs() const;

	const std::vector<double>& getWavelengthsPDFVec() const noexcept;

	std::string toString() const;

	void terminateSecondary();
	bool secondaryTerminated() const;

	~ECSampledWavelengths() noexcept = default;

private:
	std::vector<double> m_sampledWavelengthsVec{};
	std::vector<double> m_wavelengthsPDFsVec{};
};

