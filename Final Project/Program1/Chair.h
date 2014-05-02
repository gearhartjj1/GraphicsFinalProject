/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is chair.h it contains the declaration for the chair class
*/

#pragma once

#include "Cube.h"

class Chair : public Geometry
{
public:
	Chair(Cube* c);
	virtual void draw(glm::mat4 transform, glm::vec3 color);
	virtual double rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal);
private:
	Cube* cube;
	Transformation chairTransforms[6];
	glm::mat4 inverseMatrices[6];
};