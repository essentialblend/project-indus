export module indus.ui.sfml_hud;

import <SFML/Graphics.hpp>;

import indus.core.math.fp.ii;

import indus.utilities.basictimer;
import indus.utilities.timing;

import indus.engine.constructs;

import indus.integrator.constructs;

import indus.ui.hud_constructs;
import indus.ui.engine_logo;
import indus.ui.hud_system;
import indus.ui.header_system;
import indus.ui.renderconfig_system;
import indus.ui.renderstats_system;
import indus.ui.text_component;
import indus.ui.hud_component;
import indus.ui.timer_component;
import indus.ui.progbar_component;
import indus.ui.multitextrow_component;
import indus.ui.enginelogo_component;

import indus.stats.constructs;
import indus.stats.builder;


export class SFMLHUD final
{
public:
  SFMLHUD(const EngineBuildInformation& buildInformation, const RenderSystemStrings& renderStrings, const sf::Font& hudFont, const BasicTimer& basicTimer, const EngineLogo& engineLogo) noexcept;

  void drawStripHUD(sf::RenderTarget& target, const FrameSnapshot& frame) const;
  void drawStripHUDComponents(sf::RenderTarget& target, const FrameSnapshot& frameSnapshot) const;
  void drawDetailedHUD(sf::RenderTarget& target, const FrameSnapshot& frame) const;

  void drawDetailedHUDSystems(sf::RenderTarget& target) const;

  void onMouseWheelScrolled(const sf::Event::MouseWheelScrollEvent& e) noexcept;

private:
  const EngineBuildInformation& m_buildInformation;
  const RenderSystemStrings m_renderStrings;
  const sf::Font& m_HUDFont;
  std::optional<std::reference_wrapper<const BasicTimer>> m_timerRef{};

  std::reference_wrapper<const EngineLogo> m_engineLogo;

  static inline StripHUDProperties m_HUDStripProps{};
  static inline DetailedHUDProperties m_HUDDetailedProps{};

  mutable std::vector<std::unique_ptr<HUDSystem>> m_HUDSystems{};

  float m_renderStatsScrollOffset{};

  std::pair<ProgressBarComponent, HUDLayoutContext> buildStripProgressBar(const FrameSnapshot& frameSnapshot, const float stripTop, const float stripHeight, const float stripWidth) const;

  std::pair<TimerComponent, HUDLayoutContext> buildStripTimer(const float stripTop, const float stripHeight, const float stripWidth) const;

  void drawHUDRect(sf::RenderTarget& renderTarget, sf::Vector2f position, sf::Vector2f size, sf::Color color) const;
  sf::FloatRect getTextBounds(std::string_view text, unsigned characterSize) const;

  void drawDetailedHUDBackplate(sf::RenderTarget& target) const;
  void drawStripHUDBackplate(sf::RenderTarget& target) const;

  std::string composeBuildInfoHeader() const;
  std::string composeBuildInfoSubHeader() const;

  sf::Text getGenericTimerSFMLTextObject() const noexcept;
  
  std::unique_ptr<HUDComponent> buildDetailedProgressBarComponent(const FrameSnapshot& frame) const;
  std::unique_ptr<HUDComponent> buildElapsedTimerComponent(const FrameSnapshot& frame) const;

  void buildHUDSystems(const EngineLogo& engineLogo, const FrameSnapshot& frameSnapshot) const;
  std::vector<std::unique_ptr<HUDComponent>> buildHeaderComponents(const FrameSnapshot& frameSnapshot, const EngineLogo& logo, std::u32string_view glyph, float scale) const;
  std::vector<std::unique_ptr<HUDComponent>> buildRenderConfigComponents() const;
  std::vector<std::unique_ptr<HUDComponent>> buildRenderStatsComponents(const RenderStats& rs, bool isRenderComplete) const;

};

SFMLHUD::SFMLHUD(const EngineBuildInformation& buildInformation, const RenderSystemStrings& renderStrings, const sf::Font& HUDFont, const BasicTimer& basicTimer, const EngineLogo& engineLogo) noexcept : m_buildInformation{ buildInformation }, m_renderStrings{ renderStrings }, m_HUDFont{ HUDFont }, m_timerRef{ std::cref(basicTimer) }, m_engineLogo{ engineLogo } {}

void SFMLHUD::drawStripHUD(sf::RenderTarget& target, const FrameSnapshot& frameSnapshot) const
{
  drawStripHUDBackplate(target);
  drawStripHUDComponents(target, frameSnapshot);
}

