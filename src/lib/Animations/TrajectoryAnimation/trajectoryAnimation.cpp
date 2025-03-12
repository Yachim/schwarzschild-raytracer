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

std::vector<glm::vec3> TrajectoryAnimation::getPoints(int n) const {
    std::vector<glm::vec3> points = {};
    for (size_t i = 0; i < n; i++) {
        float t = float(i) / float(n - 1);
        points.push_back(m_trajectory_func(t));
    }

    return points;
}
