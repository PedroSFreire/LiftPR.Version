#include "scene_list.h"
#include "assets/cornell_box.h"
#include "assets/model.h"
#include "assets/texture.h"
#include <core/log.h>

using namespace glm;
using assets::Material;
using assets::Model;
using assets::Texture;

const std::vector<std::pair<std::string, std::function<SceneAssets()>>> SceneList::all_scenes = {
    {"cornell box", cornellBox},
    {"teapot", teapot},
    {"cornell box dragon", cornellBoxDragon},
    {"cornell box dragon 2", cornellBoxDragon2},
    {"dining room", diningRoom},
    {"classroom", classroom},
    {"bathroom", bathroom},
    {"car", car},
    {"test", testScene},
    {"smallVcm", smallVcm},
};

SceneAssets SceneList::smallVcm() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(0.5, 0.5, 1.44);
    scene_assets.camera.look_at = vec3(0.5, 0.5, 0);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 512.0f;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::string mirror_id("mirror_id");
    Material mirror = {};
    mirror.mirror_reflectance = 1.0f;
    scene_assets.materials[mirror_id] = mirror;

    std::string glass_id("glass_id");
    Material glass = {};
    glass.mIOR = 1.6f;
    glass.mirror_reflectance = 1.0f;
    scene_assets.materials[glass_id] = glass;

    auto sphere = Model::createSphere(vec3(0.25f, 0.2f, -0.5f), 0.2f, mirror_id, false);
    scene_assets.models.push_back(sphere);

    auto sphere2 = Model::createSphere(vec3(0.75f, 0.2f, -0.5f), 0.2f, glass_id, false);
    scene_assets.models.push_back(sphere2);

        assets::CornellBox::create(1, scene_assets, false);

    Light light = {
        {0.3837f, 0.998f, -0.59f, 0},
        {0.234f, 0.0f, 0.0f, 0},
        {0.0f, 0.0f, 0.234f, 0},
        {0.0f, -1.0f, 0.0f, 0},
        {2.0f, 2.0f, 2.0f, 0},
    };

    scene_assets.lights.push_back(light);

    return scene_assets;
}


SceneAssets SceneList::teapot() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(0.5, 0.5, 1.44);
    scene_assets.camera.look_at = vec3(0.5, 0.5, 0);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1024.0f;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;
    
    std::string metal_id("metal");
    Material metal;
    //metal.mirror_reflectance = vec3(1.0f);
    //metal.specular_chance = 0.0f;
    //metal.specular_color = vec3(0.0f);
    //metal.albedo = vec3(0.0f);
    //metal.IOR = -1;
    //metal.specular_chance = 0.9f;
    //metal.phong = 0.0f;
    //metal.specular_roughness = 0.1f;
    //metal.specular_color = vec3(0.9f, 0.4f, 0.0f);
    //metal.phongExponent = 0.45;
    metal.mirror_reflectance = 1.0f;
    scene_assets.materials["metal"] = metal;


    scene_assets.materials["metal"] = metal;


    std::string glass_id("glass");
    Material glass = {};
    //glass.albedo = vec3(0.9f, 0.25f, 0.25f);
    //glass.emissive = vec3(0.0f, 0.0f, 0.0f);
    //glass.specular_chance = 0.00f;
    //glass.phong = 0.00f;
    //glass.specular_roughness = 0.0f;
    //glass.specular_color = vec3(1.0f, 1.0f, 1.0f) * 0.8f;
    //glass.IOR = 1.6f;
    //glass.mIOR = 1.6f;
    //glass.refraction_chance = 1.0f;
    //glass.phongExponent = 1.0f;
    //glass.refraction_color = vec3(1.0f, 0.0f, 0.0f);
    //glass.mirror_reflectance = 1.0f;
    glass.specular_color = vec3(1.000f, 0.766f, 0.336f);
    glass.phong = 2.0f;
    glass.phongExponent = 0.35f;
    glass.metal = 01.0f;
    scene_assets.materials[glass_id] = glass;

    auto box_back = Model::createBox(vec3(0, 0.00018, -0.297), vec3(0.297, 0.5946, 0), metal_id);
    box_back.transform(rotate(translate(mat4(1), vec3(0.2252, 0, -0.5315)), radians(45.0f), vec3(0, 1, 0)));
    scene_assets.models.push_back(box_back);

    auto sphere = Model::createSphere(vec3(0.75f, 0.27f, -0.5f), 0.15f, glass_id, false);
    scene_assets.models.push_back(sphere);
    
    //auto teapot = Model::loadModel("../resources/models/teapot.obj");
    //teapot.transform(rotate(scale(translate(mat4(1), vec3(0.603, 0.108, -0.153)), vec3(0.009f)), radians(-18.0f), vec3(0, 1, 0)));
    //teapot.setMaterial(glass_id);
    //scene_assets.models.push_back(teapot);

    assets::CornellBox::create(1, scene_assets, false);

    Light light = {
        {0.3837f, 0.998f, -0.59f, 0},
        {0.234f, 0.0f, 0.0f, 0},
        {0.0f, 0.0f, 0.234f, 0},
        {0.0f, -1.0f, 0.0f, 0},
        {1.0f, 1.0f, 1.0f, 0},
    };

    scene_assets.lights.push_back(light);

    return scene_assets;
}

