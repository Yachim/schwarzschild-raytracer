#ifndef TRAJECTORY_ANIMATION_H
#define TRAJECTORY_ANIMATION_H
#include "../TransformAnimation/transformAnimation.h"
#include "../LambdaAnimation/lambdaAnimation.h"
#include "../curveVisualization.h"

class TrajectoryAnimation : public virtual TransformAnimation, public virtual LambdaAnimation, public virtual CurveVisualization {
private:
    using LambdaAnimation::m_func;
public:
    std::function<glm::vec3(double)> m_trajectory_func;

    std::vector<glm::vec3> getPoints(int n = 100) const override;

    TrajectoryAnimation(EaseType easeType, double startTime, double duration, Transform* object);
    TrajectoryAnimation(const TrajectoryAnimation& animation);
};
#endif
