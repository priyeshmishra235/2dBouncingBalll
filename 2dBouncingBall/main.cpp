#include "includes/ball.hpp"
#include "includes/window.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

void ballCollision(std::vector<std::unique_ptr<Ball>> &balls);

int main() {
  const float halfWidth = static_cast<float>(WIDTH) / 2;
  const float halfHeight = static_cast<float>(HEIGHT) / 2;

  srand(static_cast<unsigned int>(time(0)));

  Window window(WIDTH, HEIGHT, "GL bouncing ball");

  Shader ballShader("shaders/ball.vert", "shaders/ball.frag");

  glm::mat4 projection =
      glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);

  ballShader.use();
  ballShader.setMat4("projection", glm::value_ptr(projection));

  // [NOTE] -> a very interesting issue indeed
  // see notion toggleList Misc.->[bounceBall proj.]to see why i am using this
  std::vector<std::unique_ptr<Ball>> balls;
  balls.reserve(100);
  for (int i = 0; i < 20; ++i) {
    auto b = std::make_unique<Ball>();
    b->setRandParameters();
    balls.push_back(std::move(b));
  }

  while (!window.shouldClose()) {
    window.processInput();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float dt = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    ballCollision(balls);

    for (auto &b : balls) {
      b->updatePhysics(dt, halfWidth, halfHeight);
      b->draw(ballShader);
    }

    window.swapBuffersAndPollEvents();
  }

  glfwTerminate();
  return 0;
}

void ballCollision(std::vector<std::unique_ptr<Ball>> &balls) {
  for (size_t i = 0; i < balls.size(); ++i) {
    for (size_t j = i + 1; j < balls.size(); ++j) {
      glm::vec2 delta = balls[j]->center - balls[i]->center;
      float dist = glm::length(delta);
      if (dist < balls[i]->radius + balls[j]->radius) {
        glm::vec2 normal = delta / dist;

        float p = 2.0f *
                  glm::dot(balls[i]->velocity - balls[j]->velocity, normal) /
                  (balls[i]->mass + balls[j]->mass);

        balls[i]->velocity -= p * balls[j]->mass * normal;
        balls[j]->velocity += p * balls[i]->mass * normal;

        float overlap = balls[i]->radius + balls[j]->radius - dist;
        balls[i]->center -= normal * (overlap / 2.0f);
        balls[j]->center += normal * (overlap / 2.0f);
      }
    }
  }
}
