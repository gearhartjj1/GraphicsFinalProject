/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: april 1, 2014
Description: this is mesh.cpp it contains the definitions of the functions for the mesh class
*/

#include "Mesh.h"

//then modify structure to maybe set mesh up to derive from Geometry or something to make things simple later

Face::Face()
{
	numVertices = 3;
	indices = new unsigned int[3];
}

Face::Face(int numV)
{
	if(numV > 0) {
		numVertices = numV;
		indices = new unsigned int[numV];
	} else {
		numVertices = 0;
		indices = 0;
	}
}

Face::~Face()
{
	delete indices;
}

Face::Face(const Face &original)
{
	numVertices = original.numVertices;
	indices = new unsigned int[numVertices];
	for(int i = 0; i < numVertices; i++)
	{
		indices[i] = original.indices[i];
	}
}

Face& Face::operator= (const Face &source)
{
	numVertices = source.numVertices;
	delete indices;
	indices = new unsigned int[numVertices];
	for(int i = 0; i < numVertices; i++)
	{
		indices[i] = source.indices[i];
	}
	return *this;
}

Mesh::Mesh() {
	setMesh(true);
	setColor(glm::vec3(0.0,1.0,1.0));
	buffered = false;
	filled = false;
	verticesPerFace = 3;
}

Mesh::Mesh(const Mesh &original)
{
	docopy(original, true);
}

Mesh& Mesh::operator= (const Mesh &source)
{
	docopy(source, false);
	return *this;
}

void Mesh::docopy(const Mesh &source, bool cc) {
	if(!cc)
		clear();

	setMesh(source.getIsMesh());
	filled = source.filled;
	buffered = source.buffered;
	verticesPerFace = source.verticesPerFace;
	setColor(source.getColor());
	startY = source.startY;
	for(int i = 0; i < source.getVertices().size(); i++)
	{
		Vertex* v = new Vertex(source.getVertices()[i]->position,source.getVertices()[i]->color,source.getVertices()[i]->normal);
		vertices.push_back(v);
	}
	for(int i = 0; i < source.getFaces().size();i++)
	{
		Face*  f = new Face(*source.getFaces()[i]);
		faces.push_back(f);
	}
}

//constructor builds the mesh from the given file
Mesh::Mesh(string fileName)
{
	setMesh(true);
	startY = 0;
	setColor(glm::vec3(0.0,1.0,1.0));
	buffered = false;
	filled = false;
	verticesPerFace = 3;
	ifstream fin;
	fin.open(fileName);

	string type;
	fin >> type;

	if(type == "extrusion")
	{
		makeExtrusion(&fin);
	}
	else if(type == "surfrev")
	{
		makeSurfRev(&fin);
	}

	fin.close();
}

Mesh::~Mesh()
{
	clear();
}

void Mesh::clear()
{
	for(int i = 0; i < vertices.size(); i++)
	{
		if(vertices[i])
			delete vertices[i];
	}
	for(int i = 0; i < faces.size(); i++)
	{
		if(faces[i])
			delete faces[i];
	}
	vertices.clear();
	faces.clear();
	filled = buffered = false;
	verticesPerFace = 3;
	startY = 0;
}

//expects a pointer to a filestream that is open to an input file
void Mesh::makeExtrusion(ifstream* file)
{
	double height = 0;
	*file >> height;
	setHeight(height);
	int numPoints = 0;
	*file >> numPoints;
	glm::vec3* basePoints = new glm::vec3[numPoints];
	for(int i = 0; i < numPoints-1; i++)
	{
		double x,z;
		*file >> x;
		*file >> z;
		basePoints[i] = glm::vec3(x,0,z);
	}
	bool ccw=false;
	if(numPoints >= 3) {
		double a=0;
		for(int i=0; i < numPoints; ++i) {
			a += basePoints[i].x*(double)basePoints[(i+1)%numPoints].z - basePoints[(i+1)%numPoints].x*(double)basePoints[i].z;
		}
		if(a > 1e-8)
			ccw=true;
	}
	if(ccw) {
		std::reverse(basePoints, basePoints + numPoints);
	}

	extrude(height,basePoints,numPoints-1);
	//makePolygon(basePoints, numPoints-1);

	delete[] basePoints;
}

