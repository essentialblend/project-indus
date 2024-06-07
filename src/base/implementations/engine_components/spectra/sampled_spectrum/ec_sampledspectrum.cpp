//module ec_sampledspectrum;
//
//import <stdexcept>;
//import <cmath>;
//
//import h_spectraconstants;
//import h_mathutilities;
//
//
//ECSampledSpectrum::ECSampledSpectrum(const Float x) noexcept
//{
//	m_sampleValues.fill(x);
//}
//
//ECSampledSpectrum::ECSampledSpectrum(std::span<const Float> span) noexcept
//{
//	std::copy(span.begin(), span.end(), m_sampleValues.begin());
//}
//
//double ECSampledSpectrum::operator[](const size_t index) const
//{
//	if(index < 0 || index >= HSpectraConstants::numSpectrumSamples)
//	{
//		throw std::out_of_range("Index out of range");
//	}
//	return m_sampleValues[index];
//}
//
//double& ECSampledSpectrum::operator[](const size_t index)
//{
//	if (index < 0 || index >= HSpectraConstants::numSpectrumSamples)
//	{
//		throw std::out_of_range("Index out of range");
//	}
//	return m_sampleValues[index];
//}
//
//ECSampledSpectrum::operator bool() const noexcept
//{
//	for (std::size_t i{}; i < HSpectraConstants::numSpectrumSamples; ++i)
//		if (m_sampleValues[i] != 0)
//			return true;
//	return false;
//}
//
//Float ECSampledSpectrum::interpolateSpectrumSamples(std::span<const Float> lambdas, std::span<const Float> samples, const Float l) const
//{
//	if (l <= lambdas[0]) return samples[0];
//	if (l >= lambdas[lambdas.size() - 1]) return samples[samples.size() - 1];
//	int offset{ static_cast<int>(std::lower_bound(lambdas.begin(), lambdas.end(), l) - lambdas.begin()) };
//	Float t = (l - lambdas[static_cast<long long>(offset - 1)]) / (lambdas[offset] - lambdas[static_cast<long long>(offset - 1)]);
//
//	return HLerp<Float>(t, samples[static_cast<long long>(offset - 1)], samples[offset]);
//}
//
//
//HRGB ECSampledSpectrum::toRGB(const ECSampledWavelengths& swl) const
//{
//	HXYZ intermediate{ toXYZ(swl) };
//	HRGB linearRGB{ XYZToSRGB(intermediate) };
//	return HRGB(linearToSRGB(linearRGB[0]), linearToSRGB(linearRGB[1]), linearToSRGB(linearRGB[2]));
//
//}
//
//HXYZ ECSampledSpectrum::toXYZ(const ECSampledWavelengths& swl) const
//{
//	ECSampledSpectrum X{}, Y{}, Z{};
//	for (int i = 0; i < HSpectraConstants::numSpectrumSamples; ++i) {
//		X[i] = interpolateSpectrumSamples(HSpectraConstants::CIE_LAMBDA, HSpectraConstants::CIE_X, swl[i]);
//		Y[i] = interpolateSpectrumSamples(HSpectraConstants::CIE_LAMBDA, HSpectraConstants::CIE_Y, swl[i]);
//		Z[i] = interpolateSpectrumSamples(HSpectraConstants::CIE_LAMBDA, HSpectraConstants::CIE_Z, swl[i]);
//	}
//
//	ECSampledSpectrum pdf{ swl.getSpectralPDFs() };
//
//	return HXYZ(
//		safeDivide(X * *this, pdf).getAverageValue(),
//		safeDivide(Y * *this, pdf).getAverageValue(),
//		safeDivide(Z * *this, pdf).getAverageValue()
//	) / HSpectraConstants::CIE_Y_Integral;
//
//}
//
//ECSampledSpectrum ECSampledSpectrum::operator-() const
//{
//	ECSampledSpectrum result{ *this };
//	for (auto& sample : result.m_sampleValues) { sample = -sample; };
//	return result;
//}
//
//ECSampledSpectrum& ECSampledSpectrum::operator+=(const ECSampledSpectrum& other)
//{
//	for (std::size_t i{}; i < HSpectraConstants::numSpectrumSamples; ++i) { m_sampleValues[i] += other.m_sampleValues[i]; }
//	return *this;
//}
//
//ECSampledSpectrum& ECSampledSpectrum::operator-=(const ECSampledSpectrum& other)
//{
//	for (std::size_t i{}; i < HSpectraConstants::numSpectrumSamples; ++i) { m_sampleValues[i] -= other.m_sampleValues[i]; }
//	return *this;
//}
//
//ECSampledSpectrum& ECSampledSpectrum::operator*=(const ECSampledSpectrum& other)
//{
//	for (std::size_t i{}; i < HSpectraConstants::numSpectrumSamples; ++i) { m_sampleValues[i] *= other.m_sampleValues[i]; }
//	return *this;
//}
//
//ECSampledSpectrum& ECSampledSpectrum::operator/=(const ECSampledSpectrum& other)
//{
//	for (int i = 0; i < HSpectraConstants::numSpectrumSamples; ++i)
//	{
//		other.m_sampleValues[i] != 0 ? m_sampleValues[i] /= other.m_sampleValues[i] : m_sampleValues[i] = 0;
//	}
//
//	return *this;
//}
//
//ECSampledSpectrum& ECSampledSpectrum::operator*=(const Float scalar)
//{
//	for (std::size_t i{}; i < HSpectraConstants::numSpectrumSamples; ++i) { m_sampleValues[i] *= scalar; }
//	return *this;
//}
//
//ECSampledSpectrum& ECSampledSpectrum::operator/=(const Float scalar)
//{
//	for (int i = 0; i < HSpectraConstants::numSpectrumSamples; ++i)
//	{
//		scalar != 0 ? m_sampleValues[i] /= scalar : m_sampleValues[i] = 0;
//	}
//
//	return *this;
//}
//
//ECSampledSpectrum ECSampledSpectrum::operator+(const ECSampledSpectrum& other) const
//{
//	ECSampledSpectrum result{ *this };
//	result += other;
//	return result;
//}
//
//ECSampledSpectrum ECSampledSpectrum::operator-(const ECSampledSpectrum& other) const
//{
//	ECSampledSpectrum result{ *this };
//	result -= other;
//	return result;
//}
//
//ECSampledSpectrum ECSampledSpectrum::operator*(const ECSampledSpectrum& other) const
//{
//	ECSampledSpectrum result{ *this };
//	result *= other;
//	return result;
//}
//
//ECSampledSpectrum ECSampledSpectrum::operator/(const ECSampledSpectrum& other) const
//{
//	ECSampledSpectrum result{ *this };
//	result /= other;
//	return result;
//}
//
//Float ECSampledSpectrum::getAverageValue() const
//{
//	Float sum{};
//	for (const auto& sample : m_sampleValues)
//		sum += sample;
//
//	return (sum / HSpectraConstants::numSpectrumSamples);
//}
//
//Float ECSampledSpectrum::getMaxComponentValue() const
//{
//	return *std::max_element(m_sampleValues.begin(), m_sampleValues.end());
//}
//
//Float ECSampledSpectrum::getMinComponentValue() const
//{
//	return *std::min_element(m_sampleValues.begin(), m_sampleValues.end());
//}
//
//Float ECSampledSpectrum::hasNaNs() const
//{
//	return std::any_of(m_sampleValues.begin(), m_sampleValues.end(), [](Float v) { return std::isnan(v); });
//}
//
//// Non-members.
//ECSampledSpectrum safeDivide(const ECSampledSpectrum& a, const ECSampledSpectrum& b)
//{
//	ECSampledSpectrum r;
//	for (int i{}; i < HSpectraConstants::numSpectrumSamples; ++i)
//		r[i] = (b[i] != 0) ? a[i] / b[i] : 0.;
//	return r;
//}
//
//HXYZ multiplyMatrix(const std::array<std::array<Float, 3>, 3>& matrix, const HXYZ& xyzColor)
//{
//	HXYZ result{};
//	result[0] = matrix[0][0] * xyzColor[0] + matrix[0][1] * xyzColor[1] + matrix[0][2] * xyzColor[2];
//	result[1] = matrix[1][0] * xyzColor[0] + matrix[1][1] * xyzColor[1] + matrix[1][2] * xyzColor[2];
//	result[2] = matrix[2][0] * xyzColor[0] + matrix[2][1] * xyzColor[2] + matrix[2][2] * xyzColor[2];
//
//	return result;
//}
//
//HRGB XYZToSRGB(const HXYZ& xyzColor)
//{
//	static const std::array<std::array<Float, 3>, 3> XYZToSRGBMatrix = { {
//		{  3.2406, -1.5372, -0.4986 },
//		{ -0.9689,  1.8758,  0.0415 },
//		{  0.0557, -0.2040,  1.0570 }
//	} };
//
//	HXYZ rgb{ multiplyMatrix(XYZToSRGBMatrix, xyzColor) };
//	return HRGB{ rgb[0], rgb[1], rgb[2] };
//}
//
//Float linearToSRGB(const Float x)
//{
//	if (x <= 0.0031308) {
//		return 12.92 * x;
//	}
//	else {
//		return 1.055 * std::pow(x, 1.0 / 2.4) - 0.055;
//	}
//}
