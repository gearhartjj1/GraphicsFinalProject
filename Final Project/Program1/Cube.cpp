/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is cube.cpp it contains the implementation of the cube class
*/
#include "Cube.h"

Cube::Cube()
{
	setHeight(1.0f);
	setColor(glm::vec3(1,0,0));

	cube[0] = glm::vec4(-0.5,0,0.5,1);
	cube[1] = glm::vec4(-0.5,1.0,0.5,1);
	cube[2] = glm::vec4(0.5,1.0,0.5,1);
	cube[3] = glm::vec4(0.5,0,0.5,1);

	cube[4] = glm::vec4(-0.5,0,0.5,1);
	cube[5] = glm::vec4(0.5,0,0.5,1);
	cube[6] = glm::vec4(0.5,0,-0.5,1);
	cube[7] = glm::vec4(-0.5,0,-0.5,1);
	
	cube[8] = glm::vec4(-0.5,0,0.5,1);
	cube[9] = glm::vec4(-0.5,0,-0.5,1);
	cube[10] = glm::vec4(-0.5,1.0,-0.5,1);
	cube[11] = glm::vec4(-0.5,1.0,0.5,1);
	
	cube[12] = glm::vec4(-0.5,1.0,0.5,1);
	cube[13] = glm::vec4(0.5,1.0,0.5,1);
	cube[14] = glm::vec4(0.5,1.0,-0.5,1);
	cube[15] = glm::vec4(-0.5,1.0,-0.5,1);

	cube[16] = glm::vec4(-0.5,0,-0.5,1);
	cube[17] = glm::vec4(0.5,0,-0.5,1);
	cube[18] = glm::vec4(0.5,1.0,-0.5,1);
	cube[19] = glm::vec4(-0.5,1.0,-0.5,1);

	cube[20] = glm::vec4(0.5,0,0.5,1);
	cube[21] = glm::vec4(0.5,0,-0.5,1);
	cube[22] = glm::vec4(0.5,1.0,-0.5,1);
	cube[23] = glm::vec4(0.5,1.0,0.5,1);

	for(int i = 0; i < 4; i++)
		cubeNormals[i] = glm::vec4(0,0,1,0);//front

	for(int i = 4; i < 8; i++)
		cubeNormals[i] = glm::vec4(0,-1,0,0);//left

	for(int i = 8; i < 12; i++)
		cubeNormals[i] = glm::vec4(-1,0,0,0);

	for(int i = 12; i < 16; i++)
		cubeNormals[i] = glm::vec4(0,1,0,0);

	for(int i = 16; i < 20; i++)
		cubeNormals[i] = glm::vec4(0,0,-1,0);

	for(int i = 20; i < 24; i++)
		cubeNormals[i] = glm::vec4(1,0,0,0);
}

void Cube::draw(glm::mat4 transform, glm::vec3 color)
{
	bufferColor(color);

	setInverse(glm::inverse(transform));
	properInverse = getInverse();
	setForward(transform);

	//sadly the data needs to be buffered on draw because now there shall be shapes other than cubes sad day
	bufferData();

	glUniformMatrix4fv(this->getModelLoc(), 1, GL_FALSE, &transform[0][0]);

	glDrawArrays(GL_QUADS, 0, 24);
}

void Cube::bufferData()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->getVbo());
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), cube, GL_STATIC_DRAW);
	glEnableVertexAttribArray(this->getVertexLoc());
	glVertexAttribPointer(this->getVertexLoc(), 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->getNbo());
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4),cubeNormals,GL_STATIC_DRAW);
	glEnableVertexAttribArray(this->getNormalLoc());
	glVertexAttribPointer(this->getNormalLoc(),4,GL_FLOAT,GL_FALSE,0,0);
}

//how would I get the normal at the point?...
double Cube::rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal)
{
	glm::vec4 cubespacenormal;
	double time = rayCubeIntersection(Position,direction,properInverse,cubespacenormal);
	normal = getForward() * cubespacenormal;
	color = getColor();
	return time;
}