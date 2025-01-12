#include "input.h"
#include "../Objects/Camera/camera.h"
#include <glm/geometric.hpp>
#include <iostream>

Input* Input::m_instance(nullptr);
std::mutex Input::m_mutex;

Input* Input::getInstance() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_instance == nullptr) {
        m_instance = new Input();
    }
    return m_instance;
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Input* instance = Input::getInstance();

    if (action == GLFW_PRESS) {
        instance->m_keymap[key] = true;
        instance->m_modmap[mods] = true;
    }
    else if (action == GLFW_RELEASE) {
        if (instance->m_keymap[key]) instance->m_keymap[key] = false;
        if (instance->m_modmap[mods]) instance->m_modmap[mods] = false;
    }
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Input* instance = Input::getInstance();

    if (action == GLFW_PRESS) {
        instance->m_lClicked = button == GLFW_MOUSE_BUTTON_LEFT;
        instance->m_rClicked = button == GLFW_MOUSE_BUTTON_RIGHT;
    }
    else if (action == GLFW_RELEASE) {
        instance->m_lClicked = instance->m_lClicked && button != GLFW_MOUSE_BUTTON_LEFT;
        instance->m_rClicked = instance->m_rClicked && button != GLFW_MOUSE_BUTTON_RIGHT;
    }
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Input* instance = Input::getInstance();

    instance->m_mouse = glm::vec2(xpos, ypos);
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Input* instance = Input::getInstance();

    instance->m_scroll = glm::vec2(xoffset, yoffset);
}

void Input::setInputEnabled(bool enabled) {
    m_inputEnabled = enabled;
}

bool Input::getInputEnabled() const {
    return m_inputEnabled;
}

float Input::getSpecifiedAxis(int positive, int negative) {
    if (!m_inputEnabled) return 0.;

    float axis = 0.;
    if (m_keymap[positive]) {
        axis += 1.;
    }
    if (m_keymap[negative]) {
        axis -= 1.;
    }
    return axis;
}

float Input::getAxis() {
    return getSpecifiedAxis(m_forwardKey, m_backKey);
}

glm::vec2 Input::getAxis2D() {
    return glm::vec2(getAxis(), getSpecifiedAxis(m_rightKey, m_leftKey));
}

glm::vec3 Input::getAxis3D() {
    glm::vec2 axis2d = getAxis2D();
    return glm::vec3(axis2d.y, getSpecifiedAxis(m_upKey, m_downKey), axis2d.x);
}

bool Input::isPressed(int key) {
    return m_inputEnabled && m_keymap[key];
}

bool Input::isPressed(int key, int mod) {
    return m_inputEnabled && m_keymap[key] && m_modmap[mod];
}

bool Input::isLClicked() const {
    return m_inputEnabled && m_lClicked;
}

bool Input::isRClicked() const {
    return m_inputEnabled && m_rClicked;
}

glm::vec2 Input::getMouse() const {
    return m_inputEnabled ? m_mouse : glm::vec2(0.);
}

glm::vec2 Input::getScroll() const {
    return m_inputEnabled ? m_scroll : glm::vec2(0.);
}
