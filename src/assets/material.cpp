#include "material.h"

namespace assets {

using namespace glm;

Material Material::createLambertian(const vec3& color) {
    Material mat = {};
    mat.albedo = color;
    mat.emissive = vec3(0.0f);
    mat.specular_chance = 0.0f;
    mat.specular_roughness = 0.0f;
    mat.specular_color = vec3(0.0f);
    mat.IOR = -1.0f;
    mat.refraction_chance = 0.0f;
    mat.phongExponent = 1.0f;
    mat.refraction_color = vec3(0.0f);
    mat.phong = 1.0f;
    mat.mirror_reflectance = 0.0f;
    mat.mIOR = -1.0f;
    mat.metal = 0.0f;
    return mat;
}
Material Material::createPhong(const vec3& color, float exp) {
    Material mat = {};
    mat.albedo = vec3(0.1f);
    mat.emissive = vec3(0.0f);
    mat.specular_chance = 1.0f;
    mat.specular_roughness = 0.0f;
    mat.specular_color = color;
    mat.IOR = -1.0f;
    mat.refraction_chance = 0.0f;
    mat.phongExponent = exp;
    mat.refraction_color = vec3(0.0f);
    mat.phong = 1.0f;
    mat.mirror_reflectance = 0.0f;
    mat.mIOR = -1.0f;
    mat.metal = 0.0f;
    return mat;
}
Material Material::createEmissive(const vec3& color) {
    Material mat = {};
    mat.albedo = vec3(0.0f);
    mat.emissive = color;
    mat.specular_chance = 0.0f;
    mat.specular_roughness = 0.0f;
    mat.specular_color = vec3(0.0f);
    mat.IOR = 1.0f;
    mat.refraction_chance = 0.0f;
    mat.phongExponent = 1.0f;
    mat.refraction_color = vec3(0.0f);
    mat.phong = 0.0f;
    mat.mirror_reflectance = 0.0f;
    mat.mIOR = -1.0;
    mat.metal = 0.0f;
    return mat;
}

}  // namespace assets
