export module sampler;

import std;
import types;
import point;

export class Sampler 
{
public:
  virtual ~Sampler() = default;

  virtual void startPixelSample([[maybe_unused]] Point2i pPixel, Int sampleIndex, [[maybe_unused]] Int startingDimension = 0) = 0;

  virtual Float get1D() = 0;
  virtual Point2f get2D() = 0;
  virtual Int getSPP() const noexcept = 0;

  virtual Point2f getPixel2D() = 0;

  virtual std::unique_ptr<Sampler> clone() const = 0;
};
