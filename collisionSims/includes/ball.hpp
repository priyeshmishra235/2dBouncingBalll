#pragma once
#include "../extLibs/glad/glad.h"
#include "shader.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Ball {
private:
  unsigned int vao{}, vbo{}, ebo{};

public:
  // Physics params
  glm::vec3 velocity{50.0f};
  float mass{5.0f};
  glm::vec3 rotationAngle{0.0f};
  glm::vec3 scaleFactor{1.0f};
  glm::vec3 gravity{0.0f, -9.8f, 0.0f};
  float dampingCoeff{0.99f};

  // Rendering params
  glm::vec3 center{0.0f};
  glm::vec3 color{1.0f};
  float radius{5.0f};
  int sectorCount{36};
  int stackCount{18};
  size_t indexCount{0};

  Ball(float radius = 5.0f, float mass = 5.0f, int sectorCount = 36,
       int stackCount = 18, glm::vec3 center = {0.0f, 0.0f, 0.0f})
      : radius(radius), center(center), sectorCount(sectorCount),
        stackCount(stackCount), mass(mass) {
    generateSphere();
  }

  // Update physics
  void updatePhysics(float dt, float halfWidth, float halfHeight,
                     float halfDepth) {
    velocity += gravity * dt;
    center += velocity * dt;
    CollisionCheck(halfWidth, halfHeight, halfDepth);
  }

  // Wall collision
  void CollisionCheck(const float halfWidth, const float halfHeight,
                      const float halfDepth) {
    // X-axis bounce
    if (center.x + radius > halfWidth) {
      center.x = halfWidth - radius;
      velocity.x *= -dampingCoeff;
    } else if (center.x - radius < -halfWidth) {
      center.x = -halfWidth + radius;
      velocity.x *= -dampingCoeff;
    }

    // Y-axis bounce
    if (center.y + radius > halfHeight) {
      center.y = halfHeight - radius;
      velocity.y *= -dampingCoeff;
    } else if (center.y - radius < -halfHeight) {
      center.y = -halfHeight + radius;
      velocity.y *= -dampingCoeff;
    }

    // Z-axis bounce
    if (center.z + radius > halfDepth) {
      center.z = halfDepth - radius;
      velocity.z *= -dampingCoeff;
    } else if (center.z - radius < -halfDepth) {
      center.z = -halfDepth + radius;
      velocity.z *= -dampingCoeff;
    }
  }

  bool ballCollisions(Ball &other) {
    glm::vec3 delta = other.center - center;
    float dist = glm::length(delta);
    float sumR = radius + other.radius;

    if (dist < sumR && dist > 1e-4f) {
      glm::vec3 normal = glm::normalize(delta);
      float overlap = sumR - dist;

      center -= normal * (overlap * 0.5f);
      other.center += normal * (overlap * 0.5f);

      glm::vec3 relVel = velocity - other.velocity;
      float velAlongNormal = glm::dot(relVel, normal);
      if (velAlongNormal > 0.0f)
        return 0;

      float e = 0.99f;
      float j = -(1 + e) * velAlongNormal;
      j /= (1.0f / mass + 1.0f / other.mass);

      glm::vec3 impulse = j * normal;
      velocity += impulse / mass;
      other.velocity -= impulse / other.mass;
      return true;
    }
    return false;
  }
  // Draw sphere
  void draw(Shader &shader) {
    shader.use();

    // light params
    glm::vec3 ambient = color * 0.1f;
    glm::vec3 diffuse = color;
    glm::vec3 specular = glm::vec3(0.8f);
    float shininess = 64.0f;

    shader.setVec3("materialAmbient", ambient);
    shader.setVec3("materialDiffuse", diffuse);
    shader.setVec3("materialSpecular", specular);
    shader.setFloat("materialShininess", shininess);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, center);
    model = glm::rotate(model, glm::radians(rotationAngle.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationAngle.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationAngle.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scaleFactor);

    shader.setMat4("model", glm::value_ptr(model));

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount),
                   GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  // sphere mesh
  void generateSphere() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    const float PI = glm::pi<float>();

    for (int i = 0; i <= stackCount; ++i) {
      float stackAngle = PI / 2 - i * (PI / stackCount);
      float xy = radius * cosf(stackAngle);
      float z = radius * sinf(stackAngle);

      for (int j = 0; j <= sectorCount; ++j) {
        float sectorAngle = j * (2 * PI / sectorCount);

        float x = xy * cosf(sectorAngle);
        float y = xy * sinf(sectorAngle);

        // vertex position
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);

        // normal
        float nx = x / radius;
        float ny = y / radius;
        float nz = z / radius;
        vertices.push_back(nx);
        vertices.push_back(ny);
        vertices.push_back(nz);

        // texture coordinates
        float u = (float)j / sectorCount;
        float v = (float)i / stackCount;
        vertices.push_back(u);
        vertices.push_back(v);
      }
    }

    // indices
    for (int i = 0; i < stackCount; ++i) {
      int k1 = i * (sectorCount + 1);
      int k2 = k1 + sectorCount + 1;

      for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
        if (i != 0) {
          indices.push_back(k1);
          indices.push_back(k2);
          indices.push_back(k1 + 1);
        }
        if (i != (stackCount - 1)) {
          indices.push_back(k1 + 1);
          indices.push_back(k2);
          indices.push_back(k2 + 1);
        }
      }
    }

    indexCount = indices.size();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    GLsizei stride = 8 * sizeof(float);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
  }

  float randFloat(float a, float b) {
    return a + (b - a) * (static_cast<float>(rand()) / RAND_MAX);
  }

  void setRandVelocity() {
    velocity.x = (rand() % 2 == 0 ? 1 : -1) * randFloat(45.0f, 70.0f);
    velocity.y = (rand() % 2 == 0 ? 1 : -1) * randFloat(45.0f, 70.0f);
    velocity.z = (rand() % 2 == 0 ? 1 : -1) * randFloat(45.0f, 70.0f);
  }

  void setRandCenter(float halfSize) {
    center.x = randFloat(-halfSize, halfSize);
    center.y = randFloat(-halfSize, halfSize);
    center.z = randFloat(-halfSize, halfSize);
  }

  void setRandColor() {
    color.x = randFloat(0.0f, 1.0f);
    color.y = randFloat(0.0f, 1.0f);
    color.z = randFloat(0.0f, 1.0f);
  }

  void setRandMass() { mass = randFloat(5.0f, 100.0f); }
  void setRandRadius() { radius = randFloat(5.0f, 25.0f); }

  void setRandParameters(float halfSize) {
    setRandCenter(halfSize);
    setRandVelocity();
    // setRandColor();
    setRandRadius();
    setRandMass();
  }

  ~Ball() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
  }
};
