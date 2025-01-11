#ifndef COMBINED_ANIMATION_H
#define COMBINED_ANIMATION_H
#include "../Animation/animation.h"
#include <vector>
#include <cstddef>

class CombinedAnimation : public virtual Animation {
protected:
    size_t m_nSubanimations = 0;
    std::vector<Animation*> m_subanimations;
    double m_minSubanimationStartTime;
    double m_subanimationsDuration;

    using Animation::setEaseType;
    using Animation::getEaseType;
public:
    CombinedAnimation(double startTime, double duration);
    CombinedAnimation(const CombinedAnimation& animation);

    std::vector<Animation*> getSubanimations() const;
    void setSubanimations(std::vector<Animation*> subAnimations);

    void animate(double t) override;
};
#endif
