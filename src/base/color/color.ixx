export module color;
import <memory>;

export class IColor abstract
{
public:
    IColor() noexcept = default;

    virtual void applyGammaCorrection(const double gammaFactor) = 0;
    virtual void applyNormalization() = 0;
    virtual void undoNormalization(const double normFactor) = 0;
    virtual void applyWeights(const double weightFactor) = 0;

    virtual void addColorToSelf(const IColor& otherColor) = 0;
    virtual void multiplyColorWithSelf(const IColor& otherColor) = 0;
    virtual void transformSelfToMinValues() = 0;
    virtual void transformSelfToMaxValues() = 0;
    virtual void negateSelf() noexcept = 0;

    virtual std::shared_ptr<const IColor> getColor() const = 0;
    virtual void setColor(const IColor& otherColor) = 0;

    virtual ~IColor() noexcept = default;
};