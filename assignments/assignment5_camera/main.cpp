#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <BenFolder/camera.h>
#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

ml::Camera camera;
ml::CameraControls cameraControls;

float prevTime;

void deafutValues() {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0, 0, 0);
	camera.fov = 60;
	camera.orthoSize = 6;
	camera.nearPlane = 0.1;
	camera.farPlane = 100;
	camera.aspectRatio = float(SCREEN_WIDTH) / SCREEN_HEIGHT;
	cameraControls.firstMouse = true;
	cameraControls.yaw = 0;
	cameraControls.pitch = 0;
}



void moveCamera(GLFWwindow* window, ml::Camera* camera, ml::CameraControls* controls, float deltaTime) {
	//If right mouse is not held, release cursor and return early.
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		//Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}

	//GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Get screen mouse position this frame
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	//If we just started right clicking, set prevMouse values to current position.
	//This prevents a bug where the camera moves as soon as we click.
	if (controls->firstMouse) {
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}
	// Get mouse position delta for this frame
	controls->yaw += (mouseX - controls->prevMouseX) * controls->mouseSensitivity;
	controls->pitch -= (mouseY - controls->prevMouseY) * controls->mouseSensitivity;

	// Clamp pitch between -89 and 89 degrees
	if (controls->pitch < -89)
	{
		controls->pitch = -89;
	}
	else if (controls->pitch > 89)
	{
		controls->pitch = 89;
	}

	//Remember previous mouse position
	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	float yawRad = ew::Radians(controls->yaw);
	float pitchRad = ew::Radians(controls->pitch);

	ew::Vec3 forward = ew::Vec3(sin(yawRad) * cos(pitchRad), sin(pitchRad), -cos(yawRad) * cos(pitchRad));
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, camera->up));
	ew::Vec3 up = ew::Normalize(ew::Cross(right, forward));
	
	// foward
	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera->position += forward * controls->moveSpeed * deltaTime;
	}
	// backwards
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera->position -= forward * controls->moveSpeed * deltaTime;
	}
	// right
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera->position += right * controls->moveSpeed * deltaTime;
	}
	// left
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera->position -= right * controls->moveSpeed * deltaTime;
	}
	// up
	if (glfwGetKey(window, GLFW_KEY_E)) {
		camera->position += up * controls->moveSpeed * deltaTime;
	}
	// down
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera->position -= up * controls->moveSpeed * deltaTime;
	}
	camera->target = camera->position + forward;
};

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	deafutValues();

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

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");

	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;


		moveCamera(window, &camera, &cameraControls, deltaTime);

		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set uniforms
		shader.use();

		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", camera.ProjectionMatrix() * camera.ViewMatrix() * cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		// Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			// Settings Window
			ImGui::Begin("Settings");

			// Cubes Section
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++) {
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader(("Cube " + std::to_string(i)).c_str())) {
					ImGui::DragFloat3("Position##Cube", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation##Cube", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale##Cube", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}

			// Camera Section
			ImGui::Text("Camera");
			ImGui::Checkbox("Orthographic Projection", &camera.orthographic);
			ImGui::DragFloat3("Camera Position", &camera.position.x, 0.05f);
			ImGui::DragFloat3("Camera Target", &camera.target.x, 0.05f);
			ImGui::DragFloat("Field of View", &camera.fov, 0.05f);
			ImGui::DragFloat("Orthographic Size", &camera.orthoSize, 0.05f);
			ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.05f);
			ImGui::DragFloat("Far Plane", &camera.farPlane, 0.05f);

			// Camera Controller Section
			ImGui::Text("Camera Controller");
			ImGui::Text("Yaw: %f", cameraControls.yaw);
			ImGui::Text("Pitch: %f", cameraControls.pitch);
			ImGui::DragFloat("Move Speed", &cameraControls.moveSpeed, 0.05f);

			// Reset Button
			if (ImGui::Button("Reset Button")) {
				deafutValues();
			}

			ImGui::End();

			// Rendering
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
	camera.aspectRatio = float(width) / height;
	// this allows you to change the screen size and still have the correct aspect ratio and such
}