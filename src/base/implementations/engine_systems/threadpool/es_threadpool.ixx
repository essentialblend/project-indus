export module es_threadpool;

import <vector>;
import <functional>;
import <thread>;
import <queue>;
import <atomic>;
import <future>;
import <mutex>;
import <memory>;

import i_enginesystem;

export class ESThreadpool : public IEngineSystem
{
public:
	explicit ESThreadpool() noexcept = default;
	ESThreadpool(const ESThreadpool&) noexcept = delete;
	ESThreadpool& operator=(const ESThreadpool&) noexcept = delete;
	ESThreadpool(ESThreadpool&&) noexcept = delete;	
	ESThreadpool& operator=(ESThreadpool&&) noexcept = delete;

	virtual void initializeEngineSystem() override = 0;
	virtual void configureSystemComponents() override = 0;
	virtual void cleanupSystemResources() override = 0;

	virtual void startEngineSystem() = 0;
	virtual void stopEngineSystem() = 0;

	template<typename F>
	[[nodiscard]] auto enqueueThreadPoolTask(F&& func) -> std::future<decltype(func())>;

	[[nodiscard]] int getNumThreadsUsedByThreadpool() const noexcept;
	[[nodiscard]] bool isThreadpoolEmpty() const noexcept;
	[[nodiscard]] bool areAllThreadsOccupied() const noexcept;

	~ESThreadpool() noexcept = default;
	
private:
	std::vector<std::jthread> m_workerThreads{};
	std::queue<std::function<void()>> m_tasksQueue{};

	std::mutex m_queueMutex{};
	std::condition_variable m_tasksConditionVar{};
	std::atomic<int> m_numActiveTasks{};
	bool m_stopFlag{ false };

	void executeTasks();
};


// Required in the .ixx module interface.
template<typename F>
auto ESThreadpool::enqueueThreadPoolTask(F&& func) -> std::future<decltype(func())> {

	// Wrap the function in a std::packaged_task to tie it to a future.
	auto taskPtr = std::make_shared<std::packaged_task<decltype(func())()>>(std::forward<F>(func));

	auto futureTaskResult = taskPtr->get_future();
	{
		std::scoped_lock lock(m_queueMutex);

		if (m_stopFlag) throw std::runtime_error("enqueue on stopped ThreadPool");
		m_tasksQueue.emplace([taskPtr]() { (*taskPtr)(); });
	}
	m_tasksConditionVar.notify_one();
	return futureTaskResult;
}
