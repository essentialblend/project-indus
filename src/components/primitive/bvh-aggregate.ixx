export module bvhaggregate;

import std;
import primitive;
import intersectionconstructs;
import miscconstructs;
import bounds;
import mathfp;

struct BVHBuildPrimitive final
{
  Bounds3f bounds{};
  Point3f centroid{};
  std::shared_ptr<Primitive> primitive{};
};

export class BVHAggregate final : public Primitive
{
public:
  BVHAggregate(std::vector<std::shared_ptr<Primitive>>, int maxPrimsInNode, BVHSplitMethod splitMethod) noexcept;

  [[nodiscard]] Bounds3f getBounds() const noexcept override;
  [[nodiscard]] BVHFrameStats getStats() const noexcept;

  [[nodiscard]] std::optional<ShapeIntersection> intersect(const Ray&) const override;

  [[nodiscard]] bool intersectP(const Ray&) const override;

  void printBVHStats(int W, int H, int spp, double msTotal) const noexcept;

private:
  std::unique_ptr<BVHNode> m_root{};
  std::vector<std::shared_ptr<Primitive>> m_primitives{};
  std::vector<std::shared_ptr<Primitive>> m_ordered{};
  int m_maxPrimsInNode{};
  BVHSplitMethod m_splitMethod{};
  std::vector<LinearBVHNode> m_linearNodes{};
  mutable BVHFrameStats m_BVHFrameStats{};

  int flattenBVHTree(const BVHNode& node) noexcept;
  std::unique_ptr<BVHNode> buildRecursive(std::vector<BVHBuildPrimitive>& bps, int begin, int end);

  std::unique_ptr<BVHNode> splitBySAH(std::unique_ptr<BVHNode> node, Bounds3f& centroidBounds, const int dim, const int totalPrimitivesInBP, int begin, std::vector<BVHBuildPrimitive>& buildPrimitives, int end);

  std::unique_ptr<BVHNode> splitByMiddle(Bounds3f& centroidBounds, const int dim, std::vector<BVHBuildPrimitive>& buildPrimitives, int begin, int end, const int totalPrimitivesInBP, std::unique_ptr<BVHNode> node);

  std::unique_ptr<BVHNode> splitByEqualCounts(int begin, const int totalPrimitivesInBP, std::vector<BVHBuildPrimitive>& buildPrimitives, int end, const int dim, std::unique_ptr<BVHNode> node);
  
  template<typename FLeaf>
  bool traverseBVH(const Ray& ray, FLeaf&& leafFunc) const;
};

BVHAggregate::BVHAggregate(std::vector<std::shared_ptr<Primitive>> primitives, int maxPrimsInNode, BVHSplitMethod splitMethod) noexcept
  : m_primitives{ std::move(primitives) }, m_maxPrimsInNode{ maxPrimsInNode }, m_splitMethod{ splitMethod }
{
  if (m_primitives.empty()) return;

  std::vector<BVHBuildPrimitive> BVHBuildPrimitives; 
  BVHBuildPrimitives.reserve(int(m_primitives.size()));

  for (const auto& primitive : m_primitives) 
  { 
    Bounds3f primitiveBound{ primitive->getBounds() };

    Point3f centroidPoint3f{ (primitiveBound.getMin()[0] + primitiveBound.getMax()[0]) * Float{ 0.5 }, (primitiveBound.getMin()[1] + primitiveBound.getMax()[1]) * Float{ 0.5 }, (primitiveBound.getMin()[2] + primitiveBound.getMax()[2]) * Float{ 0.5 } };

    BVHBuildPrimitives.push_back({ primitiveBound, centroidPoint3f, primitive });
  }

  m_ordered.clear();

  m_root = buildRecursive(BVHBuildPrimitives, 0, int(BVHBuildPrimitives.size()));

  m_linearNodes.clear();
  m_linearNodes.reserve(std::size_t{ 2 * m_ordered.size() });
  
  flattenBVHTree(*m_root);
}

