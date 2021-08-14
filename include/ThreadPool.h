#pragma once

#include "Root.h"

#define NODISCARD_REASON "The returned value is not used. Calling this method is unnecessary."
#define NDMESSAGE [[nodiscard(NODISCARD_REASON)]]

namespace ecs
{

namespace impl
{

template <typename T>
class SafeQueue
{
public:
	const bool push(const T &value);
	const bool pop(T & v);
	const bool empty();
	SafeQueue<T> &append(SafeQueue<T> &&other);
	SafeQueue<T> &merge(SafeQueue<T> &&other);
	T &front();
	T &back();
	const T &front() const;
	const T &back() const;

private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
};

}  // namespace impl

class ThreadPool
{
public:
	ThreadPool(const unsigned thread_count = 2u);
	virtual ~ThreadPool();

	std::thread &getThread(const unsigned id);
	
	NDMESSAGE const unsigned getThreadCount() const;
	NDMESSAGE const unsigned idleThreadCount() const;
	NDMESSAGE const unsigned pendingTasksCount() const;
	void resize(const unsigned thread_count);
	void halt(const bool finish_tasks = false);
	void restart();

	template <typename Functor> auto addTask(Functor &&func) -> std::future<decltype(func(0))>;
	template <typename Functor> auto addTask(Functor &&func) -> std::future<decltype(func())>;

	template <typename Functor, typename... Args> auto addTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(0, arguments...))>;
	template <typename Functor, typename... Args> auto addTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(arguments...))>;

	template <typename Functor> auto addInfiniteTask(Functor &&func) -> std::future<decltype(func(0))>;
	template <typename Functor> auto addInfiniteTask(Functor &&func) -> std::future<decltype(func())>;

	template <typename Functor, typename... Args> auto addInfiniteTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(0, arguments...))>;
	template <typename Functor, typename... Args> auto addInfiniteTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(arguments...))>;

private:
	void setupThread(const int index);
	void clearQueue();

private:
	std::vector<std::unique_ptr<std::thread>> m_threads;  // threads
	std::vector<std::shared_ptr<std::atomic<bool>>> m_flags;  // stop flags for threads
	impl::SafeQueue<std::function<void(const int)> *> m_queue;  // queue of tasks
	std::atomic<bool> m_finishedFlag;  // are all tasks finished
	std::atomic<bool> m_haltFlag;  // should all threads stop working
	std::atomic<unsigned> m_waitingThreadsCount;  // number of threads waiting for tasks
	std::atomic<unsigned> m_pendingTasksCount;  // pending tasks in the queue

	std::mutex m_mutex;
	std::condition_variable m_cond;  // sync condition
};

}  // namespace ecs

#include "../src/ThreadPool.inl"
