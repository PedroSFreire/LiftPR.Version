const uint MaterialLambertian = 0;
const uint MaterialMetallic = 1;
const uint MaterialDielectric = 2;
const uint MaterialEmissive = 3;

struct Material {
    vec3  albedo;// the color used for diffuse lighting
    float specular_chance;
    vec3  emissive;// how much the surface glows
    float specular_roughness;// how rough the specular reflections are
    vec3  specular_color;// the color tint of specular reflections
    float IOR;// index of refraction. used by fresnel and refraction.
    vec3  refraction_color;// absorption for beer's law
    float refraction_chance;// percent chance of doing a refractive transmission
    float phongExponent;// phongExponent or roughness
    float phong;
    float  mirror_reflectance;
    float mIOR;
    float metal;
};

Material getZeroedMaterial() {
    Material mat;
    mat.albedo = vec3(0.0f);
    mat.emissive = vec3(0.0f);
    mat.specular_chance = 0.0f;
    mat.specular_roughness = 0.0f;
    mat.specular_color = vec3(0.0f);
    mat.IOR = -1.0f;
    mat.refraction_chance = 0.0f;
    mat.phongExponent = 1.0f;
    mat.refraction_color = vec3(0.0f);
    mat.phong = 0.0f;
    mat.mirror_reflectance = 0.0f;
    mat.mIOR =-1.0f;
    mat.metal = 0.0f;
    return mat;

}