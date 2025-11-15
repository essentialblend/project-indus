export module miscconstructs;

import <SFML/Graphics.hpp>;

import std;
import bounds;

export
{
  enum class BVHSplitMethod : std::uint8_t
  {
    SAH, 
    Middle,
    EqualCounts
  };

  struct BVHNode final
  {
    Bounds3f nodeBounds{};
    std::unique_ptr<BVHNode> leftChild{};
    std::unique_ptr<BVHNode> rightChild{};
    int firstPrimitiveOffset{ -1 };
    int primitiveCount{};
    std::uint8_t splitAxis{};
  };

  struct LinearBVHNode final
  {
    Bounds3f nodeBounds{};
    int firstPrimitiveOffset{ -1 };
    int primitiveCount{};
    int secondChildArrIdx{ -1 };
    std::uint8_t splitAxis{};
  };

  //struct HUDDetailedProperties
  //{
  //  HUDDetailedProperties() = delete;

  //  explicit HUDDetailedProperties(sf::RenderTarget& target)
  //  {
  //    const float targetSizeWidth{ static_cast<float>(target.getSize().x) };
  //    const float targetSizeHeight{ static_cast<float>(target.getSize().y) };

  //    detailedHUDBackplateSize = { std::max(0.0f, targetSizeWidth - 2.0f * kBackplateEdgeMargin), std::max(0.0f, targetSizeHeight - 2.0f * kBackplateEdgeMargin) };

  //    //const float headerMax{ detailedHUDBackplateSize.x - 2.0f * kTextPadding };
  //  }

  //  const float kBackplateEdgeMargin{ 32.0f };
  //  const sf::Color kBackplateColor{ 14, 14, 16, 205 };
  //  const sf::Vector2f kBackplatePos{ kBackplateEdgeMargin, kBackplateEdgeMargin };
  //  const float kTextPadding{ 20.0f };
  //  const sf::Vector2f kTargetBounds{};

  //  sf::Vector2f detailedHUDBackplateSize{};

  //  const unsigned kHeaderFontSize{ 15u };
  //  const unsigned kSubHeaderFontSize{ 14u };
  //};
}

