#pragma once

/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is geometry.h file it has the class declarations for the geometry objects
*/

#include "glew.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "intersections.h"


struct Transformation
{
	Transformation();
	glm::mat4 translate;
	glm::mat4 rotate;
	glm::mat4 scale;
};

class Geometry
{
public:
	//member functions
	Geometry() {height = 1; geoColor = glm::vec3(1,1,1); selectedColor = glm::vec3(0.1,0.1,0.75);isMesh = false;}
	virtual void draw(glm::mat4 transform, glm::vec3 color) = 0;
	void bufferColor(glm::vec3 color);
	//will return the time of intersection if any and set the color and normal of the point
	virtual double rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal) = 0;
	//setters
	void setHeight(float h) {height = h;}
	void setColor(glm::vec3 c) {geoColor = c;}
	void setInverse(glm::mat4 i) {inverseTransform = i;}
	void setMesh(bool m) {isMesh = m;}
	static void setVbo(unsigned int v) {vbo = v;}
	static void setCbo(unsigned int c) {cbo = c;}
	static void setNbo(unsigned int n) {nbo = n;}
	static void setIbo(unsigned int i) {ibo = i;}
	static void setModelLoc(unsigned int m) {u_modelMatrix = m;}
	static void setVertexLoc(unsigned int vLoc) {vLocation = vLoc;}
	static void setColorLoc(unsigned int cLoc) {vColor = cLoc;}
	static void setNormalLoc(unsigned int nLoc) {vNormal = nLoc;}

	//getters
	float getHeight() {return height;}
	glm::vec3 getColor() {return geoColor;}
	glm::vec3 getSelectedColor() {return selectedColor;}
	glm::mat4 getInverse() {return inverseTransform;}
	bool getIsMesh() {return isMesh;}
	static unsigned int getVbo() {return vbo;}
	static unsigned int getCbo() {return cbo;}
	static unsigned int getNbo() {return nbo;}
	static unsigned int getIbo() {return ibo;}
	static unsigned int getModelLoc() {return u_modelMatrix;}
	static unsigned int getVertexLoc() {return vLocation;}
	static unsigned int getColorLoc() {return vColor;}
	static unsigned int getNormalLoc() {return vNormal;}

private:
	bool isMesh;
	float height;
	glm::vec3 geoColor;
	glm::vec3 selectedColor;
	glm::mat4 inverseTransform;
	static unsigned int vbo;
	static unsigned int cbo;
	static unsigned int nbo;
	static unsigned int ibo;
	static unsigned int u_modelMatrix;
	static unsigned int vLocation;
	static unsigned int vColor;
	static unsigned int vNormal;
};