//used to put the relevant vertices and faces into the mesh for any given points
void Mesh::makePolygon(glm::vec3 base[], int numPoints)
{
	if(!testConvex(base,numPoints))
	{
		return;
	}

	//triangulate the polygon
	for(int i = 1; i < numPoints-1; i++)
	{
		Face* f = new Face(3);
		f->indices[0] = vertices.size();
		f->indices[1] = vertices.size() + i;
		f->indices[2] = vertices.size() + i+1;
		faces.push_back(f);
	}

	//put in polygon base points
	Vertex* v = new Vertex();
	glm::vec3 norm = glm::cross((base[numPoints-1]-base[0]),(base[1]-base[0]));
	if(norm.x != 0 || norm.y != 0 || norm.z != 0)
	{
		//have to calculate the normal for the first and last point slightly differently because they are the same point
		v->normal = glm::normalize(glm::vec4(norm.x,norm.y,norm.z,0));
	}
	else
	{
		v->normal = (glm::vec4(norm.x,norm.y,norm.z,0));
	}
	v->position = glm::vec4(base[0].x,base[0].y,base[0].z,1);
	vertices.push_back(v);

	for(int i = 1; i < numPoints-1; i++)
	{
		v = new Vertex();
		v->position = glm::vec4(base[i].x,base[i].y,base[i].z,1);
		norm = glm::cross((base[i-1]-base[i]),(base[i+1]-base[i]));
		if(norm.x != 0 || norm.y != 0 || norm.z != 0)
		{
			v->normal = glm::normalize(glm::vec4(norm.x,norm.y,norm.z,0));
		}
		else
		{
			v->normal = (glm::vec4(norm.x,norm.y,norm.z,0));
		}
		vertices.push_back(v);
	}
	v = new Vertex();
	v->position = glm::vec4(base[numPoints-1].x,base[numPoints-1].y,base[numPoints-1].z,1);
	norm = glm::cross((base[numPoints-2]-base[numPoints-1]),(base[0]-base[numPoints-1]));
	if(norm.x != 0 || norm.y != 0 || norm.z != 0)
	{
		v->normal = glm::normalize(glm::vec4(norm.x,norm.y,norm.z,0));
	}
	else
	{
		v->normal = (glm::vec4(norm.x,norm.y,norm.z,0));
	}

	vertices.push_back(v);

	filled = true;
}

void Mesh::makeSurfRev(ifstream* file)
{
	int numSlices = 0;
	float start = 0, end = 0;
	*file >> numSlices;
	int numPoints = 0;
	*file >> numPoints;
	glm::vec3* linePoints = new glm::vec3[numPoints];
	for(int i = 0; i < numPoints; i++)
	{
		double x,y;
		*file >> x;
		//stop process if an x value is negative
		if(x<0)
		{
			return;
		}
		*file >> y;
		if(y>end)
		{
			end = y;
		}
		if(y<start)
		{
			start = y;
		}
		linePoints[i] = glm::vec3(x,y,0);
	}

	setHeight(end-start);
	//used to ensure proper transformations of height
	startY = start;

	surfRev(numSlices,linePoints,numPoints);

	delete[] linePoints;
}

void Mesh::extrude(double height, glm::vec3 base[], int numPoints)
{
	//if the base is convex then the caps of the prism should be generated
	bool convex = testConvex(base, numPoints);
	glm::vec3* top = new glm::vec3[numPoints];

	for(int i = 0; i < numPoints; i++)
	{
		top[i] = base[numPoints-i-1];
		top[i].y = top[i].y + height;
	}
	if(convex)
	{
		makePolygon(base,numPoints);
		makePolygon(top,numPoints);
	}

	//make sides of the polygon
	for(int i = 0; i < numPoints; i++)
	{
		glm::vec3* wall = new glm::vec3[4];
		wall[0] = base[numPoints-i-1];
		if((i+1)<numPoints)
		{
			wall[1] = base[numPoints-(i+1)-1];
			wall[2] = top[i+1];
		}
		else
		{
			wall[1] = base[numPoints-1];
			wall[2] = top[0];
		}
		wall[3] = top[i];
		makePolygon(wall,4);
	}
}