template<typename FLeaf>
bool BVHAggregate::traverseBVH(const Ray& ray, FLeaf&& leafFunc) const
{
  if (m_linearNodes.empty())
  {
    ++m_BVHFrameStats.primaryRays;
    return false;
  }

  // Get ray's cached negative direction flags 
  Ray rayLocal{ ray };
  const auto& dirIsNeg{ rayLocal.getDirIsNeg() };

  // Setup a simple manual stack where we store 64 nodes to visit (nodes of a tree sorted in memory typically have a depth < 64)
  int nodesToVisit[64]{};
  int toVisitOffset{ 0 };
  int current{ 0 };

  // Traverse the flattened tree by DFS
  while (true)
  {
    // Get mutable current node
    const LinearBVHNode& currLinearNode{ m_linearNodes[static_cast<Idx>(current)] };
    const auto optHit{ currLinearNode.nodeBounds.intersectPRange(rayLocal) };

    ++m_BVHFrameStats.boxTests;

    // If we intersect the node's bounds, check whether it's a leaf node or an interior node
    if (optHit)
    {
      ++m_BVHFrameStats.nodesVisited;

      // If it's a leaf node, check its primitives one by one for intersection
      if (currLinearNode.primitiveCount > 0)
      {
        ++m_BVHFrameStats.leavesVisited;

        // If arg-passed leaf function returns true, we have an intersection and can exit
        if (leafFunc(currLinearNode, rayLocal))
        {
          ++m_BVHFrameStats.primaryRays;
          return true;
        }
        
        if (toVisitOffset == 0) break;

        // Pop next node to visit off the stack
        current = nodesToVisit[--toVisitOffset];
      }
      // If it's an interior node, push the far child onto the stack and continue with the near child based on the ray direction
      else
      {

        if (dirIsNeg[currLinearNode.splitAxis])
        {
          // Add the first child to the stack to visit later
          nodesToVisit[toVisitOffset] = current + 1;
          ++toVisitOffset;
          // Set current to the second child as the ray is in the -ve direction of the split axis
          current = currLinearNode.secondChildArrIdx;
        }
        else
        {
          // Add the second child to the stack to visit later
          nodesToVisit[toVisitOffset] = currLinearNode.secondChildArrIdx;
          ++toVisitOffset;
          // Set current to the first child as the ray is in the +ve direction of the split axis
          current = current + 1;
        }
      }
    }
    // If we don't intersect the node's bounds, pop the next node to visit off the stack
    else
    {
      if (toVisitOffset == 0) break;
      current = nodesToVisit[--toVisitOffset];
    }
  }

  ++m_BVHFrameStats.primaryRays;

  return false;
}

Bounds3f BVHAggregate::getBounds() const noexcept
{
  if (!m_root) return Bounds3f{};
  
  return m_root->nodeBounds;
}

BVHFrameStats BVHAggregate::getStats() const noexcept
{
  return m_BVHFrameStats;
}

bool BVHAggregate::intersectP(const Ray& ray) const
{
  // The lambda simply checks each primitive and returns true, nothing else
  auto leafTestLambda = [&](const LinearBVHNode& node, const Ray& ray)
  {
    for (Idx i{}; i < node.primitiveCount; ++i)
    {
      ++m_BVHFrameStats.primitiveTests;
      if (m_ordered[node.firstPrimitiveOffset + i]->intersectP(ray)) return true;
    }
    return false;
  };

  return traverseBVH(ray, leafTestLambda);
}

std::optional<ShapeIntersection> BVHAggregate::intersect(const Ray& ray) const
{
  std::optional<ShapeIntersection> best{};
  Ray rayLocal{ ray };

  // The lambda checks each primitive and updates the ray's tMax and the best intersection found so far, if any
  auto leafIntersectLambda = [&](const LinearBVHNode& node, Ray& rayRef)
  {
    for (Idx i{}; i < node.primitiveCount; ++i)
    {
      ++m_BVHFrameStats.primitiveTests;

      const auto& prim{ m_ordered[node.firstPrimitiveOffset + i] };
      if (auto optShapeIntersect{ prim->intersect(rayRef) })
      {
        rayRef.setTMax(optShapeIntersect->tHit);
        best = std::move(optShapeIntersect);
      }
    }
    return false;
  };

  traverseBVH(rayLocal, leafIntersectLambda);

  return best;
}

