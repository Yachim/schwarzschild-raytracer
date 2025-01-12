#ifndef TRAJECTORY_ANIMATION_H
#define TRAJECTORY_ANIMATION_H
#include "../TransformAnimation/transformAnimation.h"
#include "../LambdaAnimation/lambdaAnimation.h"

class TrajectoryAnimation : public virtual TransformAnimation, public virtual LambdaAnimation {
private:
    using LambdaAnimation::m_func;
public:
    std::function<glm::vec3(double)> m_trajectory_func;

    TrajectoryAnimation(EaseType easeType, double startTime, double duration, Transform* object);
    TrajectoryAnimation(const TrajectoryAnimation& animation);
};
#endif
