import es_timermanager;

ESTimerManager& ESTimerManager::getInstance() noexcept
{
    static ESTimerManager instance;
    return instance;
}

void ESTimerManager::initializeEntity()
{
    for(auto& timer : m_masterTimerList)
	{
		timer.resetTimer();
	}
}

void ESTimerManager::manageEntityResources()
{
    m_masterTimerList.clear();
}

void ESTimerManager::addTimer(const UTimer& timerToAdd)
{
    m_masterTimerList.push_back(timerToAdd);
}

const UTimer& ESTimerManager::findTimer(std::string_view timerName) const
{
    auto it{ std::find_if(m_masterTimerList.begin(), m_masterTimerList.end(), [timerName](const UTimer& timer) 
        {
        	return timer.getTimerName() == timerName;
        })};

    if(it != m_masterTimerList.end())
	{
		return *it;
	}
	else
	{
		throw std::runtime_error("Timer not found.");
	}
}

UTimer& ESTimerManager::findTimerMutable(std::string_view timerName)
{
    return const_cast<UTimer&>(std::as_const(*this).findTimer(timerName));
}

void ESTimerManager::removeTimer(std::string_view timerName)
{
    m_masterTimerList.erase(
        std::remove_if(m_masterTimerList.begin(), m_masterTimerList.end(),
            [timerName](const UTimer& timer) {
                return timer.getTimerName() == timerName;
            }),
        m_masterTimerList.end());
}