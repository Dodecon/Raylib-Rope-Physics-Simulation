#include <vector>
#include <queue>
#include <functional>
#include <atomic>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <latch>


class Threadpool
{
private:

	// stores threads
	std::vector<std::thread> threads;

	// stores tasks
	std::queue<std::function<void()>> taskQueue;

	// a mutex and a condition variable for the taskQueue
	std::mutex queue_mtx;
	std::condition_variable queue_cv;

	// a flag used to destroy the threadpool
	std::atomic<bool> end_Pool;


public:

	int ThreadCount;

	Threadpool(size_t threadAmount)
		: ThreadCount(threadAmount)
	{
		// initialize end flag
		end_Pool.store(false);

		for (size_t i = 0; i < threadAmount; ++i)
		{
			// fill the threads vector
			threads.emplace_back([this] {

				for (;;) {

					// check for available tasks and perform them

					std::function<void()>task;

					// put threads to sleep if no tasks available
					std::unique_lock<std::mutex> lock(queue_mtx);
					queue_cv.wait(lock, [this] {return !taskQueue.empty() || end_Pool.load() == true; });

					//break the cycle if we wish to end the threadpool
					if (end_Pool.load() == true && taskQueue.empty()) {
						return;
					}

					// get a task from the queue

					task = std::move(taskQueue.front());
					// delite a task from the queue
					taskQueue.pop();

					// let other threads access the queue
					lock.unlock();
					//perform the task
					task();
				}
				});

		}
	};

	// a destructor
	~Threadpool() {

		end_Pool = true;

		// wake up all threads
		queue_cv.notify_all();

		// join the threads
		for (std::thread& t : threads) {

			if (t.joinable()) {
				t.join();
			}
		}
	};

	//add new tasks to the queue. fire and forget
	template<typename Func, typename... Args>
	void EnQueue(Func&& func, Args&&... args) {

		//create a task
		auto task = [func = std::move(func), args...]() {
			func(args...);
			};

		{
			std::lock_guard<std::mutex>lock(queue_mtx);

			// push the task
			taskQueue.push(std::move(task));
		}
		// wake up one thread to perform the task
		queue_cv.notify_one();
	};


	// put multiple tasks to the tasks queue ONCE
	template<typename Func>
	void EnqueueBatch(std::vector<Func>& tasks)
	{
		if (tasks.empty()) return;

		{
			// Lock once
			std::lock_guard<std::mutex> lock(queue_mtx);

			// Move all tasks into the queue while holding the lock
			for (auto& task : tasks) {
				taskQueue.push(std::move(task));
			}
		}

		// Notify all sleeping threads to wake up and grab work
		// We don't know exactly how many to notify, so wake everyone (fast enough)
		queue_cv.notify_all();
	}

	//evenly split the job across threads
	template<typename Func>
	void ParralelFor(int startIndex, int endIndex, Func&& func) {

		int numThreads = threads.size();
		int totalIndecies = (endIndex - startIndex);
		if (totalIndecies < 0) return;

		if (totalIndecies < numThreads) numThreads = totalIndecies;
		if (numThreads == 0) return;

		int chunkSize = totalIndecies / numThreads;

		std::latch latch(numThreads);

		for (size_t t = 0; t < numThreads; ++t) {

			int chunkStart = startIndex + chunkSize * t;
			int chunkEnd = (t == numThreads - 1) ? endIndex : (chunkStart + chunkSize);


			EnQueue([&, chunkStart, chunkEnd] {

				for (size_t i = 0; i < chunkEnd - chunkStart; i++) {

					func(chunkStart + i);
				}
				latch.count_down();
			});

		}
		latch.wait();
	}
};