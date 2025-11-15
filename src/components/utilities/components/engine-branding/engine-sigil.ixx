export module enginesigil;

import <../dep/SFML/Graphics.hpp>;

import std;

export class EngineSigil final
{
public:
  EngineSigil(const sf::Font& glyphFont) noexcept;
  
  void draw(sf::RenderTarget& target, const sf::RenderStates& states, std::u32string_view glyph, float scale) const;

  sf::FloatRect getLocalBounds(float scale, std::u32string_view glyph) const noexcept;

  static constexpr float getUnitSize() noexcept;

private:
  std::reference_wrapper<const sf::Font> m_font;
  static constexpr float kUnitSize{ 128.f };
};

EngineSigil::EngineSigil(const sf::Font& glyphFont) noexcept : m_font(glyphFont) {}

void EngineSigil::draw(sf::RenderTarget& target, const sf::RenderStates& states, std::u32string_view glyph, float scale) const
{
  const float pixelSz{ scale * kUnitSize };

  sf::String s; for (char32_t ch : glyph) s += static_cast<sf::Uint32>(ch);

  sf::Text t{};
  t.setFont(m_font.get());
  t.setString(s);
  t.setCharacterSize(static_cast<unsigned>(pixelSz));

  const auto b = t.getLocalBounds();
  t.setOrigin(b.left + 0.5f * b.width, b.top);
  t.setFillColor(sf::Color(224, 224, 224, 85));

  target.draw(t, states);
}

sf::FloatRect EngineSigil::getLocalBounds(float scale, std::u32string_view glyph) const noexcept
{
  const float pixelSz{ scale * kUnitSize };

  sf::Text textObj{};
  textObj.setFont(m_font.get());

  sf::String textStr{};

  for (char32_t ch : glyph)
    textStr += static_cast<sf::Uint32>(ch);

  textObj.setString(textStr);
  textObj.setCharacterSize(static_cast<unsigned>(pixelSz));

  return textObj.getLocalBounds();
}

constexpr float EngineSigil::getUnitSize() noexcept
{
  return kUnitSize;
}