int BVHAggregate::flattenBVHTree(const BVHNode& node) noexcept
{
  // Get current node offset in the linear array and add a new linear node
  const int currNodeOffset{ static_cast<int>(m_linearNodes.size()) };

  m_linearNodes.emplace_back();
  LinearBVHNode& linear{ m_linearNodes[currNodeOffset] };

  // Copy over the node bounds based on the current BVH node and whether it is a leaf or an interior node
  linear.nodeBounds = node.nodeBounds;

  // If it's a leaf node, copy over the primitive info and return
  if (node.primitiveCount > 0)
  {
    linear.firstPrimitiveOffset = node.firstPrimitiveOffset;
    linear.primitiveCount = node.primitiveCount;
    linear.secondChildArrIdx = -1;
    linear.splitAxis = 0;

    return currNodeOffset;
  }

  // Else, it's an interior node, so we recursively flatten the left child first, index the right child and store it in the linear node
  linear.primitiveCount = 0;
  linear.splitAxis = node.splitAxis;

  flattenBVHTree(*node.leftChild);
  const int rightIndex{ flattenBVHTree(*node.rightChild) };

  // Assign the right child index in the linear node
  linear.secondChildArrIdx = rightIndex;

  return currNodeOffset;
}

std::unique_ptr<BVHNode>
BVHAggregate::buildRecursive(std::vector<BVHBuildPrimitive>& buildPrimitives, int begin, int end)
{
  // Allocate a node on the heap and cache total primitives in the buildPrimitives array
  auto node{ std::make_unique<BVHNode>() };

  const int totalPrimitivesInBP{ end - begin };

  // Compute bounds over [begin,end), expand node bounds to include all primitives
  for (int i{ begin }; i < end; ++i)
    node->nodeBounds = Bounds3f::getUnion(node->nodeBounds, buildPrimitives[i].bounds);

  // If the node has no surface area, create a leaf node and unwind the recursion
  if (isZero(node->nodeBounds.getSurfaceArea())) 
  {
    node->firstPrimitiveOffset = static_cast<int>(m_ordered.size());
    
    node->primitiveCount = totalPrimitivesInBP;
    
    for (int i{ begin }; i < end; ++i) m_ordered.push_back(buildPrimitives[i].primitive);
    
    return node;
  }

  // Compute the centroid bounds over [begin,end), expand centroid bounds
  Bounds3f centroidBounds;
  for (int i{ begin }; i < end; ++i)
    centroidBounds = Bounds3f::getUnion(centroidBounds, Bounds3f{ buildPrimitives[i].centroid, buildPrimitives[i].centroid });

  // Choose axis to split along, we use the longest axis and store this information in the node
  const int dim{ centroidBounds.getMaxDimension() };
  const bool doCentroidsCoincide{ centroidBounds.getMax()[dim] == centroidBounds.getMin()[dim] };

  node->splitAxis = static_cast<std::uint8_t>(dim);

  // If we have few enough primitives or centroids coincide, create a leaf node and unwind the recursion 
  if (totalPrimitivesInBP <= m_maxPrimsInNode || doCentroidsCoincide)
  {
    node->firstPrimitiveOffset = static_cast<int>(m_ordered.size());
    node->primitiveCount = totalPrimitivesInBP;
    
    for (int i{ begin }; i < end; ++i) m_ordered.push_back(buildPrimitives[i].primitive);
    
    return node;
  }

  // Now split the node based on the selected split method. EqualCounts splits based on primitive count, Middle splits based on the geometric midpoint of the centroids, SAH(Surface Area Heuristic) is a cost-based method (TBD)
  switch (m_splitMethod) 
  {
    case BVHSplitMethod::EqualCounts: 
    {
      return splitByEqualCounts(begin, totalPrimitivesInBP, buildPrimitives, end, dim, std::move(node));
    }
    case BVHSplitMethod::Middle: 
    {
      return splitByMiddle(centroidBounds, dim, buildPrimitives, begin, end, totalPrimitivesInBP, std::move(node));
    }
    // SAH is default, HLBVH pending parallelization
    case BVHSplitMethod::SAH:
    default: 
    {
      return splitBySAH(std::move(node), centroidBounds, dim, totalPrimitivesInBP, begin, buildPrimitives, end);
    }
  }
}

