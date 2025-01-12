#ifndef LAMBDA_ANIMATION_H
#define LAMBDA_ANIMATION_H
#include "../Animation/animation.h"
#include <functional>

class LambdaAnimation : public virtual Animation {
public:
    std::function<void(double)> m_func;

    LambdaAnimation(EaseType easeType, double startTime, double duration);
    LambdaAnimation(const LambdaAnimation& animation);

    void animate(double t) override;
};
#endif
