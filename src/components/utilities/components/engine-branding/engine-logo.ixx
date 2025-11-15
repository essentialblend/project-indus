export module enginelogo;

import <SFML/Graphics.hpp>;

import std;
import enginesigil;
import hudcomponent;
import hudconstructs;

export class EngineLogo final
{
public:
  EngineLogo(EngineSigil& sigil, const sf::Font& logoFont) noexcept;
  
  void setText(std::string_view text);
  void draw(sf::RenderTarget& target, const sf::RenderStates& states, std::u32string_view glyph, float scale) const;

  const sf::Font& getFont() const noexcept;
  const sf::FloatRect getEngineLogoBounds() const noexcept;

private:
  std::reference_wrapper<EngineSigil> m_sigil;
  std::reference_wrapper<const sf::Font> m_logoFont;

  mutable sf::FloatRect m_engineLogoBounds{};

  std::string m_text{ "indus" };
};

EngineLogo::EngineLogo(EngineSigil& sigil, const sf::Font& logoFont) noexcept : m_sigil{ sigil }, m_logoFont{ logoFont } {}

void EngineLogo::setText(std::string_view text) 
{
  m_text.assign(text.begin(), text.end());
}

void EngineLogo::draw(sf::RenderTarget& target, const sf::RenderStates& states, std::u32string_view glyph, float scale) const
{
  m_sigil.get().draw(target, states, glyph, scale);

  const float pxText{ scale * 128.0f };

  sf::Text textObj{};
  textObj.setFont(m_logoFont.get());
  textObj.setString(m_text);
  textObj.setCharacterSize(static_cast<unsigned>(pxText));

  const sf::FloatRect textBounds{ textObj.getLocalBounds() };
  textObj.setOrigin(textBounds.left + 0.5f * textBounds.width, textBounds.top);
  textObj.setFillColor(sf::Color(246, 246, 246, 235));
  textObj.setOutlineColor(sf::Color(0, 0, 0, 110));
  textObj.setOutlineThickness(std::max(0.6f, 0.012f * pxText));

  const sf::FloatRect glyphBounds{ m_sigil.get().getLocalBounds(scale, glyph) };
  const float centerShiftY{ 0.5f * (glyphBounds.height - textBounds.height) };

  sf::RenderStates textStates{ states };
  textStates.transform.translate(0.f, centerShiftY);
  target.draw(textObj, textStates);

  const float compositeHeight{ std::max(glyphBounds.height, centerShiftY + textBounds.height) };
  const float compositeWidth{ std::max(glyphBounds.width,  textBounds.width) };
  m_engineLogoBounds = { 0.f, 0.f, compositeWidth, compositeHeight };
}

const sf::Font& EngineLogo::getFont() const noexcept
{
  return m_logoFont;
}

const sf::FloatRect EngineLogo::getEngineLogoBounds() const noexcept
{
  return m_engineLogoBounds;
}
