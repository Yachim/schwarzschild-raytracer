#ifndef BOBBING_ANIMATION_H
#define BOBBING_ANIMATION_H
#include "../CombinedAnimation/combinedAnimation.h"
#include "../TranslateAnimation/translateAnimation.h"
#include "../TransformAnimation/transformAnimation.h"
#include "../../Objects/Transform/transform.h"
#include "../curveVisualization.h"
#include <glm/vec3.hpp>

class BobbingAnimation : public virtual CombinedAnimation, public virtual TransformAnimation, public virtual CurveVisualization {
private:
    TranslateAnimation animation1 = { EaseType::EASE_OUT, 0., 1., m_object };
    TranslateAnimation animation2 = { EaseType::EASE_IN_OUT, 1., 2., m_object };
    TranslateAnimation animation3 = { EaseType::EASE_IN, 3., 1., m_object };

    using CombinedAnimation::setSubanimations;
    using CombinedAnimation::getSubanimations;
public:
    BobbingAnimation(double startTime, double duration, Transform* object);
    BobbingAnimation(const BobbingAnimation& animation);

    std::vector<glm::vec3> getPoints(int n = 100) const override;

    void setPoints(glm::vec3 start, glm::vec3 dir);
};
#endif
