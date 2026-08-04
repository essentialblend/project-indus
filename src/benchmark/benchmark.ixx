export module indus.benchmark;

import std;

import indus.engine.constructs;

export namespace Benchmark
{
  void configure(int argc, char** argv, IndusConfig& engineCfg);
}

void Benchmark::configure(int argc, char** argv, IndusConfig& engineCfg)
{
  if (argc <= 1) return;

  const bool isBenchmarkRequested
  {
    std::find_if(argv + 1, argv + argc, [](const char* arg)
    {
      return std::string_view{ arg } == "--benchmark";
    }) != argv + argc
  };

  if (!isBenchmarkRequested)
    throw std::invalid_argument("command-line benchmark options require --benchmark");

  engineCfg.headless = true;
  engineCfg.writeImage = false;

  auto& filmCfg{ engineCfg.filmCfg };

  for (int argIdx{ 1 }; argIdx < argc; ++argIdx)
  {
    const std::string_view arg{ argv[argIdx] };
    auto requireValue = [&](std::string_view option) -> std::string_view
    {
      if (++argIdx >= argc)
        throw std::invalid_argument(std::string(option) + " requires a value");

      return argv[argIdx];
    };

    if (arg == "--benchmark")
    {
      continue;
    }
    else if (arg == "--threads")
    {
      engineCfg.runtimeThreads = std::stoull(std::string(requireValue(arg)));
    }
    else if (arg == "--aggregate")
    {
      const std::string_view value{ requireValue(arg) };

      if (value == "bvh") engineCfg.aggregateType = AggregateType::SAHBVH;
      else if (value == "list") engineCfg.aggregateType = AggregateType::List;
      else throw std::invalid_argument("--aggregate must be bvh or list");
    }
    else if (arg == "--resolution")
    {
      const std::string value{ requireValue(arg) };
      const std::size_t separator{ value.find('x') };

      if (separator == std::string::npos)
        throw std::invalid_argument("--resolution must be WIDTHxHEIGHT");

      filmCfg.resolution = { std::stoi(value.substr(0, separator)), std::stoi(value.substr(separator + 1)) };
      filmCfg.crop = Bounds2i{ Point2i{}, filmCfg.resolution };
      engineCfg.camCfg.screenWindow = Bounds2f{ { -filmCfg.aspect(), -1 }, { filmCfg.aspect(), 1 } };
    }
    else if (arg == "--strata")
    {
      engineCfg.samplerCfg.strata = Strata2D{ std::stoi(std::string(requireValue(arg))) };
      engineCfg.samplerCfg.samplesPerPixel = static_cast<Int>(engineCfg.samplerCfg.strata.getTotal());
    }
    else if (arg == "--spp")
    {
      const Int samplesPerPixel{ std::stoi(std::string(requireValue(arg))) };

      if (samplesPerPixel < 1)
        throw std::invalid_argument("--spp must be at least 1");

      engineCfg.samplerCfg.isStratified = false;
      engineCfg.samplerCfg.samplesPerPixel = samplesPerPixel;
    }
    else if (arg == "--write-image")
    {
      engineCfg.writeImage = true;
    }
    else
    {
      throw std::invalid_argument("unknown benchmark option: " + std::string(arg));
    }
  }
}
