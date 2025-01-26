#include "bSplineAnimation.h"

BSplineAnimation::BSplineAnimation(EaseType easeType, double startTime, double duration, Transform* object) :
    Animation(easeType, startTime, duration),
    TransformAnimation(easeType, startTime, duration, object) {}

BSplineAnimation::BSplineAnimation(const BSplineAnimation& animation) :
    Animation(animation),
    TransformAnimation(animation) {}

std::vector<glm::vec3> BSplineAnimation::getControlPoints() const {
    return m_controlPoints;
}
void BSplineAnimation::setControlPoints(std::vector<glm::vec3> controlPoints) {
    m_controlPoints = controlPoints;
    m_totalSegments = m_controlPoints.size() - 3;
}

void BSplineAnimation::animate(double t) {
    double scaledT = t * m_totalSegments;

    // clamping occurs when t = 1,
    // then m_controlPoints[i + 3 = m_controlPoints.size()] is out of bounds
    int i = glm::clamp((int)std::floor(scaledT), 0, (int)(m_totalSegments - 1));
    double localT = scaledT - i;

    double t2 = localT * localT;
    double t3 = t2 * localT;
    glm::vec3 pos = glm::vec4(1, localT, t2, t3) * B_SPLINE_MATRIX * glm::transpose(glm::mat4x3(
        m_controlPoints[i],
        m_controlPoints[i + 1],
        m_controlPoints[i + 2],
        m_controlPoints[i + 3]
    ));

    m_object->setPos(pos);
}
