export module color_rgb;

import <cmath>;
import <memory>;
import <stdexcept>;

import color;
import vec3;
import interval;

export class ColorRGB final : public IColor, public std::enable_shared_from_this<ColorRGB>
{
public:
	explicit ColorRGB() noexcept = default;
	explicit ColorRGB(const double x) noexcept : m_colorVector{ x } {}
	explicit ColorRGB(const double x, const double y, const double z) noexcept : m_colorVector{ x, y, z } {}
	explicit ColorRGB(const Vec3& other) noexcept : m_colorVector{ other } {}

	ColorRGB(const ColorRGB& other) noexcept = default;
	ColorRGB(ColorRGB&& other) noexcept = default;
	ColorRGB& operator=(const ColorRGB& other) noexcept = default;
	ColorRGB& operator=(ColorRGB&& other) noexcept = default;

	ColorRGB& operator+=(const ColorRGB& other) noexcept;
	ColorRGB& operator-=(const ColorRGB& other) noexcept;
	ColorRGB& operator*=(const ColorRGB& other) noexcept;
	ColorRGB& operator*=(const double scalar) noexcept;
	ColorRGB operator+(const ColorRGB& other) const noexcept;
	ColorRGB operator-(const ColorRGB& other) const noexcept;
	ColorRGB operator*(const ColorRGB& other) const noexcept;
	ColorRGB operator*(const double scalar) const noexcept;
	ColorRGB operator/(const double scalar) const;

	virtual void applyGammaCorrection(const double gammaFactor) override;
	virtual void applyWeights(const double weightFactor) noexcept override;
	virtual void applyNormalization() override;
	virtual void undoNormalization(const double normFactor = 256) override;
	virtual void transformSelfToMinValues() noexcept override;
	virtual void transformSelfToMaxValues() noexcept override;

	virtual void addColorToSelf(const IColor& otherColor) override;
	virtual void multiplyColorWithSelf(const IColor& otherColor) override;
	virtual void multiplyScalarWithSelf(const double scalar) override;
	virtual void negateSelf() noexcept override;

	virtual std::shared_ptr<const IColor> getColorImmutable() const override;
	virtual std::shared_ptr<IColor> getColorMutable() override;
	virtual void setColor(const IColor& otherColor) override;

	double getRedComponent() const noexcept;
	double getGreenComponent() const noexcept;
	double getBlueComponent() const noexcept;

	~ColorRGB() noexcept = default;
private:
	Vec3 m_colorVector{};
};
