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
#include "Sphere.h"
#include "catmullclark.h"

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
#include <math.h>

class SceneGraph
{
public:
	//member functions
	SceneGraph();
	~SceneGraph();
	SceneGraph(Geometry* g, int w, int d, int numN, glm::vec3 trans, glm::vec3 scale, float rot);
	
	bool rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4 lightLoc, glm::vec4 eyePos);
	void fillGraph(string inputName);
	void traverse(glm::mat4 m) const;
	void draw(glm::mat4 m);
	void addChild(SceneGraph* sg, int x, int z);
	void addChild(SceneGraph* sg, SceneGraph* parent);

	//recursive functions
	SceneGraph* getNextNode(SceneGraph* current);
	SceneGraph* getPreviousNode(SceneGraph* current);
	//returns the time the node is hit by the ray and the appropriate color and normal value
	double rayTraceStack(SceneGraph* node, glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal);

	//getters
	int getWidth() {return width;}
	int getDepth() {return depth;}
	int getNumNodes() {return numNodes;}
	float getTransX() {return transX;}
	float getTransY() {return transY;}
	float getTransZ() {return transZ;}
	float getScaleX() {return scaleX;}
	float getScaleY() {return scaleY;}
	float getScaleZ() {return scaleZ;}
	float getRotY() {return rotY;}
	double getFloorScale() {return floorScale;}
	bool getSelected() {return selected;}
	Geometry* getGeometry() {return geo;}
	void setGeometry(Geometry *g) { geo = g; }
	float getReflect() {return reflectivity;}

	//setters
	void setTransX(float tX) {transX = tX;}
	void setTransY(float tY) {transY = tY;}
	void setTransZ(float tZ) {transZ = tZ;}
	void setScaleX(float sX) {scaleX = sX;}
	void setScaleY(float sY) {scaleY = sY;}
	void setScaleZ(float sZ) {scaleZ = sZ;}
	void setRotY(float rY) {rotY = rY;}
	void setReflectivity(float r) {reflectivity = r;}
	void setSelected(bool s) {selected = s;}
	static void setCube(Cube* c) {cube = c;}
	static void setChair(Chair* ch) {chair = ch;}
	static void setTable(Table* tb) {table = tb;}
	static void setSphere(Sphere* sp) {sphere = sp;}

private:
	//function to get the top of the tower
	SceneGraph* getTowerTop(SceneGraph* current);
	//intersection testing functions

	//do not delete the geometry pointers that were given from the widget!!!
	static Cube* cube;
	static Chair* chair;
	static Table* table;
	static Sphere* sphere;
	int width, depth, numNodes;
	float reflectivity;
	//used to determine if the particular node is currently selected
	bool selected;
	//keeps track of which node section on the floor is selected
	int selectedIndex, nextIndex;
	double floorScale;
	float transX, transY, transZ;
	float scaleX, scaleY, scaleZ;
	float rotY;
	Geometry* geo;
	SceneGraph* parent;
	SceneGraph** children;
	vector<Mesh*> meshes;
	vector<string> fileNames;
	vector<glm::mat4> wallInverseTransforms, wallForwardTransforms;
};