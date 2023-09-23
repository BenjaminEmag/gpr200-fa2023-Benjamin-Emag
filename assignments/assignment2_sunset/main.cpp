#include <stdio.h>
#include <math.h>

#include <BenFolder/shader.h>
#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

std::string vertexShaderSource = MyLib::loadShaderSourceFromFile("assets/vertexShader.vert");
std::string fragmentShaderSource = MyLib::loadShaderSourceFromFile("assets/fragmentShader.frag");


struct Vertex {
	float x, y, z;
	float u, v;
};

// Define vertices for a rectangle (4 vertices)
MyLib::Vertex vertices[4] = {
	{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f }, // Bottom left
	{  0.5f, -0.5f, 0.0f, 1.0f, 0.0f }, // Bottom right
	{  0.5f,  0.5f, 0.0f, 1.0f, 1.0f }, // Top right
	{ -0.5f,  0.5f, 0.0f, 0.0f, 1.0f }  // Top left
};


// Define indices for a rectangle (6 indices, forming two triangles)
unsigned int indices[6] = {
    0, 1, 2, // First triangle
    2, 3, 0  // Second triangle
};

float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;

bool showImGUIDemoWindow = true;
bool drawWireframe = false;

int main()
{
	printf("Initializing...");
	if (!glfwInit())
	{
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL)
	{
		printf("GLFW failed to create window");
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

		glDisable(GL_CULL_FACE);

	MyLib::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();

	unsigned int vao = MyLib::createVAO(vertices, 4, indices, 6);

	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		// shader.setVec3("Color", triangleColor[0], triangleColor[1], triangleColor[2]);
		// shader.setInt("Brightness", triangleBrightness);
		shader.setFloat("Time", glfwGetTime());

		if (drawWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			// Create ImGui UI for shader parameters
			if (ImGui::CollapsingHeader("Speed Settings"))
			{

			}

			if (ImGui::CollapsingHeader("Color Settings"))
			{

			}

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::ColorEdit3("Color", triangleColor);
			ImGui::SliderFloat("Brightness", &triangleBrightness, 0.0f, 1.0f);
			ImGui::End();


			if (showImGUIDemoWindow)
			{
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

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
}

