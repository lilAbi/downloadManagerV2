#pragma once
#include "core/logger.h"
#include "eventHandler.h"

#include <typeindex>

//Queue events and dispatch them
class EventManager {
    using EventHandlerVector = std::vector< std::shared_ptr<BaseEventFunctionHandler> >;
public:
    static EventManager& get();

    //fire off an event
    template<typename EventType>
    bool publish(std::shared_ptr<EventType> event);

     //register callback functions to be used for fired event
    template<typename T, typename EventType>
    void subscribe(T* instance, void (T::*OnEventMemberFunction)(std::shared_ptr<EventType> event));

     //remove copy/move constructor/assignment operators
     EventManager(const EventManager&) = delete;
     EventManager(EventManager&&) = delete;
     EventManager& operator=(const EventManager&) = delete;
     EventManager& operator=(EventManager&&) = delete;

private:
    EventManager();
    ~EventManager() = default;

private:
    Logger*                     m_logger = &Logger::get();
    //each key in the map is an "Event Type" that has a value of a vector of subscribed "Event Handlers (callback functions)"
    std::flat_map< std::type_index, std::shared_ptr<EventHandlerVector> > m_subscribers;
};

template<typename EventType>
bool EventManager::publish(std::shared_ptr<EventType> event) {
    //check if EventType has an existing EventHandlerVector
    if ( const std::type_index id = typeid(EventType); m_subscribers.contains(id) ) {
        //iterate through all the event handlers and handle the event
        for ( const auto& event_handler : *(m_subscribers[id]) ) {
            if (event_handler) event_handler->call(std::static_pointer_cast<Event>(event));
        }
    } else {
        m_logger->critical("Event's registered EventHandlerVector was not found");
        return false;
    }
    return true;
}

template<typename T, typename EventType>
void EventManager::subscribe(T* instance, void(T::*OnEventMemberFunction)(std::shared_ptr<EventType> event)) {
    const std::type_index id = typeid(EventType);
    //check if EventType has an existing EventHandlerVector
    if ( !m_subscribers.contains(id) ) {
        m_subscribers[id] = std::make_shared<EventHandlerVector>(); //create it
    }
    //"subscribe" a callback to be called when an event fires
    m_subscribers[id]->push_back( std::make_shared< MemberFunctionEventHandler<T,EventType> >(instance, OnEventMemberFunction) );
}