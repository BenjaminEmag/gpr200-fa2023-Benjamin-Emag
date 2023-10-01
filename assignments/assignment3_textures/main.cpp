#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <BenFolder/texture.h>

struct Vertex {
	float x, y, z;
	float u, v;
};

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

Vertex vertices[4] = {
	{-1.0, -1.0, 0.0, 0.0, 0.0},
	{1.0, -1.0, 0.0, 1.0, 0.0},
	{1.0, 1.0, 0.0, 1.0, 1.0},
	{-1.0, 1.0, 0.0, 0.0, 1.0}
};
unsigned short indices[6] = {
	0, 1, 2,
	2, 3, 0
};

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures", NULL, NULL);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	ew::Shader background("assets/background.vert", "assets/background.frag");
	ew::Shader character("assets/littleguy.vert", "assets/littleguy.frag");

	unsigned int characterTexture = loadTexture("assets/textures/character.png", 2, 2); // nearest neighbor filtering.
	unsigned int backgroundTexture = loadTexture("assets/textures/background.png", 1, 1); // billinear filtering 
	unsigned int noiseTexture = loadTexture("assets/textures/noise.png", 1, 0);

	int imageSizeWidth = 128;
	int imageSizeHeight = 128;
	float speed = 1.0;
	float alphaRate = 0.5;
	float noiseRate = 1.0;
	float scrollSpeed = 1.0;

	unsigned int quadVAO = createVAO(vertices, 4, indices, 6);

	glBindVertexArray(quadVAO);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		background.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, backgroundTexture);
		background.setInt("background", 0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		background.setInt("noiseTexture", 1);

		background.setFloat("noiseRate", noiseRate);
		background.setFloat("time", glfwGetTime());
		background.setFloat("scrollSpeed", scrollSpeed);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);


		// character little guy
		character.use();
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, characterTexture);
		character.setInt("characterTexture", 5); 
		character.setFloat("time", glfwGetTime());
		character.setVec2("imgSize", imageSizeWidth, imageSizeHeight);
		character.setVec2("aspectRatio", SCREEN_WIDTH, SCREEN_HEIGHT);
		character.setFloat("speed", speed);
		character.setFloat("alphaRate", alphaRate);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");

			ImGui::SliderInt("Image Width", &imageSizeWidth, 1, 1024);
			ImGui::SliderInt("Image Height", &imageSizeHeight, 1, 1024);
			ImGui::SliderFloat("Speed", &speed, 0.0f, 10.0f);
			ImGui::SliderFloat("Alpha Rate", &alphaRate, 0.0f, 1.0f);
			ImGui::SliderFloat("Noise Rate", &noiseRate, 0.0f, 10.0f);
			ImGui::SliderFloat("Scroll Speed", &scrollSpeed, 0.0f, 5.0f);

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Vertex Buffer Object 
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertexData, GL_STATIC_DRAW);

	//Element Buffer Object
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(0);

	//UV attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