void SFMLHUD::drawStripHUDComponents(sf::RenderTarget& target, const FrameSnapshot& frameSnapshot) const
{
  const float stripTop{ static_cast<float>(target.getSize().y) - m_HUDStripProps.kStripLength };
  const float stripHeight{ m_HUDStripProps.kStripLength };
  const float stripWidth{ static_cast<float>(target.getSize().x) };

  const auto [barComponent, barCtx] { buildStripProgressBar(frameSnapshot, stripTop, stripHeight, stripWidth) };

  const auto [timerComponent, timerCtx] { buildStripTimer(stripTop, stripHeight, stripWidth) };

  const std::vector<std::pair<const HUDComponent*, HUDLayoutContext>> components
  {
    { &barComponent,   barCtx },
    { &timerComponent, timerCtx }
  };

  for (const auto& [componentPtr, ctx] : components)
  {
    componentPtr->draw(target, ctx);
  }
}

void SFMLHUD::drawStripHUDBackplate(sf::RenderTarget& target) const
{
  const float w{ static_cast<float>(target.getSize().x) };
  const float h{ static_cast<float>(target.getSize().y) };
  
  const float top{ h - m_HUDStripProps.kStripLength };

  drawHUDRect(target, { 0.f, top }, { w, m_HUDStripProps.kStripLength }, m_HUDStripProps.kBackplateColor);
}

void SFMLHUD::drawDetailedHUD(sf::RenderTarget& target, const FrameSnapshot& frame) const
{
  drawDetailedHUDBackplate(target);

  buildHUDSystems(m_engineLogo.get(), frame);

  drawDetailedHUDSystems(target);
}

void SFMLHUD::drawDetailedHUDSystems(sf::RenderTarget& target) const
{
  const HUDBackplateProperties bp{ target };

  float y{ bp.kBackplatePos.y + (GenericPadding::kGenericPadding * m_HUDDetailedProps.kLogoScale) };

  for (const auto& sys : m_HUDSystems)
  {
    const float centerX{ bp.kBackplatePos.x + 0.5f * bp.kBackplateSize.x };
    const float leftX{ bp.kBackplatePos.x + GenericPadding::kGenericPadding };

    HUDLayoutContext ctx
    {
      { centerX, y },
      bp.kBackplateSize.x - 2.f * GenericPadding::kGenericPadding,
      m_HUDFont,
      leftX
    };

    y += sys->drawSystem(target, ctx) + GenericPadding::kGenericPadding;
  }
}

void SFMLHUD::onMouseWheelScrolled(const sf::Event::MouseWheelScrollEvent& e) noexcept
{
  constexpr float kScrollStep{ 24.0f };
  m_renderStatsScrollOffset += -e.delta * kScrollStep;
}

void SFMLHUD::drawDetailedHUDBackplate(sf::RenderTarget& target) const
{
  const HUDBackplateProperties bp{ target };
  drawHUDRect(target, bp.kBackplatePos, bp.kBackplateSize, bp.kBackplateColor);
}

void SFMLHUD::buildHUDSystems(const EngineLogo& engineLogo, const FrameSnapshot& frameSnapshot) const
{
  m_HUDSystems.clear();

  const std::u32string_view sigilGlyph{ U"\uE000" };
  const float logoScale{ m_HUDDetailedProps.kLogoScale };

  m_HUDSystems.emplace_back(std::make_unique<HeaderSystem>(buildHeaderComponents(frameSnapshot, engineLogo, sigilGlyph, logoScale)));

  m_HUDSystems.emplace_back(std::make_unique<RenderConfigSystem>(buildRenderConfigComponents()));

  const bool hasRenderStatsObj{ frameSnapshot.renderStats.has_value() };

  const RenderStats rs{ hasRenderStatsObj ? frameSnapshot.renderStats.value() : RenderStats{} };

  const bool isRenderComplete{ frameSnapshot.progressUnitNormalized >= 1.0f };

  auto stats{ std::make_unique<RenderStatsSystem>(buildRenderStatsComponents(rs, isRenderComplete)) };

  stats->applyScrollDelta(m_renderStatsScrollOffset);
  
  m_HUDSystems.emplace_back(std::move(stats));
}

std::pair<ProgressBarComponent, HUDLayoutContext> SFMLHUD::buildStripProgressBar(const FrameSnapshot& frameSnapshot, const float stripTop, const float stripHeight, const float stripWidth) const
{
  sf::Text timerMeasure{ getGenericTimerSFMLTextObject() };
  const float padding{ m_HUDStripProps.kStripGenericMargin };

  const sf::FloatRect timerBounds{ timerMeasure.getLocalBounds() };

  const float timerWidth{ timerBounds.width - timerBounds.left };
  const float barWidth{ std::max(0.0f, stripWidth - (2.0f * m_HUDStripProps.kStripGenericMargin) - timerWidth - m_HUDStripProps.kStripGenericMargin) };
  const float barY{ stripTop + (stripHeight - m_HUDStripProps.kProgBarLength) * 0.5f };

  HUDLayoutContext ctx{ { padding, barY }, barWidth, m_HUDFont };

  const float progress{ clampUnit(frameSnapshot.progressUnitNormalized) };
  const int percentInt{ static_cast<int>(progress * 100.0f + 0.5f) };
  const std::string percentText{ std::to_string(percentInt) + "%" };

  const ProgressBarStyle style
  {
    m_HUDStripProps.kProgBarLength,
    m_HUDStripProps.kProgBarBGColor,
    m_HUDStripProps.kProgBarMainColor,
    sf::Color::White,
    m_HUDStripProps.kTextFontSize
  };

  return { ProgressBarComponent{ progress, style, percentText, 0.f }, ctx };
}

