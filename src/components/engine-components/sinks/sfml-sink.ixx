export module sfmlsink;

import <SFML/Graphics.hpp>;

import std;
import displaysinkbase;
import types;
import engineconstructs;
import image;
import basictimer;
import miscutil;
import sfmlhud;
import bvhaggregate;
import enginesigil;
import enginelogo;

export class SFMLDisplaySink final : public DisplaySinkBase
{
public:
  explicit SFMLDisplaySink(Point2i pixelResolution, const BasicTimer& basicTimer, const EngineBuildInformation& buildInfo, const ImmutableEngineSystems& immutableEngineSystems, const DisplaySinkConfig& displaySinkCfg);

  virtual void update(const FrameSnapshot& frame) override;
  virtual void present() override;

  virtual bool isSinkOpen() const noexcept override;

  [[nodiscard]] std::string toString() const override;

private:
  sf::RenderWindow m_mainWindow{};
  sf::Texture m_texture{};
  sf::Sprite m_textureSprite{};

  sf::Font m_HUDFont{};
  sf::View m_HUDView{};
  bool m_isHUDVisible{ true };
  HUDMode m_HUDMode{ HUDMode::Strip };
  std::optional<SFMLHUD> m_SFMLHUD{};

  FrameSnapshot m_lastFrameSnapshot{};

  void setupSFMLObjects(Point2i& pixelResolution);
  void setupSFMLHUD(const ImmutableEngineSystems& immutableEngineSystems, const EngineBuildInformation& buildInfo, const BasicTimer& basicTimer);
  void handleSFMLEvents();
  void drawHUD();
  void drawRenderedImage();
};

SFMLDisplaySink::SFMLDisplaySink(Point2i pixelResolution, const BasicTimer& basicTimer, const EngineBuildInformation& buildInfo, const ImmutableEngineSystems& immutableEngineSystems, const DisplaySinkConfig& displaySinkCfg) : DisplaySinkBase{ std::cref(basicTimer), std::cref(buildInfo), std::cref(immutableEngineSystems) }, m_mainWindow{ sf::VideoMode{ unsigned(displaySinkCfg.windowResolution[0]), unsigned(displaySinkCfg.windowResolution[1]) }, "indus-preview" }
{
  setupSFMLObjects(pixelResolution);
  setupSFMLHUD(immutableEngineSystems, buildInfo, basicTimer);
}

void SFMLDisplaySink::setupSFMLHUD(const ImmutableEngineSystems& immutableEngineSystems, const EngineBuildInformation& buildInfo, const BasicTimer& basicTimer)
{
  std::string accelerationAlgorithmStr{"N.A"};

  RenderSystemStrings renderSystemStrings
  {
    immutableEngineSystems.m_integrator.getSchedulerString(),
    immutableEngineSystems.m_sampler.toString(),    
    immutableEngineSystems.m_sampler.getRNG().toString(),
    immutableEngineSystems.m_integrator.toString(),
    immutableEngineSystems.m_renderScene.getSceneRoot()->toString(),
    immutableEngineSystems.m_camera.toString(),
    immutableEngineSystems.m_film.toString(),
    immutableEngineSystems.m_film.getFilter().toString(),
    toString()
  };

  static sf::Font glyphFont;  
  static sf::Font logoFont;   
  
  glyphFont.loadFromFile("dep/fonts/indus-logo.otf");
  logoFont.loadFromFile("dep/fonts/Babylonica-Regular.ttf");

  static EngineSigil engineSigil{ glyphFont };
  static EngineLogo engineLogo{ engineSigil, logoFont };

  m_SFMLHUD.emplace(buildInfo, renderSystemStrings, m_HUDFont, basicTimer, engineLogo);
}

void SFMLDisplaySink::setupSFMLObjects(Point2i& pixelResolution)
{
  m_mainWindow.setFramerateLimit(30);

  m_texture.create(static_cast<unsigned>(pixelResolution[0]), static_cast<unsigned>(pixelResolution[1]));

  m_textureSprite.setTexture(m_texture, true);

  std::vector<sf::Uint8> blank(
    static_cast<std::size_t>(pixelResolution[0]) *
    static_cast<std::size_t>(pixelResolution[1]) * 4u,
    32
  );

  for (std::size_t i{}; i < blank.size(); i += 4)
  {
    blank[i + 0] = 16;
    blank[i + 1] = 16;
    blank[i + 2] = 16;
    blank[i + 3] = 255;
  }

  m_texture.update(blank.data());

  m_HUDView = m_mainWindow.getDefaultView();

  if (!m_HUDFont.loadFromFile("dep/fonts/JetBrainsMono-Regular.ttf"))
  {
    throw std::runtime_error("HUD font not found");
  }
}

