import bsdf;

import std;
import bxdf;
import vec3;
import core_util;
import fresneldielectric;

BSDF::BSDF(const OrthonormalBasis& basis) noexcept : m_basis{ basis } {}

auto BSDF::evaluate(const Vec3& w_o, const Vec3& w_i) const noexcept -> ColorRGB 
{
  // BSDFs work in local-space. So we convert world to local -> evaluate BSDF -> convert back to world and pass on to the integrator
  const Vec3 localW_o{ worldToLocal(w_o) };
  const Vec3 localW_i{ worldToLocal(w_i) };

  return m_bxdfs.front()->evaluate(localW_o, localW_i);
}

auto BSDF::sample(const Vec3& unitW_o, const Sample2D& uniformSample) const -> std::tuple<Vec3, double, ColorRGB, BxDFType>
{
  if (m_bxdfs.empty()) 
  {
    return { Vec3{0}, 0.0, ColorRGB{0}, BxDFType::Diffuse };
  }

  const int numBxDFs{ static_cast<int>(m_bxdfs.size()) };
  const int comp{ std::min(static_cast<int>(uniformSample.u * numBxDFs), numBxDFs - 1) };
  const auto& chosenBxDF{ m_bxdfs[comp] };

  Sample2D remappedSample{ (uniformSample.u * numBxDFs) - comp, uniformSample.v };

  const Vec3 localW_o{ worldToLocal(unitW_o) };
  auto [localW_i, PDFVal, BRDFVal, BxDFTypeVal] = chosenBxDF->sample(localW_o, remappedSample);

  if (PDFVal <= 0.0) {
    return { Vec3{0}, 0.0, ColorRGB{0}, BxDFTypeVal };
  }

  double pdf{ PDFVal };
  ColorRGB f{ BRDFVal };

  if (!isSpecularBxDF(BxDFTypeVal) && numBxDFs > 1) {
    for (int i = 0; i < numBxDFs; ++i) {
      if (i == comp) continue;

      pdf += m_bxdfs[i]->PDF(localW_o, localW_i);

      const bool reflect =
        computeDot(unitW_o, m_basis.m_unitNormalVec) *
        computeDot(localToWorld(localW_i), m_basis.m_unitNormalVec) > 0.0;

      const BxDFType bxdfType = m_bxdfs[i]->type();
      const bool bxdfMatches =
        (reflect && (bxdfType & BxDFType::Reflection) == BxDFType::Reflection) ||
        (!reflect && (bxdfType & BxDFType::Transmission) == BxDFType::Transmission);

      if (bxdfMatches) {
        f += m_bxdfs[i]->evaluate(localW_o, localW_i);
      }
    }
    pdf /= numBxDFs;
  }

  return { localToWorld(localW_i), pdf, f, BxDFTypeVal };
}

auto BSDF::PDF(const Vec3& unitW_o, const Vec3& unitW_i) const noexcept -> double 
{
  const Vec3 unitLocalW_o{ worldToLocal(unitW_o) };
  const Vec3 unitLocalW_i{ worldToLocal(unitW_i) };

  return m_bxdfs.front()->PDF(unitLocalW_o, unitLocalW_i);

}

void BSDF::addBxDF(std::unique_ptr<BxDF> bxdf) noexcept 
{
  m_bxdfs.push_back(std::move(bxdf));
}

void BSDF::clearBxDFs() noexcept
{
  m_bxdfs.clear();
}

auto BSDF::worldToLocal(const Vec3& vec) const noexcept -> Vec3
{
  return Vec3{ computeDot(vec, m_basis.m_unitTangentVec), computeDot(vec, m_basis.m_unitBitangentVec), computeDot(vec, m_basis.m_unitNormalVec) };
}

auto BSDF::localToWorld(const Vec3& vec) const noexcept -> Vec3
{
  return (m_basis.m_unitTangentVec * vec[0]) + (m_basis.m_unitBitangentVec * vec[1]) + (m_basis.m_unitNormalVec * vec[2]);
}
