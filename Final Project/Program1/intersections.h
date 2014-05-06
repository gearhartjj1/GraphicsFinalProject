#pragma once

#include "glm\glm.hpp"

double rayCubeIntersection(const glm::vec3& P0, const glm::vec3& V0, const glm::mat4& inverse, glm::vec4 &objspacenormal);

double rayTriangleIntersection(const glm::vec3& P0, const glm::vec3& V0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::mat4& inverse);

double calculateArea(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

double raySphereIntersection(const glm::vec3& P0, const glm::vec3& V0, const glm::mat4& T);