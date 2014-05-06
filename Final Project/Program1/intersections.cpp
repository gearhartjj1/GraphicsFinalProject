#include "intersections.h"
#include <algorithm>
#include <numeric>
#include <math.h>



using namespace glm;

const float EPS = 1e-12;//try this one, change it if it doesn't work

double rayCubeIntersection(const vec3& P0, const vec3& V0, const mat4& inverse, int &near, bool &lesserplane)
{
	vec4 p = vec4(P0, 1), d = vec4(V0, 0);
	p = inverse*p;
	d = inverse*d;

	float tnear = -std::numeric_limits<float>::infinity(), 
		tfar = std::numeric_limits<float>::infinity();

	near=-1;
	lesserplane = false;
	for(int i=0; i < 3; ++i) {
		if(abs(d[i]) < EPS && (p[i] < -.5-EPS || p[i] > .5+EPS)) return -1;
		float t1, t2;
		if(i!=1) {
			t1 = (-.5 - p[i]) / d[i];
			t2 = (.5 - p[i]) / d[i];
		} else {
			t1 = (0. - p[i]) / d[i];
			t2 = (1. - p[i]) / d[i];
		}
		lesserplane=false;
		if(t1 > t2) {
			std::swap(t1, t2);
			lesserplane=true;
		}
		if(t1 > tnear) {
			near = i;
			tnear = t1;
		}
		tfar = std::min(tfar, t2);
		if(tnear > tfar + EPS) return -1;
		if(tfar < EPS) return -1;//This epsilon makes rays cast from the surface of objects work
	}

	return tnear;
}

double calculateArea(const vec3& p1, const vec3& p2, const vec3& p3)
{
	glm::mat3 matrix1 = glm::mat3(p1.y,p2.y,p3.y,p1.z,p2.z,p3.z,1,1,1);
	glm::mat3 matrix2 = glm::mat3(p1.z,p1.z,p3.z,p1.x,p2.x,p3.x,1,1,1);
	glm::mat3 matrix3 = glm::mat3(p1.x,p2.x,p3.x,p1.y,p2.y,p3.y,1,1,1);

	double det1 = glm::determinant(matrix1);
	double det2 = glm::determinant(matrix2);
	double det3 = glm::determinant(matrix3);

	double area = 0.5 * (sqrt((pow(det1,2)+pow(det2,2)+pow(det3,2))));
	return area;
}

double rayTriangleIntersection(const vec3& P0, const vec3& V0, const vec3& p1, const vec3& p2, const vec3& p3, const mat4& inverse)
{
	vec4 p = vec4(P0, 1), d = vec4(V0, 0);
	p = inverse * p;
	d = inverse * d;
	//get position and direction in local space
	glm::vec3 localP0 = glm::vec3(p.x,p.y,p.z);
	glm::vec3 localV0 = glm::vec3(d.x,d.y,d.z);

	double t;
	//calculate triangle normal
	glm::vec3 normal = glm::cross((p1-p2),(p3-p2));
	normal = glm::normalize(normal);

	double equationTop = glm::dot(normal,(p2-localP0));

	localV0 +=localP0;
	double equationBottom = glm::dot(normal,(localV0-localP0));
	if(abs(equationBottom) < EPS) return -1;
	t = equationTop/equationBottom;

	if(t < EPS) return -1;

	glm::vec3 intersect = localP0 + (localV0-localP0)*(float)t;
	//calculate area of each sub-triangle
	double totalArea = calculateArea(p1,p2,p3);
	double area1 = calculateArea(intersect,p2,p3) / totalArea;
	double area2 = calculateArea(intersect,p3,p1) / totalArea;
	double area3 = calculateArea(intersect,p1,p2) / totalArea;
	//check if the point is in the triangle
	if(abs((area1+area2+area3)-1) >= EPS)
	{
		return -1;
	}
	return t;
}

double raySphereIntersection(const vec3& P0, const vec3& V0, const mat4& inverse)
{
	vec4 p = vec4(P0, 1), v = vec4(V0, 0);
	p = inverse*p;
	v = inverse*v;
	double t0=0, t1=0;
	double a = 1;
	double b = 2*dot(vec3(v), vec3(p));
	double c = length(vec3(p))*length(vec3(p))-1;
	
	if(b*b-4*a*c < 0) {return -1;}
	t0 = (-b+sqrt(b*b-4*a*c))/2/a;
	t1 = (-b-sqrt(b*b-4*a*c))/2/a;
	if(0 < t0 && t0 < t1) {return t0;}
	else if(0 < t1 && t1 < t0) {return t1;}
	else {return -1;}
}