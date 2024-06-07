module es_timermanager;

void ESTimerManager::initializeEntity([[maybe_unused]] const std::vector<std::any>& args)
{
    for(auto& timer : m_masterTimerList)
	{
		timer->resetTimer();
	}
}

void ESTimerManager::manageEntityResources()
{
    m_masterTimerList.clear();
}

void ESTimerManager::addTimer(std::string_view timerName)
{
    m_masterTimerList.push_back(std::make_shared<EUTimer>(timerName));
}

const EUTimer& ESTimerManager::findTimer(std::string_view timerName) const
{
    auto it{ std::find_if(m_masterTimerList.begin(), m_masterTimerList.end(), [timerName](const std::shared_ptr<EUTimer>& timerPtr)
    {
        	return timerPtr->getTimerName() == timerName;
    })};

    if(it != m_masterTimerList.end())
	{
        return *(*it);
	}
	else
	{
		throw std::runtime_error("Timer not found.");
	}
}

EUTimer& ESTimerManager::findTimerMutable(std::string_view timerName)
{
    return const_cast<EUTimer&>(std::as_const(*this).findTimer(timerName));
}

void ESTimerManager::removeTimer(std::string_view timerName)
{
    m_masterTimerList.erase(
        std::remove_if(m_masterTimerList.begin(), m_masterTimerList.end(),
            [timerName](const std::shared_ptr<EUTimer>& timerPtr) {
                return timerPtr->getTimerName() == timerName;
            }),
        m_masterTimerList.end());
}

std::vector<std::shared_ptr<EUTimer>>& ESTimerManager::getTimersMutable() noexcept
{
    return m_masterTimerList;
}
