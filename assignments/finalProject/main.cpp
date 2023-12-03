#include <stdio.h>
#include <math.h>
#include <string>
#include <random>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
    ew::Vec3 position = ew::Vec3(0.0, 0.0, 0.0);
    ew::Vec3 color = ew::Vec3(0.0, 0.0, 0.0);
};

struct Material {
    float ambientK = (float)0.2; // (0-1)
    float diffuseK = (float)0.5; // (0-1)
    float specular = (float)0.2; // (0-1)
    float shininess = (float)128.0;
};

int main() {
    printf("Initializing...");
    if (!glfwInit()) {
        printf("GLFW failed to init!");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
    if (window == NULL) {
        printf("GLFW failed to create window");
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("GLAD Failed to load GL headers");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
    unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg", GL_REPEAT, GL_LINEAR);

    ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");

    // Create pond mesh
    ew::Mesh pondMesh(ew::createPond(3.0f,20));

    // Initialize lights
    const int LIGHT_MAX = 5;
    int numLights = 1;
    Light lights[LIGHT_MAX];
    Material material1;

    // random on startup can't figure out how to make it random when removing them and all
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-3.0f, 3.0f);
    std::uniform_real_distribution<float> colorDis(0.0f, 1.0f);

    for (auto i = 0; i < LIGHT_MAX; i++) {
        lights[i].position = ew::Vec3(dis(gen), 2.5, dis(gen));
        lights[i].color = ew::Vec3(colorDis(gen), colorDis(gen), colorDis(gen));
    }

    // Initialize transforms
    ew::Transform pondTransform;
    pondTransform.position = ew::Vec3(0, -1.0, 0);

    for (int i = 0; i < LIGHT_MAX; i++)
        lights[i].position = pondTransform.position;

    resetCamera(camera, cameraController);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glDisable(GL_CULL_FACE);

        float time = (float)glfwGetTime();
        float deltaTime = time - prevTime;
        prevTime = time;

        camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
        cameraController.Move(window, &camera, deltaTime);

        //RENDER
        glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glBindTexture(GL_TEXTURE_2D, brickTexture);
        shader.setInt("_Texture", 0);
        shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

        shader.setMat4("_Model", pondTransform.getModelMatrix());
        pondMesh.draw();

        // Render point lights
        shader.setInt("_numLights", numLights);
        shader.setVec3("_Camerapose", camera.position);
        shader.setFloat("_Material.ambientK", material1.ambientK);
        shader.setFloat("_Material.diffuseK", material1.diffuseK);
        shader.setFloat("_Material.specular", material1.specular);
        shader.setFloat("_Material.shininess", material1.shininess);

        for (int i = 0; i < numLights; i++)
        {
            lights[i].position = pondTransform.position;
            shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
            shader.setVec3("_Lights[" + std::to_string(i) + "].position", lights[i].position);
        }

        unlitShader.use();
        unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

        // Render UI
        {
            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");
            if (ImGui::CollapsingHeader("Camera")) {
                ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
                ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
                ImGui::Checkbox("Orthographic", &camera.orthographic);
                if (camera.orthographic) {
                    ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
                }
                else {
                    ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
                }
                ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
                ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
                ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
                ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
                if (ImGui::Button("Reset")) {
                    resetCamera(camera, cameraController);
                }
            }
            ImGui::ColorEdit3("BG color", &bgColor.x);

            ImGui::SliderInt("Number of Lights", &numLights, 0, LIGHT_MAX);

            for (auto i = 0; i < numLights; i++)
            {
                if (ImGui::CollapsingHeader(("Light " + std::to_string(i + 1)).c_str()))
                {
                    ImGui::DragFloat3("Light Position", &lights[i].position.x, 0.1f);
                    ImGui::ColorEdit3("Light Color", &lights[i].color.x);
                }
            }

            // material values
            ImGui::DragFloat("ambientK", &material1.ambientK, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("diffuseK", &material1.diffuseK, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("specular", &material1.specular, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("shininess", &material1.shininess, 1.0f, 0.0f, 6400.0f);

            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window);
    }
    printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
    camera.position = ew::Vec3(0, 0, 5);
    camera.target = ew::Vec3(0);
    camera.fov = 60.0f;
    camera.orthoHeight = 6.0f;
    camera.nearPlane = 0.1f;
    camera.farPlane = 100.0f;
    camera.orthographic = false;

    cameraController.yaw = 0.0f;
    cameraController.pitch = 0.0f;
}
