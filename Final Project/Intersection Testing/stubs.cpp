#include "stubs.h"
#include <algorithm>
#include <numeric>

using namespace glm;

const float EPS = 1e-12;//try this one, change it if it doesn't work

double Test_RaySphereIntersect(const vec3& P0, const vec3& V0, const mat4& T) {
	// TODO fill this in.
	// See the documentation of this function in stubs.h.

	return -1;
}

double Test_RayPolyIntersect(const vec3& P0, const vec3& V0, const vec3& p1, const vec3& p2, const vec3& p3, const mat4& T) {
	// TODO fill this in.
	// See the documentation of this function in stubs.h.

	return -1;
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