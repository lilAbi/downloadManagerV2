#include "commandQueue.h"

void CommandQueue::wait(const std::stop_token& stop_token) {
    std::unique_lock unique_lock{m_mutex};
    m_cv.wait(
        unique_lock,
        stop_token,
        [this] { return ( !m_deque.empty() ); }
    );
}