std::pair<TimerComponent, HUDLayoutContext> SFMLHUD::buildStripTimer(const float stripTop, const float stripHeight, const float stripWidth) const
{
  sf::Text timerMeasure{ getGenericTimerSFMLTextObject() };
 
  const sf::FloatRect timerBounds{ timerMeasure.getLocalBounds() };
  
  const float timerWidth{ timerBounds.width - timerBounds.left };
  const float timerY{ stripTop + (stripHeight - timerBounds.height) * 0.5f };

  HUDLayoutContext ctx{ { stripWidth - timerWidth - m_HUDStripProps.kStripGenericMargin, timerY }, timerWidth, m_HUDFont };

  const TimerStyle style{ sf::Color::White, m_HUDStripProps.kTextFontSize };
  const std::string timerText{ formatElapsedTime(m_timerRef->get().getSeconds()) };

  return { TimerComponent{ timerText, style }, ctx };
}

void SFMLHUD::drawHUDRect(sf::RenderTarget& renderTarget, sf::Vector2f position, sf::Vector2f size, sf::Color color) const
{
  sf::RectangleShape rect(size);

  rect.setPosition(position);
  rect.setFillColor(color);
  renderTarget.draw(rect);
}

sf::FloatRect SFMLHUD::getTextBounds(std::string_view text, unsigned characterSize) const
{
  sf::Text t{};
  t.setFont(m_HUDFont);
  t.setString(std::string{ text });
  t.setCharacterSize(characterSize);

  return t.getLocalBounds();
}

std::string SFMLHUD::composeBuildInfoHeader() const
{
  std::string s{ m_buildInformation.engineName.empty() ? "indus" : std::string(m_buildInformation.engineName) };

  if (!m_buildInformation.engineVersion.empty())
    s += " " + std::string(m_buildInformation.engineVersion);

  s += "  |  build: " + (m_buildInformation.buildConfig.empty() ? std::string("--") : std::string(m_buildInformation.buildConfig));

  s += "  |  threads: " + std::to_string(m_buildInformation.runtimeThreads);

  return s;
}

std::string SFMLHUD::composeBuildInfoSubHeader() const
{
  std::string s;

  if (!m_buildInformation.gitHash.empty())
    s += "git: " + std::string(m_buildInformation.gitHash);

  if (!m_buildInformation.buildTimestamp.empty())
  {
    if (!s.empty()) s += "  |  ";
    s += "built: " + std::string(m_buildInformation.buildTimestamp);
  }

  if (!m_buildInformation.author.empty())
  {
    if (!s.empty()) s += "  |  ";
    s += "author: " + std::string(m_buildInformation.author);
  }

  return s;
}

sf::Text SFMLHUD::getGenericTimerSFMLTextObject() const noexcept
{
  sf::Text localTimerMeasureTextObj{};

  localTimerMeasureTextObj.setFont(m_HUDFont);
  localTimerMeasureTextObj.setString("88:88:88");
  localTimerMeasureTextObj.setCharacterSize(m_HUDStripProps.kTextFontSize);

  return localTimerMeasureTextObj;
}

std::vector<std::unique_ptr<HUDComponent>> SFMLHUD::buildHeaderComponents(const FrameSnapshot& frameSnapshot, const EngineLogo& logo, std::u32string_view glyph, float scale) const
{
  std::vector<std::unique_ptr<HUDComponent>> comps{};

  comps.push_back(std::make_unique<EngineLogoComponent>(logo, glyph, scale, GenericPadding::kGenericPadding));
  
  comps.push_back(std::make_unique<TextComponent>(composeBuildInfoHeader(), m_HUDDetailedProps.kHUDMainHeaderFontSize, m_HUDDetailedProps.kFontColor, TextAnchor::Left, (GenericPadding::kGenericPadding * 0.5f)));
  
  comps.push_back(std::make_unique<TextComponent>(composeBuildInfoSubHeader(), m_HUDDetailedProps.kHUDMainSubHeaderFontSize, m_HUDDetailedProps.kFontColor, TextAnchor::Left, (GenericPadding::kGenericPadding * 0.5f)));

  comps.push_back(buildDetailedProgressBarComponent(frameSnapshot));

  comps.push_back(buildElapsedTimerComponent(frameSnapshot));

  return comps;
}

