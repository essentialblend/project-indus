export module threadpool;

import <vector>;
import <functional>;
import <thread>;
import <queue>;
import <atomic>;
import <future>;
import <mutex>;

export class MT_ThreadPool
{
public:
	explicit MT_ThreadPool() noexcept = default;
	explicit MT_ThreadPool(std::size_t numThreads) noexcept;

	MT_ThreadPool(const MT_ThreadPool&) = delete;
	MT_ThreadPool operator=(const MT_ThreadPool&) = delete;

	template<typename F, typename... Args>
	auto enqueueThreadPoolTask(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;

	void stopThreadPool();

private:
	std::vector<std::jthread> m_workerThreads;
	std::queue<std::function<void()>> m_tasksQueue{};

	std::mutex m_queueMutex{};
	std::condition_variable m_conditionVar{};
	bool m_stopFlag{};

	void executeTasks();
};

template<typename F, typename... Args>
auto MT_ThreadPool::enqueueThreadPoolTask(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
	auto taskToPerform = std::make_shared<std::packaged_task<typename std::invoke_result<F, Args...>::type>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	auto futureResultFromTask = taskToPerform->get_future();
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		if (m_stopFlag) {
			throw std::runtime_error("enqueue on stopped ThreadPool");
		}
		m_tasksQueue.emplace([taskToPerform]() { (*taskToPerform)(); });
	}
	m_conditionVar.notify_one();
	return futureResultFromTask;
}
