#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "../Plane/plane.h"
#include <glm/vec3.hpp>

class Rectangle : public Plane {
public:
    Rectangle();
    Rectangle(glm::vec3 pos);

    float getWidth() const;
    void setWidth(float width);

    float getHeight() const;
    void setHeight(float height);

    void loadShader(GLuint program, std::string prefix) override;

    ObjectType getType() const override;

private:
    float m_width = 1.;
    float m_height = 1.;

    bool m_locationsSet = false;
    GLint m_widthLoc;
    GLint m_heightLoc;
};
#endif