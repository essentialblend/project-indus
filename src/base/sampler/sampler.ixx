export module sampler;

import std;
import core_constructs;

export class Sampler 
{
public:
  virtual ~Sampler() = default;

  // Called before starting a pixel’s sampling sequence
  virtual void startPixelSample(int px, int py, int sampleIndex, int dimension = 0) = 0;

  // Step to next sample for this pixel
  virtual bool startNextSample() = 0;

  // Jump to arbitrary sample index (used by some integrators)
  virtual bool setSampleNumber(std::size_t sampleNum) = 0;

  virtual double get1D() = 0;
  virtual Sample2D get2D() = 0;
  virtual int getSPP() const noexcept = 0;

  // Reserve N-sized arrays for specialized integrators (lights, DOF, etc.)
  virtual void request1DArray(int n) = 0;
  virtual void request2DArray(int n) = 0;
  virtual std::span<const double> get1DArray(int n) const = 0;
  virtual std::span<const Sample2D> get2DArray(int n) const = 0;

  // For stratified/QMC samplers: round array counts up safely
  virtual int roundCount(int n) const = 0;

  virtual std::unique_ptr<Sampler> clone(int seed) const = 0;
};
