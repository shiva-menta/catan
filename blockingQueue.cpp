#include <mutex>
#include <condition_variable>
#include <deque>

using namespace std;

template <typename T>
class queue
{
private:
    mutex              mutex;
    condition_variable condition;
    deque<T>           queue;
public:
    void push(T const& value) {
        {
            unique_lock<mutex> lock(this->mutex);
            queue.push_front(value);
        }
        this->d_condition.notify_one();
    }
    T pop() {
        unique_lock<mutex> lock(this->mutex);
        this->condition.wait(lock, [=]{ return !this->queue.empty(); });
        T rc(move(this->queue.back()));
        this->queue.pop_back();
        return rc;
    }
};