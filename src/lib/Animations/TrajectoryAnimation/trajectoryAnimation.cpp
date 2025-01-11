#include "trajectoryAnimation.h"

TrajectoryAnimation::TrajectoryAnimation(EaseType easeType, double startTime, double duration, Transform* object) :
    Animation(easeType, startTime, duration),
    TransformAnimation(easeType, startTime, duration, object) {}

TrajectoryAnimation::TrajectoryAnimation(const TrajectoryAnimation& animation) :
    Animation(animation),
    TransformAnimation(animation),
    m_func(animation.m_func) {}

void TrajectoryAnimation::animate(double t) {
    m_object->setPos(m_func(t));
}
