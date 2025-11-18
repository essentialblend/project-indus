export module indus.ui.displaysink_base;

import std;

import indus.engine.systems;
import indus.engine.constructs;

import indus.integrator.constructs;

import indus.utilities.basictimer;

export class DisplaySinkBase
{
public:
  explicit DisplaySinkBase(const BasicTimer& basicTimer, const EngineBuildInformation& buildInfo, const ImmutableEngineSystems& immutableEngineSystems) noexcept;

  virtual void present() = 0;
  virtual void update(const FrameSnapshot& frame) = 0;
  virtual bool isSinkOpen() const noexcept = 0;

  [[nodiscard]] virtual std::string toString() const = 0;

  virtual ~DisplaySinkBase() noexcept = default;
protected:
  std::optional<std::reference_wrapper<const BasicTimer>> m_mainTimer{};
  std::optional<std::reference_wrapper<const EngineBuildInformation>> m_engineInfo{};
  std::optional<std::reference_wrapper<const ImmutableEngineSystems>> m_immutableEngineSystems{};
};

DisplaySinkBase::DisplaySinkBase(const BasicTimer& basicTimer, const EngineBuildInformation& buildInfo, const ImmutableEngineSystems& immutableEngineSystems) noexcept : m_mainTimer{ std::cref(basicTimer) }, m_engineInfo{ std::cref(buildInfo) }, m_immutableEngineSystems{ std::cref(immutableEngineSystems) } {}