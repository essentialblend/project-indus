export module indus.ui.renderstats_system;

import std;

import indus.ui.hud_component;
import indus.ui.hud_system;
import indus.ui.hud_constructs;

import indus.stats.constructs;

export class RenderStatsSystem final : public HUDSystem
{
public:
  explicit RenderStatsSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType = HUDSystemType::RenderStats);

  float drawSystem(sf::RenderTarget& target, HUDLayoutContext ctx) const override;

  void applyScrollDelta(float deltaPixels) noexcept;

private:
  mutable float m_scrollOffset{};
  mutable float m_maxScroll{};

  float drawTitle(sf::RenderTarget& target, const HUDLayoutContext& ctx, const std::vector<std::unique_ptr<HUDComponent>>& comps) const;

  StatsViewportGeometry computeViewportGeometry(const HUDLayoutContext& ctx, float titleHeight, const HUDBackplateProperties& bp) const;

  float measureContentHeight(float width, const sf::Font& font, const std::vector<std::unique_ptr<HUDComponent>>& comps) const;

  void renderContentTexture(sf::RenderTexture& textRT, float width, const sf::Font& font, const std::vector<std::unique_ptr<HUDComponent>>& comps) const;

  void applyEdgeFades(sf::RenderTexture& textRT, float contentHeight, float maxScroll) const;

  void drawIntoViewport(sf::RenderTarget& target, const StatsViewportGeometry& g, const sf::Texture& texture) const;
};

RenderStatsSystem::RenderStatsSystem(std::vector<std::unique_ptr<HUDComponent>> components, HUDSystemType systemType) : HUDSystem{ std::move(components), systemType } {}

float RenderStatsSystem::drawSystem(sf::RenderTarget& target, HUDLayoutContext ctx) const
{
  const auto& comps{ getComponents() };
  const std::size_t n{ comps.size() };
  
  if (n == 0) return 0.0f;

  const float titleHeight{ drawTitle(target, ctx, comps) };

  const HUDBackplateProperties bp{ target };
  
  const StatsViewportGeometry geom{ computeViewportGeometry(ctx, titleHeight, bp) };
  if (geom.height <= 0.0f) return titleHeight;

  const float contentHeight{ measureContentHeight(geom.width, ctx.font, comps) };
  if (contentHeight <= 0.0f) return titleHeight;

  sf::RenderTexture textRT{};
  textRT.create(static_cast<unsigned>(geom.width), static_cast<unsigned>(contentHeight));
  renderContentTexture(textRT, geom.width, ctx.font, comps);

  const float maxScroll{ std::max(0.0f, contentHeight - geom.height) };
  m_scrollOffset = std::clamp(m_scrollOffset, 0.0f, maxScroll);

  applyEdgeFades(textRT, contentHeight, maxScroll);
  textRT.display();

  drawIntoViewport(target, geom, textRT.getTexture());

  return titleHeight + geom.height;
}

void RenderStatsSystem::applyScrollDelta(float deltaPixels) noexcept
{
  m_scrollOffset += deltaPixels;
}

float RenderStatsSystem::drawTitle(sf::RenderTarget& target, const HUDLayoutContext& ctx, const std::vector<std::unique_ptr<HUDComponent>>& comps) const
{
  if (comps.empty())
  {
    return 0.0f;
  }

  float consumed{};

  const sf::Vector2f titlePos{ ctx.origin.x, ctx.origin.y };

  HUDLayoutContext titleCtx
  {
    titlePos,
    ctx.usableWidth,
    ctx.font,
    ctx.xLeftAnchor
  };

  consumed += comps.front()->draw(target, titleCtx);

  return consumed;
}

StatsViewportGeometry RenderStatsSystem::computeViewportGeometry(const HUDLayoutContext& ctx, float titleHeight, const HUDBackplateProperties& bp) const
{
  const float left{ ctx.xLeftAnchor };
  const float top{ ctx.origin.y + titleHeight };

  const float width{ ctx.usableWidth };
  const float vpBottom{ bp.kBackplatePos.y + bp.kBackplateSize.y - GenericPadding::kGenericPadding };
  const float height{ std::max(0.0f, vpBottom - top) };
  const float yBaseCenter{ top + 0.5f * height };

  return StatsViewportGeometry{ left, top, width, height, yBaseCenter };
}

