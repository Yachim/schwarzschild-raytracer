#include "transformAnimation.h"

TransformAnimation::TransformAnimation(EaseType easeType, double startTime, double duration, Transform* object) :
    Animation(easeType, startTime, duration),
    m_object(object) {}
TransformAnimation::TransformAnimation(const TransformAnimation& animation) :
    Animation(animation.getEaseType(), animation.getStartTime(), animation.getDuration()) {}

Transform* TransformAnimation::getObject() const {
    return m_object;
}
void TransformAnimation::setObject(Transform* object) {
    m_object = object;
}
