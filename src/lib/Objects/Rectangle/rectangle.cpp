#include <glad/glad.h>
#include "rectangle.h"

Rectangle::Rectangle() : Plane() {}
Rectangle::Rectangle(glm::vec3 pos) : Plane(pos) {}

float Rectangle::getWidth() {
    return m_width;
}
void Rectangle::setWidth(float width) {
    m_width = width;
}

float Rectangle::getHeight() {
    return m_height;
}
void Rectangle::setHeight(float height) {
    m_height = height;
}

void Rectangle::setupShader(GLuint program, std::string prefix) {
    Plane::setupShader(program, prefix + ".plane");

    m_widthLoc = glGetUniformLocation(program, (prefix + ".width").c_str());
    m_heightLoc = glGetUniformLocation(program, (prefix + ".height").c_str());
}

void Rectangle::loadShader() {
    Plane::loadShader();

    glUniform1f(m_widthLoc, m_width);
    glUniform1f(m_heightLoc, m_height);
}
