#include "trajectoryAnimation.h"

TrajectoryAnimation::TrajectoryAnimation(EaseType easeType, double startTime, double duration, Transform* object) :
    Animation(easeType, startTime, duration),
    LambdaAnimation(easeType, startTime, duration),
    TransformAnimation(easeType, startTime, duration, object) {
    m_func = [&](double t) {
        m_object->setPos(m_trajectory_func(t));
        };
}

TrajectoryAnimation::TrajectoryAnimation(const TrajectoryAnimation& animation) :
    Animation(animation),
    LambdaAnimation(animation),
    TransformAnimation(animation),
    m_trajectory_func(animation.m_trajectory_func) {
    m_func = [&](double t) {
        m_object->setPos(m_trajectory_func(t));
        };
}
