import indus.engine;
import indus.benchmark;

int main(int argc, char** argv)
{
	// Define the configuration object, parameterize various engine systems
	IndusConfig engineCfg{};

	// Parameterize the Film
	auto& filmCfg{ engineCfg.filmCfg };

	filmCfg.filename = "test";
	filmCfg.resolution = { 1920, 1080 };
	filmCfg.crop = Bounds2i{ Point2i{}, filmCfg.resolution };
	filmCfg.diagonalMM = Float{ 43.266615 };
	filmCfg.imagingRatio = Float{ 1 };
	filmCfg.filterType = FilterType::Gaussian;
	filmCfg.filterPreset = FilterPreset::High;

	// Parameterize the Camera
	Point3f lookFrom{ 0, 1.5, -5 };
	Point3f lookAt{ 0, 0, 0 };
	Vec3f upHintVec{ 0, 1.0, 0 };

	engineCfg.camCfg.cameraShutter = { 0.0, 1.0 };
	engineCfg.camCfg.cameraToWorld = Transform4f::lookAt(lookFrom, lookAt, upHintVec);
	engineCfg.camCfg.focalDistance = Float{ 3.75 };
	engineCfg.camCfg.fovDegrees = Float{ 55 };
	engineCfg.camCfg.lensRadius = Float{ 0.075 };
	engineCfg.camCfg.screenWindow = Bounds2f{ { -filmCfg.aspect(), -1 }, { filmCfg.aspect(), 1 } };
	engineCfg.camCfg.renderingSpace = RenderingSpace::World;

	// ... the Sampler
	engineCfg.samplerCfg.strata = Strata2D{ 7 };
	engineCfg.samplerCfg.samplesPerPixel = static_cast<Int>(engineCfg.samplerCfg.strata.getTotal());
	engineCfg.samplerCfg.isStratified = true;
	engineCfg.samplerCfg.isJitter = true;
	
	// ... the Integrator
	engineCfg.integratorCfg.maxDepth = 25;
	engineCfg.integratorCfg.useRR = true;

	// ... and finally, the DisplaySink
	engineCfg.displaySinkCfg.sinkType = SinkType::SFML;
	engineCfg.displaySinkCfg.windowResolution = { filmCfg.resolution[0] / 2, filmCfg.resolution[1] / 2 };

	Benchmark::configure(argc, argv, engineCfg);

	// Run Indus. Currently, immediately one-shot renders a hard-coded scene
	Indus engine{ engineCfg };

	engine.runEngine();
}

