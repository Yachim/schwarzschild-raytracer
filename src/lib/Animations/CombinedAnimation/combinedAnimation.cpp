#include "combinedAnimation.h"

CombinedAnimation::CombinedAnimation(double startTime, double duration) : Animation(EaseType::LINEAR, startTime, duration) {}
CombinedAnimation::CombinedAnimation(const CombinedAnimation& animation) :
    Animation(animation),
    m_subanimations(animation.getSubanimations()) {}

std::vector<Animation*> CombinedAnimation::getSubanimations() const {
    return m_subanimations;
}
void CombinedAnimation::setSubanimations(std::vector<Animation*> subAnimations) {
    m_subanimations = subAnimations;

    double minStartTime = subAnimations[0]->getStartTime();
    double maxEndTime = minStartTime + subAnimations[0]->getDuration();

    for (size_t i = 1; i < subAnimations.size(); i++) {
        double startTime = subAnimations[i]->getStartTime();
        double endTime = startTime + subAnimations[i]->getDuration();

        if (startTime < minStartTime) minStartTime = startTime;
        if (endTime > maxEndTime) maxEndTime = endTime;
    }

    m_minSubanimationStartTime = minStartTime;
    m_subanimationsDuration = maxEndTime - minStartTime;
}

void CombinedAnimation::animate(double t) {
    for (auto subanimation : m_subanimations) {
        float startT = (subanimation->getStartTime() - m_minSubanimationStartTime) / m_subanimationsDuration;
        subanimation->setPlaying(false);
        if (t < startT || t > startT + subanimation->getDuration() / m_subanimationsDuration) continue;
        subanimation->setPlaying(true);

        subanimation->animate(subanimation->ease((t - startT) * m_subanimationsDuration / subanimation->getDuration()));
    }
}
