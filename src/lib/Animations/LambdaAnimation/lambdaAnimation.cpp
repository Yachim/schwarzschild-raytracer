#include "lambdaAnimation.h"

LambdaAnimation::LambdaAnimation(EaseType easeType, double startTime, double duration) :
    Animation(easeType, startTime, duration) {}
LambdaAnimation::LambdaAnimation(const LambdaAnimation& animation) :
    Animation(animation),
    m_func(animation.m_func) {}

void LambdaAnimation::animate(double t) {
    m_func(t);
}
