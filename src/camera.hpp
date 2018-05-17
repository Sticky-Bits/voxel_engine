#ifndef CAMERA_H
#define CAMERA_H

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 25.0f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	Camera(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH,
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
		float MovementSpeed = SPEED,
		float MouseSensitivity = SENSITIVTY,
		float Zoom = ZOOM
	);
	Camera(
		float posX,
		float posY,
		float posZ,
		float upX,
		float upY,
		float upZ,
		float yaw,
		float pitch,
		glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f),
		float MovementSpeed = SPEED,
		float MouseSensitivity = SENSITIVTY,
		float Zoom = ZOOM
	);
	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	float Yaw;
	float Pitch;
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

private:
	void updateCameraVectors();
};

#endif