#ifndef OBJECT_LOADER_H
#define OBJECT_LOADER_H

#include <vector>
#include <mutex>
#include <GLFW/glfw3.h>
#include "../Objects/Light/light.h"
#include "../Objects/Sphere/sphere.h"
#include "../Objects/Plane/plane.h"
#include "../Objects/Disk/disk.h"
#include "../Objects/HollowDisk/hollowDisk.h"
#include "../Objects/LateralCylinder/lateralCylinder.h"
#include "../Objects/Rectangle/rectangle.h"
#include "../Objects/Box/box.h"

class ObjectLoader {
private:
    static ObjectLoader* m_instance;
    static std::mutex m_mutex;

    std::vector<Light*> m_lights;
    std::vector<Object*> m_objects;

    bool m_locationsSet = false;
    GLint m_numObjectsLoc;
    GLint m_numLightsLoc;

protected:
    ObjectLoader() {};
    ~ObjectLoader() {};

public:
    ObjectLoader(ObjectLoader& other) = delete;
    void operator=(const ObjectLoader&) = delete;
    static ObjectLoader* getInstance();

    void addLight(Light* light);
    void addObject(Object* object);

    void load(GLuint program);
};
#endif
