export module indus.engine.systems;

import indus.film.base;
import indus.camera.base;
import indus.integrator.base;
import indus.sampler.base;
import indus.scene;

export struct ImmutableEngineSystems
{
  const FilmBase& m_film;
  const CameraBase& m_camera;
  const Sampler& m_sampler;
  const Integrator& m_integrator;
  const Scene& m_renderScene;
};