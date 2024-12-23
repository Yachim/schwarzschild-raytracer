#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "objectLoader.h"
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

void ObjectLoader::addSphere(Sphere* sphere) {
    m_spheres.push_back(sphere);
}

void ObjectLoader::addPlane(Plane* plane) {
    m_planes.push_back(plane);
}

void ObjectLoader::addDisk(Disk* disk) {
    m_disks.push_back(disk);
}

void ObjectLoader::addHollowDisk(HollowDisk* hollowDisk) {
    m_hollowDisks.push_back(hollowDisk);
}

void ObjectLoader::addLateralCylinder(LateralCylinder* lateralCylinder) {
    m_lateralCylinders.push_back(lateralCylinder);
}

// returns the length of the array
// objectsOffset is the offset for the objects glsl array
uint ObjectLoader::loadType(GLuint program, ObjectType type, uint objectsOffset) {
    std::vector<Object*> objects;
    std::string numStr;
    std::string glslListName;
    switch (type) {
    case ObjectType::SPHERE:
        objects.insert(objects.end(), m_spheres.begin(), m_spheres.end());
        numStr = "num_spheres";
        glslListName = "spheres";
        break;
    case ObjectType::PLANE:
        objects.insert(objects.end(), m_planes.begin(), m_planes.end());
        numStr = "num_planes";
        glslListName = "planes";
        break;
    case ObjectType::DISK:
        objects.insert(objects.end(), m_disks.begin(), m_disks.end());
        numStr = "num_disks";
        glslListName = "disks";
        break;
    case ObjectType::HOLLOW_DISK:
        objects.insert(objects.end(), m_hollowDisks.begin(), m_hollowDisks.end());
        numStr = "num_hollow_disks";
        glslListName = "hollow_disks";
        break;
    case ObjectType::LATERAL_CYLINDER:
        objects.insert(objects.end(), m_lateralCylinders.begin(), m_lateralCylinders.end());
        numStr = "num_cylinders";
        glslListName = "cylinders";
        break;
    default:
        return 0;
    }


    glUniform1i(glGetUniformLocation(program, numStr.c_str()), objects.size());
    for (uint i = 0; i < objects.size(); i++) {
        objects[i]->setupShader(program, (glslListName + "[" + std::to_string(i) + "]").c_str());
        objects[i]->loadShader();

        glUniform1i(glGetUniformLocation(program, ("objects[" + std::to_string(objectsOffset + i) + "].type").c_str()), type);
        glUniform1i(glGetUniformLocation(program, ("objects[" + std::to_string(objectsOffset + i) + "].index").c_str()), i);
    }

    return objects.size();
}

void ObjectLoader::load(GLuint program) {
    uint offset = loadType(program, ObjectType::SPHERE, 0);
    offset += loadType(program, ObjectType::PLANE, offset);
    offset += loadType(program, ObjectType::DISK, offset);
    offset += loadType(program, ObjectType::HOLLOW_DISK, offset);
    offset += loadType(program, ObjectType::LATERAL_CYLINDER, offset);

    glUniform1i(glGetUniformLocation(program, "num_lights"), m_lights.size());
    for (uint i = 0; i < m_lights.size(); i++) {
        m_lights[i]->setupShader(program, ("lights[" + std::to_string(i) + "]").c_str());
        m_lights[i]->loadShader();
    }
}
