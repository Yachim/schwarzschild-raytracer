#include "animation.h"
#include <stdexcept>
#include <cmath>

// https://easings.net/
double Animation::ease(double t) {
    switch (m_easeType) {
    case EaseType::LINEAR:
        return t;
    case EaseType::EASE_IN:
        return t * t;
        break;
    case EaseType::EASE_OUT:
        return 1. - (1 - t) * (1 - t);
        break;
    case EaseType::EASE_IN_OUT:
        return t < 0.5 ? 2. * t * t : 1. - std::pow(-2. * t + 2., 2.) / 2.;

    default:
        throw std::invalid_argument("Invalid easeType");
    }
}

Animation::Animation(EaseType easeType, double startTime, double duration) :
    m_easeType(easeType),
    m_startTime(startTime),
    m_duration(duration) {}
Animation::Animation(const Animation& old_animation) :
    m_easeType(old_animation.getEaseType()),
    m_startTime(old_animation.getStartTime()),
    m_duration(old_animation.getDuration()),
    m_repeating(old_animation.getRepeating()),
    m_isPlaying(old_animation.isPlaying()) {}

EaseType Animation::getEaseType() const {
    return m_easeType;
}
void Animation::setEaseType(EaseType easeType) {
    m_easeType = easeType;
}

double Animation::getStartTime() const {
    return m_startTime;
}
void Animation::setStartTime(double startTime) {
    if (startTime < 0) {
        throw std::invalid_argument("Start time cannot be negative");
    }
    m_startTime = startTime;
}

double Animation::getDuration() const {
    return m_duration;
}
void Animation::setDuration(double duration) {
    if (duration <= 0) {
        throw std::invalid_argument("Duration must be greater than 0");
    }
    m_duration = duration;
}

bool Animation::getRepeating() const {
    return m_repeating;
}
void Animation::setRepeating(bool repeating) {
    m_repeating = repeating;
}

bool Animation::isPlaying() const {
    return m_isPlaying;
}
void Animation::setPlaying(bool playing) {
    m_isPlaying = playing;
}
