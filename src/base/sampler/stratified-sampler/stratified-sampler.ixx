export module stratifiedsampler;

import std;
import sampler;

export class StratifiedSampler final : public Sampler 
{
public:
  explicit StratifiedSampler(int nx, int ny, bool jittered, unsigned int seed = std::random_device{}());

  void startPixelSample(int px, int py, int sampleIndex, int dimension = 0) override;
  bool startNextSample() override;
 
  double get1D() override;
  Sample2D get2D() override;
  int getSPP() const noexcept override;

  void request1DArray(int n) override;
  void request2DArray(int n) override;
  std::span<const double> get1DArray(int n) const override;
  std::span<const Sample2D> get2DArray(int n) const override;

  std::unique_ptr<Sampler> clone(int seed) const override;
  int roundCount(int n) const override;
  bool setSampleNumber(std::size_t sampleIndex) override;

private:
  int m_samplesPerPixel{};
  int m_currentSample{};
  int m_currentPixelX{}, m_currentPixelY{};
  int m_nx{}, m_ny{};
  bool m_jittered{ true };

  std::mt19937 m_rng;
  std::uniform_real_distribution<double> m_dist{ 0.0, 1.0 };

  std::vector<double> m_array1D{};
  std::vector<Sample2D> m_array2D{};
};
