export module h_rgbsigmoidpolynomial;

import <cmath>;

import h_mathutilities;
import h_spectraconstants;


export class HRGBSigmoidPolynomial
{
public:
	constexpr explicit HRGBSigmoidPolynomial() noexcept = default;
	constexpr explicit HRGBSigmoidPolynomial(const Float, const Float, const Float) noexcept;

	constexpr HRGBSigmoidPolynomial(const HRGBSigmoidPolynomial&) = default;
	constexpr HRGBSigmoidPolynomial(HRGBSigmoidPolynomial&&) noexcept = default;
	constexpr HRGBSigmoidPolynomial& operator=(const HRGBSigmoidPolynomial&) = default;
	constexpr HRGBSigmoidPolynomial& operator=(HRGBSigmoidPolynomial&&) noexcept = default;

	Float operator()(const Float) const noexcept;

	constexpr Float getMaxValue() const noexcept;

	~HRGBSigmoidPolynomial() = default;
private:
	Float c0{}, c1{}, c2{};

	static Float sigmoid(const Float) noexcept;
	static Float evaluatePolynomial(const Float, const Float, const Float, const Float) noexcept;

};

constexpr HRGBSigmoidPolynomial::HRGBSigmoidPolynomial(const Float c0, const Float c1, const Float c2) noexcept : c0{ c0 }, c1{ c1 }, c2{ c2 } {}

Float HRGBSigmoidPolynomial::operator()(const Float lambda) const noexcept {
	return sigmoid(evaluatePolynomial(lambda, c2, c1, c0));
}

constexpr Float HRGBSigmoidPolynomial::getMaxValue() const noexcept {
	Float result{ std::max((*this)(HSpectraConstants::lambdaMIN), (*this)(HSpectraConstants::lambdaMAX)) };
	Float lambda{ -c1 / (2 * c0) };
	if (lambda >= HSpectraConstants::lambdaMIN && lambda <= HSpectraConstants::lambdaMAX)
		result = std::max(result, (*this)(lambda));
	return result;
}

Float HRGBSigmoidPolynomial::sigmoid(const Float x) noexcept
{
	if (std::isinf(x))
		return x > 0 ? 1 : 0;
	return 0.5 + x / (2 * std::sqrt(1 + x * x));
}

// CARE ARG ORDER. c2, c1, c0
Float HRGBSigmoidPolynomial::evaluatePolynomial(const Float lambda, const Float c0, const Float c1, const Float c2) noexcept
{
	return c2 * lambda * lambda + c1 * lambda + c0;
}
