#ifndef TRANSFORM_ANIMATION_H
#define TRANSFORM_ANIMATION_H
#include "../Animation/animation.h"
#include "../../Objects/Transform/transform.h"
#include <glm/vec3.hpp>

class TranslateAnimation : public virtual Animation {
private:
    Transform* m_object;
    glm::vec3 m_startPos = glm::vec3(0.);
    glm::vec3 m_endPos = glm::vec3(0.);
public:
    TranslateAnimation(EaseType easeType, double startTime, double duration, Transform* object);
    TranslateAnimation(const TranslateAnimation& animation);

    Transform* getObject() const;
    void setObject(Transform* object);

    glm::vec3 getStartPos() const;
    void setStartPos(const glm::vec3 startPos);

    glm::vec3 getEndPos() const;
    void setEndPos(const glm::vec3 endPos);

    void animate(double t) override;
};
#endif