SceneAssets SceneList::testScene() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(0.5, 0.5, 1.44);
    scene_assets.camera.look_at = vec3(0.5, 0.5, 0);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1024.0f;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::string glass_id("glass_id");
    Material glass = {};
    glass.mIOR = 1.3;
    glass.mirror_reflectance = 1.6f;
    scene_assets.materials[glass_id] = glass;

    std::string mirror_id("mirror_id");
    Material mirror = {};
    //glass.mIOR = 1.9;
    mirror.mirror_reflectance = 1.6f;
    scene_assets.materials[mirror_id] = mirror;

    auto box_back = Model::createBox(vec3(0, 0.00018, -0.297), vec3(0.297, 0.3946, 0), mirror_id);
    box_back.transform(rotate(translate(mat4(1), vec3(0.1252, 0, -0.7315)), radians(15.0f), vec3(0, 1, 0)));
    scene_assets.models.push_back(box_back);

    auto sphere = Model::createSphere(vec3(0.4f, 0.7f, -0.6f), 0.15f, glass_id, false);
    scene_assets.models.push_back(sphere);

    //auto sphere1 = Model::createSphere(vec3(0.35f, 0.2f, -0.5f), 0.1f, mirror_id, false);
    //scene_assets.models.push_back(sphere1);


    assets::CornellBox::create(1, scene_assets, false);

    Light light = {
        {0.3837f, 0.998f, -0.59f, 0},
        {0.234f, 0.0f, 0.0f, 0},
        {0.0f, 0.0f, 0.234f, 0},
        {0.0f, -1.0f, 0.0f, 0},
        {0.5f, 0.5f, 0.5f, 0},
    };

    scene_assets.lights.push_back(light);

    return scene_assets;


}

