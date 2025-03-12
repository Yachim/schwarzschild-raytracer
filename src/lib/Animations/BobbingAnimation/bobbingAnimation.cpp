#include "bobbingAnimation.h"

BobbingAnimation::BobbingAnimation(double startTime, double duration, Transform* object) :
    Animation(EaseType::LINEAR, startTime, duration),
    TransformAnimation(EaseType::LINEAR, startTime, duration, object),
    CombinedAnimation(startTime, duration) {
    setPoints(glm::vec3(0.), glm::vec3(0., 0.5, 0.));

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

void BobbingAnimation::setPoints(glm::vec3 start, glm::vec3 dir) {
    animation1.setStartPos(start);
    animation1.setEndPos(start + dir);

    animation2.setStartPos(start + dir);
    animation2.setEndPos(start - dir);

    animation3.setStartPos(start - dir);
    animation3.setEndPos(start);
}

std::vector<glm::vec3> BobbingAnimation::getPoints(int n) const {
    return {
        animation2.getEndPos(),
        animation1.getEndPos()
    };
}
