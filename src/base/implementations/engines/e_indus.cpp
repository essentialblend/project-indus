module e_indus;

import eu_statsoverlay;

import <memory>;

void EIndus::initializeEntity([[maybe_unused]] const std::vector<std::any>& args)
{
	m_activeEngineSystems = std::any_cast<EngineSystemTypes>(args[0]);

	m_framebuffer = std::make_unique<ECFramebuffer>();
	m_framebuffer->setFramebufferSize(m_engineStats.imageResObj.getTotalPixels());

	m_activeEngineSystems.masterOverlayManagerObj->addOverlay(std::make_shared<EUStatsOverlay>());
	m_activeEngineSystems.masterOverlayManagerObj->initializeEntity();

	m_activeEngineSystems.masterTimerManagerObj->addTimer("PDH Query Timer.");
	m_activeEngineSystems.masterTimerManagerObj->addTimer("Render Timer.");
	m_activeEngineSystems.masterTimerManagerObj->initializeEntity();

	m_activeEngineSystems.windowTypeObj->initializeEntity(args);
}

void EIndus::startEntity()
{
	for(auto& timer : m_activeEngineSystems.masterTimerManagerObj->getTimersMutable())
	{
		timer->startEntity();
	}

	m_activeEngineSystems.windowTypeObj->startEntity();
}

void EIndus::stopEntity()
{
	m_framebuffer->clearFramebuffer();
}

const ECFramebuffer& EIndus::getEngineFramebuffer() const noexcept
{
	return *m_framebuffer;
}

void EIndus::setCallbackFunctors()
{

}
