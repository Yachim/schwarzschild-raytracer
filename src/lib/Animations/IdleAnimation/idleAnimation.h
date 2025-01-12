#ifndef IDLE_ANIMATION_H
#define IDLE_ANIMATION_H
#include "../LambdaAnimation/lambdaAnimation.h"

class IdleAnimation : public virtual LambdaAnimation {
private:
    using Animation::setEaseType;
    using Animation::getEaseType;
    using LambdaAnimation::m_func;
public:
    IdleAnimation(double startTime, double duration);
    IdleAnimation(const IdleAnimation& animation);
};
#endif
