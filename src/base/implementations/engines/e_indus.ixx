export module e_indus;

import <vector>;
import <memory>;

import core_constructs;
import vec3;
import renderer;
import color;
import camera;
import overlay;
import window;

import i_engine;



export class EIndus : public IEngine
{
public:
	explicit EIndus() noexcept = default;
	EIndus(const EIndus&) = delete;
	EIndus& operator=(const EIndus&) = delete;
	EIndus(EIndus&&) noexcept = delete;	
	EIndus& operator=(EIndus&&) noexcept = delete;

	virtual void initializeEntity() override;
	virtual void configureEntity() override;

	virtual void startEntity() override;
	virtual void stopEntity() override;

	[[nodiscard]] std::vector<std::unique_ptr<IColor>>& getEngineFramebuffer() noexcept;

	~EIndus() noexcept = default;

private:
	//Window
	//Renderer
	//Overlay
	//Framebuffer
	// World Object List.
};