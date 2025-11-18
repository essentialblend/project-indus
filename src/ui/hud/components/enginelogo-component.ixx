export module indus.ui.enginelogo_component;

import std;

import indus.ui.engine_logo;
import indus.ui.hud_constructs;
import indus.ui.hud_component;

export class EngineLogoComponent final : public HUDComponent
{
public:
  EngineLogoComponent(const EngineLogo& logo, std::u32string_view glyph, float scale, float componentPadding);
  float draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const override;

private:
  std::reference_wrapper<const EngineLogo> m_logo;
  std::u32string m_glyph{};
  float m_scale{};
};

EngineLogoComponent::EngineLogoComponent(const EngineLogo& logo, std::u32string_view glyph, float scale, float componentPadding) : HUDComponent{ componentPadding }, m_logo { logo }, m_glyph{ std::move(glyph) }, m_scale{ scale } {}

float EngineLogoComponent::draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const
{
  sf::RenderStates states{};
  states.transform.translate(ctx.origin);

  m_logo.get().draw(target, states, m_glyph, m_scale);

  const sf::FloatRect b{ m_logo.get().getEngineLogoBounds() };
  const float h{ std::floor(b.height + 0.5f) };

  return h + m_componentPadding;
}