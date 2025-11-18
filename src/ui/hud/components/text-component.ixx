export module indus.ui.text_component;

import std;

import indus.ui.hud_component;
import indus.ui.hud_constructs;

export class TextComponent final : public HUDComponent
{
public:
  TextComponent(std::string text, unsigned size, sf::Color color, TextAnchor anchor, float componentPadding) noexcept;

  float draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const override;

private:
  std::string m_text{};
  unsigned m_size{};
  sf::Color m_SFMLColor{};
  TextAnchor m_textAnchor{};
};

TextComponent::TextComponent(std::string text, unsigned size, sf::Color color, TextAnchor anchor, float componentPadding) noexcept : HUDComponent{ componentPadding }, m_text { std::move(text) }, m_size{ size }, m_SFMLColor{ color }, m_textAnchor{ anchor } {}

float TextComponent::draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const
{
  sf::Text textObj{};

  textObj.setFont(ctx.font);
  textObj.setString(m_text);
  textObj.setCharacterSize(m_size);
  textObj.setFillColor(m_SFMLColor);

  const sf::FloatRect textObjLocalBounds{ textObj.getLocalBounds() };

  float x{};

  if (m_textAnchor == TextAnchor::Left)
  {
    x = ctx.xLeftAnchor;
  }
  else
  {
    x = ctx.origin.x - 0.5f * textObjLocalBounds.width;
  }

  textObj.setPosition(x - textObjLocalBounds.left, ctx.origin.y - textObjLocalBounds.top);

  target.draw(textObj);

  return std::floor(textObjLocalBounds.height + 0.5f) + m_componentPadding;
}