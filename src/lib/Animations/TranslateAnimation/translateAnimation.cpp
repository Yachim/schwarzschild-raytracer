#include "translateAnimation.h"

TranslateAnimation::TranslateAnimation(EaseType easeType, double startTime, double duration, Transform* object) :
    Animation(easeType, startTime, duration),
    m_object(object) {}
TranslateAnimation::TranslateAnimation(const TranslateAnimation& animation) :
    Animation(animation),
    m_object(animation.getObject()),
    m_startPos(animation.getStartPos()),
    m_endPos(animation.getEndPos()) {}

Transform* TranslateAnimation::getObject() const {
    return m_object;
}
void TranslateAnimation::setObject(Transform* object) {
    m_object = object;
}

glm::vec3 TranslateAnimation::getStartPos() const {
    return m_startPos;
}
void TranslateAnimation::setStartPos(const glm::vec3 startPos) {
    m_startPos = startPos;
}

glm::vec3 TranslateAnimation::getEndPos() const {
    return m_endPos;
}
void TranslateAnimation::setEndPos(const glm::vec3 endPos) {
    m_endPos = endPos;
}

void TranslateAnimation::animate(double t) {
    m_object->setPos(m_startPos + (m_endPos - m_startPos) * float(t));
}
