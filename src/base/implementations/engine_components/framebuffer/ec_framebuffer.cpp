module ec_framebuffer;

import <stdexcept>;
import <vector>;
import <memory>;

import eu_color;

const std::vector<std::unique_ptr<const EUColor>>& ECFramebuffer::getFramebuffer() const noexcept
{
	return reinterpret_cast<const std::vector<std::unique_ptr<const EUColor>>&>(m_framebuffer);
}

std::size_t ECFramebuffer::getFramebufferSize() const noexcept
{
	return m_framebuffer.size();
}

const EUColor& ECFramebuffer::getFramebufferColorAtIndex(const std::size_t flattened1DIndex) const
{
	if(flattened1DIndex >= m_framebuffer.size())
	{
		throw std::out_of_range("Index out of range in ECFramebuffer::getFramebufferColorAtIndex");
	}
	return *m_framebuffer.at(flattened1DIndex);
}

void ECFramebuffer::setFramebuffer(std::vector<std::unique_ptr<EUColor>>& framebufferVec) noexcept
{
	m_framebuffer = std::move(framebufferVec);
}

void ECFramebuffer::setFramebufferSize(const std::size_t framebufferSize) noexcept
{
	m_framebuffer.resize(framebufferSize);
}

void ECFramebuffer::clearFramebuffer() noexcept
{
	m_framebuffer.clear();
}
