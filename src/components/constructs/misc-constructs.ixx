export module miscconstructs;

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

  struct BVHFrameStats 
  {
    std::uint64_t primaryRays{};
    std::uint64_t nodesVisited{};
    std::uint64_t boxTests{};
    std::uint64_t leavesVisited{};
    std::uint64_t primitiveTests{};
  };
}

