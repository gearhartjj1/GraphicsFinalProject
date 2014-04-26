/**
 * Author: Zach J. Wheeler
 * Date: 4/26/14
 * Graphics Final Project
 * Implement the half-edge data structure
 */


#include "HalfEdge.h"

#include <vector>
using std::vector;

#include <utility>
using std::pair;
using std::make_pair;


HalfEdge::HalfEdge() {

}

// Convert from facelist to half-edge
// O(N*x^2 + M), where N is # vertices, x is maximum # of vertices per face, and M is # faces
// Since faces usually have few vertices, the x^2 term is small, so this is efficient
HalfEdge::HalfEdge(const Mesh &mesh) {
	// Copy the vertices
	v.resize(mesh.getVertices().size(), 0);
	for(int i=0; i < (int)mesh.getVertices().size(); ++i) {
		v[i] = new vertex;
		v[i]->id = i;
		v[i]->p = 0;
		v[i]->v = *mesh.getVertices()[i];
	}

	// Create the faces, creating the half-edges and establishing vertex adjacency for
	//  assigning the sym pointers afterward
	f.resize(mesh.getFaces().size(), 0);
	vector<vector<pair<int, link*> > > adj(mesh.getVertices().size());
	for(int i=0; i < (int)mesh.getFaces().size(); ++i) {
		f[i] = new face;
		f[i]->id = i;
		
		link *p = new link;
		f[i]->p = p;

		Face &face = *mesh.getFaces()[i];
		for(int j=0; j < face.numVertices; ++j) {
			int k=(j+1)%face.numVertices;

			p->f = f[i];
			p->v = v[face.indices[k]];
			v[face.indices[k]]->p = p;
			if(k!=0)
				p->next = new link;
			else
				p->next = f[i]->p;//complete the cycle
			
			adj[face.indices[k]].push_back(make_pair(face.indices[j], p));
			adj[face.indices[j]].push_back(make_pair(face.indices[k], p));
			
			p = p->next;
		}
	}

	// Assign the sym pointers
	// O(N*x^2), where N is # vertices and x is the maximum # of vertices per face
	for(int i=0; i < (int)adj.size(); ++i) {
		for(int j=0; j < (int)adj[i].size(); ++j) {
			int cnt=0;
			for(int k=j+1; k < (int)adj[i].size(); ++k) {
				if(adj[i][j].first == adj[i][k].first) {
					adj[i][j].second->sym = adj[i][k].second;
					adj[i][k].second->sym = adj[i][j].second;
					++cnt;
				}
			}
			assert(cnt==1);
		}
	}
}

HalfEdge::HalfEdge(const HalfEdge &o) {
	*this = o;
}

HalfEdge& HalfEdge::operator= (const HalfEdge &o) {
	f.resize(o.f.size(), 0);
	v.resize(o.v.size(), 0);
	for(int i=0; i < (int)f.size(); ++i)
		f[i] = new face(*o.f[i]);
	for(int i=0; i < (int)v.size(); ++i)
		v[i] = new vertex(*o.v[i]);

	//TODO: finish

	return *this;
}

HalfEdge::~HalfEdge() {
	for(int i=0; i < (int)f.size(); ++i) {
		link *start=f[i]->p;
		link *cur = start;
		do {
			link *next = cur->next;
			delete cur;
			cur = next;
		} while(cur != start);
	}
	for(int i=0; i < (int)f.size(); ++i)
		delete f[i];
	for(int i=0; i < (int)v.size(); ++i)
		delete v[i];
}

Mesh HalfEdge::tofacelist() const {
	Mesh mesh;
	mesh.getVertices().resize(v.size(), 0);
	for(int i=0; i < (int)v.size(); ++i)
		mesh.getVertices()[i] = new Vertex(v[i]->v);

	mesh.getFaces().resize(f.size(), 0);
	for(int i=0; i < (int)f.size(); ++i) {
		vector<int> c;
		link *start = f[i]->p;
		link *cur = start;
		do {
			c.push_back(cur->v->id);
			cur = cur->next;
		} while(cur != start);
		mesh.getFaces()[i] = new Face(c.size());
		for(int j=0; j < (int)c.size(); ++j)
			mesh.getFaces()[i]->indices[j] = c[j];
	}

	return mesh;
}

void HalfEdge::subdivide(int iterations) {

}
