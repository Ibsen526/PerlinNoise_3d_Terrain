#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 camPos;
	glm::vec3 camFront;

	Camera();
	void MoveCamera(float xPos, float yPos);
	void RotateCamera(SDL_Window* window, const int SCREEN_W, const int SCREEN_H);
	void ChangeSpeedKey(const Uint8* currentKeyStates, const float base, float& speed);

private:
	const float baseCamSpeed = 2.0f;
	const float sensitivity = 60.0f;
	float yaw = 45.0f; // For the mouse rotation (yaw x-rotation, pitch y-rotation)
	float pitch = 0.0f;
};