void Mesh::surfRev(int numSlices, glm::vec3 linePoints[], int numPoints)
{
	float angleBetween = 360 / numSlices;
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f),angleBetween,glm::vec3(0,1,0));
	glm::vec3* originalPoints = linePoints;
	//arrays of the points for the caps if they are necessary
	//in the event the shap has a hole in one of the ends
	glm::vec3* topCap = new glm::vec3[numSlices];
	glm::vec3* bottomCap = new glm::vec3[numSlices];

	for(int i = 0; i < numSlices; i++)
	{
		topCap[numSlices-1-i] = originalPoints[numPoints-1];
		bottomCap[i] = originalPoints[0];
		glm::vec3* newPoints = new glm::vec3[numPoints];
		for(int k = 0; k < numPoints; k++)
		{
			glm::vec4 point = glm::vec4(originalPoints[k].x,originalPoints[k].y,originalPoints[k].z,1);
			point = rotate * point;
			newPoints[k] = glm::vec3(point.x,point.y,point.z);
		}
		for(int j = 0; j < numPoints-1; j++)
		{
			glm::vec3* side = new glm::vec3[4];
			side[0] = originalPoints[j];
			if((j+1)<numPoints)
			{
				side[1] = originalPoints[j+1];
				side[2] = newPoints[j+1];
			}
			else
			{
				side[1] = originalPoints[0];
				side[2] = newPoints[0];
			}
			side[3] = newPoints[j];
			int numPts = 4;
			//do detections to make sure it is a square side or a triangle side
			//check if the middle points are the same
			if((side[1].x>side[2].x-0.00001 || side[1].x<side[2].x+0.00001) && (side[1].y>side[2].y-0.00001 || side[1].y<side[2].y+0.00001) && (side[1].z>side[2].z-0.0001 || side[1].z<side[2].z+0.0001))
			{
				side[2].x = side[3].x;
				side[2].y = side[3].y;
				side[2].z = side[3].z;
				numPts = 3;
			}
			//check if the first and last points are the same
			if(side[0].x==side[3].x && side[0].y==side[3].y && side[0].z==side[3].z)
			{
				numPts = 3;
			}

			makePolygon(side,numPts);
		}
		originalPoints = newPoints;
		//delete[] newPoints;
	}

	//add end cap if necessary
	if(linePoints[numPoints-1].x!=0 && (linePoints[0].y!=linePoints[numPoints-1].y || linePoints[0].z!=linePoints[numPoints-1].z))
	{
		makePolygon(topCap,numSlices);
	}
	if(linePoints[0].x!=0 && (linePoints[0].y!=linePoints[numPoints-1].y || linePoints[0].z!=linePoints[numPoints-1].z))
	{
		makePolygon(bottomCap,numSlices);
	}
	delete[] topCap;
}

bool Mesh::testConvex(glm::vec3 points[], int numPoints)
{
	glm::vec3 normal = glm::cross((points[1]-points[0]),(points[numPoints-2]-points[0]));
	normal = glm::normalize(normal);
	glm::vec3 t;
	for(int i = 1; i < numPoints-1; i++)
	{
		t = glm::cross((points[i+1]-points[i]),(points[i-1]-points[i]));
		t = glm::normalize(t);
		if(t.x == -normal.x && t.y == -normal.y && t.z == -normal.z)
		{
			return false;
		}
	}
	t = glm::cross((points[0]-points[numPoints-1]),(points[numPoints-2]-points[numPoints-1]));
	if(t.x==-normal.x && t.y==-normal.y && t.z==-normal.z)
	{
		return false;
	}

	return true;
}

