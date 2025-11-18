export module indus.ui.hud_component;

import <../dep/SFML/Graphics.hpp>;

import indus.ui.hud_constructs;

export class HUDComponent
{
public:
  explicit HUDComponent(float padding = 0.f) noexcept;
  virtual float draw(sf::RenderTarget& target, const HUDLayoutContext& HUDLayoutContext) const = 0;
  virtual ~HUDComponent() = default;

protected:
  const float m_componentPadding{};
};

HUDComponent::HUDComponent(float padding) noexcept : m_componentPadding{ padding } {}
