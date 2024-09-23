#pragma once
#include <GLFW/glfw3.h>
#include <bx/math.h>
#include "Camera.h"

class InputManager
{
public:
	static void initialize(GLFWwindow* window);
	static void destroy();

	static void update(Camera& camera, float deltaTime);

	static bool isKeyPressed(int key);
	static void getMouseMovement(double* x, double* y);

private:
	InputManager() = default;
	~InputManager() = default;

	static GLFWwindow* m_window;

	static double m_mouseX;
	static double m_mouseY;
	static bool m_FirstMouse;
};
