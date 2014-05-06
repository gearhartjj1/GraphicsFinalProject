/*
Author: Zach J. Wheeler
Course: COMP 361, Computer Graphics
Date: 5/2/14
Description: sphere geometry
*/

#include "Sphere.h"
#include "catmullclark.h"

#include "glm/glm.hpp"

Sphere::Sphere() {
	setHeight(1.0f);
	setColor(glm::vec3(1,0,1));

	glm::vec4 v[8] = {
		glm::vec4(-.5f, 0.f, -.5f, 1.f),
		glm::vec4(-.5f, 0.f, .5f, 1.f),
		glm::vec4(.5f, 0.f, .5f, 1.f),
		glm::vec4(.5f, 0.f, -.5f, 1.f),
		glm::vec4(.5f, 1.f, -.5f, 1.f),
		glm::vec4(.5f, 1.f, .5f, 1.f),
		glm::vec4(-.5f, 1.f, .5f, 1.f),
		glm::vec4(-.5f, 1.f, -.5f, 1.f),
	};
	glm::vec4 n[8] = {
		glm::normalize(glm::vec4(-1.f,-1.f,-1.f,0.f)),
		glm::normalize(glm::vec4(-1.f,-1.f,1.f,0.f)),
		glm::normalize(glm::vec4(1.f,-1.f,1.f,0.f)),
		glm::normalize(glm::vec4(1.f,-1.f,-1.f,0.f)),
		glm::normalize(glm::vec4(1.f,1.f,-1.f,0.f)),
		glm::normalize(glm::vec4(1.f,1.f,1.f,0.f)),
		glm::normalize(glm::vec4(-1.f,1.f,1.f,0.f)),
		glm::normalize(glm::vec4(-1.f,1.f,-1.f,0.f))
	};
	Face* f[6] = {
		new Face(4),
		new Face(4),
		new Face(4),
		new Face(4),
		new Face(4),
		new Face(4)
	};
	
	//bottom
	f[0]->indices[3] = 0;
	f[0]->indices[2] = 3;
	f[0]->indices[1] = 2;
	f[0]->indices[0] = 1;
	
	//top
	f[1]->indices[3] = 4;
	f[1]->indices[2] = 7;
	f[1]->indices[1] = 6;
	f[1]->indices[0] = 5;
	
	//left
	f[2]->indices[3] = 0;
	f[2]->indices[2] = 1;
	f[2]->indices[1] = 6;
	f[2]->indices[0] = 7;
	
	//front
	f[3]->indices[3] = 3;
	f[3]->indices[2] = 0;
	f[3]->indices[1] = 7;
	f[3]->indices[0] = 4;

	//right
	f[4]->indices[3] = 2;
	f[4]->indices[2] = 3;
	f[4]->indices[1] = 4;
	f[4]->indices[0] = 5;

	//back
	f[5]->indices[3] = 1;
	f[5]->indices[2] = 2;
	f[5]->indices[1] = 5;
	f[5]->indices[0] = 6;

	mesh.getVertices().resize(8);
	for(int i=0; i < 8; ++i)
		mesh.getVertices()[i] = new Vertex(v[i], getColor(), n[i]);
	mesh.getFaces().assign(f, f+6);
	mesh.setVertexNormals(true);

	catmullclark(mesh, 3);

	//until catmullclark runs
	//mesh.setFilled(true);
	//mesh.triangulate();

	//find average radius, scale, translate down
	float r = 0;
	float hi=0,lo=200;
	for(int i=0; i < (int)mesh.getVertices().size(); ++i) {
		hi = glm::max(hi, mesh.getVertices()[i]->position.y);
		lo = glm::min(lo, mesh.getVertices()[i]->position.y);
	}
	for(int i=0; i < (int)mesh.getVertices().size(); ++i)
		r += glm::length(glm::vec3(mesh.getVertices()[i]->position) - glm::vec3(0.f, .5f*(hi-lo), 0.f));
	r /= mesh.getVertices().size();
	for(int i=0; i < (int)mesh.getVertices().size(); ++i)
		mesh.getVertices()[i]->position = glm::scale(glm::mat4(1.f), glm::vec3(.5/r, .5/r, .5/r)) * 
				glm::translate(glm::mat4(1.f), glm::vec3(0.f,-lo,0.f)) * mesh.getVertices()[i]->position;
}

void Sphere::draw(glm::mat4 transform, glm::vec3 color)
{
	setInverse(glm::inverse(transform));
	setForward(transform);
	mesh.draw(transform, color);
}

void Sphere::bufferData()
{
	mesh.bufferData(getColor());
}

double Sphere::rayTrace(glm::vec3 Position, glm::vec3 direction, glm::vec3& color, glm::vec4& normal)
{
	glm::vec4 objspacenormal;
	double time = raySphereIntersection(Position,direction,getInverse(),objspacenormal);
	color = getColor();
	normal = getForward() * objspacenormal;
	return time;
}