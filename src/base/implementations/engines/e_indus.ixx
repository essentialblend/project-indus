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
	EIndus(EIndus&&) = delete;	
	EIndus& operator=(EIndus&&) = delete;

	virtual void initializeEngine() override;
	virtual void setupEngineSystems() override;
	virtual void runEngine() override;
	virtual void shutdownEngine() override;
	virtual void setupScene() override;

	[[nodiscard]] std::vector<std::unique_ptr<IColor>>& getEngineFramebuffer() noexcept;

private:
	//Window
	//Renderer
	//Overlay
	//Framebuffer
	// World Object List.
};