std::unique_ptr<BVHNode> BVHAggregate::splitBySAH(std::unique_ptr<BVHNode> node, Bounds3f& centroidBounds, const int dim, const int totalPrimitivesInBP, int begin, std::vector<BVHBuildPrimitive>& buildPrimitives, int end)
{
  // Declare bucket count, more buckets = more traversal
  constexpr int kNumBuckets{ 32 };

  // BucketInfo stores primitive-count in one bucket and a combined/total bounds
  struct BucketInfo { int count{}; Bounds3f bounds{}; };
  // Cache centroid bounds for convenience along the chosen axis
  struct CentroidBoundsFloat { Float min{}; Float max{}; Float span{}; };


  std::array<BucketInfo, kNumBuckets> bucketsArray{};

  // Cache surface areas of the node's bounds and its inverse
  const Float parentArea{ node->nodeBounds.getSurfaceArea() };
  const Float invParentArea{ parentArea > Float{} ? Float{ 1 } / parentArea : Float{} };

  CentroidBoundsFloat centroidBF{ .min{ centroidBounds.getMin()[dim] }, .max{ centroidBounds.getMax()[dim] }, .span{ centroidBounds.getMax()[dim] - centroidBounds.getMin()[dim] } };

  // If total primitives are less than 2, we have a degenerate case and we can simply partition by equal counts
  if (totalPrimitivesInBP <= 2)
  {
    splitByEqualCounts(begin, totalPrimitivesInBP, buildPrimitives, end, dim, std::move(node));
  }

  // We don't have a degenerate case, traverse the buildPrimitives array
  for (int i{ begin }; i < end; ++i)
  {
    // Normalize the primitive, scale it by kNumBuckets and floor. This assigns each primitive to a valid bin/bucket 
    const Float uniPrimitiveVal{ (buildPrimitives[i].centroid[dim] - centroidBF.min) / centroidBF.span };

    int binForUniPrim{ static_cast<int>(kNumBuckets * uniPrimitiveVal) };

    // If any primitive gets assigned kNumBucket'th bin, we simply assign it to the previous bin to stay bounded
    if (binForUniPrim == kNumBuckets) binForUniPrim = kNumBuckets - 1;

    // Incr that bucket's primitive counter, expand the bucket's bounds to encompass the primitive's
    bucketsArray[binForUniPrim].count++;
    bucketsArray[binForUniPrim].bounds = Bounds3f::getUnion(bucketsArray[binForUniPrim].bounds, buildPrimitives[i].bounds);
  }

  // Now we do the prefix span. We go through the buckets left to right, accumulating primitives, their bounds, and their traversal costs
  constexpr int kNumSplits{ kNumBuckets - 1 };
  std::array<Float, kNumSplits> aggrCosts{};

  int totalPrimsInPrefixAccum{};
  Bounds3f prefixAccumBounds{};

  for (int i{}; i < kNumSplits; ++i)
  {
    // For each bucket, accumulate the bounds, primitive counts, and traversal costs
    prefixAccumBounds = Bounds3f::getUnion(prefixAccumBounds, bucketsArray[i].bounds);
    totalPrimsInPrefixAccum += bucketsArray[i].count;
    // This is the cumulative expected intersection work: Num Accum Primitives (N_L) * Accum BB Surface Area (A_L). Pending normalization by SA of the Parent BB (A_P). Same for the suffix variant
    aggrCosts[i] += totalPrimsInPrefixAccum * prefixAccumBounds.getSurfaceArea();
  }

  // The same thing, but in reverse, for the suffix span
  int totalPrimsInSuffixAccum{};
  Bounds3f suffixAccumBounds{};

  for (int i{ kNumSplits }; i >= 1; --i)
  {
    suffixAccumBounds = Bounds3f::getUnion(suffixAccumBounds, bucketsArray[i].bounds);
    totalPrimsInSuffixAccum += bucketsArray[i].count;
    aggrCosts[std::int64_t{ i - 1 }] += totalPrimsInSuffixAccum * suffixAccumBounds.getSurfaceArea();
  }

  // After the two loops, aggrCosts now holds the unnormalized isect work (N_L * A_L) + (NR * AR). Now we simply loop through the buckets and find the best candidate with the lowest trav cost, and cache the bucket index
  int bestSplitBucketIdx{ -1 };
  Float minAggrCost{ infinity<Float> };

  for (int i{}; i < kNumSplits; ++i)
  {
    if (aggrCosts[i] < minAggrCost)
    {
      minAggrCost = aggrCosts[i];
      bestSplitBucketIdx = i;
    }
  }

  // Since we assume C_T / C_I = 0.5, we divide C_split entirely by C_I, giving us C_split = 0.5 + (1 * ((N_L * A_L) + (N_R * A_R)))
  const Float normSplitCost{ Float{ 0.5 } + (minAggrCost * invParentArea) };
  // Since C_leaf = N * C_I, our normalization gets us C_leaf = N
  const Float leafCost{ static_cast<Float>(totalPrimitivesInBP) };

  // If we have more than maxPrimsInNode nodes or the cost isn't greater than the leaf-cost, we partition 
  if (totalPrimitivesInBP > m_maxPrimsInNode || normSplitCost < leafCost)
  {
    auto midIt{ std::partition(buildPrimitives.begin() + begin, buildPrimitives.begin() + end, [&](const BVHBuildPrimitive& primitive)
      {
        // Again, take the primitive's uniform floored value and place it into the correct bucket
        const Float uniPrimVal{ (primitive.centroid[dim] - centroidBF.min) / centroidBF.span };
        int b{ static_cast<int>(uniPrimVal * kNumBuckets) };

        if (b == kNumBuckets) b = kNumBuckets - 1;

        // Simply return whether its index is less than our chosen split bucket, partitioning and splitting our primitives appropriately
        return b <= bestSplitBucketIdx;
      }) };

    // Get the mid index, and recurse on the bipartite
    const int mid{ static_cast<int>(midIt - buildPrimitives.begin()) };

    node->leftChild = buildRecursive(buildPrimitives, begin, mid);
    node->rightChild = buildRecursive(buildPrimitives, mid, end);

    return node;
  }

  // Now the node is a leaf, it must be at the tail end of the ordered primitives array. Then, return the node after adding all the leaf-primitives, store its size
  node->firstPrimitiveOffset = static_cast<int>(m_ordered.size());
  node->primitiveCount = totalPrimitivesInBP;
  
  for (int i{ begin }; i < end; ++i) 
    m_ordered.push_back(buildPrimitives[i].primitive);

  return node;
}