SceneAssets SceneList::cornellBox() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(0.5, 0.5, 1.44);
    scene_assets.camera.look_at = vec3(0.5, 0.5, 0);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1024.0f;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::string glass_id("glass_id");
    Material glass = {};
    //glass.mIOR = 1.1f;
    glass.mirror_reflectance = 1.0f;
    scene_assets.materials[glass_id] = glass;

    std::string gray("gray diffuse");
    scene_assets.materials[gray] = Material::createLambertian(vec3(0.73f, 0.73f, 0.73f));

    auto box_front = Model::createBox(vec3(0, 0.00018, -0.297), vec3(0.297, 0.297, 0), glass_id);
    auto box_back = Model::createBox(vec3(0, 0.00018, -0.297), vec3(0.297, 0.5946, 0), glass_id);

    box_front.transform(rotate(translate(mat4(1), vec3(0.468, 0, -0.117)), radians(-18.0f), vec3(0, 1, 0)));
    box_back.transform(rotate(translate(mat4(1), vec3(0.2252, 0, -0.5315)), radians(15.0f), vec3(0, 1, 0)));

    assets::CornellBox::create(1, scene_assets, false);
    scene_assets.models.push_back(box_front);
    scene_assets.models.push_back(box_back);

    Light light = {
        {0.3837f, 0.998f, -0.59f, 0},
        {0.234f, 0.0f, 0.0f, 0},
        {0.0f, 0.0f, 0.234f, 0},
        {0.0f, -1.0f, 0.0f, 0},
        {2.0f, 2.0f, 2.0f, 0},
    };


    scene_assets.lights.push_back(light);
    return scene_assets;
}
/*
SceneAssets SceneList::cornellBoxDragon() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(0.5, 0.5, 1.44);
    scene_assets.camera.look_at = vec3(0.5, 0.5, 0);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1024.0f;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::string mat1_id("mat1");
    Material mat1;
    //mat1.specular_chance = 0.8f;
    //mat1.phong = 0.0f;
    //mat1.specular_roughness = 0.6f;
    //mat1.specular_color = vec3(0.9f, 0.4f, 0.0f);
    
    //mat1.mIOR = 2.0f;
    mat1.mirror_reflectance = 1.0f;
    
        
    scene_assets.materials[mat1_id] = mat1;

    std::string glass_id("glass_id");
    Material glass = {};
    glass.mIOR = 1.9f;
    glass.mirror_reflectance = 1.0f;
    scene_assets.materials[glass_id] = glass;

    std::string glass_id1("glass_id1");
    Material glass1 = {};
    glass1.mIOR = 1.6f;
    glass1.mirror_reflectance = 1.0f;
    scene_assets.materials[glass_id1] = glass1;

    //auto sphere = Model::createSphere(vec3(0.0f, 1.0f, -1.0f), 0.15f, glass_id, false);
    //scene_assets.models.push_back(sphere);

    //auto sphere1 = Model::createSphere(vec3(1.0f, 1.0f, -1.0f), 0.15f, glass_id, false);
    //scene_assets.models.push_back(sphere1);

    auto sphere = Model::createSphere(vec3(0.8f, 0.3f, -0.6f), 0.15f, glass_id1, false);
    scene_assets.models.push_back(sphere);


    auto dragon = Model::loadModel("../resources/models/dragon.obj");
    auto t = rotate(scale(translate(mat4(1), vec3(0.468f, 0.288f, -0.63f)), vec3(1.08f)), radians(135.0f), vec3(0, 1, 0));
    dragon.transform(t);
    dragon.setMaterial(mat1_id);
    scene_assets.models.push_back(dragon);

    assets::CornellBox::create(1, scene_assets, false);

    Light light = {
        {0.3837f, 0.998f, -0.59f, 0},
        {0.234f, 0.0f, 0.0f, 0},
        {0.0f, 0.0f, 0.234f, 0},
        {0.0f, -1.0f, 0.0f, 0},
        {1.0f, 1.0f, 1.0f, 0},
    };


    scene_assets.lights.push_back(light);

    return scene_assets;
}

*/



SceneAssets SceneList::cornellBoxDragon() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(0.5, 0.5, 1.44);
    scene_assets.camera.look_at = vec3(0.5, 0.5, 0);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    //scene_assets.camera.focus_distance = 10.0f;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::string mat1_id("mat1");
    Material mat1;
    mat1.specular_chance = 0.8f;
    mat1.specular_roughness = 0.6f;
    mat1.specular_color = vec3(0.9f, 0.4f, 0.0f);
    mat1.phong = 2;
    mat1.metal = 0;
    mat1.phongExponent = 0.6f;
    scene_assets.materials[mat1_id] = mat1;

    std::string glass_id("glass_id");
    Material glass = {};
    glass.mIOR = 1.1f;
    glass.mirror_reflectance = 1.0f;
    scene_assets.materials[glass_id] = glass;

    auto sphere = Model::createSphere(vec3(0.7f, 0.27f, -0.3f), 0.15f, glass_id, false);
    scene_assets.models.push_back(sphere);

    auto dragon = Model::loadModel("../resources/models/dragon.obj");
    auto t =
        rotate(scale(translate(mat4(1), vec3(0.468f, 0.288f, -0.63f)), vec3(1.08f)), radians(135.0f), vec3(0, 1, 0));
    dragon.transform(t);
    dragon.setMaterial(mat1_id);
    scene_assets.models.push_back(dragon);

    assets::CornellBox::create(1, scene_assets, true);

    Light light = {
        {0.3837f, 0.998f, -0.59f, 0},
        {0.234f, 0.0f, 0.0f, 0},
        {0.0f, 0.0f, 0.234f, 0},
        {0.0f, -1.0f, 0.0f, 0},
        {10.0f, 10.0f, 10.0f, 0},
    };

    scene_assets.lights.push_back(light);

    return scene_assets;
}



