#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <mutex>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <map>

const float MIN_DELTA_MOUSE = 1.25;

// thread safe
// https://refactoring.guru/design-patterns/singleton/cpp/example#example-1
class Input {
private:
    static Input* m_instance;
    static std::mutex m_mutex;

#pragma region config
    int m_forwardKey = GLFW_KEY_W;
    int m_leftKey = GLFW_KEY_A;
    int m_backKey = GLFW_KEY_S;
    int m_rightKey = GLFW_KEY_D;
    int m_upKey = GLFW_KEY_E;
    int m_downKey = GLFW_KEY_Q;
#pragma endregion

    // keycode, pressed
    std::map<int, bool> m_keymap;

    bool m_lClicked = false;
    bool m_rClicked = false;
    glm::vec2 m_mouse = glm::vec2(0.5, 0.5);
    glm::vec2 m_scroll = glm::vec2();

    float getSpecifiedAxis(int positive, int negative);

protected:
    Input() {};
    ~Input() {};

public:
    Input(Input& other) = delete;
    void operator=(const Input&) = delete;
    static Input* getInstance();

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    float getAxis();         // forward/backward
    glm::vec2 getAxis2D();   // forward/backward, left/right
    glm::vec3 getAxis3D();   // up/down, left/right, forward/backward
    bool isPressed(int key); // key or mod
    bool isLClicked();
    bool isRClicked();
    glm::vec2 getMouse();
    glm::vec2 getScroll();
};
#endif
