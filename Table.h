/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is table.h it contains the declaration of the table class
*/

#pragma once

#include "Cube.h"

class Table : public Geometry
{
public:
	Table(Cube* c);
	virtual void draw(glm::mat4 transform, glm::vec3 color);
private:
	Cube* cube;
	Transformation tableTransforms[5];
};