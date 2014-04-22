/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is vertex,h it contains the declaration of the vertex struct
*/

#pragma once

#include "glew.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

struct Vertex
{
	Vertex() {}
	Vertex(glm::vec4 pos, glm::vec3 c, glm::vec4 norm) { position = pos; color = c; normal = norm; }
	glm::vec4 position;
	glm::vec3 color;
	glm::vec4 normal;
};