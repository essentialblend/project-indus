export module threadpool;

import <vector>;
import <functional>;
import <thread>;
import <queue>;
import <atomic>;
import <future>;
import <mutex>;
import <memory>;

export class MT_ThreadPool
{
public:
	explicit MT_ThreadPool() noexcept = default;
	MT_ThreadPool(const MT_ThreadPool&) = delete;
	MT_ThreadPool operator=(const MT_ThreadPool&) = delete;

    void initiateThreadPool();
    template<typename F>
    auto enqueueThreadPoolTask(F&& func)->std::future<decltype(func())>;
	void stopThreadPool();

private:
	std::vector<std::jthread> m_workerThreads;
	std::queue<std::function<void()>> m_tasksQueue{};

	std::mutex m_queueMutex{};
	std::condition_variable m_conditionVar{};
    bool m_stopFlag{ false };

	void executeTasks();
};

template<typename F>
auto MT_ThreadPool::enqueueThreadPoolTask(F&& func) -> std::future<decltype(func())> {

    // Wrap the function in a std::packaged_task to tie it to a future.
    auto taskPtr = std::make_shared<std::packaged_task<decltype(func())()>>(std::forward<F>(func));

    auto futureTaskResult = taskPtr->get_future();
    {
        std::scoped_lock lock(m_queueMutex);

        if (m_stopFlag) throw std::runtime_error("enqueue on stopped ThreadPool");
        m_tasksQueue.emplace([taskPtr]() { (*taskPtr)(); });
    }
    m_conditionVar.notify_one();
    return futureTaskResult;
}


