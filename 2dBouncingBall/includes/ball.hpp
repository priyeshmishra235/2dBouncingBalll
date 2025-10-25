#include "../extLibs/glad/glad.h"
#include "shader.hpp"
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <vector>
class Ball {
private:
  unsigned int vao;
  unsigned int vbo;

public:
  // physics params
  glm::vec2 velocity;
  float mass;

  // ball params
  glm::vec2 center;
  glm::vec3 color;
  float radius;
  const int numSegments;
  std::vector<float> vertices;

  void updatePhysics(float dt, float halfWidth, float halfHeight) {
    center += velocity * dt;
    CollisionCheck(halfWidth, halfHeight);
  }
  // wall collision check
  void CollisionCheck(const float halfWidth, const float halfHeight) {
    float right_boundary = halfWidth;
    float left_boundary = -halfWidth;
    float top_boundary = halfHeight;
    float bottom_boundary = -halfHeight;

    if (center.x + radius >= right_boundary) {
      center.x = right_boundary - radius;
      velocity.x = -fabs(velocity.x);
    } else if (center.x - radius <= left_boundary) {
      center.x = left_boundary + radius;
      velocity.x = fabs(velocity.x);
    }
    if (center.y + radius >= top_boundary) {
      center.y = top_boundary - radius;
      velocity.y = -fabs(velocity.y);
    } else if (center.y - radius <= bottom_boundary) {
      center.y = bottom_boundary + radius;
      velocity.y = fabs(velocity.y);
    }
  }
  // draw a ball
  void draw(Shader &shader) {
    // shader.use();// to improve efficiency and call it each time for each ball
    // called once in main
    shader.setVec3("color", color);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(center.x, center.y, 0.0f));
    shader.setMat4("model", glm::value_ptr(model));
    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, this->numSegments + 2);
    glBindVertexArray(0);
  }
  // generate a ball
  Ball(float mass = 5.0f, float radius = 25.0f, int numSegments = 32,
       glm::vec2 center = {0.0f, 0.0f})
      : radius(radius), numSegments(numSegments), center(center), mass(mass) {
    vertices.clear();
    vertices.reserve((numSegments + 2) * 2);

    vertices.push_back(0.0f); // x
    vertices.push_back(0.0f); // y

    for (int i = 0; i <= numSegments; ++i) {
      float angle = 2.0f * glm::pi<float>() * i / numSegments;
      float x = radius * cos(angle);
      float y = radius * sin(angle);

      vertices.push_back(x);
      vertices.push_back(y);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
  }
  float randFloat(float a, float b) {
    return a + (b - a) * (static_cast<float>(rand()) / RAND_MAX);
  }
  void setRandVelocity() {
    velocity.x = (rand() % 2 == 0 ? 1 : -1) * randFloat(75.0f, 250.0f);
    velocity.y = (rand() % 2 == 0 ? 1 : -1) * randFloat(150.0f, 250.0f);
  }
  void setRandCenter() {
    center.x = (rand() % 2 == 0 ? 1 : -1) * randFloat(0.0f, 360.0f);
    center.y = (rand() % 2 == 0 ? 1 : -1) * randFloat(0.0f, 260.0f);
  }
  void setRandColor() {
    color.x = randFloat(0.0f, 1.0f);
    color.y = randFloat(0.0f, 1.0f);
    color.z = randFloat(0.0f, 1.0f);
  }
  void setRandMass() { mass = randFloat(5.0f, 100.0f); }
  void setRandParameters() {
    setRandCenter();
    setRandVelocity();
    setRandColor();
    setRandMass();
  }
  ~Ball() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }
};
