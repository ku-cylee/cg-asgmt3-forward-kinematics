#include "scene.h"
#include "binary/animation.h"
#include "binary/skeleton.h"
#include "binary/player.h"
#include "glm/gtx/quaternion.hpp"

Shader* Scene::vertexShader = nullptr;
Shader* Scene::fragmentShader = nullptr;
Program* Scene::program = nullptr;
Camera* Scene::camera = nullptr;
Object* Scene::player = nullptr;
Texture* Scene::diffuse = nullptr;
Material* Scene::material = nullptr;
Object* Scene::lineDraw = nullptr;
Texture* Scene::lineColor = nullptr;
Material* Scene::lineMaterial = nullptr;

void Scene::setup(AAssetManager* aAssetManager) {
    Asset::setManager(aAssetManager);

    Scene::vertexShader = new Shader(GL_VERTEX_SHADER, "vertex.glsl");
    Scene::fragmentShader = new Shader(GL_FRAGMENT_SHADER, "fragment.glsl");

    Scene::program = new Program(Scene::vertexShader, Scene::fragmentShader);

    Scene::camera = new Camera(Scene::program);
    Scene::camera->eye = vec3(0.0f, 0.0f, 80.0f);

    Scene::diffuse = new Texture(Scene::program, 0, "textureDiff", playerTexels, playerSize);
    Scene::material = new Material(Scene::program, diffuse);
    Scene::player = new Object(program, material, playerVertices, playerIndices);
    player->worldMat = scale(vec3(1.0f / 3.0f));

    Scene::lineColor = new Texture(Scene::program, 0, "textureDiff", {{0xFF, 0x00, 0x00}}, 1);
    Scene::lineMaterial = new Material(Scene::program, lineColor);
    Scene::lineDraw = new Object(program, lineMaterial, {{}}, {{}}, GL_LINES);
}

void Scene::screen(int width, int height) {
    Scene::camera->aspect = (float) width/height;
}

#define BONES_COUNT 28

vec3 slice(vector<float> vec, int startIdx = 0) {
    return vec3(vec[startIdx], vec[startIdx + 1], vec[startIdx + 2]);
}

quat getRotationQuat(vec3 angles) {
    float halfAngleX = radians(angles[0] / 2);
    float halfAngleY = radians(angles[1] / 2);
    float halfAngleZ = radians(angles[2] / 2);

    quat rotX = quat(cos(halfAngleX), sin(halfAngleX), 0, 0);
    quat rotY = quat(cos(halfAngleY), 0, sin(halfAngleY), 0);
    quat rotZ = quat(cos(halfAngleZ), 0, 0, sin(halfAngleZ));

    return rotZ * rotX * rotY;
}

float elapsedTime = 0.0f;
bool mouseDown = false;
void Scene::update(float deltaTime) {
    elapsedTime += deltaTime;

    Scene::program->use();

    Scene::camera->update();

    // Scene::player->load(playerVertices, playerIndices);
    // Scene::player->draw();

    int prevTime = int(elapsedTime);
    float timeDelta = elapsedTime - prevTime;
    vector<float> prevMotion = motions[prevTime % 4];
    vector<float> nextMotion = motions[(prevTime + 1) % 4];
    vector<mat4> animations = { translate(mat4(1.0f), mix(slice(prevMotion), slice(nextMotion), timeDelta)) };
    for (int idx = 1; idx < BONES_COUNT; idx++) {
        quat prevQuat = getRotationQuat(slice(prevMotion, 3 * idx + 3));
        quat nextQuat = getRotationQuat(slice(nextMotion, 3 * idx + 3));

        mat4 parentAniMtx = animations[jParents[idx]];
        mat4 rotateMtx = mat4_cast(mix(prevQuat, nextQuat, timeDelta));
        animations.push_back(parentAniMtx * translate(rotateMtx, jOffsets[idx]));
    }

    // Line Drawer & Debugger
    glLineWidth(10);
    mat4 scaleMtx = mat4(.333f, 0, 0, 0,
                         0, .333f, 0, 0,
                         0, 0, .333f, 0,
                         0, 0, 0, 1);
    for (int idx = 1; idx < BONES_COUNT; idx++) {
        vec3 parentOffset = vec3(scaleMtx * animations[idx] * vec4(0, 0, 0, 1));
        vec3 currentOffset = vec3(scaleMtx * animations[jParents[idx]] * vec4(0, 0, 0, 1));
        Scene::lineDraw->load({{parentOffset}, {currentOffset}}, {0, 1});
        Scene::lineDraw->draw();
    }

    LOG_PRINT_DEBUG("You can also debug variables with this function: %f", M_PI);
}

void Scene::mouseUpEvents(float x, float y, bool doubleTouch) {

}

void Scene::mouseDownEvents(float x, float y, bool doubleTouch) {

}

void Scene::mouseMoveEvents(float x, float y, bool doubleTouch) {

}