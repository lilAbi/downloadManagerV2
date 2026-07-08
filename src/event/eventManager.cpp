#include "eventManager.h"

EventManager& EventManager::get() {
    static EventManager instance;
    return instance;
}

EventManager::EventManager() {
    m_logger->info("Event Manager Initialized");
}