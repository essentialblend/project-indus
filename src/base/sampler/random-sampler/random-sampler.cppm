import randomsampler;

import std;
import core_constructs;


RandomSampler::RandomSampler(int spp, unsigned int seed)
  : m_samplesPerPixel{ spp }, m_currentSample{ 0 }, m_rng{ seed } {
}

// Reset sampler state for a new pixel & sample index
void RandomSampler::startPixelSample(int px, int py, int sampleIndex, int /*dimension*/) 
{
  m_currentPixelX = px;
  m_currentPixelY = py;
  m_currentSample = sampleIndex;
}

// Step to next sample for this pixel
bool RandomSampler::startNextSample() 
{
  ++m_currentSample;
  return m_currentSample < m_samplesPerPixel;
}

// Random-access set sample number (useful for progressive integrators)
bool RandomSampler::setSampleNumber(std::size_t sampleNum)
{
  m_currentSample = static_cast<int>(sampleNum);
  return m_currentSample < m_samplesPerPixel;
}

// Return a single uniform random sample in [0,1)
double RandomSampler::get1D() 
{
  return m_dist(m_rng);
}

// Return a 2D uniform random sample
Sample2D RandomSampler::get2D() 
{
  return { m_dist(m_rng), m_dist(m_rng) };
}

int RandomSampler::getSPP() const noexcept
{
  return m_samplesPerPixel;
}

// Reserve an array of N 1D samples for later retrieval
void RandomSampler::request1DArray(int n) 
{
  m_array1D.resize(n);
  for (auto& v : m_array1D) v = m_dist(m_rng);
}

// Reserve an array of N 2D samples
void RandomSampler::request2DArray(int n) 
{
  m_array2D.resize(n);
  for (auto& v : m_array2D) 
  { 
    v.u = m_dist(m_rng); v.v = m_dist(m_rng); 
  }
}

// Retrieve previously requested arrays
std::span<const double> RandomSampler::get1DArray(int /*n*/) const { return m_array1D; }
std::span<const Sample2D> RandomSampler::get2DArray(int /*n*/) const { return m_array2D; }

// Round counts for stratified/QMC samplers; trivial for RNG
int RandomSampler::roundCount(int n) const { return n; }

// Clone a sampler for parallel threads with new seed
std::unique_ptr<Sampler> RandomSampler::clone(int seed) const 
{
  return std::make_unique<RandomSampler>(m_samplesPerPixel, seed);
}
