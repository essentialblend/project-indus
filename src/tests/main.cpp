#pragma once

#include "vec3-tests.hpp"
#include "point3-tests.hpp"

#include "mat4-tests.hpp"
#include "transform-tests.hpp"
#include "onb-tests.hpp"
#include "normal-tests.hpp"
#include "ray-tests.hpp"
#include "hitrec-tests.hpp"
#include "samplingutils-tests.hpp"
#include "colorrgb-tests.hpp"

int main()
{
  vector3Tests();
  point3Tests();
  mat4Tests();
  transformConformanceTests();
  onbTests();
  normalTests();
  rayTests();
  hitRecordTests();
  coreUtilTests();
  colorRGBTests();
}
