export module eu_color;

import <memory>;

import i_engineutility;

export class EUColor abstract : public IEngineUtility
{
public:
	EUColor() noexcept = default;

	virtual void addTo(const EUColor&) = 0;
	virtual void multiplyWith(const EUColor&) = 0;
	virtual void multiplyScalarWith(const double) = 0;
	virtual void negate() noexcept = 0;
	virtual void gammaCorrect(const double) = 0;

	virtual const EUColor& getColor() = 0;
	virtual void setColor(const EUColor&) = 0;

	virtual ~EUColor() noexcept = default;
};