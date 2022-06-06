#include "pch.h"
#include "ThreadPool.h"
#include <memory>

namespace Library
{
	ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency())
	{
	}

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
		{
			std::lock_guard<std::mutex> taskqueue_lock{ _taskqueue_mutex };

			if (_is_stopped) throw std::runtime_error("ThreadPool had been stopped");
			
			std::shared_ptr<std::packaged_task<void()>> task = std::make_shared<std::packaged_task<void()>>(std::forward<std::function<void()>>(function));
			_futures.push_back(task->get_future());
			_taskqueue.push(task);
		}

		// Wait till a thread becomes available to work
		_threadpool_notifier.notify_one();
	}

	void ThreadPool::JoinAllTasks()
	{
		/*std::unique_lock<std::mutex> taskqueue_lock{ _taskqueue_mutex };

		_threadpool_notifier.wait(taskqueue_lock,
			[this]() { return _taskqueue.empty(); }
		);*/

		for (std::size_t i = 0; i < _futures.size(); ++i)
			_futures[i].get();
	}

	void ThreadPool::Stop()
	{
		{
			std::lock_guard<std::mutex> taskqueue_lock{ _taskqueue_mutex };
			_is_stopped = true;
		}

		_threadpool_notifier.notify_all();
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

				// Wait IF:
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

				//_threadpool_notifier.notify_all();
			}

			(*task)();	// Do the Task
		}
	}
}