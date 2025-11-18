export module indus.ui.multitextrow_component;

import <../dep/SFML/Graphics.hpp>;

import std;

import indus.ui.hud_component;
import indus.ui.hud_constructs;

export class MultiTextRowComponent final : public HUDComponent
{
public:
  MultiTextRowComponent(std::vector<std::string> entries, unsigned fontSize, sf::Color color, float componentPadding);

  float draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const override;

private:
  std::vector<std::string> m_entries{};
  unsigned m_fontSize{};
  sf::Color m_color{};
};

MultiTextRowComponent::MultiTextRowComponent(std::vector<std::string> entries, unsigned fontSize, sf::Color color, float componentPadding) : HUDComponent{ componentPadding }, m_entries{ std::move(entries) }, m_fontSize{ fontSize }, m_color{ color } {}

float MultiTextRowComponent::draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const
{
  std::string line{};

  for (std::size_t i{}; i < m_entries.size(); ++i)
  {
    line += m_entries[i];
    
    if (i + 1 < m_entries.size())
      line += "  |  ";
  }

  sf::Text text{};
  
  text.setFont(ctx.font);
  text.setString(line);
  text.setCharacterSize(m_fontSize);
  text.setFillColor(m_color);

  const sf::FloatRect lb{ text.getLocalBounds() };

  text.setPosition(ctx.origin.x, ctx.origin.y - lb.top);

  target.draw(text);

  return std::floor(lb.height + 0.5f) + m_componentPadding;
}