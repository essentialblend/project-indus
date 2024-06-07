module es_overlaymanager;


void ESOverlayManager::initializeEntity([[maybe_unused]] const std::vector<std::any>& args)
{
	for(auto& overlay : m_overlayList)
	{
		overlay->initializeEntity();
	}
}

void ESOverlayManager::addOverlay(std::shared_ptr<EUOverlay> overlay)
{
	m_overlayList.push_back(overlay);
}

std::vector<std::shared_ptr<EUOverlay>>& ESOverlayManager::getOverlaysMutable() noexcept
{
	return m_overlayList;
}
