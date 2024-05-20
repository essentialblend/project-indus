import es_threadpool;

int ESThreadpool::getNumThreadsUsedByThreadpool() const noexcept
{
	return static_cast<int>(m_workerThreads.size());
}

bool ESThreadpool::isThreadpoolEmpty() const noexcept
{
	return m_tasksQueue.empty();
}

bool ESThreadpool::areAllThreadsOccupied() const noexcept
{
	return (m_numActiveTasks == m_workerThreads.size());
}

void ESThreadpool::initializeEntity()
{

}

void ESThreadpool::startEntity()
{
	const unsigned int numAvailableThreads = std::thread::hardware_concurrency();
	const unsigned int numThreadsToUse = numAvailableThreads > 1 ? static_cast<int>(numAvailableThreads * 0.5) + (static_cast<int>(numAvailableThreads / 6)) : 1;

	for (std::size_t i{}; i < static_cast<int>(numThreadsToUse); ++i)
	{
		m_workerThreads.emplace_back(&ESThreadpool::executeTasks, this);
	}
}

void ESThreadpool::stopEntity()
{
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_stopFlag = true;
	}
	m_tasksConditionVar.notify_all();

	while (m_numActiveTasks > 0) {
		std::this_thread::yield();
	}

	for (std::jthread& workerThread : m_workerThreads)
	{
		if (workerThread.joinable())
		{
			workerThread.join();
		}
	}
}

void ESThreadpool::executeTasks()
{
	while (true) {
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_tasksConditionVar.wait(lock, [this] {
				return !m_tasksQueue.empty() || m_stopFlag;
				});
			if (m_stopFlag && m_tasksQueue.empty()) {
				return;
			}
			task = std::move(m_tasksQueue.front());
			m_tasksQueue.pop();
		}

		++m_numActiveTasks;
		task();
		--m_numActiveTasks;
	}
}
