export module miscconstructs;

import <SFML/Graphics.hpp>;

import std;
import bounds;

export
{
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
}

