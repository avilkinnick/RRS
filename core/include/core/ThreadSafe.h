#ifndef THREAD_SAFE_H
#define THREAD_SAFE_H

#include <mutex>

template <typename T>
class ThreadSafeValue
{
public:
    ThreadSafeValue(T& value, std::mutex& mutex)
        : value(value)
        , mutex(mutex)
    {
        mutex.lock();
    }

    ~ThreadSafeValue()
    {
        mutex.unlock();
    }

    ThreadSafeValue(const ThreadSafeValue&) = delete;
    ThreadSafeValue& operator=(const ThreadSafeValue&) = delete;
    ThreadSafeValue(ThreadSafeValue&&) = delete;
    ThreadSafeValue& operator=(ThreadSafeValue&&) = delete;

    T& operator*() { return value; }
    const T& operator*() const { return value; }

    T* operator->() { return &value; }
    const T* operator->() const { return &value; }

private:
    T& value;
    std::mutex& mutex;
};

template <typename T>
class ThreadSafe
{
public:
    ThreadSafeValue<T> lock()
    {
        return ThreadSafeValue<T>(value, mutex);
    }

private:
    T value;
    std::mutex mutex;
};

#endif // THREAD_SAFE_H
