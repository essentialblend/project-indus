export module immutableconstructs;

import std;
import filmbase;
import sampler;
import camerabase;
import integrator;
import bvhaggregate;
import scene;

export struct ImmutableEngineSystems
{
  const FilmBase& m_film;
  const CameraBase& m_camera;
  const Sampler& m_sampler;
  const Integrator& m_integrator;
  const Scene& m_renderScene;
};
