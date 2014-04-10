/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: March 1, 2014
Description: this is camera.h it contains the declarations of the camera class
*/

#pragma once

#include "glew.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera(glm::vec4 refPt, glm::vec4 pos, glm::vec4 up);
	glm::mat4 getMatrix();
	glm::vec4 getPos() {return position;}
	glm::vec4 getRef() {return referencePoint;}
	glm::vec4 getUpV() {return upVector;}
	void setRef(glm::vec4 ref) {referencePoint = ref;makeMatrix();}
	void setPos(glm::vec4 pos) {position = pos;makeMatrix();}
	void setUpV(glm::vec4 up) {upVector = up;makeMatrix();}
	void zoom(float amount);
	void rotateVerticle(float amount);
	void rotateHorizontal(float amount);
private:
	void makeMatrix();
	glm::mat4 camMatrix;
	glm::vec4 referencePoint;
	glm::vec4 position;
	glm::vec4 upVector;
	float horizontalAngle;
	float verticleAngle;
	float zoomValue;
};

//camera = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//glm::lookAt(