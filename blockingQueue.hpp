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
    void push(T const& value);
    T pop();
};