export module e_indus;

import <vector>;
import <memory>;

import es_renderer;
import es_window;
import ec_framebuffer;
import es_timermanager;
import core_constructs;
import vec3;
import renderer;
import color;
import camera;
import overlay;
import window;

import i_engine;
import ri_singleton;

export class EIndus : public IEngine, public RISingleton<EIndus>
{
public:

	void initializeEntity([[maybe_unused]] const std::vector<std::any>& args) override;

	void startEntity() override;
	void stopEntity() override;

	[[nodiscard]] const ECFramebuffer& getEngineFramebuffer() const noexcept override;
	void setCallbackFunctors() override;

private:
	EngineStatistics m_engineStats{};
	EngineSystemTypes m_activeEngineSystems{};
	std::unique_ptr<ECFramebuffer> m_framebuffer{};
	// World Object List.
	explicit EIndus() noexcept = default;
	EIndus(const EIndus&) = delete;
	EIndus& operator=(const EIndus&) = delete;
	EIndus(EIndus&&) noexcept = delete;
	EIndus& operator=(EIndus&&) noexcept = delete;
	~EIndus() noexcept = default;

	friend class RISingleton<EIndus>;
};
