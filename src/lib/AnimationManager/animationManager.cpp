#include "animationManager.h"

AnimationManager* AnimationManager::m_instance(nullptr);
std::mutex AnimationManager::m_mutex;

AnimationManager* AnimationManager::getInstance() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_instance == nullptr) {
        m_instance = new AnimationManager();
    }
    return m_instance;
}

void AnimationManager::update(double time) {
    m_frame++;
    m_dt = m_time - time;
    m_time = time;
}
