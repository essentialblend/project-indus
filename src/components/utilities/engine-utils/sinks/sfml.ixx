export module sfmlsink;

import <SFML/Graphics.hpp>;

import std;
import displaysink;
import types;
import engineconstructs;
import image;

export class SFMLDisplaySink final : public DisplaySink
{
public:
  explicit SFMLDisplaySink(Point2i pixelResolution);
  
  virtual void update(const FrameSnapshot& frame) override;
  virtual void present() override;

  virtual bool isSinkOpen() const noexcept override;

private:
  sf::RenderWindow m_mainWindow{};
  sf::Texture m_texture{};
  sf::Sprite m_textureSprite{};

  sf::View m_HUDView{};

  //void drawHUD(const DisplayFrame& frame);
};

SFMLDisplaySink::SFMLDisplaySink(Point2i pixelResolution) : m_mainWindow{ sf::VideoMode{ unsigned(pixelResolution[0]), unsigned(pixelResolution[1]) }, "indus-preview" }
{
  m_mainWindow.setFramerateLimit(30);

  m_texture.create(static_cast<unsigned>(pixelResolution[0]), static_cast<unsigned>(pixelResolution[1]));

  m_textureSprite.setTexture(m_texture, true);

  std::vector<sf::Uint8> blank(std::size_t(pixelResolution[0]) * std::size_t(pixelResolution[1]) * 4, 32);

  for (std::size_t i{}; i < blank.size(); i += 4)
  {
    blank[i + 0] = 16;
    blank[i + 1] = 16;
    blank[i + 2] = 16;
    blank[i + 3] = 255;
  }

  m_texture.update(blank.data());

  m_HUDView = m_mainWindow.getDefaultView();
}

void SFMLDisplaySink::update(const FrameSnapshot& frameSnapshot) 
{
  const Image& img{ frameSnapshot.image };
  const auto res{ img.getImagePixelResolution() };

  if (m_texture.getSize().x != unsigned(res[0]) || m_texture.getSize().y != unsigned(res[1])) {
    m_texture.create(unsigned(res[0]), unsigned(res[1]));
    m_textureSprite.setTexture(m_texture, true);
  }
  if (img.getPixelFormat() == PixelFormat::U8 && img.getNumChannels() == 4) 
  {
    m_texture.update(img.getP8().data());
  }
}

void SFMLDisplaySink::present()
{
  sf::Event e{};

  while (m_mainWindow.pollEvent(e)) 
  {
    if (e.type == sf::Event::Closed) m_mainWindow.close();

    if (e.type == sf::Event::Resized) m_HUDView.reset(sf::FloatRect(0.f, 0.f, float(e.size.width), float(e.size.height)));
  }

  m_mainWindow.clear(sf::Color::Black);
  m_mainWindow.setView(m_mainWindow.getDefaultView());
  m_mainWindow.draw(m_textureSprite);
  
  m_mainWindow.display();
}

bool SFMLDisplaySink::isSinkOpen() const noexcept
{
  return m_mainWindow.isOpen();
}
