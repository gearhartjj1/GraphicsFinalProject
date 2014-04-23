/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: April 1, 2014
Description: this is the mesh.h file. It contains the declarations for the mesh class and the Face struct
*/

#pragma once

#include "Vertex.h"
#include "geometry.h"
#include <string>
#include <fstream>
#include <vector>
using std::vector;
using std::ifstream;
using std::string;

//set to 3 vertices with the default constructor
struct Face
{
	Face();
	Face(int numV);
	~Face();
	int numVertices;
	unsigned int* indices;
};

class Mesh : public Geometry
{
public:
	Mesh(string fileName);
	~Mesh();
	vector<Vertex*>& getVertices() {return vertices;}
	vector<Face*>& getFaces() {return faces;}
	void draw(glm::mat4 transform, glm::vec3 color);
	int getNumVertices() {return vertices.size();}
	int getNumFaces() {return faces.size();}
	int getVerticesPerFace() {return verticesPerFace;}
	bool getFilled() {return filled;}
	void bufferData(glm::vec3 c);
private:
	bool filled;
	bool buffered;
	bool testConvex(glm::vec3 points[], int numPoints);
	void makeExtrusion(ifstream* file);
	void makeSurfRev(ifstream* file);
	void extrude(double height, glm::vec3 base[], int numPoints);
	void surfRev(int numSlices, glm::vec3 linePoints[], int numPoints);
	void makePolygon(glm::vec3 base[], int numPoints);

	vector<Vertex*> vertices;//this will be buffered into the shader
	vector<Face*> faces;//this will function as the index buffer indices
	int verticesPerFace;
	double startY;
};