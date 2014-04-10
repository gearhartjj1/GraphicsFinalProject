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

private:
	Cube* cube;
	Transformation chairTransforms[6];
};