#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLuint loadTexture(const char* path);
void setupVertexPointers();

static int SCR_WIDTH = 800;
static int SCR_HEIGHT = 600;

glm::vec3 lightPos(1.2f, 0.5f, 2.0f);

int main() {

	//initialize, set window hints
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create the window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tutorial7", NULL, NULL);

	//if window not valid, terminate
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//add window to current context
	glfwMakeContextCurrent(window);
	//set callback for resize
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//check if GLAD loader loaded, if not, terminate
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//enable opengl depth test, blend, alpha params
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//build and compile shaders
	Shader shader("shaders/shader.vs", "shaders/shader.fs");
	Shader lightShader("shaders/lightingShader.vs", "shaders/lightingShader.fs");
	Shader blendShader("shaders/blendShader.vs", "shaders/blendShader.fs");

	//cube vertices
	float vertices[] = {
		//x    y    z    normal: X     Y     Z  tex:  U     V
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
	};

	float planeVertices[] = {
        // positions                            // texture Coords 
         5.0f, -0.5f,  5.0f, 1.0f, 1.0f, 1.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 1.0f, 1.0f, 1.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f, 1.0f, 1.0f, 1.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 1.0f, 1.0f, 1.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f, 1.0f, 1.0f, 1.0f,  2.0f, 2.0f
    };

	float transparentVertices[] = {
        // positions         				// texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f,  0.0f
    };

	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f-2.0f,  0.0f),
		glm::vec3(0.0f, 1.0f - 2.0f, 0.0f),
		glm::vec3(0.0f, 2.0f - 2.0f, 0.0f),
		glm::vec3(0.0f, 3.0f - 2.0f, 0.0f),
		glm::vec3(0.0f, 4.0f - 2.0f, 0.0f),
	};

	//tree leaves
	glm::vec3 leavesPositions[] = {
		glm::vec3(0.0f, 4.0f - 2.0f, 1.0f),
		glm::vec3(0.0f, 4.0f - 2.0f, -1.0f),
		glm::vec3(1.0f, 4.0f - 2.0f, 0.0f),
		glm::vec3(-1.0f, 4.0f - 2.0f, 0.0f),
		glm::vec3(0.0f, 3.0f - 2.0f, 1.0f),
		glm::vec3(0.0f, 3.0f - 2.0f, -1.0f),
		glm::vec3(1.0f, 3.0f - 2.0f, 0.0f),
		glm::vec3(-1.0f, 3.0f - 2.0f, 0.0f),
		glm::vec3(1.0f, 3.0f - 2.0f, 1.0f),
		glm::vec3(1.0f, 3.0f - 2.0f, -1.0f),
		glm::vec3(-1.0f, 3.0f - 2.0f, 1.0f),
		glm::vec3(-1.0f, 3.0f - 2.0f, -1.0f),
	};
 
	//load vertex data into VBO buffer, create VAO + EBO 
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//setup vertex pointers
	setupVertexPointers();

	//create VAO for light
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//setup vertex pointers
	setupVertexPointers();

	unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

	//setup vertex pointers
	setupVertexPointers();

	// transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    
	//setup vertex pointers
	setupVertexPointers();

	//load textures using function
	//load textures using function
	GLuint woodTexture, leavesTexture, windowTexture;
	//h-flip on load
	stbi_set_flip_vertically_on_load(true);
	woodTexture = loadTexture("img/wood.png");
	leavesTexture = loadTexture("img/leaves.png");
	windowTexture = loadTexture("img/window.png");

	shader.use();
	//shader.setInt("woodTexture", 0);
	//shader.setInt("leavesTexture", 1);

	//pass projection matrix to shader
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", projection);

	//clear
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window)) {
		//MAIN LOOP
		//get input
		processInput(window);

		//rendering commands here =============

		//clear colour set & clear
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//load textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);

		//use shader for model
		shader.use();
		//set model colour and light colour
		shader.setVec3("objectColor", 0.1, 0.5f, 0.31f);
		shader.setVec3("lightColor", 1.0f, 1.0f, 1.0);
		shader.setVec3("lightPos", lightPos);

		//render with camera
		glm::mat4 view = glm::mat4(1.0f);
		float radius = 10.0f;
		float camX = static_cast<float>(sin(glfwGetTime()) * radius);
		float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("view", view);

		//draw the box
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//draw each wood piece
		for (unsigned int i = 0; i < 5; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//change texture to leaves
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, leavesTexture);

		//draw each leaf block
		for (unsigned int i = 0; i < 12; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, leavesPositions[i]);
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//set up glowing cube
		lightShader.use();
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightShader.setMat4("model", model);

		//draw glowing cube
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//draw transparency test plane
		blendShader.use();
		blendShader.setMat4("projection", projection);
		blendShader.setMat4("view", view);
		// Render the window
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f));
        blendShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

		//====================================

		//check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//delete all resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//load texture from path function
GLuint loadTexture(const char* path) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		else
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void setupVertexPointers() {
	//setup vertex pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}
