export module bsdf;

import std;
import vector;
import bxdf;
import onb;
import constructs;
import point;
import colorrgb;
import types;
import normal;
import core_sampling_util;

import <cassert>;

export class BSDF final 
{
public:
  constexpr BSDF() noexcept = default;
  constexpr explicit BSDF(const OrthonormalBasis& basis) noexcept;
  
  [[nodiscard]] ColorRGB evaluate(const Vec3f&, const Vec3f&) const noexcept;
  [[nodiscard]] std::tuple<Vec3f, Float, ColorRGB, BxDFType> sample(const Vec3f&, const Point2f&) const;
  [[nodiscard]] Float PDF(const Vec3f& unitW_o, const Vec3f& unitW_i) const noexcept;
  
  constexpr void addBxDF(std::unique_ptr<BxDF> bxdf) noexcept;
  constexpr void clearBxDFs() noexcept;

  constexpr const OrthonormalBasis& getBasis() noexcept;

  ~BSDF() noexcept = default;

protected:
  BSDF(const BSDF&) = delete;
  BSDF& operator=(const BSDF&) = delete;
  BSDF(BSDF&&) noexcept = default;
  BSDF& operator=(BSDF&&) noexcept = default;

private:
  OrthonormalBasis m_basis{};
  std::vector<std::unique_ptr<BxDF>> m_bxdfs{};
};

constexpr BSDF::BSDF(const OrthonormalBasis& basis) noexcept : m_basis{ basis } {}

ColorRGB BSDF::evaluate(const Vec3f& w_o, const Vec3f& w_i) const noexcept
{
  // BSDFs work in local-space. So we convert world to local -> evaluate BSDF -> convert back to world and pass on to the integrator
  const Vec3f localW_o{ m_basis.worldToLocal(w_o) };
  const Vec3f localW_i{ m_basis.worldToLocal(w_i) };

  return m_bxdfs.front()->evaluate(localW_o, localW_i);
}

std::tuple<Vec3f, Float, ColorRGB, BxDFType> BSDF::sample(const Vec3f& unitW_o, const Point2f& uniformSample) const
{
  if (m_bxdfs.empty())
  {
    return { Vec3f{Float(0.0)}, Float(0.0), ColorRGB{Float(0.0)}, BxDFType::Diffuse };
  }

  const Int numBxDFs{ static_cast<Int>(m_bxdfs.size()) };
  const Int comp{ std::min(static_cast<Int>(uniformSample[0] * numBxDFs), numBxDFs - 1) };
  const auto& chosenBxDF{ m_bxdfs[comp] };

  Point2f remappedSample{ (uniformSample[0] * numBxDFs) - comp, uniformSample[1] };

  const Vec3f localW_o{ m_basis.worldToLocal(unitW_o) };
  auto [localW_i, PDFVal, BRDFVal, BxDFTypeVal] = chosenBxDF->sample(localW_o, remappedSample);

  if (PDFVal <= 0.0) 
  {
    return { Vec3f{0}, Float(0.0), ColorRGB{0}, BxDFTypeVal };
  }

  Float pdf{ PDFVal };
  ColorRGB f{ BRDFVal };

  if (!isSpecularBxDF(BxDFTypeVal) && numBxDFs > 1) 
  {
    for (Idx i{}; i < numBxDFs; ++i)
    {
      if (i == comp) continue;

      pdf += m_bxdfs[i]->PDF(localW_o, localW_i);

      const bool reflect = computeDot(unitW_o, m_basis.getNormal()) * computeDot(m_basis.localToWorld(localW_i), m_basis.getNormal()) > 0.0;

      const BxDFType bxdfType = m_bxdfs[i]->type();
      const bool bxdfMatches = (reflect && (bxdfType & BxDFType::Reflection) == BxDFType::Reflection) || (!reflect && (bxdfType & BxDFType::Transmission) == BxDFType::Transmission);

      if (bxdfMatches) f += m_bxdfs[i]->evaluate(localW_o, localW_i);
    }

    pdf /= numBxDFs;
  }

  return { m_basis.localToWorld(localW_i), pdf, f, BxDFTypeVal };
}

Float BSDF::PDF(const Vec3f& unitW_o, const Vec3f& unitW_i) const noexcept
{
  const Vec3f unitLocalW_o{ m_basis.worldToLocal(unitW_o) };
  const Vec3f unitLocalW_i{ m_basis.worldToLocal(unitW_i) };

  return m_bxdfs.front()->PDF(unitLocalW_o, unitLocalW_i);

}

constexpr void BSDF::addBxDF(std::unique_ptr<BxDF> bxdf) noexcept
{
  m_bxdfs.push_back(std::move(bxdf));
}

constexpr void BSDF::clearBxDFs() noexcept
{
  m_bxdfs.clear();
}

constexpr const OrthonormalBasis& BSDF::getBasis() noexcept
{
  return m_basis;
}
