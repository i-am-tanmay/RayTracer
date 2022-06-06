#pragma once
#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <future>


namespace Library
{
	class ThreadPool final
	{
	public:
		ThreadPool();
		ThreadPool(const std::size_t thread_count);
		~ThreadPool();

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;

		void EnqueueTask(std::function<void()>&& function);

		std::size_t Thread_Count() const { return _threads.size(); }
		void Resume();
		void Pause();
		void Stop();

	private:
		void IdleWorker();

	private:
		std::vector<std::thread> _threads;
		std::mutex _taskqueue_mutex;
		std::queue<std::function<void()>> _taskqueue;
		std::condition_variable _threadpool_notifier;

		bool _is_paused{ false };
		bool _is_stopped{ false };
	};
}