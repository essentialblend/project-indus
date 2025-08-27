import stratifiedsampler;

import std;
import core_constructs;


StratifiedSampler::StratifiedSampler(int nx, int ny, bool jittered, unsigned int seed) : m_samplesPerPixel{ nx * ny }, m_nx{ nx }, m_ny{ ny }, m_jittered{ jittered }, m_rng{ seed }, m_dist{ 0.0, 1.0 } {}

int StratifiedSampler::getSPP() const noexcept 
{ 
  return m_samplesPerPixel;
}

void StratifiedSampler::startPixelSample(int, int, int sampleIndex, int) 
{ 
  m_currentSample = sampleIndex; 
}

bool StratifiedSampler::startNextSample() 
{ 
  return ++m_currentSample < m_samplesPerPixel;
}

double StratifiedSampler::get1D() 
{
  double du = m_jittered ? m_dist(m_rng) : 0.5;
  return (m_currentSample + du) / m_samplesPerPixel;
}

Sample2D StratifiedSampler::get2D() 
{
  int ix = m_currentSample % m_nx;
  int iy = m_currentSample / m_nx;
  double du = m_jittered ? m_dist(m_rng) : 0.5;
  double dv = m_jittered ? m_dist(m_rng) : 0.5;
  return { (ix + du) / m_nx, (iy + dv) / m_ny };
}

void StratifiedSampler::request1DArray(int count) 
{
  m_array1D.resize(count);
  for (int i = 0; i < count; ++i) m_array1D[i] = get1D();
}

void StratifiedSampler::request2DArray(int count) 
{
  m_array2D.resize(count);
  for (int i = 0; i < count; ++i) m_array2D[i] = get2D();
}

std::span<const double> StratifiedSampler::get1DArray(int) const 
{ 
  return m_array1D; 
}

std::span<const Sample2D> StratifiedSampler::get2DArray(int) const 
{ 
  return m_array2D; 
}

std::unique_ptr<Sampler> StratifiedSampler::clone(int seed) const 
{
  return std::make_unique<StratifiedSampler>(m_nx, m_ny, m_jittered, seed);
}

int StratifiedSampler::roundCount(int n) const
{
  return ((n + m_samplesPerPixel - 1) / m_samplesPerPixel) * m_samplesPerPixel;
}

bool StratifiedSampler::setSampleNumber(std::size_t sampleIndex) 
{
  m_currentSample = static_cast<int>(sampleIndex);
  return m_currentSample < m_samplesPerPixel;
}

