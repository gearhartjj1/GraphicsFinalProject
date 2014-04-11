/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is the header file for scenegraph it contains the declarations for the scenegraph
*/

#pragma once

#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/glm.hpp"
#include "geometry.h"
#include "Cube.h"
#include "Chair.h"
#include "Table.h"
#include "Mesh.h"

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <fstream>
using std::ifstream;
using std::getline;
#include <stdio.h>
using std::atoi;
using std::atof;
#include <sstream>
using std::stringstream;

class SceneGraph
{
public:
	//member functions
	SceneGraph();
	~SceneGraph();
	SceneGraph(Geometry* g, int w, int d, int numN, glm::vec3 trans, glm::vec3 scale, float rot);
	void fillGraph(string inputName);
	void traverse(glm::mat4 m) const;
	void draw(glm::mat4 m);
	void addChild(SceneGraph* sg, int x, int z);
	void addChild(SceneGraph* sg);

	//getters
	int getWidth() {return width;}
	int getDepth() {return depth;}
	float getTransX() {return transX;}
	float getTransY() {return transY;}
	float getTransZ() {return transZ;}
	float getScaleX() {return scaleX;}
	float getScaleY() {return scaleY;}
	float getScaleZ() {return scaleZ;}
	float getRotY() {return rotY;}
	double getFloorScale() {return floorScale;}

	//setters
	void setTransX(float tX) {transX = tX;}
	void setTransY(float tY) {transY = tY;}
	void setTransZ(float tZ) {transZ = tZ;}
	void setScaleX(float sX) {scaleX = sX;}
	void setScaleY(float sY) {scaleY = sY;}
	void setScaleZ(float sZ) {scaleZ = sZ;}
	void setRotY(float rY) {rotY = rY;}
	static void setCube(Cube* c) {cube = c;};
	static void setChair(Chair* ch) {chair = ch;}
	static void setTable(Table* tb) {table = tb;}

private:
	//do not delete the geometry pointers that were given from the widget!!!
	static Cube* cube;
	static Chair* chair;
	static Table* table;
	int width, depth, numNodes;
	double floorScale;
	float transX, transY, transZ;
	float scaleX, scaleY, scaleZ;
	float rotY;
	Geometry* geo;
	SceneGraph** children;
	vector<Mesh*> meshes;
	vector<string> fileNames;
};