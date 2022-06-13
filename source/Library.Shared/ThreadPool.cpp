#include "pch.h"
#include "ThreadPool.h"
#include <memory>

namespace Library
{
	ThreadPool::ThreadPool(const std::size_t thread_count)
	{
		if (thread_count == 0) throw std::runtime_error("ThreadPool cannot be of size 0");

		_threads.reserve(thread_count);
		for (std::size_t i = 0; i < thread_count; ++i) _threads.emplace_back([this]() { IdleWorker(); });
	}

	ThreadPool::~ThreadPool()
	{
		Stop();

		for (auto& task_thread : _threads)
			if (task_thread.joinable()) task_thread.join();
	}

	void ThreadPool::EnqueueTask(std::function<void()>&& function)
	{
		if (_is_stopped) throw std::runtime_error("ThreadPool had been stopped");

		{
			std::lock_guard<std::mutex> taskqueue_lock{ _taskqueue_mutex };

			std::shared_ptr<std::packaged_task<void()>> task = std::make_shared<std::packaged_task<void()>>(std::forward<std::function<void()>>(function));
			_futures.push(task->get_future());
			_taskqueue.push(task);
		}

		// Wait till a thread becomes available to work
		_threadpool_notifier.notify_one();
	}

	void ThreadPool::JoinAllTasks()
	{
		std::size_t size = _futures.size();
		for (std::size_t i = 0; i < size; ++i)
		{
			_futures.front().get();
			_futures.pop();
		}
	}

	void ThreadPool::Stop()
	{
		{
			std::lock_guard<std::mutex> taskqueue_lock{ _taskqueue_mutex };
			_is_stopped = true;
		}

		_threadpool_notifier.notify_all();
	}

	void ThreadPool::ClearPending()
	{
		std::lock_guard<std::mutex> taskqueue_lock{ _taskqueue_mutex };

		std::size_t size = _taskqueue.size();
		for (std::size_t i = 0; i < size; ++i)
			_taskqueue.pop();
	}

	void ThreadPool::Resume()
	{
		{
			std::lock_guard<std::mutex> taskqueue_lock{ _taskqueue_mutex };
			_is_paused = false;
		}

		_threadpool_notifier.notify_all();
	}

	void ThreadPool::Pause()
	{
		{
			std::lock_guard<std::mutex> taskqueue_lock{ _taskqueue_mutex };
			_is_paused = true;
		}

		_threadpool_notifier.notify_all();
	}

	void ThreadPool::IdleWorker()
	{
		while (true)
		{
			std::shared_ptr<std::packaged_task<void()>> task;

			{
				std::unique_lock<std::mutex> taskqueue_lock{ _taskqueue_mutex };

				// Wait for new task, ie, Wait IF:
				//	(1) No Tasks in Queue
				//	(2) ThreadPool is Paused
				//	(3) ThreadPool is Stopped
				_threadpool_notifier.wait(taskqueue_lock,
					[this]() { return (!_taskqueue.empty() && !_is_paused) || _is_stopped; }
				);

				// Terminate Thread
				if (_is_stopped) return;

				task = std::move(_taskqueue.front());
				_taskqueue.pop();
			}

			(*task)();	// Do the Task
		}
	}
}