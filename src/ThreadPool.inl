#pragma once

namespace ecs
{

namespace impl
{

template <typename T>
inline const bool SafeQueue<T>::push(const T &value)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push(value);
	return true;
}

template <typename T>
inline const bool SafeQueue<T>::pop(T & v)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if(m_queue.empty())
	{
		return false;
	}
	v = m_queue.front();
	m_queue.pop();
	return true;
}

template <typename T>
inline const bool SafeQueue<T>::empty()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.empty();
}

template <typename T>
inline SafeQueue<T> &SafeQueue<T>::append(SafeQueue<T> &&other)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while(!other.empty())
	{
		this->push(other.front());
		other.pop();
	}
	return *this;
}

template <typename T>
inline SafeQueue<T> &SafeQueue<T>::merge(SafeQueue<T> &&other)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	SafeQueue<T> result;
	while(!m_queue.empty() && !other.empty())
	{
		if(!m_queue.empty())
		{
			result.push(m_queue.front());
			m_queue.pop();
		}
		if(!other.empty())
		{
			result.push(other.front());
			m_queue.pop();
		}
	}
	*this = result;
	return *this;
}

template <typename T>
inline T &SafeQueue<T>::front()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.front();
}

template <typename T>
inline const T &SafeQueue<T>::front() const
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.front();
}

template <typename T>
inline T &SafeQueue<T>::back()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.back();
}

template <typename T>
inline const T &SafeQueue<T>::back() const
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.back();
}

}  // namespace impl

ThreadPool::ThreadPool(const unsigned thread_count)
:
m_threads(),
m_flags(),
m_queue(),
m_finishedFlag(false),
m_haltFlag(false),
m_waitingThreadsCount(0u),
m_pendingTasksCount(0u)
{
	this->resize(thread_count);
}

ThreadPool::~ThreadPool()
{
	this->halt(true);  // terminate immediately, but first wait for threads to finish their tasks
}

inline std::thread &ThreadPool::getThread(const unsigned index)
{
	return *m_threads.at(index);  // dereference from std::unique_ptr
}

inline const unsigned ThreadPool::size() const
{
	return static_cast<unsigned>(m_threads.size());
}

inline const unsigned ThreadPool::idleThreadCount() const
{
	return m_waitingThreadsCount;
}

inline const unsigned ThreadPool::pendingTasksCount() const
{
	return m_pendingTasksCount;
}

inline void ThreadPool::resize(const unsigned thread_count)
{
	if(!m_haltFlag && !m_finishedFlag)
	{
		const unsigned old_thread_count = static_cast<unsigned>(m_threads.size());
		if(old_thread_count <= thread_count)  // if the new size is greater than the old one
		{
			//  it's safe to resize as threads (and flags) are added, not removed
			m_threads.resize(thread_count);
			m_flags.resize(thread_count);

			for(unsigned index = old_thread_count; index < thread_count; index++)
			{
				m_flags[index] = std::make_shared<std::atomic<bool>>(false);
				this->setupThread(index);
			}
		}
		else  // the new size is lower than the old one
		{
			//  finish extra threads as they are to be removed
			for(unsigned index = old_thread_count - 1u; index >= thread_count; index--)
			{
				*(m_flags[index]) = true;
				m_threads[index]->detach();
			}

			{  // safety scope for std::unique_lock
				std::unique_lock<std::mutex> lock(m_mutex);
				m_cond.notify_all();  // sync all waiting threads
			}

			//  threads are detached so it's safe to remove them
			//  flags are safe to remove as well, because threads hold copies of shared_ptr
			m_threads.resize(thread_count);
			m_flags.resize(thread_count);
		}
	}
}

//  wait for all computing threads to finish and stop all threads
//  may be called asynchronously to not pause the calling thread while waiting
//  if is_wait == true, all the functions in the queue are run, otherwise the queue is cleared 
//    without running the functions
inline void ThreadPool::halt(const bool finish_tasks)
{
	if(!finish_tasks)  // don't wait for tasks to finish, stop all threads
	{
		if(m_haltFlag)  // if already stopped
		{
			return;
		}
		m_haltFlag = true;

		for(unsigned index = 0u, count = this->size(); index < count; index++)
		{
			*(m_flags[index]) = true;  // stop all threads, dereference from std::shared_ptr
		}
		this->clearQueue();
	}
	else  // finish remaining tasks and then stop all threads
	{
		if(m_finishedFlag || m_haltFlag)
		{
			return;
		}
		m_finishedFlag = true;  // all waiting threads should finish their tasks
	}

	{  // safety scope for std::unique_lock
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_all();  // sync all waiting threads
	}

	for(unsigned index = 0u; index < static_cast<unsigned>(m_threads.size()); index++)
	{
		if(m_threads[index]->joinable())  // wait for all working threads to finish their tasks
		{
			m_threads[index]->join();
		}
	}

	// if there were no threads in the pool, but some functors in the queue, the functors are not
	//   deleted by the threads, therefore they must be deleted here
	this->clearQueue();
	m_threads.clear();
	m_flags.clear();
}

