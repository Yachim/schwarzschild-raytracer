#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "objectLoader.h"
#include <iostream>
#include <string>

ObjectLoader* ObjectLoader::m_instance(nullptr);
std::mutex ObjectLoader::m_mutex;

ObjectLoader* ObjectLoader::getInstance() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_instance == nullptr) {
        m_instance = new ObjectLoader();
    }
    return m_instance;
}

void ObjectLoader::addLight(Light* light) {
    m_lights.push_back(light);
}

void ObjectLoader::addObject(Object* object) {
    m_objects.push_back(object);
}

void ObjectLoader::load(GLuint program) {
    if (!m_locationsSet) {
        m_numObjectsLoc = glGetUniformLocation(program, "num_objects");
        m_numLightsLoc = glGetUniformLocation(program, "num_lights");
        m_locationsSet = true;
    }

    int sphereIndex = 0;
    int planeIndex = 0;
    int diskIndex = 0;
    int hollowDiskIndex = 0;
    int lateralCylinderIndex = 0;
    int rectangleIndex = 0;
    int boxIndex = 0;

    glUniform1i(m_numObjectsLoc, m_objects.size());
    for (int i = 0; i < m_objects.size(); i++) {
        std::string glslListName;
        ObjectType type = m_objects[i]->getType();
        int* index;
        switch (type) {
        case ObjectType::SPHERE:
            glslListName = "spheres";
            index = &sphereIndex;
            break;
        case ObjectType::PLANE:
            glslListName = "planes";
            index = &planeIndex;
            break;
        case ObjectType::DISK:
            glslListName = "disks";
            index = &diskIndex;
            break;
        case ObjectType::HOLLOW_DISK:
            glslListName = "hollow_disks";
            index = &hollowDiskIndex;
            break;
        case ObjectType::LATERAL_CYLINDER:
            glslListName = "cylinders";
            index = &lateralCylinderIndex;
            break;
        case ObjectType::RECTANGLE:
            glslListName = "rectangles";
            index = &rectangleIndex;
            break;
        case ObjectType::BOX:
            glslListName = "boxes";
            index = &boxIndex;
            break;
        default:
            std::cout << "Invalid type: " << type << std::endl;
            continue;
        }

        m_objects[i]->loadShader(program, (glslListName + "[" + std::to_string(*index) + "]").c_str());

        glUniform1i(glGetUniformLocation(program, ("objects[" + std::to_string(i) + "].type").c_str()), type);
        glUniform1i(glGetUniformLocation(program, ("objects[" + std::to_string(i) + "].index").c_str()), *index);

        (*index)++;
    }

    glUniform1i(m_numLightsLoc, m_lights.size());
    for (int i = 0; i < m_lights.size(); i++) {
        m_lights[i]->loadShader(program, ("lights[" + std::to_string(i) + "]").c_str());
    }
}
