#include "materialObject.h"

const Material DEFAULT_MATERIAL = Material();

MaterialObject::MaterialObject() {}
MaterialObject::MaterialObject(Material* mat) : m_material(mat) {}

const Material* MaterialObject::getMaterial() const {
    if (!m_material) return &DEFAULT_MATERIAL;
    return m_material;
}
void MaterialObject::setMaterial(Material* mat) {
    m_material = mat;
}