void Mesh::draw(glm::mat4 transform, glm::vec3 color)
{
	setInverse(glm::inverse(transform));
	//makes sure the data has been buffered to the graphics card before attempting to draw the shape
	bufferData(color);
	if(!buffered)
		return;
	int numPoints = getNumVertices();
	int indicesPerFace = getVerticesPerFace();
	int numFaces = getNumFaces();
	int numIndices = (numFaces) * indicesPerFace;

	glm::mat4 t = glm::translate(glm::mat4(1.0f),glm::vec3(0,-startY,0));
	t = transform * t;

	glUniformMatrix4fv(getModelLoc(), 1, GL_FALSE, &t[0][0]);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}

//need way to find normal of the given point
double Mesh::rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal)
{
	double t = std::numeric_limits<double>::infinity();
	for(int i = 0; i < faces.size(); i++)
	{
		glm::vec3 P0(vertices[faces[i]->indices[0]]->position);
		glm::vec3 P1(vertices[faces[i]->indices[1]]->position);
		glm::vec3 P2(vertices[faces[i]->indices[2]]->position);
		double time = rayTriangleIntersection(Position,direction,P0,P1,P2,getInverse());
		if(time > 0 && time < t)
		{
			t = time;
			color = getColor();
		}
	}
	return t;
}

void Mesh::bufferData(glm::vec3 c)
{
	if(!filled)
		return;
	int numPoints = getNumVertices();
	int indicesPerFace = getVerticesPerFace();
	int numFaces = getNumFaces();
	int numIndices = (numFaces) * indicesPerFace;

	glm::vec4* points = new glm::vec4[numPoints];
	glm::vec4* normals = new glm::vec4[numPoints];
	glm::vec3* colors = new glm::vec3[numPoints];

	unsigned int* indices = new unsigned int[numIndices];

	for(int i = 0; i < numPoints; i++)
	{
		points[i] = (getVertices())[i]->position;
		normals[i] = (getVertices())[i]->normal;
		if(normals[i].x==0 && normals[i].y==0 && normals[i].z ==0)
		{
			int stuff = 0;
		}
		colors[i] = c;
	}
	
	for(int i = 0; i < numFaces; i++)
	{
		for(int j = 0; j < indicesPerFace; j++)
		{
			indices[(i*indicesPerFace)+j] = faces[i]->indices[j];
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, getVbo());
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(glm::vec4), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(getVertexLoc());
	glVertexAttribPointer(getVertexLoc(), 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, getNbo());
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(glm::vec4),normals,GL_STATIC_DRAW);
	glEnableVertexAttribArray(getNormalLoc());
	glVertexAttribPointer(getNormalLoc(),4,GL_FLOAT,GL_FALSE,0,0);

	glBindBuffer(GL_ARRAY_BUFFER, getCbo());
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(glm::vec3),colors,GL_STATIC_DRAW);
	glEnableVertexAttribArray(getColorLoc());
	glVertexAttribPointer(getColorLoc(),3,GL_FLOAT,GL_FALSE,0,0);


	//set up the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getIbo());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	delete points;
	delete normals;
	delete colors;
	delete indices;
	buffered = true;
}

void Mesh::triangulate() {
	int n=faces.size();
	for(int i=0; i < n; ++i) {
		assert(faces[i]);
		Face &face = *faces[i];
		//the current face will become one triangle, the rest will be pushed onto the back
		//this method has poor face locality, but allows the algorithm to be implemented with minimal array copying
		if(face.numVertices > 3) {
			for(int j=2; j < face.numVertices-1; ++j) {
				Face *next = new Face(3);
				next->indices[0] = face.indices[0];
				next->indices[1] = face.indices[j];
				next->indices[2] = face.indices[j+1];
				faces.push_back(next);
			}

			unsigned *idx = new unsigned[3];
			idx[0] = face.indices[0];
			idx[1] = face.indices[1];
			idx[2] = face.indices[2];
			delete face.indices;
			face.indices = idx;
			face.numVertices = 3;
		}
	}
	verticesPerFace=3;
}
