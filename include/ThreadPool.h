#pragma once

#include "Root.h"

#define NODISCARD_REASON "The returned value is not used. Calling this method is unnecessary."
#define NDMESSAGE [[nodiscard(NODISCARD_REASON)]]

namespace ecs
{

namespace impl
{

/**
 * @brief Class representing thread-safe queue.
 * @tparam T The type of objects/values held by SafeQueue.
 */
template <typename T>
class SafeQueue
{
public:
	/**
	 * @brief Adds a new value to the queue.
	 * @param value The new value.
	 * @return True if pushed successfully, false otherwise.
	 */
	const bool push(const T &value);

	/**
	 * @brief Removes the first value from the queue.
	 * @param v The value removed from the queue (it is an output result).
	 * @return True if removed an element successfully, false otherwise.
	 */
	const bool pop(T & value);

	/**
	 * @brief Checks if the queue is empty.
	 * @return True if empty, false otherwise.
	 */
	const bool empty();

	/**
	 * @brief Appends an other queue instance to the end of this instance.
	 * @param other The other queue instance.
	 * @return This instance with the appended other queue.
	 */
	SafeQueue<T> &append(SafeQueue<T> &&other);

	/**
	 * @brief Merges this instance with an other queue.
	 * @param other The other queue instance.
	 * @return This instance merged with the other instance.
	 * 
	 * Merge() is different from append(), because order of held values is preserved.
	 * Example:
	 * Q1 = { A B C D E }
	 * Q2 = { X Y Z }
	 * Q1.merge(Q2) = { A X B Y C Z D E }
	 */
	SafeQueue<T> &merge(SafeQueue<T> &&other);

	/**
	 * @brief Access to the front of the queue.
	 * @return The first value in the queue.
	 */
	T &front();

	/**
	 * @brief Access to the back of the queue.
	 * @return The last value in the queue.
	 */
	T &back();

	/**
	 * @brief Access to const value at the front of the queue.
	 * @return The const first value in the queue.
	 */
	const T &front() const;

	/**
	 * @brief Access to const value at the back of the queue.
	 * @return The const last value in the queue.
	 */
	const T &back() const;

private:
	std::queue<T> m_queue;  /**< The container holding values of SafeQueue. */
	std::mutex m_mutex;		/**< The global mutex of SafeQueue. */
};

}  // namespace impl

/**
 * @brief The class representing dynamic pool of computing threads.
 */
class ThreadPool
{
public:
	/**
	 * @brief The constructor of the ThreadPool class.
	 * @param thread_count The number of created threads.
	 * 
	 * By default, thread count is set to minimal logical value, which is 2. To unleash full power
	 *   of multithreading, check your hardware capabilities and pass the maximal value.
	 * @code
	 * std::size_t thcount = std::thread::hardware_concurrency
	 * @endcode
	 */
	ThreadPool(const unsigned thread_count = 2u);

	ThreadPool(const ThreadPool &) = delete;			/**< Deleted copy constructor. */
    ThreadPool(ThreadPool &&) = delete;					/**< Deleted move constructor. */
    ThreadPool &operator=(const ThreadPool &) = delete;	/**< Deleted copy assignment. */
    ThreadPool &operator=(ThreadPool &&) = delete;		/**< Deleted move assignment. */
	virtual ~ThreadPool();	/**< The virtual destructor, which calls halt(true) and haltInfinite(). */

	/**
	 * @brief Gets the thread instance from the pool.
	 * @param id The index of the thread.
	 * @return The thread instance.
	 */
	std::thread &getThread(const unsigned id);
	
	/**
	 * @brief Gets the total number of threads in the pool.
	 * @return The thread count.
	 */
	NDMESSAGE const unsigned totalThreadCount() const;

	/**
	 * @brief Gets the number of idle threads in the pool.
	 * @return The idle thread count.
	 */
	NDMESSAGE const unsigned idleThreadCount() const;

	/**
	 * @brief Gets the number of pending tasks in the queue.
	 * @return The pending tasks count.
	 */
	NDMESSAGE const unsigned pendingTasksCount() const;

