import ec_framebuffer;

import<stdexcept>;

void ECFramebuffer::initializeEngineComponent()
{
	// Empty, no initialization needed.
}

void ECFramebuffer::configureEngineComponent() 
{
	// Empty, no configuration needed.
}

void ECFramebuffer::cleanupComponentResources() 
{
	m_framebuffer.clear();
}

const std::vector<std::unique_ptr<const IColor>>& ECFramebuffer::getFramebuffer() const noexcept
{
	return reinterpret_cast<const std::vector<std::unique_ptr<const IColor>>&>(m_framebuffer);
}

std::size_t ECFramebuffer::getFramebufferSize() const noexcept
{
	return m_framebuffer.size();
}

const IColor& ECFramebuffer::getFramebufferColorAtIndex(const std::size_t flattened1DIndex) const
{
	if(flattened1DIndex >= m_framebuffer.size())
	{
		throw std::out_of_range("Index out of range in ECFramebuffer::getFramebufferColorAtIndex");
	}
	return *m_framebuffer.at(flattened1DIndex);
}


void ECFramebuffer::setFramebuffer(std::vector<std::unique_ptr<IColor>>& framebufferVec) noexcept
{
	m_framebuffer = std::move(framebufferVec);
}

void ECFramebuffer::setFramebufferSize(const std::size_t framebufferSize) noexcept
{
	m_framebuffer.reserve(framebufferSize);
}