
import indus;
import vector;
import std;
import point;
import independentsampler;
import onb;
import types;
import matrix;
import lcg;
import cameratransform;
import bounds;
import engineconstructs;

int main()
{
	IndusConfig engineCfg{};
  const auto& filmCfg{ engineCfg.filmCfg };

	engineCfg.filmCfg.filename = "test";
	engineCfg.filmCfg.resolution = { 1920, 1080 };

	engineCfg.camCfg.cameraShutter = { 0.0, 1.0 };
	engineCfg.camCfg.cameraToWorld = Transform4f::lookAt(Point3f{ 0, 2.0, -5.5 }, Point3f{ 0, 0.5, 0 }, Vec3f{ 0, 1.0, 0 });

	engineCfg.camCfg.focalDistance = 10.0;
	engineCfg.camCfg.fovDegrees = 45.0;
	engineCfg.camCfg.lensRadius = 0.0;
	engineCfg.camCfg.screenWindow = Bounds2f{ { -filmCfg.aspect(), -1 }, { filmCfg.aspect(), 1 } };
  engineCfg.camCfg.renderingSpace = RenderingSpace::World;

	engineCfg.samplerCfg.strata = Strata2D{ 12 };
	engineCfg.samplerCfg.samplesPerPixel = static_cast<Int>(engineCfg.samplerCfg.strata.getTotal());
	engineCfg.samplerCfg.isStratified = true;
	engineCfg.samplerCfg.isJitter = true;

	engineCfg.integratorCfg.maxDepth = 8;
  engineCfg.integratorCfg.useRR = true;

	Indus engine{ engineCfg };
	engine.run();
}

