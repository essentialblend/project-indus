export module indus.ui.hud_constructs;

import <SFML/Graphics.hpp>;

export
{
  struct GenericPadding { static inline float kGenericPadding{ 20.f }; };

  struct StripHUDProperties final
  {
    const float kStripLength{ 30 };
    const float kProgBarLength{ kStripLength * 0.55f };

    const unsigned kTextFontSize{ 12 };

    const sf::Color kBackplateColor{ 12, 12, 16, 185 };
    const sf::Color kProgBarBGColor{ 255, 255, 255, 22 };
    const sf::Color kProgBarMainColor{ 102, 186, 255, 230 };

    const float kStripGenericMargin{ 17.0f };
  };

  struct HUDBackplateProperties final
  {
    HUDBackplateProperties() = delete;
    HUDBackplateProperties(sf::RenderTarget& target)
    {
      const float targetSzWidth{ static_cast<float>(target.getSize().x) };
      const float targetSzHeight{ static_cast<float>(target.getSize().y) };

      kBackplateSize = { std::max(0.0f, targetSzWidth - 2.0f * kBackplateMargin), std::max(0.0f, targetSzHeight - 2.0f * kBackplateMargin) };
    }

    const sf::Color kBackplateColor{ 14, 14, 16, 205 };
    
    const float kBackplateMargin{ 32.0f };
    sf::Vector2f kBackplateSize{};
    const sf::Vector2f kBackplatePos{ kBackplateMargin, kBackplateMargin };
  };

  struct DetailedHUDProperties final
  {
    const unsigned kBaseFontSize{ 12u };

    const unsigned kHUDMainHeaderFontSize{ kBaseFontSize + 3u };
    const unsigned kHUDMainSubHeaderFontSize{ kBaseFontSize + 2u };

    const unsigned kSectionHeaderFontSize{ kHUDMainHeaderFontSize };

    const sf::Color kFontColor{ sf::Color::White };

    const std::u32string_view kSigilGlyph{ U"\uE000" };
    const float kLogoScale{ 0.85f };
  };

  enum class TextAnchor { Left, Center };

  struct HUDLayoutContext 
  {
    sf::Vector2f origin{};
    float usableWidth{};
    const sf::Font& font;
    float xLeftAnchor{};
  };

  struct ProgressBarStyle 
  {
    float barHeight{};
    sf::Color bgColor{};
    sf::Color fgColor{};
    sf::Color textColor{};
    unsigned textSize{};
  };

  struct TimerStyle 
  {
    sf::Color textColor{};
    unsigned textSize{};
  };

  enum class HUDSystemType { Header, RenderConfig, RenderStats };
};