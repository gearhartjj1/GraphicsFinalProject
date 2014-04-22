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
	zoomValue = 0;
	makeMatrix();
}

void Camera::makeMatrix()
{
	camMatrix = glm::lookAt(glm::vec3(referencePoint.x,referencePoint.y,referencePoint.z),glm::vec3(position.x,position.y,position.z),glm::vec3(upVector.x,upVector.y,upVector.z));
}

glm::mat4 Camera::getMatrix()
{
	glm::mat4 rotateH = glm::rotate(glm::mat4(1.0f),horizontalAngle,glm::vec3(0,1,0));
	glm::mat4 rotateV = glm::rotate(glm::mat4(1.0f),verticleAngle,glm::vec3(1,0,0));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,zoomValue));
	glm::mat4 transform = translate * rotateV * rotateH;
	return camMatrix * transform;
}

void Camera::zoom(float amount)
{
	zoomValue += amount;
	//makeMatrix();
}

void Camera::rotateHorizontal(float amount)
{
	horizontalAngle+=amount;
	//makeMatrix();
}

void Camera::rotateVerticle(float amount)
{
	verticleAngle+=amount;
	//makeMatrix();
}