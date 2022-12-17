#pragma once
#include <mutex>
#include <list>
#include <optional>

namespace zcockpit::common {
	template<typename T>
	class ThreadSafeQueue {
		std::list<T> queue_;
		mutable std::mutex mutex_;

		[[nodiscard]] bool empty() const {
			return queue_.empty();
		}

	public:
		ThreadSafeQueue() = default;
		~ThreadSafeQueue() = default;
		ThreadSafeQueue(const ThreadSafeQueue<T>&) = delete;
		ThreadSafeQueue& operator=(const ThreadSafeQueue<T>&) = delete;
		ThreadSafeQueue(ThreadSafeQueue<T>&& other) = delete;

		[[nodiscard]] auto size() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return queue_.size();
		}

		std::optional<T> pop() {
			std::lock_guard<std::mutex> lock(mutex_);
			if (queue_.empty()) {
				return std::nullopt;
			}
			T temp = queue_.front();
			queue_.pop_front();
			return temp;
		}

		void push(T item) {
			std::list<T> tmp;
			tmp.push_back(std::move(item));
			std::lock_guard<std::mutex> lock(mutex_);
			queue_.splice(std::end(queue_), tmp);
		}
	};
}