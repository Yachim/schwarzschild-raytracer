#include "animationManager.h"
#include <cmath>

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

    std::vector<size_t> toRemove;
    for (size_t i = 0; i < m_animations.size(); i++) {
        Animation* animation = m_animations[i];
        float startTime = animation->getStartTime();
        bool repeating = animation->getRepeating();
        animation->setPlaying(false);
        if (m_time < startTime) continue;
        if (m_time > startTime + animation->getDuration() && !repeating) {
            toRemove.push_back(i);
            continue;
        }
        animation->setPlaying(true);

        float t = (m_time - startTime) / animation->getDuration();
        if (repeating) t = std::modf(t, &t);
        animation->animate(animation->ease(t));
    }

    for (auto index : toRemove) {
        m_animations.erase(m_animations.begin() + index, m_animations.begin() + index + 1);
    }
}

void AnimationManager::addAnimation(Animation* animation) {
    m_animations.push_back(animation);
}
