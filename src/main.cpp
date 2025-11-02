
import indus;
import vector;
import std;
import point;
import independentsampler;
import onb;
import types;
import lcg;
import cameratransform;
import bounds;
import engineconstructs;
import vector; 
import squarematrix;
import transform;
import mathfp;
import mathconstants;
import mathtrig;
import mathalgebra;


int main()
{
	IndusConfig engineCfg{};
  auto& filmCfg{ engineCfg.filmCfg };

	engineCfg.filmCfg.filename = "test";
	engineCfg.filmCfg.resolution = { 640, 360 };
	filmCfg.crop = Bounds2i{ Point2i{ 0, 0 }, filmCfg.resolution };
	filmCfg.diagonalMM = Float{ 43.266615 };
	filmCfg.filterRadius = Vec2f{ Float{ 0.5 } };
	filmCfg.imagingRatio = Float{ 1 };

	engineCfg.camCfg.cameraShutter = { 0.0, 1.0 };
	engineCfg.camCfg.cameraToWorld = Transform4f::lookAt(Point3f{ 0, 2, -5 }, Point3f{ 0, 0, 0 }, Vec3f{ 0, 1.0, 0 });

	engineCfg.camCfg.focalDistance = Float{ 4.225 };
	engineCfg.camCfg.fovDegrees = Float{ 45 };
	engineCfg.camCfg.lensRadius = Float{ 0.066 };
	engineCfg.camCfg.screenWindow = Bounds2f{ { -filmCfg.aspect(), -1 }, { filmCfg.aspect(), 1 } };
  engineCfg.camCfg.renderingSpace = RenderingSpace::World;

	engineCfg.samplerCfg.strata = Strata2D{ 5 };
	engineCfg.samplerCfg.samplesPerPixel = static_cast<Int>(engineCfg.samplerCfg.strata.getTotal());
	engineCfg.samplerCfg.isStratified = true;
	engineCfg.samplerCfg.isJitter = true;
	
	engineCfg.integratorCfg.maxDepth = 50;
  engineCfg.integratorCfg.useRR = true;

	Indus engine{ engineCfg };

	engine.runEngine();
}