std::vector<std::unique_ptr<HUDComponent>> SFMLHUD::buildRenderConfigComponents() const
{
  std::vector<std::unique_ptr<HUDComponent>> comps{};

  const float padTitle{ GenericPadding::kGenericPadding * 0.6f };
  const float padRow{ GenericPadding::kGenericPadding * 0.35f };

  const auto titleCol{ m_HUDDetailedProps.kFontColor };
  const auto itemCol{ m_HUDDetailedProps.kFontColor };

  comps.push_back(std::make_unique<TextComponent>("RENDER CONFIG", m_HUDDetailedProps.kHUDMainHeaderFontSize, titleCol, TextAnchor::Left, padTitle));

  comps.push_back(std::make_unique<MultiTextRowComponent>(std::vector<std::string>{ "Scheduler: " + m_renderStrings.scheduler, "Transport: " + m_renderStrings.transport, "Accelerator: " + m_renderStrings.accelerator }, m_HUDDetailedProps.kHUDMainSubHeaderFontSize, itemCol, padRow));

  comps.push_back(std::make_unique<MultiTextRowComponent>(std::vector<std::string>{ "Sampler: " + m_renderStrings.sampler, "RNG: " + m_renderStrings.rng }, m_HUDDetailedProps.kHUDMainSubHeaderFontSize, itemCol, padRow));

  comps.push_back(std::make_unique<MultiTextRowComponent>(std::vector<std::string>{ "Camera: " + m_renderStrings.camera, "Film: " + m_renderStrings.film, "Filter: " + m_renderStrings.filter }, m_HUDDetailedProps.kHUDMainSubHeaderFontSize, itemCol, padRow));

  return comps;
}

std::unique_ptr<HUDComponent> SFMLHUD::buildDetailedProgressBarComponent(const FrameSnapshot& frame) const
{
  const float p{ clampUnit(frame.progressUnitNormalized) };

  const int pctInt{ static_cast<int>(p * 100.f + 0.5f) };
  const std::string pctText{ std::to_string(pctInt) + "%" };

  const ProgressBarStyle style{ m_HUDStripProps.kProgBarLength, m_HUDStripProps.kProgBarBGColor, m_HUDStripProps.kProgBarMainColor, sf::Color::White, m_HUDStripProps.kTextFontSize };

  return std::make_unique<ProgressBarComponent>(p, style, pctText, GenericPadding::kGenericPadding * 0.5f);
}

std::unique_ptr<HUDComponent> SFMLHUD::buildElapsedTimerComponent(const FrameSnapshot& frame) const
{
  const bool done{ frame.progressUnitNormalized >= 1.f };

  const std::string prefix{ done ? "Total time: " : "Elapsed time: " };
  const std::string t{ prefix + formatElapsedTime(m_timerRef->get().getSeconds()) };

  return std::make_unique<TextComponent>(t, m_HUDDetailedProps.kHUDMainSubHeaderFontSize, m_HUDDetailedProps.kFontColor, TextAnchor::Left, GenericPadding::kGenericPadding * 0.5f);
}

std::vector<std::unique_ptr<HUDComponent>> SFMLHUD::buildRenderStatsComponents(const RenderStats& stats, bool isRenderComplete) const
{
  std::vector<std::unique_ptr<HUDComponent>> components{};

  const sf::Color textColor{ m_HUDDetailedProps.kFontColor };
  const unsigned sectionHeaderSize{ m_HUDDetailedProps.kSectionHeaderFontSize };
  const unsigned entryFontSize{ m_HUDDetailedProps.kBaseFontSize };

  auto makeRowText{ [](const KeyValueRow& row) { return row.label + ": " + row.value; } };

  components.push_back(std::make_unique<TextComponent>("RENDER STATS", sectionHeaderSize, textColor, TextAnchor::Left, GenericPadding::kGenericPadding));

  const std::vector<SubSection> sections{ StatsBuilder::buildSubsections(stats, isRenderComplete) };

  for (const SubSection& section : sections)
  {
    components.push_back(std::make_unique<TextComponent>(section.title, sectionHeaderSize, textColor, TextAnchor::Left, GenericPadding::kGenericPadding * 0.75f));

    for (std::size_t i{}; i < section.rows.size(); ++i)
    {
      const bool isLastRow{ (i + 1) == section.rows.size() };
      const float padAfterRow{ isLastRow ? GenericPadding::kGenericPadding * 1.25f : GenericPadding::kGenericPadding * 0.50f };

      components.push_back(std::make_unique<TextComponent>(makeRowText(section.rows[i]), entryFontSize, textColor, TextAnchor::Left, padAfterRow));
    }
  }
  return components;
}