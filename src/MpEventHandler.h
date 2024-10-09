#ifndef MP_EVENT_HANDLER_H
#define MP_EVENT_HANDLER_H

#include <functional>
#include <memory>

// Template class for handling different function signatures
template<typename... Args>
class MpEventHandler
{
public:
    using EventCallback = std::function<void(Args...)>;

    // Constructor to initialize with a callback function
    EventCallback(CallbackFunction func);

    // Method to call the stored function with arguments
    void Call(Args... args);

private:
    CallbackFunction _func;
};

#endif // MP_EVENT_HANDLER_H
