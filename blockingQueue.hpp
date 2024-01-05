#include <mutex>
#include <condition_variable>
#include <deque>

template <typename T>
class blockingQueue
{
    private:
        std::mutex              _mutex;
        std::condition_variable condition;
        std::deque<T>           queue;
    public:
        void push(T const& value) {
            {
                std::unique_lock<std::mutex> lock(this->_mutex);
                queue.push_front(value);
            }
            this->condition.notify_one();
        }
        T pop() {
            std::unique_lock<std::mutex> lock(this->_mutex);
            this->condition.wait(lock, [=]{ return !this->queue.empty(); });
            T rc(move(this->queue.back()));
            this->queue.pop_back();
            return rc;
        }
};