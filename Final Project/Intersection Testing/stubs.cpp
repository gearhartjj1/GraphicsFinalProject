#include "stubs.h"
#include <algorithm>
#include <numeric>
#include <math.h>

using namespace glm;

const float EPS = 1e-12;//try this one, change it if it doesn't work

double Test_RaySphereIntersect(const vec3& P0, const vec3& V0, const mat4& T) {
	// TODO fill this in.
	// See the documentation of this function in stubs.h.

	mat4 inv = inverse(T);
	vec4 p = vec4(P0	, 1), v = vec4(V0, 0);
	p = inv*p;
	v = inv*v;
	int a = 1;
	vec4 b = 2*v*p;
	vec4 c = p*p;
	
	double t0=0, t1=0;
	t0 = (-b+sqrt(b*b-4*a*c))/2/a;
	t1 = (-b-sqrt(b*b-4*a*c))/2/a;
	if(0<t0<t1+EPS) {return t0;}
	if(0<t1<t0+EPS) {return t1;}
	
	return -1;
}

double Test_RayPolyIntersect(const vec3& P0, const vec3& V0, const vec3& p1, const vec3& p2, const vec3& p3, const mat4& T) {
	vec4 p = vec4(P0, 1), d = vec4(V0, 0);
	mat4 inv = inverse(T);
	mat4 dirInv = inverse(mat4(T[0][0],T[0][1],T[0][2],0,T[1][0],T[1][1],T[1][2],0,T[2][0],T[2][1],T[2][2],0,T[3][0],T[3][1],T[3][2],T[3][3]));
	p = inv * p;
	d = dirInv * d;
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

double Test_RayCubeIntersect(const vec3& P0, const vec3& V0, const mat4& T) {
	vec4 p = vec4(P0, 1), d = vec4(V0, 0);
	mat4 inv = inverse(T);
	p = inv*p;
	d = inv*d;

	float tnear = -std::numeric_limits<float>::infinity(), 
		tfar = std::numeric_limits<float>::infinity();

	for(int i=0; i < 3; ++i) {
		if(abs(d[i]) < EPS && (p[i] < -.5-EPS || p[i] > .5+EPS)) return -1;
		float t1 = (-.5 - p[i]) / d[i], t2 = (.5 - p[i]) / d[i];
		if(t1 > t2) std::swap(t1, t2);
		tnear = std::max(tnear, t1);
		tfar = std::min(tfar, t2);
		if(tnear > tfar + EPS) return -1;
		if(tfar < EPS) return -1;//This epsilon makes rays cast from the surface of objects work
	}

	return tnear;
}