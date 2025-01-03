#include <glad/glad.h>
#include "rectangle.h"

Rectangle::Rectangle() : Plane() {}
Rectangle::Rectangle(glm::vec3 pos) : Plane(pos) {}

float Rectangle::getWidth() const {
    return m_width;
}
void Rectangle::setWidth(float width) {
    m_width = width;
}

float Rectangle::getHeight() const {
    return m_height;
}
void Rectangle::setHeight(float height) {
    m_height = height;
}

void Rectangle::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_widthLoc = glGetUniformLocation(program, (prefix + ".width").c_str());
        m_heightLoc = glGetUniformLocation(program, (prefix + ".height").c_str());
        m_locationsSet = true;
    }

    Plane::loadShader(program, prefix + ".plane");

    glUniform1f(m_widthLoc, m_width);
    glUniform1f(m_heightLoc, m_height);
}

ObjectType Rectangle::getType() const {
    return ObjectType::RECTANGLE;
}
