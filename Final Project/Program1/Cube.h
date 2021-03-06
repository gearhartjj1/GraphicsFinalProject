/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is cube.h it contains the declaration of the cube class
*/

#pragma once

#include "geometry.h"

class Cube : public Geometry
{
public:
	virtual void draw(glm::mat4 transform, glm::vec3 color);
	virtual double rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal);
	void bufferData();
	Cube();
private:
	glm::vec4 cube[24];
	glm::vec4 cubeNormals[24];
};