	/**
	 * @brief Resizes the thread pool (changes the number of working threads).
	 * @param thread_count The new thread count.
	 *
	 * If the new thread count is lower than the previous one, all extra threads will finish their
	 *   tasks and only then will be deleted.
	 * If the new thread count is greater, the existing threads are left uninterrupted.
	 */
	void resize(const unsigned thread_count);

	/**
	 * @brief Stops all threads.
	 * @param finish_tasks If true, all threads clear the queue before stopping, else they finish immediately.
	 *
	 * This member function also stops all infinite tasks.
	 */
	void halt(const bool finish_tasks = false);

	/**
	 * @brief Stops all threads with infinite tasks.
	 * 
	 * The behaviour is not the same as in halt() member function, because this one only halts
	 *   currently running infinite tasks. Status of queued tasks availability is not changed
	 *   (besides that all threads which were computing infinite tasks are now free to empty the
	 *   queue).
	 * This method DOES NOT halt any threads for good, it only breaks the while(true) loops in
	 *   infinite tasks.
	 * It is also called by the ThreadPool destructor.
	 *
	 * @warning In order to add a new infinite task to the queue, ThreadPool has to be restarted
	 *          with restart() member function.
	 */
	void haltInfinite();

	/**
	 * @brief Restarts the thread pool.
	 * 
	 * Calls halt(true) and sets all flags to their initial values.
	 */
	void restart();

	/**
	 * @brief Adds a new task to the queue.
	 * @param func The function which will be executed by one of the threads.
	 * @tparam Functor Signature of the mentioned function. User doesn't need to explicitly specify it.
	 * @return The std::future of the passed task, which after execution will hold the returned result (if any).
	 *
	 * Functions require the first argument to have a const int, because addTask() passes a thread
	 *   id to it for the use by the function.
	 *
	 * Example:
	 * @code
	 * void task(const int thread_id) { std::cout << "Hello from thread " << thread_id; }
	 *
	 * int main(){  // main as an example, the caller function/method can be whatever
	 *     ecs::ThreadPool TP(4);  // setting thread count to 4
	 *     TP.addTask(task);
	 * }
	 * // Output: Hello from thread <0-3>
	 * @endcode
	 */
	template <typename Functor>
	auto addTask(Functor &&func) -> std::future<decltype(func(0))>;

	/**
	 * @brief Adds a new task to the queue.
	 * @param func The function which will be executed by one of the threads.
	 * @tparam Functor Signature of the mentioned function. User doesn't need to explicitly specify it.
	 * @return The std::future of the passed task, which after execution will hold the returned result (if any).
	 * 
	 * This method is and overload of the previous addTask() and it does not require to add a const
	 *   int argument to the function parameters.
	 *
	 * Example:
	 * @code
	 * void task() { std::cout << "Hello world!"; }
	 *
	 * int main(){  // main as an example, the caller function/method can be whatever
	 *     ecs::ThreadPool TP(4);  // setting thread count to 4
	 *     TP.addTask(task);
	 * }
	 * // Output: Hello world!
	 * @endcode
	 */
	template <typename Functor>
	auto addTask(Functor &&func) -> std::future<decltype(func())>;

	/**
	 * @brief Adds a new task to the queue.
	 * @param func The function which will be executed by one of the threads.
	 * @param arguments The extra arguments passed to the function func.
	 * @tparam Functor Signature of the mentioned function. User doesn't need to explicitly specify it.
	 * @tparam Args Types of the extra erguments passed to the function func.
	 * @return The std::future of the passed task, which after execution will hold the returned result (if any).
	 * 
	 * Functions require the first argument to have a const int, because addTask() passes a thread
	 *   id to it for the use by the function.
	 *
	 * Example:
	 * @code
	 * void task(const int id, std::string additional_parameter) { std::cout << additional_parameter << " from thread " << id; }
	 *
	 * int main(){  // main as an example, the caller function/method can be whatever
	 *     ecs::ThreadPool TP(4);  // setting thread count to 4
	 *     TP.addTask(task, "Hello");
	 * }
	 * // Output: Hello from thread <0-3>
	 * @endcode
	 */
	template <typename Functor, typename... Args>
	auto addTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(0, arguments...))>;

