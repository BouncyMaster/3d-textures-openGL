#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <file_ops.h>
#include "data.h"
#include "camera.h"

unsigned int VAO, VBO, shader_program;

short scr[2] = {800, 800};

float delta_time, last_frame = 0;

struct camera main_camera;

float lastX = 400, lastY = 400; // scr_width / 2
bool first_mouse = true, perspective_changed = true;

void
framebuffer_size_callback(GLFWwindow *window, int w, int h)
{
	glViewport(0, 0, w, h);

	perspective_changed = true;

	scr[0] = w;
	scr[1] = h;
}

void
keyboard_callback(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera_process_keyboard(FORWARD, delta_time,
				&main_camera);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera_process_keyboard(BACKWARD, delta_time,
				&main_camera);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera_process_keyboard(LEFT, delta_time,
				&main_camera);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera_process_keyboard(RIGHT, delta_time,
				&main_camera);
}

void
mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (first_mouse) {
		lastX = xpos;
		lastY = ypos;
		first_mouse = false;
	}

	vec2 offset = {xpos - lastX, lastY - ypos};

	lastX = xpos;
	lastY = ypos;

	camera_process_mouse(offset, &main_camera);
}

void
scroll_callback(GLFWwindow* window, double offsetX, double offsetY)
{
	perspective_changed = true;
	camera_process_mousescroll((short)offsetY, &main_camera);
}

void
set_data(void)
{
	camera_init((vec3){0, 0, 3}, (vec3){0, 1, 0}, &main_camera);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

	const char *vertex_shader_source = file_to_str("shaders/vertex.glsl"),
		*fragment_shader_source = file_to_str("shaders/fragment.glsl");

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
}


int
main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(scr[0], scr[1],
			"camera", 0, 0);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	set_data();

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	mat4 projection, model, view, pv, pvm;

	int pvm_loc = glGetUniformLocation(shader_program, "pvm");

	glEnable(GL_DEPTH_TEST);
	glClearColor(.2, .3, .3, 1);

	float current_frame;
	while (!glfwWindowShouldClose(window)) {
		keyboard_callback(window);

		current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (perspective_changed) {
			perspective_changed = false;

			glm_perspective(glm_rad(main_camera.zoom),
				(float)scr[0]/(float)scr[1], .1, 100, projection);
		}

		camera_getviewmatrix(view, &main_camera);
		glm_mat4_mul(projection, view, pv);

		for (short i = 0; i < sizeof(positions)/sizeof(vec3); ++i) {
			glm_translate_make(model, positions[i]);
			glm_rotate_y(model, sin(current_frame), model);
			glm_rotate_x(model, glm_rad(20 * i), model);

			glm_mat4_mul(pv, model, pvm);
			glUniformMatrix4fv(pvm_loc, 1, GL_FALSE,
				(float *)pvm);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDrawArrays(GL_TRIANGLE_STRIP, 6, 8);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader_program);

	glfwTerminate();
}