std::unique_ptr<BVHNode> BVHAggregate::splitByMiddle(Bounds3f& centroidBounds, const int dim, std::vector<BVHBuildPrimitive>& buildPrimitives, int begin, int end, const int totalPrimitivesInBP, std::unique_ptr<BVHNode> node)
{
  // Split at the geometric midpoint of the centroid bounds along the selected dimension
  const Float splitPos{ (centroidBounds.getMin()[dim] + centroidBounds.getMax()[dim]) * Float { 0.5 } };

  // Partition the build primitives based on the split position
  auto midIt{ std::partition(buildPrimitives.begin() + begin, buildPrimitives.begin() + end, [&](const BVHBuildPrimitive& p) { return p.centroid[dim] < splitPos; }) };

  // Compute the mid index from the partitioned iterator
  int mid{ static_cast<int>(midIt - buildPrimitives.begin()) };

  // If we get a bad split (all on one side), just do an equal counts split
  if (mid == begin || mid == end)
  {
    mid = begin + totalPrimitivesInBP / 2;

    std::nth_element(buildPrimitives.begin() + begin, buildPrimitives.begin() + mid, buildPrimitives.begin() + end, [dim](const BVHBuildPrimitive& a, const BVHBuildPrimitive& b) { return a.centroid[dim] < b.centroid[dim]; });
  }

  // Else, recursively build left and right child nodes as usual
  node->leftChild = buildRecursive(buildPrimitives, begin, mid);
  node->rightChild = buildRecursive(buildPrimitives, mid, end);

  return node;
}