SceneAssets SceneList::cornellBoxDragon2() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(0.5, 0.2, 0.6);
    scene_assets.camera.look_at = vec3(0.5, 0.2, 0);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1024.0f;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::string mat1_id("mat1");
    Material mat1;
    // mat1.specular_chance = 0.8f;
    // mat1.phong = 0.0f;
    // mat1.specular_roughness = 0.6f;
    // mat1.specular_color = vec3(0.9f, 0.4f, 0.0f);

    mat1.mIOR = 1.3f;
    mat1.mirror_reflectance = 1.0f;

    scene_assets.materials[mat1_id] = mat1;

    std::string glass_id("glass_id");
    Material glass = {};
    //glass.mIOR = 1.9; 
    glass.mirror_reflectance = 1.0f;
    scene_assets.materials[glass_id] = glass;

    // auto sphere = Model::createSphere(vec3(0.0f, 1.0f, -1.0f), 0.15f, glass_id, false);
    // scene_assets.models.push_back(sphere);

    // auto sphere1 = Model::createSphere(vec3(1.0f, 1.0f, -1.0f), 0.15f, glass_id, false);
    // scene_assets.models.push_back(sphere1);

    auto sphere = Model::createSphere(vec3(0.2f, 0.5f, -0.4f), 0.15f, mat1_id, false);
    scene_assets.models.push_back(sphere);

    auto sphere1 = Model::createSphere(vec3(0.5f, 0.2f, -0.8f), 0.15f, glass_id, false);
    scene_assets.models.push_back(sphere1);

    auto dragon = Model::loadModel("../resources/models/dragon.obj");
    //auto t = rotate(scale(translate(mat4(1), vec3(0.468f, 0.288f, -0.63f)), vec3(0.5f)), radians(135.0f), vec3(0, 1, 0));
    auto t = rotate(scale(translate(mat4(1), vec3(0.8f, 0.5f, -0.4f)), vec3(0.5f)), radians(135.0f), vec3(0, 1, 0));
    dragon.transform(t);
    dragon.setMaterial(mat1_id);
    scene_assets.models.push_back(dragon);
    //auto dragon1 = Model::loadModel("../resources/models/dragon.obj");
    //auto t1 = rotate(scale(translate(mat4(1), vec3(0.2f, 0.4f, -0.2f)), vec3(0.5f)), radians(210.0f), vec3(0, 1, 0));
    //dragon1.transform(t1);
    //dragon1.setMaterial(mat1_id);
    //scene_assets.models.push_back(dragon1);

    assets::CornellBox::create(1, scene_assets, false);

    Light light = {
        {0.3837f, 0.998f, -0.59f, 0},
        {0.234f, 0.0f, 0.0f, 0},
        {0.0f, 0.0f, 0.234f, 0},
        {0.0f, -1.0f, 0.0f, 0},
        {2.0f, 2.0f, 2.0f, 0},
    };

    scene_assets.lights.push_back(light);

    return scene_assets;
}



SceneAssets SceneList::diningRoom() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(3, 4, 9);
    scene_assets.camera.look_at = vec3(1.7, 3.4, 6);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1280.0f;
    scene_assets.camera.aspect_ratio = 1;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::shared_ptr<pbrt::Scene> scene = pbrt::importPBRT("../resources/scenes/dining-room/scene.pbrt");
    //        std::shared_ptr<pbrt::Scene> scene = pbrt::importPBRT("../resources/scenes/cornell-box/scene.pbrt");
    scene->makeSingleLevel();
    scene_assets.textures = std::vector<Texture>();

    traverse(scene->world, scene_assets);

//    scene_assets.materials["emissive"] = Material::createEmissive(vec3(150.0f));
    //    auto sphere = Model::createSphere(vec3(90, 30, 2), 15.0f, "emissive", false);
//    auto sphere = Model::createSphere(vec3(0, 3, 0), .3f, "emissive", false);
//    scene_assets.models.push_back(sphere);

    vec3 v1 = vec3(-10, 2, 50);
    vec3 v2 = vec3(-10, 1, -10);
    vec3 normal = normalize(cross(v1, v2));
    Light light = {
        {22, 15, 4, 0},
        vec4(v1, 0),
        vec4(v2, 0),
        vec4(normal, 0),
        {15.0f, 15.0f, 15.0f, 0},
    };

    scene_assets.lights.push_back(light);

    return scene_assets;
}

