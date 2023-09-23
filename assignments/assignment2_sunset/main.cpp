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


MyLib::Vertex vertices[4] = {
	//x    y    z    u    v
	{ -1, -1, 0.0, 0.0, 0.0 }, //Bottom left
	{ 1, -1, 0.0, 1.0, 0.0 }, //Bottom right
	{ 1,  1, 0.0, 1.0, 1.0 },  //Top right
	{ -1, 1, 0.0, 0.0, 1.0 }  //Top left
};

unsigned int indices[6] = {
    0, 1, 2, // First triangle
    2, 3, 0  // Second triangle
};

float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;

bool showImGUIDemoWindow = false;
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

	// Set Values
	float sunColor[3] = {0.76, 0.86, 0.14};
	float dayColorTop[3] = { 1.0, 0.35, 0.77 };
	float dayColorBottom[3] = { 0.56, 0.46, 0.92 };
	float nightColorTop[3] = { 0.01, 0.05, 0.19 };
	float nightColorBottom[3] = { 0.24, 0.27, 0.36 };
	float sunSpeed = 1.0;
	float sunSize = 0.2;
	float buildingColor[3] = { 0.4, 0.4, 0.4 };

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		shader.setVec2("aspectRatio", SCREEN_WIDTH, SCREEN_HEIGHT);
		shader.setFloat("Time", glfwGetTime());
		shader.setVec3("sunColor", sunColor[0], sunColor[1], sunColor[2]);
		shader.setVec3("dayColorTop", dayColorTop[0], dayColorTop[1], dayColorTop[2]);
		shader.setVec3("dayColorBottom", dayColorBottom[0], dayColorBottom[1], dayColorBottom[2]);
		shader.setVec3("nightColorTop", nightColorTop[0], nightColorTop[1], nightColorTop[2]);
		shader.setVec3("nightColorBottom", nightColorBottom[0], nightColorBottom[1], nightColorBottom[2]);
		shader.setVec3("buildingColor", buildingColor[0], buildingColor[1], buildingColor[2]);
		shader.setFloat("sunSpeed", sunSpeed);
		shader.setFloat("sunSize", sunSize);

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
				ImGui::SliderFloat("Sun Speed", &sunSpeed, 0.1f, 5.0f); 
				ImGui::SliderFloat("Sun Size", &sunSize, 0.1f, 0.9f);  
			}

			if (ImGui::CollapsingHeader("Color Settings"))
			{
				ImGui::ColorEdit3("Sun Color", sunColor);             
				ImGui::ColorEdit3("Day Color Top", dayColorTop);      
				ImGui::ColorEdit3("Day Color Bottom", dayColorBottom); 
				ImGui::ColorEdit3("Night Color Top", nightColorTop);  
				ImGui::ColorEdit3("Night Color Bottom", nightColorBottom);
				ImGui::ColorEdit3("Building Color", buildingColor);   
			}
			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::ColorEdit3("Triangle Color", triangleColor);
			ImGui::SliderFloat("Brightness", &triangleBrightness, 0.0f, 1.0f);
			ImGui::Checkbox("Draw Wireframe", &drawWireframe);
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