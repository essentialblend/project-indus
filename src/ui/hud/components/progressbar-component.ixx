export module indus.ui.progbar_component;

import <../dep/SFML/Graphics.hpp>;

import std;

import indus.ui.hud_component;
import indus.ui.hud_constructs;

export class ProgressBarComponent final : public HUDComponent
{
public:
  ProgressBarComponent(float progressValue, ProgressBarStyle styleBundle, std::string percentText, float componentPadding);

  float draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const override;

private:
  float m_progressValue{};
  ProgressBarStyle m_progBarStyle{};
  std::string m_percentString{};
};

ProgressBarComponent::ProgressBarComponent(float progressValue, ProgressBarStyle styleBundle, std::string percentText, float componentPadding) : HUDComponent{ componentPadding }, m_progressValue{ std::clamp(progressValue, 0.0f, 1.0f) }, m_progBarStyle{ std::move(styleBundle) }, m_percentString{ std::move(percentText) } {}

float ProgressBarComponent::draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const
{
  const sf::Vector2f originPosition = ctx.origin;
  const float componentWidth = ctx.usableWidth;
  const float barHeight = m_progBarStyle.barHeight;
  const float barWidth = componentWidth;
  const float filledRegionWidth = m_progressValue * barWidth;

  sf::RectangleShape backgroundRectangle;
  backgroundRectangle.setSize({ barWidth, barHeight });
  backgroundRectangle.setPosition(originPosition);
  backgroundRectangle.setFillColor(m_progBarStyle.bgColor);
  target.draw(backgroundRectangle);

  sf::RectangleShape filledRectangle;
  filledRectangle.setSize({ filledRegionWidth, barHeight });
  filledRectangle.setPosition(originPosition);
  filledRectangle.setFillColor(m_progBarStyle.fgColor);
  target.draw(filledRectangle);

  sf::Text percentText{};
  percentText.setFont(ctx.font);
  percentText.setString(m_percentString);
  percentText.setCharacterSize(m_progBarStyle.textSize);
  percentText.setFillColor(m_progBarStyle.textColor);

  const sf::FloatRect tb{ percentText.getLocalBounds() };

  const float centerX{ originPosition.x + (barWidth - tb.width) * 0.5f - tb.left };
  const float centerY{ originPosition.y + (barHeight - tb.height) * 0.5f - tb.top };

  const float snappedX{ std::floor(centerX + 0.5f) };
  const float snappedY{ std::floor(centerY + 0.5f) };

  percentText.setPosition({ snappedX, snappedY });
  target.draw(percentText);

  return barHeight + m_componentPadding;
}
