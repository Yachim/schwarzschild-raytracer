#ifndef OBJECT_LOADER_H
#define OBJECT_LOADER_H

#include <vector>
#include <mutex>
#include "../Light/light.h"
#include "../Sphere/sphere.h"
#include "../Plane/plane.h"
#include "../Disk/disk.h"
#include "../HollowDisk/hollowDisk.h"
#include "../LateralCylinder/lateralCylinder.h"

class ObjectLoader {
private:
    static ObjectLoader* m_instance;
    static std::mutex m_mutex;

    std::vector<Light*> m_lights;
    std::vector<Sphere*> m_spheres;
    std::vector<Plane*> m_planes;
    std::vector<Disk*> m_disks;
    std::vector<HollowDisk*> m_hollowDisks;
    std::vector<LateralCylinder*> m_lateralCylinders;

    uint loadType(GLuint program, ObjectType type, uint objectsOffset);

protected:
    ObjectLoader() {};
    ~ObjectLoader() {};

public:
    ObjectLoader(ObjectLoader& other) = delete;
    void operator=(const ObjectLoader&) = delete;
    static ObjectLoader* getInstance();

    void addLight(Light* light);
    void addSphere(Sphere* sphere);
    void addPlane(Plane* plane);
    void addDisk(Disk* disk);
    void addHollowDisk(HollowDisk* hollowDisk);
    void addLateralCylinder(LateralCylinder* lateralCylinder);

    void load(GLuint program);
};
#endif
