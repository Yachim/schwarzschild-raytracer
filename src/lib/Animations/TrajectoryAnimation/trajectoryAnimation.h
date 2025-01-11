#ifndef TRAJECTORY_ANIMATION_H
#define TRAJECTORY_ANIMATION_H
#include "../TransformAnimation/transformAnimation.h"

class TrajectoryAnimation : public virtual TransformAnimation {
public:
    glm::vec3(*m_func)(double t);

    TrajectoryAnimation(EaseType easeType, double startTime, double duration, Transform* object);
    TrajectoryAnimation(const TrajectoryAnimation& animation);

    void animate(double t) override;
};
#endif
