/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is geometry.cpp it contains the implementation of various geometry classes like the basic 
box and furniture based on the box
*/

#include "geometry.h"

Transformation::Transformation()
{
	translate = glm::mat4(1.0);
	rotate = glm::mat4(1.0);
	scale = glm::mat4(1.0);
}

unsigned int Geometry::cbo;
unsigned int Geometry::vbo;
unsigned int Geometry::nbo;
unsigned int Geometry::ibo;
unsigned int Geometry::u_modelMatrix;
unsigned int Geometry::vLocation;
unsigned int Geometry::vColor;
unsigned int Geometry::vNormal;

void Geometry::bufferColor(glm::vec3 color)
{
	glm::vec3 colors[24];
	for(int i = 0; i < 24; i++)
	{
		colors[i] = color;
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->getCbo());
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec3), colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(this->getColorLoc());//new
	glVertexAttribPointer(this->getColorLoc(),3,GL_FLOAT,GL_FALSE,0,0);//new
}
