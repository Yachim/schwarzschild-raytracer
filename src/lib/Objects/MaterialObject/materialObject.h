#ifndef MATERIAL_OBJECT_H
#define MATERIAL_OBJECT_H

#include "../Object/object.h"
#include "../Material/material.h"

class MaterialObject : public virtual Object {
public:
    MaterialObject();
    MaterialObject(Material* mat);

    const Material* getMaterial() const;
    void setMaterial(Material* mat);

    void loadShader(GLuint program, std::string prefix) override {};

protected:
    Material* m_material = nullptr;
};
#endif
