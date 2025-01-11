#include "rotateAnimation.h"
#include <glm/gtc/quaternion.hpp>

RotateAnimation::RotateAnimation(EaseType easeType, double startTime, double duration, Transform* object) :
    Animation(easeType, startTime, duration),
    m_object(object) {}
RotateAnimation::RotateAnimation(const RotateAnimation& animation) :
    Animation(animation),
    m_object(animation.getObject()),
    m_axis(animation.getAxis()),
    m_startAngle(animation.getStartAngle()),
    m_endAngle(animation.getEndAngle()) {}

Transform* RotateAnimation::getObject() const {
    return m_object;
}
void RotateAnimation::setObject(Transform* object) {
    m_object = object;
}

glm::vec3 RotateAnimation::getAxis() const {
    return m_axis;
}
void RotateAnimation::setAxis(glm::vec3 axis) {
    m_axis = axis;
}

float RotateAnimation::getStartAngle() const {
    return m_startAngle;
}
void RotateAnimation::setStartAngle(float startAngle) {
    m_startAngle = startAngle;
}

float RotateAnimation::getEndAngle() const {
    return m_endAngle;
}
void RotateAnimation::setEndAngle(float endAngle) {
    m_endAngle = endAngle;
}

void RotateAnimation::animate(double t) {
    float angle = m_startAngle + (m_endAngle - m_startAngle) * t;
    m_object->setAxes(glm::angleAxis(angle, m_axis));
}