void ThreadPool::restart()
{
	this->halt(true);  // wait for threads to finish all queued tasks
	m_finishedFlag = false;
	m_haltFlag = false;
	m_waitingThreadsCount = 0u;
	m_pendingTasksCount = 0u;
}

// run the user's task that accepts argument int - id of the running thread
// return value is a templatized operator, which returns std::future, where the user can get the
//   result and rethrow the caught exceptions
template <typename Functor>
inline auto ThreadPool::addTask(Functor &&func) -> std::future<decltype(func(0))>
{
	if(!m_finishedFlag && !m_haltFlag)
	{
		auto package = std::make_shared<std::packaged_task<decltype(func(0))(const int)>>
			(std::forward<Functor>(func));

		auto task = new std::function<void(const int)>(
		[package](const int id)
		{
			(*package)(id);
		});

		m_pendingTasksCount++;
		m_queue.push(task);

		// notify a waiting thread that the task has been added
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_one();
		return package->get_future();
	}
	else
	{
		return std::future<decltype(func(0))>();
	}
}

template <typename Functor>
inline auto ThreadPool::addTask(Functor &&func) -> std::future<decltype(func())>
{
	if(!m_finishedFlag && !m_haltFlag)
	{
		auto package = std::make_shared<std::packaged_task<decltype(func())()>>
			(std::forward<Functor>(func));

		auto task = new std::function<void(const int)>(
		[package](const int id)
		{
			(*package)();
		});

		m_pendingTasksCount++;
		m_queue.push(task);

		// notify a waiting thread that the task has been added
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_one();
		return package->get_future();
	}
	else
	{
		return std::future<decltype(func())>();
	}
}

//  passing functor of whatever arguments and return type, which is then wrapped in
//    void task(const int id)
template <typename Functor, typename... Args>
inline auto ThreadPool::addTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(0, arguments...))>
{
	if(!m_finishedFlag && !m_haltFlag)
	{
		auto package = std::make_shared<std::packaged_task<decltype(func(0, arguments...))(const int)>>
		(
			std::bind(std::forward<Functor>(func), std::placeholders::_1, std::forward<Args>(arguments)...)
		);

		auto task = new std::function<void(const int)>(
		[package](const int id)
		{
			(*package)(id);
		});

		m_pendingTasksCount++;
		m_queue.push(task);

		// notify a waiting thread that the task just has been added
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_one();
		return package->get_future();
	}
	else
	{
		return std::future<decltype(func(0, arguments...))>();
	}
}

template <typename Functor, typename... Args>
inline auto ThreadPool::addTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(arguments...))>
{
	if(!m_finishedFlag && !m_haltFlag)
	{
		auto package = std::make_shared<std::packaged_task<decltype(func(arguments...))()>>
		(
			std::bind(std::forward<Functor>(func), std::forward<Args>(arguments)...)
		);

		auto task = new std::function<void(const int)>(
		[package](const int id)
		{
			(*package)();
		});

		m_pendingTasksCount++;
		m_queue.push(task);

		// notify a waiting thread that the task just has been added
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_one();
		return package->get_future();
	}
	else
	{
		return std::future<decltype(func(arguments...))>();
	}
}

template <typename Functor>
inline auto ThreadPool::addInfiniteTask(Functor &&func) -> std::future<decltype(func(0))>
{
	if(!m_finishedFlag && !m_haltFlag)
	{
		auto package = std::make_shared<std::packaged_task<decltype(func(0))(const int)>>
			(std::forward<Functor>(func));

		auto task = new std::function<void(const int)>(
		[package, this](const int id)
		{
			while(!m_haltFlag && !(*m_flags.at(id)))
			{
				package->reset();
				(*package)(id);
			}
		});

		m_pendingTasksCount++;
		m_queue.push(task);

		// notify a waiting thread that the task has been added
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_one();
		return package->get_future();
	}
	else
	{
		return std::future<decltype(func(0))>();
	}
}