float RenderStatsSystem::measureContentHeight(float width, const sf::Font& font, const std::vector<std::unique_ptr<HUDComponent>>& comps) const
{
  if (comps.size() <= 1) return 0.0f;

  sf::RenderTexture rt{};
  rt.create(1u, 1u);

  float y{};
  
  for (std::size_t i{ 1 }; i < comps.size(); ++i)
  {
    const sf::Vector2f pos{ 0.0f, y };
    
    HUDLayoutContext ctx{ pos, width, font, 0.0f };
    
    y += comps[i]->draw(rt, ctx);
  }

  return y;
}

void RenderStatsSystem::renderContentTexture(sf::RenderTexture& textRT, float width, const sf::Font& font, const std::vector<std::unique_ptr<HUDComponent>>& comps) const
{
  textRT.clear(sf::Color(0, 0, 0, 0));

  float y{};
  for (std::size_t i{ 1 }; i < comps.size(); ++i)
  {
    const sf::Vector2f pos{ 0.0f, y };
    HUDLayoutContext ctx{ pos, width, font, 0.0f };
    y += comps[i]->draw(textRT, ctx);
  }
}

void RenderStatsSystem::applyEdgeFades(sf::RenderTexture& textRT, float contentHeight, float maxScroll) const
{
  const float rawFadeH{ GenericPadding::kGenericPadding * 3.5f };
  const float fadeH{ std::min(rawFadeH, 0.5f * contentHeight) };
  if (fadeH <= 0.0f) return;

  const float width{ static_cast<float>(textRT.getSize().x) };

  const sf::Color dark{ 0, 0, 0, 255 };
  const sf::Color keep{ 255, 255, 255, 255 };

  if (m_scrollOffset > 0.0f)
  {
    sf::VertexArray topGrad{ sf::TriangleStrip, 4 };

    topGrad[0].position = { 0.0f, 0.0f };
    topGrad[1].position = { width, 0.0f };
    topGrad[2].position = { 0.0f, fadeH };
    topGrad[3].position = { width, fadeH };

    topGrad[0].color = dark;
    topGrad[1].color = dark;
    topGrad[2].color = keep;
    topGrad[3].color = keep;

    textRT.draw(topGrad, sf::BlendMultiply);
  }

  if (m_scrollOffset < maxScroll)
  {
    sf::VertexArray botGrad{ sf::TriangleStrip, 4 };

    const float y0{ contentHeight - fadeH };

    botGrad[0].position = { 0.0f, y0 };
    botGrad[1].position = { width, y0 };
    botGrad[2].position = { 0.0f, contentHeight };
    botGrad[3].position = { width, contentHeight };

    botGrad[0].color = keep;
    botGrad[1].color = keep;
    botGrad[2].color = dark;
    botGrad[3].color = dark;

    textRT.draw(botGrad, sf::BlendMultiply);
  }
}

void RenderStatsSystem::drawIntoViewport(sf::RenderTarget& target, const StatsViewportGeometry& geom, const sf::Texture& texture) const
{
  const float cy{ geom.yBaseCenter + m_scrollOffset };

  sf::View view{};
  view.setCenter(geom.left + 0.5f * geom.width, cy);
  view.setSize(geom.width, geom.height);

  const sf::Vector2u winSize{ target.getSize() };
  view.setViewport(
  {
    geom.left / static_cast<float>(winSize.x),
    geom.top / static_cast<float>(winSize.y),
    geom.width / static_cast<float>(winSize.x),
    geom.height / static_cast<float>(winSize.y)
  });

  const sf::View oldView{ target.getView() };
  target.setView(view);

  sf::Sprite sprite{ texture };
  sprite.setPosition(geom.left, geom.top);

  target.draw(sprite);
  target.setView(oldView);
}