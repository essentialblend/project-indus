export module threadpool;

import std;
import jobbase;

export class ThreadPool final
{
public:
  explicit ThreadPool(std::size_t nThreads);

  std::size_t getSize() const noexcept;

  void enqueueJob(std::shared_ptr<JobBase> job);
  void removeJob(const std::shared_ptr<JobBase>& job);

  void drainThreadPool();
  void shutdownThreadPool();

  void disableThreadPool();
  void reEnableThreadPool();

  void forEachThread(const std::function<void()>& functor);

  [[nodiscard]] ThreadPool& getThreadPool() noexcept;
  
  ~ThreadPool();

private:
  std::vector<std::jthread> m_threads{};

  std::mutex m_mutex{};
  std::condition_variable m_conditionVariable{};
  std::deque<std::shared_ptr<JobBase>> m_jobsDeque{};

  std::size_t m_activeWorkers{ 0 };

  std::function<void()> m_foreachFn{};
  std::size_t m_foreachRemaining{ 0 };
  bool m_foreachActive{ false };

  bool m_disabled{ false };
  bool m_shuttingDown{ false };
  
  void worker(std::stop_token stopToken);
};

ThreadPool::ThreadPool(std::size_t numThreads)
{
  const std::size_t n{ numThreads > 0 ? numThreads : 1 };

  const std::size_t workers{ n > 0 ? n - 2 : 0 };
  
  m_threads.reserve(workers);
  
  for (std::size_t i{}; i < workers; ++i)
    m_threads.emplace_back([this](std::stop_token stopToken) { worker(stopToken); });
}

std::size_t ThreadPool::getSize() const noexcept
{
  return m_threads.size();
}

void ThreadPool::enqueueJob(std::shared_ptr<JobBase> job)
{
  if (!job) return;
  {
    std::scoped_lock scopedLock(m_mutex);

    if (m_shuttingDown) return;
    
    m_jobsDeque.emplace_back(std::move(job));
  }

  m_conditionVariable.notify_one();
}

void ThreadPool::removeJob(const std::shared_ptr<JobBase>& job)
{
  if (!job) return;
  {
    std::scoped_lock scopedLock(m_mutex);

    auto it = std::find_if(m_jobsDeque.begin(), m_jobsDeque.end(), [&](const std::shared_ptr<JobBase>& j) { return j.get() == job.get(); });
    
    if (it != m_jobsDeque.end()) m_jobsDeque.erase(it);
  }

  m_conditionVariable.notify_all();
}

void ThreadPool::drainThreadPool() 
{
  std::unique_lock uniqueLock(m_mutex);

  m_conditionVariable.wait(uniqueLock, [this]
  {
    return m_shuttingDown || (m_jobsDeque.empty() && m_activeWorkers == 0);
  });
}

void ThreadPool::disableThreadPool() 
{
  std::scoped_lock scopedLock(m_mutex);

  if (m_shuttingDown) return;
  
  m_disabled = true;
}

void ThreadPool::reEnableThreadPool() 
{
  {
    std::scoped_lock scopedLock(m_mutex);
    
    if (m_shuttingDown) return;
    
    m_disabled = false;
  }
  
  m_conditionVariable.notify_all();
}

void ThreadPool::forEachThread(const std::function<void()>& functor)
{
  const auto areForeachTasksRemaining = [this]
  {
    return m_foreachRemaining == 0;
  };

  if (!functor) return;
  {
    std::scoped_lock scopedLock(m_mutex);

    m_foreachFn = functor;
    m_foreachRemaining = m_threads.size();
    m_foreachActive = true;
  }
  
  m_conditionVariable.notify_all();
  
  functor();
  
  {
    std::unique_lock uniqueLock(m_mutex);

    m_conditionVariable.wait(uniqueLock, areForeachTasksRemaining);

    m_foreachActive = false;
    m_foreachFn = {};
  }
}

ThreadPool& ThreadPool::getThreadPool() noexcept
{
  return *this;
}

void ThreadPool::shutdownThreadPool() 
{
  {
    std::scoped_lock scopedLock(m_mutex);

    if (m_shuttingDown) return;
    
    m_shuttingDown = true;
  }
  
  m_conditionVariable.notify_all();

  for (auto& t : m_threads) t.request_stop();
  // jthread’s destructor joins; clearing will join all workers deterministically
  m_threads.clear();

  // After workers exit, no one touches the queue; safe to clear
  std::scoped_lock scopedLock2(m_mutex);
  m_jobsDeque.clear();
}

ThreadPool::~ThreadPool() 
{
  { 
    std::scoped_lock scopedLock(m_mutex); 
    m_shuttingDown = true; 
  }

  m_conditionVariable.notify_all();

  for (auto& t : m_threads) t.request_stop();

  m_threads.clear();
}

void ThreadPool::worker(std::stop_token stopToken) 
{
  // Loop infinitely
  for (;;) 
  {
    std::shared_ptr<JobBase> job;
    {
      // Acquire a unique lock on the main threadpool (TP) mutex. Then, park the thread based on various conditionals by calling wait(..) on the conditional variable (CV)
      std::unique_lock uniqueLock(m_mutex);

      // This lambda handles conditionals that require joining the thread. m_shuttingDown awakens the thread so it can stop op and join. stop_requested() similarly returns true when we call stop_requested on the token. Similarly, any foreachActive tasks wake the thread. Similarly, if the threadpool isn't disabled or the job queue isn't empty, we wake up to work. Basically, we either wake up to terminate and join, or to work 
      const auto shouldWakeThreadQ = [this, &stopToken]
      {
        return m_shuttingDown || stopToken.stop_requested() || m_foreachActive || (!m_disabled && !m_jobsDeque.empty());
      };

      // Stay parked till wake up is desired
      m_conditionVariable.wait(uniqueLock, shouldWakeThreadQ);

      // Upon waking up, first handle any synchronous tasks before proceeding to standard jobs. That is, we check each conditional to perform the right task
      if (m_foreachActive) 
      {
        // Get a local copy of the function to isolate it from the thread state, m_foreachFn can be reassigned
        auto f{ m_foreachFn };
        
        // To prevent extraneous locks, we unlock and handle the function execution. We only lock to prevent state, not to gate logic execution
        uniqueLock.unlock();
        
        f();
        
        // Now lock and update the state. Notify all if all synchronous (foreach) tasks are complete
        uniqueLock.lock();
        
        if (--m_foreachRemaining == 0) m_conditionVariable.notify_all();
        
        // Start next iteration and continue listening
        continue;
      }

      // Handle the other conditionals, trivial.
      if (m_shuttingDown || stopToken.stop_requested()) break;
      
      // If we have work, first clean out any non-work containing jobs, so we can focus on the actual ones. Get the front job later and call runStep() on it
      while (!m_jobsDeque.empty() && !m_jobsDeque.front()->hasWork())
      {
        m_jobsDeque.pop_front();
      }

      if (m_jobsDeque.empty()) continue;
      
      job = m_jobsDeque.front();
      ++m_activeWorkers;
    }

    job->runStep();
    
    {
      std::scoped_lock scopedLock(m_mutex);
      
      --m_activeWorkers;
      
      if (!job->hasWork()) 
      {
        auto it = std::find_if(m_jobsDeque.begin(), m_jobsDeque.end(), [&](const std::shared_ptr<JobBase>& j) { return j.get() == job.get(); });

        if (it != m_jobsDeque.end()) m_jobsDeque.erase(it);
      }
    }
    m_conditionVariable.notify_all();
  }
}

