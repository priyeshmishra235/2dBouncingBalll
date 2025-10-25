#pragma once
#include "../extLibs/glad/glad.h"
#include "camera.hpp"
#include <GLFW/glfw3.h>
#include <ctime>
#include <stdexcept>

float lastX = 400.0f; // initial window center x
float lastY = 300.0f; // initial window center y
bool firstMouse = true;

class Window;
Camera camera(glm::vec3(0.0f, 0.0f, 50.0f));

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

class Window {
public:
  Window(int width, int height, const char *title) : window(nullptr) {
    if (!glfwInit()) {
      throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
      glfwTerminate();
      throw std::runtime_error("Failed to create GLFW window");
    }
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    srand(static_cast<unsigned int>(time(0)));
  }
  void handleResize(int width, int height) { glViewport(0, 0, width, height); }
  void processInput(float dt) {
    // exit control
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
    // camera controls
    if (glfwGetKey(getWindow(), GLFW_KEY_W) == GLFW_PRESS)
      camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(getWindow(), GLFW_KEY_S) == GLFW_PRESS)
      camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(getWindow(), GLFW_KEY_A) == GLFW_PRESS)
      camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(getWindow(), GLFW_KEY_D) == GLFW_PRESS)
      camera.ProcessKeyboard(RIGHT, dt);
    if (glfwGetKey(getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
      camera.ProcessKeyboard(UP, dt);
    if (glfwGetKey(getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
      camera.ProcessKeyboard(DOWN, dt);
  }

  ~Window() {
    if (window) {
      glfwDestroyWindow(window);
    }
    glfwTerminate();
  }

  GLFWwindow *getWindow() const { return window; }

  bool shouldClose() const { return glfwWindowShouldClose(window); }

  void swapBuffersAndPollEvents() {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

private:
  GLFWwindow *window;

  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;
};

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  Window *appWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
  if (appWindow) {
    appWindow->handleResize(width, height);
  }
}
void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
