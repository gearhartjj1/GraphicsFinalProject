/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is table.cpp it contains the implementation of the table class
*/

#include "Table.h"

Table::Table(Cube* c)
{
	cube = c;
	setColor(glm::vec3(0,1,0));

	double distanceBetweenLegs = 1;
	double legHeight = 1;
	setHeight(legHeight);
	double legWidth = 0.1;
	glm::mat4 legScale = glm::scale(glm::mat4(1.0f),glm::vec3(legWidth,legHeight,legWidth));
	glm::mat4 seatScale = glm::scale(glm::mat4(1.0f),glm::vec3(distanceBetweenLegs,0.1,distanceBetweenLegs));

	//scale for the table legs
	tableTransforms[0].scale = legScale;
	tableTransforms[1].scale = legScale;
	tableTransforms[2].scale = legScale;
	tableTransforms[3].scale = legScale;
	//scale for the table seat
	tableTransforms[4].scale = seatScale;

	//translation for the table legs
	tableTransforms[0].translate = glm::translate(glm::mat4(1.0f),glm::vec3((distanceBetweenLegs-legWidth)/2,0,(distanceBetweenLegs-legWidth)/2));
	tableTransforms[1].translate = glm::translate(glm::mat4(1.0f),glm::vec3(-(distanceBetweenLegs-legWidth)/2,0,-(distanceBetweenLegs-legWidth)/2));
	tableTransforms[2].translate = glm::translate(glm::mat4(1.0f),glm::vec3(-(distanceBetweenLegs-legWidth)/2,0,(distanceBetweenLegs-legWidth)/2));
	tableTransforms[3].translate = glm::translate(glm::mat4(1.0f),glm::vec3((distanceBetweenLegs-legWidth)/2,0,-(distanceBetweenLegs-legWidth)/2));

	//translation for the table top
	tableTransforms[4].translate = glm::translate(glm::mat4(1.0f),glm::vec3(0,legHeight,0));
}

void Table::draw(glm::mat4 transform, glm::vec3 color)
{
	for(int i = 0; i < 5; i++)
	{
		glm::mat4 pieceTransform = tableTransforms[i].translate * tableTransforms[i].scale * tableTransforms[i].rotate;
		cube->draw(transform * pieceTransform, color);
	}
}