SceneAssets SceneList::classroom() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(-3, 2.8, 4);
    scene_assets.camera.look_at = vec3(-1, 1, 0);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1280.0f;
    scene_assets.camera.aspect_ratio = 1;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::shared_ptr<pbrt::Scene> scene = pbrt::importPBRT("../resources/scenes/classroom/scene.pbrt");
    scene->makeSingleLevel();
    scene_assets.textures = std::vector<Texture>();

    traverse(scene->world, scene_assets);


    vec3 v1 = vec3(-3.0f, 3.0f, 0.0f);
    vec3 v2 = vec3(0.0f, 3.0f, 3.0f);
    vec3 normal = normalize(cross(v1, v2));

    Light light = {
        {-20, 1.8, 1, -3},
        {-3.0f, 3.0f, 0.0f, 0},
        {0.0f, 3.0f, 3.0f, 0},
        vec4(normal,0),
        {30.0f, 30.0f, 30.0f, 0},
    };

    scene_assets.lights.push_back(light);
    return scene_assets;
}

SceneAssets SceneList::car() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(7, 2, -3);
    //scene_assets.camera.eye = vec3(5, 15, -3);
    scene_assets.camera.look_at = vec3(-5, 2, -3);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1280.0f;
    scene_assets.camera.aspect_ratio = 1;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::shared_ptr<pbrt::Scene> scene = pbrt::importPBRT("../resources/scenes/veach-ajar/scene.pbrt");
    scene->makeSingleLevel();
    scene_assets.textures = std::vector<Texture>();

    traverse(scene->world, scene_assets);

    vec3 v1 = vec3(0.0f, 2.0f, 0.0f);
    vec3 v2 = vec3(0.0f, 0.0f, 5.0f);
    vec3 normal = normalize(cross(v1, v2));

    Light light = {
        {-8, 1, -6, 51},
        vec4(v1, 0),
        vec4(v2, 0),
        vec4(normal, 0),
        {20.0f, 20.0f, 20.0f, 0},
    };

    scene_assets.lights.push_back(light);
    return scene_assets;
}

SceneAssets SceneList::bathroom() {
    SceneAssets scene_assets;
    scene_assets.camera.eye = vec3(10, 25, 51);
    scene_assets.camera.look_at = vec3(6, 25, 41);
    scene_assets.camera.up = vec3(0, 1, 0);
    scene_assets.camera.field_of_view = 40;
    scene_assets.camera.aperture = 0.0f;
    scene_assets.camera.resX = 1280.0f;
    scene_assets.camera.aspect_ratio = 1;
    scene_assets.camera.gamma_correction = true;
    scene_assets.camera.has_sky = false;

    std::shared_ptr<pbrt::Scene> scene = pbrt::importPBRT("../resources/scenes/bathroom/scene.pbrt");
    scene->makeSingleLevel();
    scene_assets.textures = std::vector<Texture>();

    traverse(scene->world, scene_assets);

    Light light = {
        {10, 26, 3, 51},
        {15.0f, 0.0f, 0.0f, 0},
        {0.0f, 0.0f, 15.0f, 0},
        {0.0f, -1.0f, 0.0f, 0},
        {5.0f, 5.0f, 5.0f, 0},
    };

    scene_assets.lights.push_back(light);
    return scene_assets;
}

