# pragma once

#include <coroutine>
#include <exception>
#include <utility>

template<typename T> class Generator {
public:
	struct promise_type {
		T current_value_;
		std::exception_ptr exception_;
		Generator get_return_object() {
			return Generator{ std::coroutine_handle<promise_type>::from_promise(*this) };
		}
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		std::suspend_always yield_value(T value) {
			current_value_ = std::move(value);
			return {};
		}
		void return_void() {}
		void unhandled_exception() {
			exception_ = std::current_exception();
		}
	};
private:
	std::coroutine_handle<promise_type> handle_;
	
public:
	bool next() {
		if (!handle_ || handle_.done()) {
			return false;
		}
		handle_.resume();
		if (handle_.done()) {
			if (handle_.promise().exception_) {
				std::rethrow_exception(handle_.promise().exception_);
			}
			return false;
		}
		return true;
	}
	T value() {
		return std::move(handle_.promise().current_value_);
	}
	~Generator() {
		if (handle_) {
			handle_.destroy();
		}
	}
	Generator(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
	Generator(const Generator&) = delete;
	Generator& operator=(const Generator&) = delete;
	Generator(Generator&& other) noexcept : handle_(other.handle_) {
		other.handle_ = nullptr;
	}
	Generator& operator=(Generator&& other) noexcept {
		if (this != &other) {
			if (handle_) {
				handle_.destroy();
			}
			handle_ = other.handle_;
			other.handle_ = nullptr;
		}
		return *this;
	}
};
