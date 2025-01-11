#include "bobbingAnimation.h"

BobbingAnimation::BobbingAnimation(double startTime, double duration, Transform* object) :
    Animation(EaseType::LINEAR, startTime, duration),
    CombinedAnimation(startTime, duration),
    m_object(object) {
    setPoints(glm::vec3(0.), glm::vec3(0., 1., 0.), 0.5);

    setSubanimations(std::vector<Animation*>{&animation1, & animation2, & animation3});
    setRepeating(true);
}
BobbingAnimation::BobbingAnimation(const BobbingAnimation& animation) :
    Animation(animation),
    CombinedAnimation(animation),
    m_object(animation.getObject()) {
    setSubanimations(std::vector<Animation*>{&animation1, & animation2, & animation3});
    setRepeating(true);
}

Transform* BobbingAnimation::getObject() const {
    return m_object;
}
void BobbingAnimation::setObject(Transform* object) {
    m_object = object;
}

void BobbingAnimation::setPoints(glm::vec3 start, glm::vec3 dir, float dist) {
    animation1.setStartPos(start);
    animation1.setEndPos(start + dir * dist);

    animation2.setStartPos(start + dir * dist);
    animation2.setEndPos(start - dir * dist);

    animation3.setStartPos(start - dir * dist);
    animation3.setEndPos(start);
}
