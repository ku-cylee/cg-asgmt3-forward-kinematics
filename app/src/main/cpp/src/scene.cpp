#include "scene.h"
#include "binary/animation.h"
#include "binary/skeleton.h"
#include "binary/player.h"

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

bool mouseDown = false;
void Scene::update(float deltaTime) {
    Scene::program->use();

    Scene::camera->update();

    // Scene::player->load(playerVertices, playerIndices);
    // Scene::player->draw();

    // Line Drawer & Debugger
    glLineWidth(10);
    mat4 scaleDownMtx = mat4(.333f, 0, 0, 0,
                             0, .333f, 0, 0,
                             0, 0, .333f, 0,
                             0, 0, 0, 1);
    for (int idx = 0; idx < 28; idx++) {
        mat4 tfMat = mat4(1.0f);
        for (int parent = jParents[idx];
             parent >= 0;
             parent = jParents[parent]) tfMat = translate(tfMat, jOffsets[parent]);
        vec3 parentOffset = vec3(scaleDownMtx * tfMat * vec4(0, 0, 0, 1));
        vec3 currentOffset = vec3(scaleDownMtx * tfMat * vec4(jOffsets[idx], 1));
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