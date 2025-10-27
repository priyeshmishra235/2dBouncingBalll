#include "includes/ball.hpp"
#include "includes/box.hpp"
#include "includes/window.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

Window window(WIDTH, HEIGHT, "GL bouncing ball");
Shader ballShader("shaders/ball.vert", "shaders/ball.frag");
Shader boxShader("shaders/box.vert", "shaders/box.frag");
Box box0(200.0f);
Box light(25.0f);
bool startSimulation{false};

int main() {
  ballShader.use();
  ballShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
  glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 1000.0f);
  ballShader.setVec3("lightPos", lightPos);

  box0.setRandColor();
  float halfSize = box0.halfSize;

  int totalBalls = 50;
  std::vector<std::unique_ptr<Ball>> balls;
  balls.reserve(100);
  for (int i = 0; i < totalBalls; ++i) {
    auto b = std::make_unique<Ball>(25.0f);
    b->setRandParameters(halfSize);
    // b->color = glm::vec3(0.5f, 0.5f, 0.5f);
    balls.push_back(std::move(b));
  }

  double lastTime = glfwGetTime();

  while (!window.shouldClose()) {
    double currentTime = glfwGetTime();
    float dt = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    window.processInput(dt);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ballShader.setViewProjection(
        camera.GetViewMatrix(),
        camera.GetProjectionMatrix(static_cast<float>(WIDTH) / HEIGHT));
    boxShader.setViewProjection(
        camera.GetViewMatrix(),
        camera.GetProjectionMatrix(static_cast<float>(WIDTH) / HEIGHT));

    box0.draw(boxShader);
    light.center = glm::vec3(lightPos);
    light.draw(boxShader);

    for (auto &b : balls) {
      b->draw(ballShader);
      b->updatePhysics(dt, halfSize, halfSize, halfSize);
    }

    if (glfwGetKey(window.getWindow(), GLFW_KEY_Y)) {
      startSimulation = true;
    }
    if (startSimulation) {
      for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
          if (balls[i]->ballCollisions(*balls[j])) {
            // balls[i]->color = glm::vec3(0.0f, 0.0f, 1.0f);
          }
        }
      }
    }

    window.swapBuffersAndPollEvents();
  }
  glfwTerminate();
  return 0;
}
