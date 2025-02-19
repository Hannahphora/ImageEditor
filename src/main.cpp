#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../ext/stb/stb_image.h"
#include "../ext/stb/stb_image_write.h"
#define GLAD_GL_IMPLEMENTATION
#include "../ext/glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "../ext/GLFW/glfw3.h"

#include "Shader.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cstdlib>
#include <iostream>

void ProcessInput(GLFWwindow* window);
void error_callback(int error, const char* desc);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char** argv) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// init glfw
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		std::cerr << "Error: Failed to init GLFW\n";
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// init window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window", NULL, NULL);
	if (!window) {
		std::cerr << "Error: Failed to create window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// init glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Error: Failed to init glad\n";
		return -1;
	}

    Shader shader("res\\shaders\\shader.vs", "res\\shaders\\shader.fs");

    // vert data for fullscreen quad
    float vertices[] = {
        // positions           // texture coords
         -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // top-left
         -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom-left
          1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom-right

         -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // top-left
          1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom-right
          1.0f,  1.0f, 0.0f,   1.0f, 1.0f  // top-right
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind VAO and load vert data into VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // vert positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load img and create texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    int imgWidth, imgHeight, nrChannels;
    const char* imagePath = "res\\imgs\\miku.jpg";
    unsigned char* data = stbi_load(imagePath, &imgWidth, &imgHeight, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        else {
            std::cerr << "Unsupported number of channels: " << nrChannels << "\n";
            format = GL_RGB;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else std::cerr << "Error: Failed to load texture\n";
    stbi_image_free(data);

    // -~< MAIN LOOP >~-

	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    // cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void error_callback(int error, const char* desc) {
	std::cerr << "Error: " << desc << '\n';
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}