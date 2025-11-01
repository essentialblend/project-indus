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
};

SFMLDisplaySink::SFMLDisplaySink(Point2i pixelResolution) : m_mainWindow{ sf::VideoMode{ unsigned(pixelResolution[0]), unsigned(pixelResolution[1]) }, "indus-preview" }
{
  m_mainWindow.setFramerateLimit(30);

  m_texture.create(static_cast<unsigned>(pixelResolution[0]), static_cast<unsigned>(pixelResolution[1]));

  m_textureSprite.setTexture(m_texture, true);
}

void SFMLDisplaySink::present(const DisplayFrame& frame)
{
  sf::Event event{};

  while (m_mainWindow.pollEvent(event))
  {
    if (event.type == sf::Event::Closed) m_mainWindow.close();
  }

  m_texture.update(frame.rgbaPixels.data());
  m_mainWindow.clear(sf::Color::Black);
  
  m_mainWindow.draw(m_textureSprite);
  m_mainWindow.display();

}

bool SFMLDisplaySink::isSinkOpen() const noexcept
{
  return m_mainWindow.isOpen();
}
