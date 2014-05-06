/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is Chair.cpp it contains the implementation of the chair class
*/

#include "Chair.h"

Chair::Chair(Cube* c)
{
	cube = c;
	setColor(glm::vec3(1,0,1));

	double distanceBetweenLegs = 0.9;
	double legHeight = 0.9;
	setHeight(legHeight*2);
	double legWidth = 0.1;
	glm::mat4 legScale = glm::scale(glm::mat4(1.0f),glm::vec3(legWidth,legHeight,legWidth));
	glm::mat4 seatScale = glm::scale(glm::mat4(1.0f),glm::vec3(distanceBetweenLegs,0.1,distanceBetweenLegs));
	glm::mat4 backScale = glm::scale(glm::mat4(1.0f), glm::vec3(distanceBetweenLegs,(distanceBetweenLegs),0.1));

	//scale for the chair legs
	chairTransforms[0].scale = legScale;
	chairTransforms[1].scale = legScale;
	chairTransforms[2].scale = legScale;
	chairTransforms[3].scale = legScale;
	//scale for the chair seat
	chairTransforms[4].scale = seatScale;
	//scale for the chair back
	chairTransforms[5].scale = backScale;

	//translation for the chair legs
	//first leg doesn't have to be moved
	//chairTransforms[0]->translate = glm::translate(glm::mat4(1.0f),glm::vec3(10,0,0));
	chairTransforms[0].translate = glm::translate(glm::mat4(1.0f),glm::vec3((distanceBetweenLegs-legWidth)/2,0,(distanceBetweenLegs-legWidth)/2));
	chairTransforms[1].translate = glm::translate(glm::mat4(1.0f),glm::vec3(-(distanceBetweenLegs-legWidth)/2,0,-(distanceBetweenLegs-legWidth)/2));
	chairTransforms[2].translate = glm::translate(glm::mat4(1.0f),glm::vec3(-(distanceBetweenLegs-legWidth)/2,0,(distanceBetweenLegs-legWidth)/2));
	chairTransforms[3].translate = glm::translate(glm::mat4(1.0f),glm::vec3((distanceBetweenLegs-legWidth)/2,0,-(distanceBetweenLegs-legWidth)/2));

	//translation for the chair seat
	chairTransforms[4].translate = glm::translate(glm::mat4(1.0f),glm::vec3(0,legHeight,0));

	//translation for the chair back
	chairTransforms[5].translate = glm::translate(glm::mat4(1.0f),glm::vec3(0,(legHeight),distanceBetweenLegs/2));
}

void Chair::draw(glm::mat4 transform, glm::vec3 color)
{
	setInverse(glm::inverse(transform));
	for(int i = 0; i < 6; i++)
	{
		glm::mat4 pieceTransform = chairTransforms[i].translate * chairTransforms[i].scale * chairTransforms[i].rotate;
		inverseMatrices[i] = glm::inverse(transform * pieceTransform);
		forwardMatrices[i] = transform * pieceTransform;
		cube->draw(transform * pieceTransform,color);
	}
}

double Chair::rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal)
{
	double t = std::numeric_limits<double>::infinity();
	for(int i = 0; i < 6; i++)
	{
		cube->setProperInverse(inverseMatrices[i]);
		cube->setForward(forwardMatrices[i]);
		glm::vec3 tempC;
		glm::vec4 tempN;
		double time = cube->rayTrace(Position,direction,tempC,tempN);
		if(time > 0 && time < t)
		{
			color = getColor();
			t = time;
			normal = tempN;
		}
	}
	return t;
}