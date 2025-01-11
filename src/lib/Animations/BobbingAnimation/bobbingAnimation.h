#ifndef BOBBING_ANIMATION_H
#define BOBBING_ANIMATION_H
#include "../CombinedAnimation/combinedAnimation.h"
#include "../TranslateAnimation/translateAnimation.h"
#include "../../Objects/Transform/transform.h"
#include <glm/vec3.hpp>

class BobbingAnimation : public virtual CombinedAnimation {
private:
    Transform* m_object;
    TranslateAnimation animation1 = { EaseType::EASE_OUT, 0., 1., m_object };
    TranslateAnimation animation2 = { EaseType::EASE_IN_OUT, 1., 2., m_object };
    TranslateAnimation animation3 = { EaseType::EASE_IN, 3., 1., m_object };
public:
    BobbingAnimation(double startTime, double duration, Transform* object);
    BobbingAnimation(const BobbingAnimation& animation);

    Transform* getObject() const;
    void setObject(Transform* object);

    void setPoints(glm::vec3 start, glm::vec3 dir, float dist);
};
#endif
