#include "../extLibs/glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

class Window;
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

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

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);
  }
  void handleResize(int width, int height) { glViewport(0, 0, width, height); }
  void processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
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
