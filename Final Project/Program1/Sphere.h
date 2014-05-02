/*
Author: Zach J. Wheeler
Course: COMP 361, Computer Graphics
Date: 5/2/14
Description: sphere geometry
*/

#pragma once

#include "geometry.h"
#include "Mesh.h"

class Sphere : public Geometry
{
public:
	virtual void draw(glm::mat4 transform, glm::vec3 color);
	virtual double rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal);
	void bufferData();
	Sphere();
private:
	Mesh mesh;
};