std::unique_ptr<BVHNode> BVHAggregate::splitByEqualCounts(int begin, const int totalPrimitivesInBP, std::vector<BVHBuildPrimitive>& buildPrimitives, int end, const int dim, std::unique_ptr<BVHNode> node)
{
  // Split in the middle of the range of primitives
  const int mid{ begin + totalPrimitivesInBP / 2 };

  // Partition the build primitives based on the midpoint along the selected dimension
  std::nth_element(buildPrimitives.begin() + begin, buildPrimitives.begin() + mid, buildPrimitives.begin() + end,
    [dim](const BVHBuildPrimitive& a, const BVHBuildPrimitive& b) {
      return a.centroid[dim] < b.centroid[dim];
    });

  // Recursively build left and right child nodes
  node->leftChild = buildRecursive(buildPrimitives, begin, mid);
  node->rightChild = buildRecursive(buildPrimitives, mid, end);

  return node;
}

void BVHAggregate::printBVHStats(int resWidth, int resHeight, int spp, double msTotal) const noexcept
{
  const double totalRays{ static_cast<double>(resWidth) * resHeight * spp };
  const double npr{ totalRays ? m_BVHFrameStats.nodesVisited / totalRays : 0.0 };
  const double bpr{ totalRays ? m_BVHFrameStats.boxTests / totalRays : 0.0 };
  const double lpr{ totalRays ? m_BVHFrameStats.leavesVisited / totalRays : 0.0 };
  const double ppr{ totalRays ? m_BVHFrameStats.primitiveTests / totalRays : 0.0 };
  const double mrps{ (msTotal > 0.0) ? (totalRays / (msTotal * 1e3)) : 0.0 };

  const double sec = msTotal * 1e-3;
  const int hh = static_cast<int>(sec / 3600.0);
  const int mm = static_cast<int>((sec - hh * 3600.0) / 60.0);
  const double ss = sec - hh * 3600.0 - mm * 60.0;
  const auto hms = std::format("{:02d}:{:02d}:{:06.3f}", hh, mm, ss);

  std::println();
  std::println("\n+------------------------+----------------+");
  std::println("| {:<22} | {:>14} |", "resolution", std::format("{}x{}", resWidth, resHeight));
  std::println("| {:<22} | {:>14} |", "spp", spp);
  std::println("| {:<22} | {:>14.0f} |", "rays", totalRays);
  std::println("| {:<22} | {:>14.3f} |", "time_ms", msTotal);
  std::println("| {:<22} | {:>14.3f} |", "time_s", sec);
  std::println("| {:<22} | {:>14} |", "time_hms", hms);
  std::println("| {:<22} | {:>14.3f} |", "Mray_per_s", mrps);
  std::println("+------------------------+----------------+");
  std::println("| {:<22} | {:>14.3f} |", "nodes_per_ray", npr);
  std::println("| {:<22} | {:>14.3f} |", "aabb_tests_per_ray", bpr);
  std::println("| {:<22} | {:>14.3f} |", "leaves_per_ray", lpr);
  std::println("| {:<22} | {:>14.3f} |", "prim_tests_per_ray", ppr);
  std::println("+------------------------+----------------+\n");
}
