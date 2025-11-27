import indus.engine;

int main()
{
	// Define the configuration object, parameterize various engine systems
	IndusConfig engineCfg{};

	// Parameterize the Film
  auto& filmCfg{ engineCfg.filmCfg };

	filmCfg.filename = "test";
	filmCfg.resolution = { 1280, 720 };
	filmCfg.crop = Bounds2i{ Point2i{}, filmCfg.resolution };
	filmCfg.diagonalMM = Float{ 43.266615 };
	filmCfg.imagingRatio = Float{ 1 };
	filmCfg.filterType = FilterType::Gaussian;
	filmCfg.filterPreset = FilterPreset::Low;

	// Parameterize the Camera
	engineCfg.camCfg.cameraShutter = { 0.0, 1.0 };
	engineCfg.camCfg.cameraToWorld = Transform4f::lookAt(Point3f{ 0, 2, -5 }, Point3f{ 0, 0, 0 }, Vec3f{ 0, 1.0, 0 });
	engineCfg.camCfg.focalDistance = Float{ 4.225 };
	engineCfg.camCfg.fovDegrees = Float{ 45 };
	engineCfg.camCfg.lensRadius = Float{ 0.055 };
	engineCfg.camCfg.screenWindow = Bounds2f{ { -filmCfg.aspect(), -1 }, { filmCfg.aspect(), 1 } };
  engineCfg.camCfg.renderingSpace = RenderingSpace::World;

	// ... the Sampler
	engineCfg.samplerCfg.strata = Strata2D{ 10 };
	engineCfg.samplerCfg.samplesPerPixel = static_cast<Int>(engineCfg.samplerCfg.strata.getTotal());
	engineCfg.samplerCfg.isStratified = true;
	engineCfg.samplerCfg.isJitter = true;
	
	// ... the Integrator
	engineCfg.integratorCfg.maxDepth = 50;
  engineCfg.integratorCfg.useRR = true;

	// ... and finally, the DisplaySink
	engineCfg.displaySinkCfg.sinkType = SinkType::SFML;
	engineCfg.displaySinkCfg.windowResolution = { filmCfg.resolution };

	// Run Indus. Currently, immediately one-shot renders a hard-coded scene
	Indus engine{ engineCfg };

	engine.runEngine();
}

