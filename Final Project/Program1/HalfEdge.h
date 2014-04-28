/**
 * Author: Zach J. Wheeler
 * Date: 4/26/14
 * Graphics Final Project
 * Declare the half-edge data structure
 */


#pragma once

#include "Mesh.h"


class HalfEdge {
private:
	struct link;
	struct face {
		int id;
		link *p;
	};
	struct vertex {
		int id;
		link *p;
		Vertex v;
	};
	struct link {
		link *next, *sym;
		vertex *v;
		face *f;
	};

	vector<face*> f;
	vector<vertex*> v;

public:
	HalfEdge();
	HalfEdge(const Mesh&);
	HalfEdge(const HalfEdge&);
	HalfEdge& operator= (const HalfEdge&);
	~HalfEdge();

	Mesh tofacelist() const;
	void subdivide(int iterations);

private:
	void updatenormals();
};