	/**
	 * @brief Adds a new task to the queue.
	 * @param func The function which will be executed by one of the threads.
	 * @param arguments The extra arguments passed to the function func.
	 * @tparam Functor Signature of the mentioned function. User doesn't need to explicitly specify it.
	 * @tparam Args Types of the extra erguments passed to the function func.
	 * @return The std::future of the passed task, which after execution will hold the returned result (if any).
	 * 
	 * This method is and overload of the previous addTask() and it does not require to add a const
	 *   int argument to the function parameters.
	 *
	 * Example:
	 * @code
	 * void task(std::string additional_parameter) { std::cout << additional_parameter; }
	 *
	 * int main(){  // main as an example, the caller function/method can be whatever
	 *     ecs::ThreadPool TP(4);  // setting thread count to 4
	 *     TP.addTask(task, "Hello world!");
	 * }
	 * // Output: Hello world!
	 * @endcode
	 */
	template <typename Functor, typename... Args>
	auto addTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(arguments...))>;

	/**
	 * @brief Adds a new infinite task to the queue.
	 * @param func The function which will be executed by one of the threads.
	 * @tparam Functor Signature of the mentioned function. User doesn't need to explicitly specify it.
	 * @return The std::future of the passed task, which will hold the returned result (if any) of the last execution.
	 *
	 * Functions require the first argument to have a const int, because addTask() passes a thread
	 *   id to it for the use by the function.
	 *
	 * The difference between addTask() and addInfiniteTask() is that in the latter the function's
	 *   body is nested inside a while(true) loop. This means that the function will be constantly
	 *   executed until halt() or haltInfinite() is called.
	 *
	 * Example:
	 * @code
	 * void task(const int thread_id) { std::cout << "Hello from thread " << thread_id << std::endl; }
	 *
	 * int main(){  // main as an example, the caller function/method can be whatever
	 *     ecs::ThreadPool TP(4);  // setting thread count to 4
	 *     TP.addInfiniteTask(task);
	 *     TP.halt(false);  // this line is important, because it stops the infinite task
	 * }
	 * // A is an index of the thread in <0-3> domain.
	 * // Output:
	 * // Hello from thread A
	 * // Hello from thread A
	 * // Hello from thread A
	 * // ...
	 * @endcode
	 *
	 */
	template <typename Functor>
	auto addInfiniteTask(Functor &&func) -> std::future<decltype(func(0))>;

	/**
	 * @brief Adds a new infinite task to the queue.
	 * @param func The function which will be executed by one of the threads.
	 * @tparam Functor Signature of the mentioned function. User doesn't need to explicitly specify it.
	 * @return The std::future of the passed task, which will hold the returned result (if any) of the last execution.
	 *
	 * This method is and overload of the previous addInfiniteTask() and it does not require to add
	 *   a const int argument to the function parameters.
	 *
	 * The difference between addTask() and addInfiniteTask() is that in the latter the function's
	 *   body is nested inside a while(true) loop. This means that the function will be constantly
	 *   executed until halt() or haltInfinite() is called.
	 *
	 * Example:
	 * @code
	 * void task() { std::cout << "Hello darkness my old friend..." << std::endl; }
	 *
	 * int main(){  // main as an example, the caller function/method can be whatever
	 *     ecs::ThreadPool TP(4);  // setting thread count to 4
	 *     TP.addInfiniteTask(task);
	 *     TP.halt(false);  // this line is important, because it stops the infinite task
	 * }
	 * // Output:
	 * // Hello darkness my old friend...
	 * // Hello darkness my old friend...
	 * // Hello darkness my old friend...
	 * // ...
	 * @endcode
	 *
	 */
	template <typename Functor>
	auto addInfiniteTask(Functor &&func) -> std::future<decltype(func())>;

	/**
	 * @brief Adds a new infinite task to the queue.
	 * @param func The function which will be executed by one of the threads.
	 * @param arguments The extra arguments passed to the function func.
	 * @tparam Functor Signature of the mentioned function. User doesn't need to explicitly specify it.
	 * @tparam Args Types of the extra erguments passed to the function func.
	 * @return The std::future of the passed task, which will hold the returned result (if any) of the last execution.
	 *
	 * Functions require the first argument to have a const int, because addTask() passes a thread
	 *   id to it for the use by the function.
	 *
	 * The difference between addTask() and addInfiniteTask() is that in the latter the function's
	 *   body is nested inside a while(true) loop. This means that the function will be constantly
	 *   executed until halt() or haltInfinite() is called.
	 *
	 * Example:
	 * @code
	 * void task(const int thread_id, const std::string &additional_parameter) { std::cout << additional_parameter << " from thread " << thread_id << std::endl; }
	 *
	 * int main(){  // main as an example, the caller function/method can be whatever
	 *     ecs::ThreadPool TP(4);  // setting thread count to 4
	 *     TP.addInfiniteTask(task, "Hello there");
	 *     TP.halt(false);  // this line is important, because it stops the infinite task
	 * }
	 * // A is an index of the thread in <0-3> domain.
	 * // Output:
	 * // Hello there from thread A
	 * // Hello there from thread A
	 * // Hello there from thread A
	 * // ...
	 * @endcode
	 *
	 */
	template <typename Functor, typename... Args>
	auto addInfiniteTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(0, arguments...))>;

	/**
	 * @brief Adds a new infinite task to the queue.
	 * @param func The function which will be executed by one of the threads.
	 * @param arguments The extra arguments passed to the function func.
	 * @tparam Functor Signature of the mentioned function. User doesn't need to explicitly specify it.
	 * @tparam Args Types of the extra erguments passed to the function func.
	 * @return The std::future of the passed task, which will hold the returned result (if any) of the last execution.
	 *
	 * This method is and overload of the previous addInfiniteTask() and it does not require to add
	 *   a const int argument to the function parameters.
	 *
	 * The difference between addTask() and addInfiniteTask() is that in the latter the function's
	 *   body is nested inside a while(true) loop. This means that the function will be constantly
	 *   executed until halt() or haltInfinite() is called.
	 *
	 * Example:
	 * @code
	 * void task(const std::string &additional_parameter) { std::cout << additional_parameter << std::endl; }
	 *
	 * int main(){  // main as an example, the caller function/method can be whatever
	 *     ecs::ThreadPool TP(4);  // setting thread count to 4
	 *     TP.addInfiniteTask(task, "Hello darkness my old friend...");
	 *     TP.halt(false);  // this line is important, because it stops the infinite task
	 * }
	 * // Output:
	 * // Hello darkness my old friend...
	 * // Hello darkness my old friend...
	 * // Hello darkness my old friend...
	 * // ...
	 * @endcode
	 *
	 */
	template <typename Functor, typename... Args>
	auto addInfiniteTask(Functor &&func, Args&& ...arguments) -> std::future<decltype(func(arguments...))>;