void traverse(const pbrt::Object::SP& object, SceneAssets& scene_assets) {
    scene_assets.materials["default"] = Material::createLambertian(vec3(0.75f));

    LF_INFO("\tNumber of meshes: {}", object->shapes.size() / 3);

    for (const auto& shape : object->shapes) {
        auto material_id = shape->material->name;
        if (!scene_assets.materials.contains(material_id)) {
            if (auto substrate = std::dynamic_pointer_cast<pbrt::SubstrateMaterial>(shape->material)) {
                LF_INFO("Implemented Material -> {}", material_id);
                Material material {};
                material.albedo = vec3(substrate->kd.x, substrate->kd.y, substrate->kd.z);
                material.specular_color = vec3(substrate->ks.x, substrate->ks.y, substrate->ks.z);
                material.specular_roughness = substrate->uRoughness;
                scene_assets.materials[material_id] = material;
            } else if (auto metal = std::dynamic_pointer_cast<pbrt::MetalMaterial>(shape->material)) {
                LF_INFO("Implemented Material -> {}", material_id);
                Material material {};
                material.specular_color = normalize(vec3(metal->k.x, metal->k.y, metal->k.z));
                material.specular_roughness =0.3f + metal->uRoughness;
                //material.specular_chance = 1.0f - metal->vRoughness;
                material.phong = 2;
                material.metal = 1.0f;
                material.phongExponent = metal->uRoughness;
                scene_assets.materials[material_id] = material;
            } else if (auto matte = std::dynamic_pointer_cast<pbrt::MatteMaterial>(shape->material)) {
                LF_INFO("Implemented Material -> {}", material_id);
                Material material {};
                material.albedo = vec3(matte->kd.x, matte->kd.y, matte->kd.z);
                //material.phong = 1.0f;
                scene_assets.materials[material_id] = material;
            } else if (auto mirror = std::dynamic_pointer_cast<pbrt::MirrorMaterial>(shape->material)) {
                LF_INFO("Implemented Material -> {}", material_id);
                Material material {};
                material.mirror_reflectance = 1.0f;
                //material.IOR = -1.0f;
                scene_assets.materials[material_id] = material;
            } else if (auto glass = std::dynamic_pointer_cast<pbrt::GlassMaterial>(shape->material)) {
                LF_INFO("Implemented Material -> {}", material_id);
                Material material {};
                //material.albedo = vec3(0.9f, 0.25f, 0.25f);
                //material.emissive = vec3(0.0f, 0.0f, 0.0f);
                //material.specular_chance = 0.02f;
                //material.phong = 0.0f;
                //material.specular_roughness = 0.0f;
                //material.specular_color = vec3(1.0f, 1.0f, 1.0f) * 0.8f;
                //material.IOR = 1.1f;
                //material.refraction_chance = 1.0f;
                //material.phongExponent = 1.0f;
                //material.refraction_color = vec3(1.0f, 0.0f, 0.0f);
                material.mIOR = glass->index;
                material.mirror_reflectance = 1.0f;
                scene_assets.materials[material_id] = material;
            } else if (auto glass = std::dynamic_pointer_cast<pbrt::UberMaterial>(shape->material)) {
                Material material {};
                material.mirror_reflectance = 1.0f;
                material.mIOR = 1.0f;
            }
            else {
                LF_WARN("Skipped Material -> {}", material_id);
                material_id = "default";
            }
        }

        if (pbrt::TriangleMesh::SP mesh = std::dynamic_pointer_cast<pbrt::TriangleMesh>(shape)) {
            // Geometry
            std::vector<assets::Vertex> vertices;
            std::vector<uint32_t> indices;

            // LF_INFO("vertex count {}", mesh->vertex.size());
            // LF_INFO("normal count {}", mesh->normal.size());
            // LF_INFO("index count {}", mesh->index.size());

            for (uint32_t i = 0; i < mesh->vertex.size(); i++) {
                assets::Vertex vertex = {};
                const auto& pos = mesh->vertex[i];
                vertex.position = vec3(pos.x, pos.y, pos.z);

                if (i < mesh->normal.size()) {
                    const auto& normal = mesh->normal[i];
                    vertex.normal = vec3(normal.x, normal.y, normal.z);
                } else {
                    vertex.normal = vec3(0);
                }

                vertex.material_index = 0;

                vertices.push_back(vertex);
            }

            for (const auto& index : mesh->index) {
                indices.push_back(index.x);
                indices.push_back(index.y);
                indices.push_back(index.z);

                // fix normals
                auto v0 = vertices[index.x];
                if (v0.normal == vec3(0)) {
                    auto v1 = vertices[index.y];
                    auto v2 = vertices[index.z];
                    auto normal = normalize(cross(v1.position - v0.position, v2.position - v0.position));
                    vertices[index.x].normal = normal;
                    vertices[index.y].normal = normal;
                    vertices[index.z].normal = normal;
                }
            }

            LF_INFO("\tNumber of triangles: {}", indices.size() / 3);
            scene_assets.models.emplace_back(std::move(vertices), std::move(indices), material_id, nullptr);

        } else {
            LF_ERROR("pbrt shape not supported");
        }
    }

    LF_INFO("shape count {}", object->shapes.size());

    for (const auto& inst : object->instances) {
        traverse(inst->object, scene_assets);
    }
}