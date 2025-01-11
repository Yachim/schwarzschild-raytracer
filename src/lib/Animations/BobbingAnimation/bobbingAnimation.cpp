#include "bobbingAnimation.h"

BobbingAnimation::BobbingAnimation(double startTime, double duration, Transform* object) :
    Animation(EaseType::LINEAR, startTime, duration),
    TransformAnimation(EaseType::LINEAR, startTime, duration, object),
    CombinedAnimation(startTime, duration) {
    setPoints(glm::vec3(0.), glm::vec3(0., 1., 0.), 0.5);

    setSubanimations(std::vector<Animation*>{&animation1, & animation2, & animation3});
    setRepeating(true);
}
BobbingAnimation::BobbingAnimation(const BobbingAnimation& animation) :
    Animation(animation),
    TransformAnimation(animation),
    CombinedAnimation(animation) {
    setSubanimations(std::vector<Animation*>{&animation1, & animation2, & animation3});
    setRepeating(true);
}

void BobbingAnimation::setPoints(glm::vec3 start, glm::vec3 dir, float dist) {
    animation1.setStartPos(start);
    animation1.setEndPos(start + dir * dist);

    animation2.setStartPos(start + dir * dist);
    animation2.setEndPos(start - dir * dist);

    animation3.setStartPos(start - dir * dist);
    animation3.setEndPos(start);
}