void SFMLDisplaySink::update(const FrameSnapshot& frameSnapshot) 
{
  const Image& img{ frameSnapshot.image };

  const bool hasImage{ img.getPixelFormat() == PixelFormat::U8 && img.getNumChannels() == 4 && !img.getP8().empty() };

  if (hasImage) 
  {
    const auto res{ img.getImagePixelResolution() };

    if (m_texture.getSize().x != unsigned(res[0]) || m_texture.getSize().y != unsigned(res[1])) 
    {
      m_texture.create(unsigned(res[0]), unsigned(res[1]));
      m_textureSprite.setTexture(m_texture, true);
    }

    m_texture.update(reinterpret_cast<const sf::Uint8*>(img.getP8().data()));
  }

  m_lastFrameSnapshot = frameSnapshot;
}

void SFMLDisplaySink::present()
{
  handleSFMLEvents();
  
  drawRenderedImage();
  drawHUD();

  m_mainWindow.display();
}

void SFMLDisplaySink::drawRenderedImage()
{
  const auto mainWinSize{ m_mainWindow.getSize() };
  m_mainWindow.clear(sf::Color::Black);

  m_mainWindow.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(mainWinSize.x), static_cast<float>(mainWinSize.y))));

  m_textureSprite.setScale(static_cast<float>(m_mainWindow.getSize().x) / static_cast<float>(m_texture.getSize().x), static_cast<float>(m_mainWindow.getSize().y) / static_cast<float>(m_texture.getSize().y));

  m_mainWindow.draw(m_textureSprite);
}

void SFMLDisplaySink::drawHUD()
{
  const auto mainWinSize{ m_mainWindow.getSize() };

  const bool canShowDetailed{ mainWinSize.y >= 420 };
  const HUDMode effective{ (m_HUDMode == HUDMode::Detailed && !canShowDetailed) ? HUDMode::Strip : m_HUDMode };

  // Rebuild HUD view every frame
  m_HUDView.reset(sf::FloatRect(0, 0, static_cast<float>(mainWinSize.x), static_cast<float>(mainWinSize.y)));
  m_HUDView.setViewport(sf::FloatRect(0, 0, 1, 1));
  m_mainWindow.setView(m_HUDView);

  if (effective != HUDMode::Hidden && m_SFMLHUD)
  {
    if (effective == HUDMode::Strip)
      m_SFMLHUD->drawStripHUD(m_mainWindow, m_lastFrameSnapshot);
    else
      m_SFMLHUD->drawDetailedHUD(m_mainWindow, m_lastFrameSnapshot);
  } 
}

void SFMLDisplaySink::handleSFMLEvents()
{
  sf::Event e{};
  const bool isResBigEnoughForDetailedHUD{ m_mainWindow.getSize().y >= 420 };

  while (m_mainWindow.pollEvent(e))
  {
    if (e.type == sf::Event::Closed) m_mainWindow.close();

    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F1)
    {
      if (isResBigEnoughForDetailedHUD)
      {
        if (m_HUDMode == HUDMode::Hidden) m_HUDMode = HUDMode::Strip;
        else if (m_HUDMode == HUDMode::Strip) m_HUDMode = HUDMode::Detailed;
        else m_HUDMode = HUDMode::Hidden;
      }
      else m_HUDMode = (m_HUDMode == HUDMode::Hidden) ? HUDMode::Strip : HUDMode::Hidden;
    }

    if (e.type == sf::Event::MouseWheelScrolled)
    {
      m_SFMLHUD->onMouseWheelScrolled(e.mouseWheelScroll);
    }
  }
}

bool SFMLDisplaySink::isSinkOpen() const noexcept
{
  return m_mainWindow.isOpen();
}

std::string SFMLDisplaySink::toString() const
{
  return "SFML-Window";
}
