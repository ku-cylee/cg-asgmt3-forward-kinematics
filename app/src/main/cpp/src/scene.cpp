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
    if (mouseDown) elapsedTime += deltaTime;

    Scene::program->use();

    Scene::camera->update();

    int prevTime = int(elapsedTime);
    float timeDelta = elapsedTime - float(prevTime);
    vector<float> prevMotion = motions[prevTime % 4];
    vector<float> nextMotion = motions[(prevTime + 1) % 4];

    vector<mat4> animations;
    vector<mat4> boneToWorld = { mat4(1.0f) };
    for (int idx = 0; idx < jNames.size(); idx++) {
        quat prevQuat = getRotationQuat(slice(prevMotion, 3 * idx + 3));
        quat nextQuat = getRotationQuat(slice(nextMotion, 3 * idx + 3));
        mat4 rotateMtx = mat4_cast(slerp(prevQuat, nextQuat, timeDelta));

        if (idx == 0) {
            mat4 translateMtx = translate(mix(slice(prevMotion), slice(nextMotion), timeDelta));
            animations.push_back(translateMtx * rotateMtx);
        } else {
            int parentIdx = jParents[idx];
            mat4 toParentMtx = translate(jOffsets[idx]);
            animations.push_back(animations[parentIdx] * toParentMtx * rotateMtx);
            boneToWorld.push_back(boneToWorld[parentIdx] * toParentMtx);
        }
    }

    vector<Vertex> vertices;
    for (int idx = 0; idx < playerVertices.size(); idx++) {
        Vertex vertex = playerVertices[idx];
        mat4 skinningMtx = mat4(0.0f);
        for (int boneIdx = 0; boneIdx < 4; boneIdx++) {
            int bone = vertex.bone[boneIdx];
            if (bone < 0) continue;
            float weight = vertex.weight[boneIdx];
            skinningMtx += weight * animations[bone] * inverse(boneToWorld[bone]);
        }
        vertex.pos = vec3(skinningMtx * vec4(vertex.pos, 1));
        vertex.nor = vec3(skinningMtx * vec4(vertex.nor, 1));
        vertices.push_back(vertex);
    }

    Scene::player->load(vertices, playerIndices);
    Scene::player->draw();
}

void Scene::mouseUpEvents(float x, float y, bool doubleTouch) {
    mouseDown = false;
}

void Scene::mouseDownEvents(float x, float y, bool doubleTouch) {
    mouseDown = true;
}

void Scene::mouseMoveEvents(float x, float y, bool doubleTouch) {

}