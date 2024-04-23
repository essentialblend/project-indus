import threadpool;

import <memory>;
import <vector>;
import <functional>;
import <thread>;
import <queue>;
import <atomic>;
import <future>;
import <mutex>;

void MT_ThreadPool::initiateThreadPool()
{
	const unsigned int numAvailableThreads = std::thread::hardware_concurrency();
	const unsigned int numThreadsToUse = numAvailableThreads > 1 ? static_cast<int>(numAvailableThreads * 0.5) : 1;

	for (std::size_t i{}; i < numThreadsToUse; ++i)
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
	while (true) {
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_conditionVar.wait(lock, [this] {
				return !m_tasksQueue.empty() || m_stopFlag;
				});
			if (m_stopFlag && m_tasksQueue.empty()) {
				return;
			}
			task = std::move(m_tasksQueue.front());
			m_tasksQueue.pop();
		}
		task();
	}
}