private:
	/**
	 * @brief Creates and initializes a thread for computing.
	 * @param index The index of the thread in the container.
	 *
	 * @warning For internal use only.
	 */
	void setupThread(const int index);

	/**
	 * @brief Clears the queue of tasks.
	 *
	 * @warning For internal use only.
	 */
	void clearQueue();

private:
	std::vector<std::unique_ptr<std::thread>> m_threads;  /**< The container for threads. */
	std::vector<std::shared_ptr<std::atomic<bool>>> m_abortFlags;  /**< Abort flags for threads. */
	impl::SafeQueue<std::function<void(const int)> *> m_queue;  /**< The queue of tasks assigned by the user. */
	std::atomic<bool> m_finishedFlag;  /**< The flag describing if all tasks have benn completed. */
	std::atomic<bool> m_haltFlag;  /**< The global flag used for hard halting computation of all threads. */
	std::atomic<bool> m_infHaltFlag;  /**< The global flag used for breaking infinite tasks. */
	std::atomic<unsigned> m_waitingThreadsCount;  /**< The number of idle threads. */
	std::atomic<unsigned> m_pendingTasksCount;  /**< The number of pending tasks in the queue. */

	std::mutex m_mutex;  /**< The global mutex of the ThreadPool class. */
	std::condition_variable m_cond;  /**< The global condition variable used for notifying and syncing all threads. */
};

}  // namespace ecs

#include "../src/ThreadPool.inl"
