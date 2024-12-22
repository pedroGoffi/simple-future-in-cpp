#ifndef FUTURE_HPP
#define FUTURE_HPP
#include <functional>
#include <vector>
#include <iostream>

// simple future
namespace sfut {
// Forward declarations
class Future;
class Done;
class Reject;
class Then;

// Base interface for Future

class Future {
public:
    virtual ~Future() = default;
    virtual void* poll(void* data) = 0;

    // then function declaration
    Then* then(std::function<Future*(void*)> callback);
};

// Done class declaration
class Done : public Future {
public:
    Done(void* result);
    void* poll(void* data) override;

private:
    void* result;
};

// Reject class declaration
class Reject : public Future {
public:
    Reject(void* excuse);
    void* poll(void* data) override;

private:
    void* excuse;
};

// Then class declaration
class Then : public Future {
public:
    Then(Future* left, std::function<Future*(void*)> callback);
    ~Then(); // Proper cleanup for chained futures
    void* poll(void* data) override;
    Future* then(std::function<Future*(void*)> callback);


private:
    Future* left;
    Future* right;
    std::function<Future*(void*)> callback;
};

class EventLoop {
    std::vector<Future*> futures;

public:
    void addFuture(Future* future);
    void run();
};


// Helper functions
Done* done(void* value);
Reject* reject(void* excuse);
void gather(std::vector<Future*>& futures);
void gather(std::vector<Future*> futures);
void* run(Future* future);
Future* await(Future* future);
}
#endif // FUTURE_HPP