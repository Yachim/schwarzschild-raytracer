#include "input.h"
#include "../Camera/camera.h"

Input* Input::m_instance(nullptr);
std::mutex Input::m_mutex;

Input* Input::GetInstance() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_instance == nullptr) {
        m_instance = new Input();
    }
    return m_instance;
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Input* instance = Input::GetInstance();

    if (action == GLFW_PRESS) {
        instance->m_keymap[key] = true;
    }
    else if (action == GLFW_RELEASE && instance->m_keymap[key]) {
        instance->m_keymap[key] = false;
    }
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Input* instance = Input::GetInstance();

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
    Input* instance = Input::GetInstance();

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    instance->m_mouse = glm::vec2(xpos / width, ypos / height);
    instance->m_deltaMouse = instance->m_mouse - instance->m_prevMouse;
    instance->m_prevMouse = instance->m_mouse;
}

float Input::getSpecifiedAxis(int positive, int negative) {
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
    return m_keymap[key];
}

bool Input::isLClicked() {
    return m_lClicked;
}

bool Input::isRClicked() {
    return m_rClicked;
}

glm::vec2 Input::getMouseDelta() {
    return m_deltaMouse;
}

glm::vec2 Input::getMouse() {
    return m_mouse;
}
