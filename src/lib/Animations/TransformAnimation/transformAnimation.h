#ifndef TRANSFORM_ANIMATION_H
#define TRANSFORM_ANIMATION_H
#include "../Animation/animation.h"
#include "../../Objects/Transform/transform.h"

class TransformAnimation : public virtual Animation {
protected:
    Transform* m_object;
public:
    TransformAnimation(EaseType easeType, double startTime, double duration, Transform* object);
    TransformAnimation(const TransformAnimation& animation);

    Transform* getObject() const;
    void setObject(Transform* object);
};
#endif
