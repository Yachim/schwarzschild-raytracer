#ifndef OBJECT_LOADER_H
#define OBJECT_LOADER_H

#include <vector>
#include <mutex>
#include <GLFW/glfw3.h>
#include "../Objects/Light/light.h"
#include "../Objects/MaterialObject/materialObject.h"

class ObjectLoader {
private:
    static ObjectLoader* m_instance;
    static std::mutex m_mutex;

    std::vector<Light*> m_lights;
    std::vector<MaterialObject*> m_objects;

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
    void addObject(MaterialObject* object);

    void load(GLuint program);
};
#endif
