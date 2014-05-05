/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is carmera.h it contains the declaration of the camera class
*/
#include "camera.h"

Camera::Camera(glm::vec4 refPt, glm::vec4 pos, glm::vec4 up)
{
	referencePoint = refPt;
	position = pos;
	upVector = up;
	horizontalAngle = 0;
	verticleAngle = 0;
	zoomValue = glm::length(pos-refPt);
	makeMatrix();
}

void Camera::makeMatrix()
{
	glm::mat4 rotx = glm::rotate(glm::mat4(1.f), horizontalAngle, glm::vec3(0.f,1.f,0.f));
	glm::vec3 perp = glm::vec3(rotx * glm::vec4(1.f,0.f,0.f,0.f));

	position = glm::rotate(glm::mat4(1.f), verticleAngle, perp) * 
			glm::rotate(glm::mat4(1.f), horizontalAngle, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(0.f,0.f,zoomValue,1.f);
	//referencePoint = glm::vec4(0.f,0.f,0.f,1.f);
	upVector = glm::normalize(glm::vec4(glm::cross(glm::vec3(position), perp), 0.f));

	glm::mat4 camera;

	camera = glm::lookAt(glm::vec3(position),glm::vec3(referencePoint),glm::vec3(upVector));

	camMatrix = camera;
}

glm::mat4 Camera::getMatrix()
{
	/*glm::mat4 rotateH = glm::rotate(glm::mat4(1.0f),horizontalAngle,glm::vec3(0,1,0));
	glm::mat4 rotateV = glm::rotate(glm::mat4(1.0f),verticleAngle,glm::vec3(1,0,0));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,zoomValue));
	glm::mat4 transform = translate * rotateV * rotateH;
	return camMatrix * transform;*/
	return camMatrix;
}

void Camera::zoom(float amount)
{
	if(zoomValue-amount > 0)
	{
		zoomValue -= amount;
	}
	makeMatrix();
}

void Camera::rotateHorizontal(float amount)
{
	horizontalAngle-=amount;
	makeMatrix();
}

void Camera::rotateVerticle(float amount)
{
	verticleAngle-=amount;
	makeMatrix();
}