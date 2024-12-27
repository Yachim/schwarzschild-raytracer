#include <glad/glad.h>
#include "box.h"

Box::Box() : MaterialObject(), Transform() {}
Box::Box(glm::vec3 pos) : MaterialObject(), Transform(pos) {}

float Box::getWidth() {
    return m_width;
}
void Box::setWidth(float width) {
    m_width = width;
}

float Box::getDepth() {
    return m_depth;
}
void Box::setDepth(float depth) {
    m_depth = depth;
}

float Box::getHeight() {
    return m_height;
}
void Box::setHeight(float height) {
    m_height = height;
}

void Box::setupShader(GLuint program, std::string prefix) {
    MaterialObject::setupShader(program, prefix + ".material");
    Transform::setupShader(program, prefix + ".transform");

    m_widthLoc = glGetUniformLocation(program, (prefix + ".width").c_str());
    m_depthLoc = glGetUniformLocation(program, (prefix + ".depth").c_str());
    m_heightLoc = glGetUniformLocation(program, (prefix + ".height").c_str());
}

void Box::loadShader() {
    MaterialObject::loadShader();
    Transform::loadShader();

    glUniform1f(m_widthLoc, m_width);
    glUniform1f(m_depthLoc, m_depth);
    glUniform1f(m_heightLoc, m_height);
}
