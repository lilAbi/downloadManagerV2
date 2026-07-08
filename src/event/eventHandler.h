#pragma once

#include <functional>
#include <memory>

class Event;

class BaseEventFunctionHandler{
public:
    virtual ~BaseEventFunctionHandler() = default;
    virtual void call(std::shared_ptr<Event> event) = 0;
};

template<typename T, typename EventType>
class MemberFunctionEventHandler : public BaseEventFunctionHandler {
    using OnEventMemberFunction = void(T::*)(std::shared_ptr<EventType>);
public:
    MemberFunctionEventHandler(T* inst, const OnEventMemberFunction func) : m_instance(inst), m_callback(func) {};

    void call(const std::shared_ptr<Event> event) override {
        (m_instance->*m_callback)(std::static_pointer_cast<EventType>(event));
    }

private:
    T* m_instance;
    OnEventMemberFunction m_callback;
};