template <typename Functor>
inline auto ThreadPool::addInfiniteTask(Functor &&func) -> std::future<decltype(func())>
{
	if(!m_finishedFlag && !m_haltFlag)
	{
		auto package = std::make_shared<std::packaged_task<decltype(func())()>>
			(std::forward<Functor>(func));

		auto task = new std::function<void(const int)>(
		[package, this](const int id)
		{
			while(!m_haltFlag && !(*m_flags.at(id)))
			{
				package->reset();
				(*package)();
			}
		});

		m_pendingTasksCount++;
		m_queue.push(task);

		// notify a waiting thread that the task has been added
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_one();
		return package->get_future();
	}
	else
	{
		return std::future<decltype(func())>();
	}
}

template <typename Functor, typename... Args>
inline auto ThreadPool::addInfiniteTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(0, arguments...))>
{
	if(!m_finishedFlag && !m_haltFlag)
	{
		auto package = std::make_shared<std::packaged_task<decltype(func(0, arguments...))(const int)>>
		(
			std::bind(std::forward<Functor>(func), std::placeholders::_1, std::forward<Args>(arguments)...)
		);

		auto task = new std::function<void(const int)>(
		[package, this](const int id)
		{
			while(!m_haltFlag && !(*m_flags.at(id)))
			{
				package->reset();
				(*package)(id);
			}
		});

		m_pendingTasksCount++;
		m_queue.push(task);

		// notify a waiting thread that the task just has been added
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_one();
		return package->get_future();
	}
	else
	{
		return std::future<decltype(func(0, arguments...))>();
	}
}

template <typename Functor, typename... Args>
inline auto ThreadPool::addInfiniteTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(arguments...))>
{
	if(!m_finishedFlag && !m_haltFlag)
	{
		auto package = std::make_shared<std::packaged_task<decltype(func(arguments...))()>>
		(
			std::bind(std::forward<Functor>(func), std::forward<Args>(arguments)...)
		);

		auto task = new std::function<void(const int)>(
		[package, this](const int id)
		{
			while(!m_haltFlag && !(*m_flags.at(id)))
			{
				package->reset();
				(*package)();
			}
		});

		m_pendingTasksCount++;
		m_queue.push(task);

		// notify a waiting thread that the task just has been added
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.notify_one();
		return package->get_future();
	}
	else
	{
		return std::future<decltype(func(arguments...))>();
	}
}

// Threads pop tasks from the queue until:
// 1) The queue is empty, then it waits (idle state);
// 2) Its flag is set to true, then it terminates without emptying the queue;
// 3) A global halt flag is set to true, then only idle threads terminate.
inline void ThreadPool::setupThread(const int index)
{
	std::shared_ptr<std::atomic<bool>> flag(m_flags[index]);  // a copy of shared ptr to the flag
	auto task_wrapper = [this, index, flag]()
	{
		std::atomic<bool> &flag_ref = *flag;
		std::function<void(const int)> *task = nullptr;
		bool any_available = m_queue.pop(task);  // true if popped any task, false otherwise
		while(true)
		{
			while(any_available)  // if there's any task in the queue
			{
				m_pendingTasksCount--;
				std::unique_ptr<std::function<void(const int)>> uq_task(task);  // the pointer here
				  // holds the task object just for automatic deletion of it after it is executed
				  // (at the end of this loop cycle)
				(*task)(index);  //  execute task
				if(flag_ref)
				{
					return;  // return even if the queue is not empty
				}
				else
				{
					any_available = m_queue.pop(task);  // assign next task
				}
			}
			// the queue is empty (there are no tasks waiting for execution)
			std::unique_lock<std::mutex> lock(m_mutex);
			m_waitingThreadsCount++;
			m_cond.wait(lock, [this, &task, &any_available, &flag_ref]()
			{
				any_available = m_queue.pop(task);
				return any_available || m_finishedFlag || flag_ref;
			});
			m_waitingThreadsCount--;
			if(!any_available)
			{
				return;  // if queue is empty and (m_finishedFlag == true or *flag == true) then return
			}
		}
	};
	m_threads[index].reset(new std::thread(task_wrapper));
}

inline void ThreadPool::clearQueue()
{
	std::function<void(const int id)> *task = nullptr;
	while(m_queue.pop(task))
	{
		delete task;
	}
}

}  // namespace ecs
