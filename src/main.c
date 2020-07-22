#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "data.h"
#include "file_ops.h"

unsigned int VAO[2], VBO[2], shader_program, texture;
mat4 projection;

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
	glm_perspective_resize((float)w/(float)h, projection);
}

void set_data(void)
{
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	// texture coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
			(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle,
			GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	// texture coord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
			(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nr_channels;

	unsigned char *data = stbi_load("data/wall.jpg", &width, &height,
			&nr_channels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	const char *vertex_shader_source = file_to_str("shaders/vertex.glsl");
	const char *fragment_shader_source =
			file_to_str("shaders/fragment.glsl");

	unsigned int vertex_shader, fragment_shader;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program); 

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glUseProgram(shader_program);
}


int main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(800, 800,
			"3d-textures", 0, 0);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	set_data();

	char curr_show = 1, is_held = 0;

	mat4 model, view;

	glm_rotate_make(model, glm_rad(20), (vec3){1, 0, 0});
	glm_translate_make(view, (vec3){0, 0, -3});
	glm_perspective_default(1, projection);

	int modelLoc = glGetUniformLocation(shader_program, "model");
	int viewLoc = glGetUniformLocation(shader_program, "view");
	int projectionLoc = glGetUniformLocation(shader_program, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
			(float *)projection);

	glEnable(GL_DEPTH_TEST);
	glClearColor(.2, .3, .3, 1);

	while(!glfwWindowShouldClose(window)) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		if((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) &&
				!is_held) {
			is_held = 1;
			curr_show = !curr_show;
		} else if((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
				&& is_held)
			is_held = 0;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm_rotate_y(model, glm_rad((float)cos(glfwGetTime())), model);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
				(float *)projection);

		if (curr_show) {
			glBindVertexArray(VAO[0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		} else {
			glBindVertexArray(VAO[1]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDrawArrays(GL_TRIANGLE_STRIP, 6, 8);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteProgram(shader_program);

	glfwTerminate();
}
