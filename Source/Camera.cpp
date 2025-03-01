#include "Camera.hpp"

Camera::Camera()
{
	view = glm::mat4(1.0f);
	camPos = glm::vec3(2.0f, 7.0f, 2.0f);
	camFront = glm::vec3(1.0f, -1.0f, 1.0f);
	view = glm::lookAt(camPos, camPos + camFront, glm::vec3(0.0f, 1.0f, 0.0f));

	proj = glm::perspective((float)glm::radians(110.0f), (16.0f / 9.0f), 0.01f, 1000.0f);
}

// Movement
void Camera::MoveCamera(float xPos, float yPos)
{
	camPos = { xPos, 7.0f, yPos };
	view = glm::lookAt(camPos, camPos + camFront, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::RotateCamera(SDL_Window* window, const int SCREEN_W, const int SCREEN_H)
{
	int mX, mY;
	SDL_GetMouseState(&mX, &mY);

	glm::vec2 vecMouse = glm::vec2(((float)mX - (float)SCREEN_W / 2.0f) / (float)SCREEN_W * 2.0f,
		(((float)SCREEN_H / 2.0f - (float)mY) / (float)SCREEN_H * 2.0f));

	yaw += vecMouse.x * sensitivity;
	pitch += vecMouse.y * sensitivity;

	if (pitch > 89.0f) // So you cannot flip the camera
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 camDirection;
	camDirection.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
	camDirection.y = sinf(glm::radians(pitch));
	camDirection.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

	camFront = glm::normalize(camDirection);


	view = glm::lookAt(camPos, camPos + camFront, glm::vec3(0.0f, 1.0f, 0.0f));

	SDL_WarpMouseInWindow(window, SCREEN_W / 2, SCREEN_H / 2); //This invokes a mousemotion event; so I flush it
	SDL_FlushEvent(SDL_MOUSEMOTION);
}

void Camera::ChangeSpeedKey(const Uint8* currentKeyStates, const float base, float& speed)
{
	Uint8 count = 0;
	if (currentKeyStates[SDL_SCANCODE_W])
	{
		count++;
	}
	else if (currentKeyStates[SDL_SCANCODE_S])
	{
		count++;
	}
	if (currentKeyStates[SDL_SCANCODE_A])
	{
		count++;
	}
	else if (currentKeyStates[SDL_SCANCODE_D])
	{
		count++;
	}

	if (count >= 2)
		speed = base / 1.4f;
	else
		speed = base;
}