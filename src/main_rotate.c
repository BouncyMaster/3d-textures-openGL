#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <file_ops.h>
#include "data.h"

unsigned int VAO[2], VBO[2], shader_program;
int projection_loc;
mat4 projection;

void
framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
	glm_perspective_resize((float)w/(float)h, projection);
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);
}

void
set_data(void)
{
	glm_perspective_default(1, projection);

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


	unsigned int texture;
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
	const char *fragment_shader_source = file_to_str("shaders/fragment.glsl");

	unsigned int vertex_shader, fragment_shader;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, 0);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, 0);
	glCompileShader(fragment_shader);

	file_to_str_free(vertex_shader_source);
	file_to_str_free(fragment_shader_source);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program); 

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glUseProgram(shader_program);

	projection_loc = glGetUniformLocation(shader_program, "projection");
}


int
main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(800, 800,
			"rotate", 0, 0);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	set_data();

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	mat4 model, view;
	vec3 positions[] = {
		{ 0.0,  0.0,  0.0},
		{ 2.0,  5.0, -15.0},
		{-1.5, -2.2, -2.5},
		{-3.8, -2.0, -12.3},
		{ 2.4, -0.4, -3.5},
		{-1.7,  3.0, -7.5},
		{ 1.3, -2.0, -2.5},
		{ 1.5,  2.0, -2.5},
		{ 1.5,  0.2, -1.5},
		{-1.3,  1.0, -1.5}
	};

	glm_translate_make(view, (vec3){0, 0, -3});

	int model_loc = glGetUniformLocation(shader_program, "model");
	int view_loc = glGetUniformLocation(shader_program, "view");
	unsigned char i;
	bool curr_show = true, is_held = false;

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);
	glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

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

		glBindVertexArray(VAO[curr_show]);
		for (i = 0; i < sizeof(positions)/sizeof(vec3); ++i) {
			glm_translate_make(model, positions[i]);
			glm_rotate_y(model, cos(glfwGetTime()), model);
			glm_rotate_x(model, glm_rad(20 * i), model);
			glm_scale(model, (vec3){0.5, 0.5, 0.5});

			glUniformMatrix4fv(model_loc, 1, GL_FALSE,
				(float *)model);

			if (curr_show) {
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLE_STRIP, 6, 8);
			} else {
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteProgram(shader_program);

	glfwTerminate();
}
