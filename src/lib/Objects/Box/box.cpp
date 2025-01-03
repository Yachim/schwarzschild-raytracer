#include <glad/glad.h>
#include "box.h"

Box::Box() : MaterialObject(), Transform() {}
Box::Box(glm::vec3 pos) : MaterialObject(), Transform(pos) {}

float Box::getWidth() const {
    return m_width;
}
void Box::setWidth(float width) {
    m_width = width;
}

float Box::getDepth() const {
    return m_depth;
}
void Box::setDepth(float depth) {
    m_depth = depth;
}

float Box::getHeight() const {
    return m_height;
}
void Box::setHeight(float height) {
    m_height = height;
}

void Box::loadShader(GLuint program, std::string prefix) {
    if (!m_locationsSet) {
        m_widthLoc = glGetUniformLocation(program, (prefix + ".width").c_str());
        m_depthLoc = glGetUniformLocation(program, (prefix + ".depth").c_str());
        m_heightLoc = glGetUniformLocation(program, (prefix + ".height").c_str());
        m_locationsSet = true;
    }

    MaterialObject::loadShader(program, prefix + ".material");
    Transform::loadShader(program, prefix + ".transform");

    glUniform1f(m_widthLoc, m_width);
    glUniform1f(m_depthLoc, m_depth);
    glUniform1f(m_heightLoc, m_height);
}

ObjectType Box::getType() const {
    return ObjectType::BOX;
}
