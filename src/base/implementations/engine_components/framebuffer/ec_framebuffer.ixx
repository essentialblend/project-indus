export module ec_framebuffer;

import <vector>;
import <span>;
import <memory>;

import i_enginecomponent;
import color;

export class ECFramebuffer : public IEngineComponent
{
public:
	explicit ECFramebuffer() noexcept = default;

	ECFramebuffer(const ECFramebuffer&) = delete;
	ECFramebuffer& operator=(const ECFramebuffer&) = delete;
	ECFramebuffer(ECFramebuffer&&) = default;
	ECFramebuffer& operator=(ECFramebuffer&&) = default;

	virtual void initializeEngineComponent() override;
	virtual void configureEngineComponent() override;
	virtual void cleanupComponentResources() override;

	const std::vector<std::unique_ptr<const IColor>>& getFramebuffer() const noexcept;
	std::size_t getFramebufferSize() const noexcept;
	const IColor& getFramebufferColorAtIndex(const std::size_t flattened1DIndex) const;

	void setFramebuffer(std::vector<std::unique_ptr<IColor>>& framebufferVec) noexcept;
	void setFramebufferSize(const std::size_t framebufferSize) noexcept;

	virtual ~ECFramebuffer() noexcept = default;

private:
	std::vector<std::unique_ptr<IColor>> m_framebuffer{};
};