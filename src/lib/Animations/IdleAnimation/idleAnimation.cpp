#include "idleAnimation.h"

IdleAnimation::IdleAnimation(double startTime, double duration) :
    Animation(EaseType::LINEAR, startTime, duration),
    LambdaAnimation(EaseType::LINEAR, startTime, duration) {
    m_func = [](double t) {};
}

IdleAnimation::IdleAnimation(const IdleAnimation& animation) :
    Animation(animation),
    LambdaAnimation(animation) {
    m_func = [](double t) {};
}
