#ifndef BSPLINE_ANIMATION_H
#define BSPLINE_ANIMATION_H
#include "../TransformAnimation/transformAnimation.h"
#include "../curveVisualization.h"
#include <vector>

const glm::mat4 B_SPLINE_MATRIX = glm::transpose(glm::mat4(
    1., 4., 1., 0.,
    -3., 0., 3., 0.,
    3., -6., 3., 0.,
    -1., 3., -3., 1.
)) / 6.f;

class BSplineAnimation : public virtual TransformAnimation, public virtual CurveVisualization {
private:
    size_t m_totalSegments;
    std::vector<glm::vec3> m_controlPoints;

    glm::vec3 getPoint(double t) const;
public:
    BSplineAnimation(EaseType easeType, double startTime, double duration, Transform* object);
    BSplineAnimation(const BSplineAnimation& animation);

    std::vector<glm::vec3> getControlPoints() const;
    void setControlPoints(std::vector<glm::vec3> controlPoints);

    std::vector<glm::vec3> getPoints(int n = 100) const override;

    void animate(double t) override;
};
#endif