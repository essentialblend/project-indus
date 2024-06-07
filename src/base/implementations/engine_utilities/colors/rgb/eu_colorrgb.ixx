export module eu_colorrgb;

import eu_color;

import <glm/vec3.hpp>;

export class EUColorRGB final : public EUColor
{
public:
	explicit EUColorRGB() noexcept = default;
	explicit EUColorRGB(const double) noexcept;
	explicit EUColorRGB(const double, const double, const double) noexcept;
	explicit EUColorRGB(const glm::dvec3&) noexcept;
	EUColorRGB(const EUColorRGB&) = default;
	EUColorRGB& operator=(const EUColorRGB&) = default;
	EUColorRGB(EUColorRGB&&) noexcept = default;
	EUColorRGB& operator=(EUColorRGB&&) noexcept = default;

	virtual void addTo(const EUColor&) override;
	virtual void multiplyWith(const EUColor&) override;
	virtual void multiplyScalarWith(const double) override;
	virtual void negate() noexcept override;
	virtual void gammaCorrect(const double) override;

	virtual const EUColor& getColor() override;
	virtual void setColor(const EUColor&) override;

	double getRedComponent() const noexcept;
	double getGreenComponent() const noexcept;
	double getBlueComponent() const noexcept;

	~EUColorRGB() noexcept = default;

private:
	glm::dvec3 m_colorVec{};
};


