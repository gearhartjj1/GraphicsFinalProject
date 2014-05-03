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
	//copy constructor
	Face(const Face &original);
	Face& operator= (const Face &source);
};

class Mesh : public Geometry
{
public:
	Mesh();
	//copy constructor
	Mesh(const Mesh &original);
	Mesh& operator= (const Mesh &source);
	Mesh(string fileName);
	~Mesh();
	vector<Vertex*>& getVertices() {return vertices;}
	const vector<Vertex*>& getVertices() const {return vertices;}
	vector<Face*>& getFaces() {return faces;}
	const vector<Face*>& getFaces() const {return faces;}
	void draw(glm::mat4 transform, glm::vec3 color);
	virtual double rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal);
	int getNumVertices() const {return vertices.size();}
	int getNumFaces() const {return faces.size();}
	int getVerticesPerFace() const {return verticesPerFace;}
	bool getFilled() const {return filled;}
	void setFilled(bool filled) { this->filled = filled; }
	bool hasVertexNormals() const { return vertexnormals; }
	void setVertexNormals(bool nv) { vertexnormals = nv; }
	void bufferData(glm::vec3 c);
	void triangulate();//triangulate the mesh, assuming every face is convex & planar
	void clear();
private:
	bool filled;
	bool buffered;
	bool vertexnormals;
	bool testConvex(glm::vec3 points[], int numPoints);
	void makeExtrusion(ifstream* file);
	void makeSurfRev(ifstream* file);
	void extrude(double height, glm::vec3 base[], int numPoints);
	void surfRev(int numSlices, glm::vec3 linePoints[], int numPoints);
	void makePolygon(glm::vec3 base[], int numPoints);
	void docopy(const Mesh&, bool);

	vector<Vertex*> vertices;//this will be buffered into the shader
	vector<Face*> faces;//this will function as the index buffer indices
	int verticesPerFace;
	double startY;
};