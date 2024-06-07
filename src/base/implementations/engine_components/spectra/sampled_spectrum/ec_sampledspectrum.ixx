export module ec_sampledspectrum;

import <memory>;
import <optional>;
import <array>;
import <span>;

import ec_sampledwavelengths;
import h_spectraconstants;
import h_mathutilities;
import h_rgb;
import h_xyz;


export class ECSampledSpectrum
{
public:
	explicit ECSampledSpectrum() noexcept = default;
	//explicit ECSampledSpectrum(const Float) noexcept;
	//explicit ECSampledSpectrum(std::span<const Float>) noexcept;

	//ECSampledSpectrum(const ECSampledSpectrum&) = default;
	//ECSampledSpectrum(ECSampledSpectrum&&) noexcept = default;
	//ECSampledSpectrum& operator=(const ECSampledSpectrum&) = default;
	//ECSampledSpectrum& operator=(ECSampledSpectrum&&) noexcept = default;

	//Float operator[] (const size_t) const;
	//Float& operator[] (const size_t);
	//operator bool() const noexcept;

	//// ColorSpace arg hasn't been ported yet.
	//HRGB toRGB(const ECSampledWavelengths&) const;
	//HXYZ toXYZ(const ECSampledWavelengths&) const;

	//ECSampledSpectrum operator-() const;
	//std::partial_ordering operator<=>(const ECSampledSpectrum&) const noexcept = default;
	//bool operator==(const ECSampledSpectrum&) const noexcept = default;

	//ECSampledSpectrum& operator+=(const ECSampledSpectrum&);
	//ECSampledSpectrum& operator-=(const ECSampledSpectrum&);
	//ECSampledSpectrum& operator*=(const ECSampledSpectrum&);
	//ECSampledSpectrum& operator/=(const ECSampledSpectrum&);
	//ECSampledSpectrum& operator*=(const Float);
	//ECSampledSpectrum& operator/=(const Float);

	//ECSampledSpectrum operator+(const ECSampledSpectrum&) const;
	//ECSampledSpectrum operator-(const ECSampledSpectrum&) const;
	//ECSampledSpectrum operator*(const ECSampledSpectrum&) const;
	//ECSampledSpectrum operator/(const ECSampledSpectrum&) const;

	//Float getAverageValue() const;
	//Float getMaxComponentValue() const;
	//Float getMinComponentValue() const;
	//Float hasNaNs() const;
	//// LuminanceY() remaining.
	
	~ECSampledSpectrum() noexcept = default;

private:
	std::array<Float, HSpectraConstants::numSpectrumSamples> m_sampleValues{};

	/*double interpolateSpectrumSamples(std::span<const Float>, std::span<const Float>, const Float) const;*/
};

export
{
	ECSampledSpectrum safeDivide(const ECSampledSpectrum&, const ECSampledSpectrum&);
	HXYZ multiplyMatrix(const std::array<std::array<Float, 3>, 3>&, const HXYZ&);
	HRGB XYZToSRGB(const HXYZ&);
	Float linearToSRGB(const Float);
}


