
import core_imports;
import indus;
import vector;
import std;
import point;
import independentsampler;
import onb;
import types;
import matrix;
import pcg32;

int main()
{
	IndusConfig engineCfg{};
  const auto& filmCfg{ engineCfg.filmCfg };

	engineCfg.filmCfg.filename = "test";
	engineCfg.filmCfg.resolution = { 1920, 1080 };

	engineCfg.camCfg.cameraShutter = { 0.0, 1.0 };

	engineCfg.camCfg.cameraToWorld = Transform4f::lookAt(Point3f{ 0, 2.0, -5.5 }, Point3f{0, 0.5, 0}, Vec3f{ 0.f, 1.f, 0.f });

	engineCfg.camCfg.focalDistance = 10.0;
	engineCfg.camCfg.fovDegrees = 45.0;
	engineCfg.camCfg.lensRadius = 0.0;
	engineCfg.camCfg.screenWindow = Bounds2f{ {-filmCfg.aspect(), -1}, {filmCfg.aspect(), 1}};

	engineCfg.samplerCfg.samplesPerPixel = 2;
	engineCfg.integratorCfg.maxDepth = 8;

	Indus engine{ engineCfg };
	engine.run();
}

