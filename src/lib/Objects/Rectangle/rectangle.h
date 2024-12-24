#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "../Plane/plane.h"
#include <glm/vec3.hpp>

class Rectangle : public Plane {
public:
    Rectangle();
    Rectangle(glm::vec3 pos);

    float getWidth();
    void setWidth(float width);

    float getHeight();
    void setHeight(float height);

    void setupShader(GLuint program, std::string prefix) override;
    void loadShader() override;

private:
    float m_width = 1.;
    float m_height = 1.;

    GLint m_widthLoc;
    GLint m_heightLoc;
};
#endif