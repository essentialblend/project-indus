export module indus.ui.timer_component;

import <../dep/SFML/Graphics.hpp>;
import std;

import indus.ui.hud_component;
import indus.ui.hud_constructs;

export class TimerComponent final : public HUDComponent
{
public:
  TimerComponent(std::string timerText, TimerStyle styleBundle);

  float draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const override;

private:
  std::string m_timerString;
  TimerStyle m_style;
};

TimerComponent::TimerComponent(std::string timerText, TimerStyle styleBundle) : m_timerString{ std::move(timerText) }, m_style{ std::move(styleBundle) } {}

float TimerComponent::draw(sf::RenderTarget& target, const HUDLayoutContext& ctx) const
{
  sf::Text timerShape{};

  timerShape.setFont(ctx.font);
  timerShape.setString(m_timerString);
  timerShape.setCharacterSize(m_style.textSize);
  timerShape.setFillColor(m_style.textColor);

  const sf::FloatRect tb{ timerShape.getLocalBounds() };

  const float regionWidth{ ctx.usableWidth };
  const sf::Vector2f originPosition{ ctx.origin };

  const float centerX{ originPosition.x + (regionWidth - tb.width) * 0.5f - tb.left };
  const float centerY{ originPosition.y - tb.top };

  const float snappedX{ std::floor(centerX + 0.5f) };
  const float snappedY{ std::floor(centerY + 0.5f) };

  timerShape.setPosition({ snappedX, snappedY });
  target.draw(timerShape);

  return tb.height;
}

