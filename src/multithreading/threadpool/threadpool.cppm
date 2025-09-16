import threadpool;

int MT_ThreadPool::getNumThreadsUsedByThreadPool() const noexcept
{
	return static_cast<int>(m_workerThreads.size());
}

bool MT_ThreadPool::isThreadPoolEmpty() const noexcept
{
	return m_tasksQueue.empty();
}

bool MT_ThreadPool::areAllThreadsOccupied() const noexcept
{
	return (m_numActiveTasks == m_workerThreads.size());
}

void MT_ThreadPool::initiateThreadPool(std::size_t numWorkerThreads)
{
	const std::size_t numThreadsAvailable{ std::thread::hardware_concurrency() };
	const std::size_t reserveCores{ static_cast<std::size_t>((numThreadsAvailable > 3) ? 2 : 1) };
	std::size_t numThreadsToUse{};

	if (numWorkerThreads <= 1)
	{
		numThreadsToUse = 1;
	}
	else
	{
		numThreadsToUse = numWorkerThreads - reserveCores;
	}
	
	for (std::size_t i{}; i < static_cast<int>(numThreadsToUse); ++i)
	{
		m_workerThreads.emplace_back(&MT_ThreadPool::executeTasks, this);
	}
}

void MT_ThreadPool::stopThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_stopFlag = true;
	}
	m_conditionVar.notify_all();
	
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

void MT_ThreadPool::executeTasks()
{

	while (true) 
	{
		
		// Acquire lock, and check for tasks with the conditionVar
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);

			// If true, wait here, check for a task. If not, go to sleep pending notify
			m_conditionVar.wait(lock, [this] 
			{
				return !m_tasksQueue.empty() || m_stopFlag;
			});


			// True, check for flag or tasks, if flagged or no-queue, return
			if (m_stopFlag && m_tasksQueue.empty()) 
			{
				return;
			}
			
			// Else steal a task from the queue, pop the task off the queue and run the task.
			task = std::move(m_tasksQueue.front());
			m_tasksQueue.pop();
		}

		++m_numActiveTasks;
		task();
		--m_numActiveTasks;
	}
}
