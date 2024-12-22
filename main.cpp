#include <iostream>
#include "./simplefuture/future.cpp"
#include <vector>
// Counter class declaration
class Counter : public sfut::Future {
public:
    Counter(int start, int end);
    void* poll(void* data) override;

private:
    int i, n;
};

// Counter implementation
Counter::Counter(int start, int end) : i(start), n(end) {}

void* Counter::poll(void* data) {
    if (i < n) {
        std::cout << i << std::endl;
        i++;
        return nullptr;  // Continue polling
    } else {
        return new int(n);  // Return the final result
    }
}

sfut::Future* example_future()
{
    sfut::Future* future = sfut::done(new int(69))
            ->then([](void *result) -> sfut::Future *{
                std::cout << *static_cast<int*>(result) << std::endl;            
                return sfut::done(new int(420));
            })
            ->then([](void *result) -> sfut::Future *{
                std::cout << *static_cast<int*>(result) << std::endl;
                return sfut::done(new int(1337));
            });

    
    return sfut::await(future);
}
int main() {
    sfut::Future* counter = (new Counter(0, 10))->then([](void* result) -> sfut::Future* {
        int x = *(int*) result;
        return new Counter(x, x + 10);
    });
    sfut::Future* future = example_future();    

    std::vector<sfut::Future*> futures;

    sfut::gather({counter, future});
    return 0;
}