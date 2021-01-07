#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

// default camera values
#define YAW -90.
#define PITCH 0.
#define SPEED 2.5
#define SENSITIVITY .1
#define ZOOM 45

enum camera_movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct camera {
	// camera attributes
	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;
	vec3 world_up;
	// euler angles
	float yaw;
	float pitch;
	bool constrain_pitch;
	// camera options
	float movement_speed;
	float mouse_sensitivity;
	short zoom;
};

void camera_make_default(vec3 position, vec3 up, struct camera *dest);
void camera_getviewmatrix(struct camera cam, mat4 out);
void camera_process_keyboard(enum camera_movement direction, float delta_time,
	struct camera *cam);
void camera_process_mouse(vec2 offset, struct camera *cam);
void camera_process_mousescroll(short offset, struct camera *cam);

#endif
