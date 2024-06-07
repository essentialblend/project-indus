export module ec_framebuffer;

import <vector>;
import <span>;
import <memory>;

import i_enginecomponent;

import eu_color;

export class ECFramebuffer final : public IEngineComponent
{
public:
	explicit ECFramebuffer() noexcept = default;

	ECFramebuffer(const ECFramebuffer&) = delete;
	ECFramebuffer& operator=(const ECFramebuffer&) = delete;
	ECFramebuffer(ECFramebuffer&&) = default;
	ECFramebuffer& operator=(ECFramebuffer&&) = default;

	const std::vector<std::unique_ptr<const EUColor>>& getFramebuffer() const noexcept;
	std::size_t getFramebufferSize() const noexcept;
	const EUColor& getFramebufferColorAtIndex(const std::size_t flattened1DIndex) const;

	void setFramebuffer(std::vector<std::unique_ptr<EUColor>>& framebufferVec) noexcept;
	void setFramebufferSize(const std::size_t framebufferSize) noexcept;

	void clearFramebuffer() noexcept;

	virtual ~ECFramebuffer() noexcept = default;

private:
	std::vector<std::unique_ptr<EUColor>> m_framebuffer{};
};