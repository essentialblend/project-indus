export module sfmlsink;

import <SFML/Graphics.hpp>;

import std;
import displaysink;
import types;

export class SFMLDisplaySink final : public DisplaySink
{
public:
  explicit SFMLDisplaySink(Point2i pixelResolution);

  virtual void present(const DisplayFrame& frame) override;

  virtual bool isSinkOpen() const noexcept override;

private:
  sf::RenderWindow m_mainWindow{};
  sf::Texture m_texture{};
  sf::Sprite m_textureSprite{};

  sf::View m_HUDView{};

  void drawHUD(const DisplayFrame& frame);
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

void SFMLDisplaySink::present(const DisplayFrame& frame)
{
  sf::Event event{};

  while (m_mainWindow.pollEvent(event))
  {
    if (event.type == sf::Event::Closed) m_mainWindow.close();

    if (event.type == sf::Event::Resized)
    {
      m_HUDView.reset(sf::FloatRect(0.0f, 0.0f, static_cast<float>(event.size.width), static_cast<float>(event.size.height)));
    }
  }

  if(frame.doUpload)
  {
    m_texture.update(frame.rgbaPixels.data());
  }
  else if (!m_textureSprite.getTexture())
  {
    std::vector<sf::Uint8> blank(frame.rgbaPixels.size(), 32);
    m_texture.update(blank.data());
  }
  
  m_mainWindow.clear(sf::Color::Black);
  m_mainWindow.setView(m_mainWindow.getDefaultView());
  m_mainWindow.draw(m_textureSprite);
  
  m_mainWindow.setView(m_HUDView);
  drawHUD(frame);

  m_mainWindow.display();
}

bool SFMLDisplaySink::isSinkOpen() const noexcept
{
  return m_mainWindow.isOpen();
}

void SFMLDisplaySink::drawHUD(const DisplayFrame& frame)
{
  const auto ws{ m_mainWindow.getSize() };

  const float pad{ 12 };
  
  const float w{ std::max(0.f, float(ws.x) - 2.f * pad) };
  const float h{ 8.0 };
  const float f{ std::clamp(frame.progressUnitNormalized, 0.0f, 1.0f) };

  sf::RectangleShape back({ w, h });
  back.setPosition(pad, pad);
  back.setFillColor(sf::Color(0, 0, 0, 140));

  sf::RectangleShape fill({ w * f, h });
  fill.setPosition(pad, pad);
  fill.setFillColor(sf::Color(255, 255, 255, 220));

  m_mainWindow.draw(back);
  m_mainWindow.draw(fill);
}
