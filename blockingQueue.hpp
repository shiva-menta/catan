#include <mutex>
#include <condition_variable>
#include <deque>

template <typename T>
class queue
{
private:
    std::mutex              mutex;
    std::condition_variable condition;
    std::deque<T>           queue;
public:
    void push(T const& value);
    T pop();
};