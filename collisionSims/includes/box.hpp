#pragma once
#include "../extLibs/glad/glad.h"
#include "shader.hpp"
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Box {
private:
  unsigned int vao{}, vbo{}, ebo{};
  size_t indexCount{0};

public:
  // Physics params
  glm::vec3 rotationAngle{0.0f};
  glm::vec3 scaleFactor{1.0f};

  // Rendering params
  glm::vec3 center{0.0f};
  glm::vec3 color{0.8f, 0.8f, 0.8f};
  float halfSize{10.0f};

  Box(float halfSize = 10.0f, float mass = 5.0f,
      glm::vec3 center = {0.0f, 0.0f, 0.0f})
      : halfSize(halfSize), center(center) {
    generateBox();
  }

  void draw(Shader &shader) {
    shader.setVec3("color", color);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, center);
    model =
        glm::rotate(model, glm::radians(rotationAngle.x), glm::vec3(1, 0, 0));
    model =
        glm::rotate(model, glm::radians(rotationAngle.y), glm::vec3(0, 1, 0));
    model =
        glm::rotate(model, glm::radians(rotationAngle.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scaleFactor);

    shader.setMat4("model", glm::value_ptr(model));

    glBindVertexArray(vao);
    glLineWidth(5.0f);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount),
                   GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  void generateBox() {
    float s = halfSize;
    std::vector<float> vertices = {-s, -s, s,  s, -s, s,  s, s, s,  -s, s, s,
                                   -s, -s, -s, s, -s, -s, s, s, -s, -s, s, -s};

    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0, 5, 4, 7, 7, 6, 5,
                                         1, 5, 6, 6, 2, 1, 4, 0, 3, 3, 7, 4,
                                         3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4};

    // // for wireframe
    // std::vector<unsigned int> indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
    //                                      6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
  }
  float randFloat(float a, float b) {
    return a + (b - a) * (static_cast<float>(rand()) / RAND_MAX);
  }
  void setRandColor() {
    color.x = randFloat(0.0f, 1.0f);
    color.y = randFloat(0.0f, 1.0f);
    color.z = randFloat(0.0f, 1.0f);
  }

  ~Box() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
  }
};
