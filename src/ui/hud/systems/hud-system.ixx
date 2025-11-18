export module indus.ui.hud_system;

import <../dep/SFML/Graphics.hpp>;

import std;

import indus.ui.hud_constructs;
import indus.ui.hud_component;

export class HUDSystem
{
public:
  explicit HUDSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType);
  virtual float drawSystem(sf::RenderTarget& target, HUDLayoutContext ctx) const;

  void addComponent(std::unique_ptr<HUDComponent> component);
  void clearDynamicComponentsFromIdx(std::size_t index);

  HUDSystemType getSystemType() const noexcept;

protected:
  const std::vector<std::unique_ptr<HUDComponent>>& getComponents() const noexcept;

private:
  std::vector<std::unique_ptr<HUDComponent>> m_components{};
  HUDSystemType m_systemType{};
};

HUDSystem::HUDSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType) : m_components{ std::move(components) }, m_systemType{ systemType } {}

float HUDSystem::drawSystem(sf::RenderTarget& target, HUDLayoutContext ctx) const
{
  float consumed{};
  bool isFirst{ true };

  for (const auto& comp : m_components)
  {
    sf::Vector2f pos{ ctx.origin.x, ctx.origin.y + consumed };

    if (!isFirst) pos.x = ctx.xLeftAnchor;

    HUDLayoutContext localCtx{ pos, ctx.usableWidth, ctx.font, ctx.xLeftAnchor };

    consumed += comp->draw(target, localCtx);
    isFirst = false;
  }
  return consumed;
}

void HUDSystem::addComponent(std::unique_ptr<HUDComponent> component)
{
  m_components.push_back(std::move(component));
}

void HUDSystem::clearDynamicComponentsFromIdx(std::size_t index)
{
  m_components.erase(m_components.begin() + index, m_components.end());
}

HUDSystemType HUDSystem::getSystemType() const noexcept
{
  return m_systemType;
}

const std::vector<std::unique_ptr<HUDComponent>>& HUDSystem::getComponents() const noexcept
{
  return m_components;
}

