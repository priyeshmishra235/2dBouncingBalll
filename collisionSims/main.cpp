#include "includes/ball.hpp"
#include "includes/box.hpp"
#include "includes/plane.hpp"
#include "includes/window.hpp"

#include <GLFW/glfw3.h>
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

Window window(WIDTH, HEIGHT, "GL bouncing ball");
Shader boxShader("shaders/box.vert", "shaders/box.frag");

int main() {
  double lastTime = glfwGetTime();

  camera.Position = glm::vec3(0.0f, 0.0f, 0.0f);

  std::vector<std::unique_ptr<Box>> boxes;
  boxes.clear();
  boxes.reserve(100);
  for (int i{0}; i < 100; ++i) {
    auto b = std::make_unique<Box>();
    b->setRandColor();
    boxes.push_back(std::move(b));
  }
  while (!window.shouldClose()) {
    double currentTime = glfwGetTime();
    float dt = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    window.processInput(dt);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    boxShader.setViewProjection(
        camera.GetViewMatrix(),
        camera.GetProjectionMatrix(static_cast<float>(WIDTH) / HEIGHT));

    float r = 100.0f;
    int n = boxes.size();
    float angleStep = glm::two_pi<float>() / n;

    for (int i = 0; i < n; ++i) {
      float theta = i * angleStep;
      float x = r * cos(theta);
      float z = r * sin(theta);
      boxes[i]->center = glm::vec3(x, i * 5, z);
      boxes[i]->draw(boxShader);
    }

    window.swapBuffersAndPollEvents();
  }
  glfwTerminate();
  return 0;
}
