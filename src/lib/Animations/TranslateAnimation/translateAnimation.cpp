#include "translateAnimation.h"

TranslateAnimation::TranslateAnimation(EaseType easeType, double startTime, double duration, Transform* object) :
    Animation(easeType, startTime, duration),
    m_object(object) {}
TranslateAnimation::TranslateAnimation(const TranslateAnimation& animation) :
    Animation(animation),
    m_object(animation.getObject()),
    m_translation(animation.getTranslation()) {}

Transform* TranslateAnimation::getObject() const {
    return m_object;
}
void TranslateAnimation::setObject(Transform* object) {
    m_object = object;
}

glm::vec3 TranslateAnimation::getTranslation() const {
    return m_translation;
}
void TranslateAnimation::setTranslation(const glm::vec3 translation) {
    m_translation = translation;
}

void TranslateAnimation::animate(double t) {
    m_object->setPos(m_object->getPos() + m_translation * float(t));
}
