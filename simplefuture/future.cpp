#ifndef FUTURE_CPP
#define FUTURE_CPP
#include "Future.hpp"

namespace sfut {

// Implementation of Future::then
Then* Future::then(std::function<Future*(void*)> callback) {
    return new Then(this, callback);
}

// Done implementation
Done::Done(void* result) : result(result) {}

void* Done::poll(void* data) {
    return result;
}

// Reject implementation
Reject::Reject(void* excuse) : excuse(excuse) {}

void* Reject::poll(void* data) {
    return excuse;
}

// Then implementation
Then::Then(Future* left, std::function<Future*(void*)> callback)
    : left(left), callback(callback), right(nullptr) {}

Then::~Then() {
    delete left;  // Clean up the left Future
    delete right; // Clean up the right Future if it was allocated
}
Future* Then::then(std::function<Future*(void*)> callback) {
    return new Then(this, callback);
}
void* Then::poll(void* data) {
    if (left != nullptr) {
        void* result = left->poll(data);
        if (result != nullptr) {
            right = callback(result);
            left = nullptr;
        }
        return nullptr;
    } else {
        return right->poll(data);
    }
}



// Helper functions
Done* done(void* value) {
    return new Done(value);
}

Reject* reject(void* excuse) {
    return new Reject(excuse);
}

void gather(std::vector<Future*>& futures) {
    bool quit = false;
    while (!quit) {
        quit = true;
        for (auto& future : futures) {
            if (future->poll(nullptr) == nullptr) {
                quit = false;
            }
        }
    }
}

void gather(std::vector<Future*> futures) {
    bool quit = false;
    while (!quit) {
        quit = true;
        for (auto& future : futures) {
            if (future->poll(nullptr) == nullptr) {
                quit = false;
            }
        }
    }
}


void* run(Future* future){
    while(true){
        void* result = future->poll(nullptr);
        if(result != nullptr){
            return result;
        }
    }
}


// 
void EventLoop::addFuture(Future* future) { this->futures.push_back(future); }
void EventLoop::run() {
    while (!this->futures.empty()) {
        for (auto it = this->futures.begin(); it != this->futures.end();) {
            void* result = (*it)->poll(nullptr);
            if (result != nullptr) {
                it = this->futures.erase(it);  // Remove resolved futures
            } else {
                ++it;
            }
        }
    }
}

Future* await(Future* future) {
    while (true) {
        void* result = future->poll(nullptr);
        if (result != nullptr) {
            return done(result);  // Return the resolved value
        }
    }
}
}

#endif /*FUTURE_CPP*/