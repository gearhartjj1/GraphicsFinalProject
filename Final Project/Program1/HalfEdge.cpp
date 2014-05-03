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

#include "glm//glm.hpp"

using namespace glm;


namespace { const float EPS = 1e-12; }


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

			p->sym = 0;
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
		assert(adj[i].size() % 2 == 0);
		vector<bool> hit(adj[i].size(), false);
		for(int j=0; j < (int)adj[i].size(); ++j) {
			if(hit[j]) continue;
			hit[j]=true;
			int cnt=0;
			for(int k=j+1; k < (int)adj[i].size(); ++k) {
				if(adj[i][j].first == adj[i][k].first) {
					assert(adj[i][j].second != adj[i][k].second);
					assert(adj[i][j].second->v != adj[i][k].second->v);
					adj[i][j].second->sym = adj[i][k].second;
					adj[i][k].second->sym = adj[i][j].second;
					hit[k]=true;
					++cnt;
				}
			}
			assert(cnt!=0);
			assert(cnt==1);
		}
	}

	if(!checkvertexcircling())
		assert(false && "failed vertex circling");
	if(!checkfacepointers())
		assert(false && "failed face pointers");
}

HalfEdge::HalfEdge(const HalfEdge &o) {
	docopy(o, true);
}

// O(N+M*x), where N is number of vertices, M is number of faces, 
//  and x is max number of vertices per face
HalfEdge& HalfEdge::operator= (const HalfEdge &o) {
	docopy(o, false);
	return *this;
}

void HalfEdge::docopy(const HalfEdge &o, bool cc) {
	if(!cc)
		clear();

	f.resize(o.f.size(), 0);
	v.resize(o.v.size(), 0);
	for(int i=0; i < (int)f.size(); ++i) {
		f[i] = new face(*o.f[i]);
		f[i]->p = 0;
	}
	for(int i=0; i < (int)v.size(); ++i) {
		v[i] = new vertex(*o.v[i]);
		v[i]->p = 0;
	}

	// Think prokaryotic fission when you read the following code

	// What's really interesting about this is that you have to 
	//  modify the original object in order to copy it.
	// Then you just change it back once you're done.

	// zigzag the next pointers with new links!
	for(int i=0; i < (int)f.size(); ++i) {
		link *start = o.f[i]->p;
		link *cur = start;
		do {
			link *next = cur->next;
			cur->next = new link;
			cur->next->next = next;
			cur = next;
		} while(cur != start);
	}
	// now line everything up and split them apart!
	for(int i=0; i < (int)f.size(); ++i) {
		link *start = o.f[i]->p;
		link *cur = start;
		do {
			link *cp = cur->next;
			link *next = cp->next;
			cp->sym = cur->sym->next;
			cp->next = next->next;
			cp->v = v[cur->v->id];
			cp->f = f[cur->f->id];
			
			v[cur->v->id]->p = cp;
			f[cur->f->id]->p = cp;

			cur->next = next;//repair what we copied from
			cur = next;
		} while(cur != start);
	}
}

HalfEdge::~HalfEdge() {
	clear();
}

void HalfEdge::clear() {
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
	f.clear();
	v.clear();
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

	mesh.setFilled(true);
	return mesh;
}

void HalfEdge::subdivide(int iterations) {
	// And now for the Catmull-Clark algorithm, finally

	// Flag newly created vertices with negative ids

	for(int iter=0; iter < iterations; ++iter) {
		
		// Add face vertices
		vector<vertex*> facev;
		for(int i=0; i < (int)f.size(); ++i) {
			vec4 pos(0.f);
			link *start = f[i]->p;
			link *cur = start;
			int cnt=0;
			do {
				pos += cur->v->v.position;
				++cnt;
				cur = cur->next;
			} while(cur != start);
			pos /= float(cnt);
			pos.w = 1;

			vertex *v = new vertex;
			v->id = -int(facev.size())-1;
			v->v.position = pos;
			v->v.color = f[i]->p->v->v.color;//for now; could also average colors
			v->p = 0;
			facev.push_back(v);
		}

		// Add edge vertices
		vector<vertex*> edgev;
		for(int i=0; i < (int)f.size(); ++i) {
			link *start = f[i]->p;
			link *cur = start;
			do {
				vec4 pos = facev[cur->f->id]->v.position + facev[cur->sym->f->id]->v.position;
				pos += cur->v->v.position + cur->sym->v->v.position;
				pos /= 4.f;
				pos.w = 1;

				link *next = cur->next;

				if(cur->sym->v->id < 0) {
					vertex *v = cur->sym->v;

					link *add = new link;
					cur->next = add;
					add->next = next;
					add->sym = cur->sym;
					add->sym->sym = add;
					add->v = cur->v;
					cur->v = v;
					add->v->p = add;
					cur->v->p = cur;
					cur->sym = add->sym->next;
					assert(cur->sym->sym == 0);
					cur->sym->sym = cur;
					add->f = cur->f;
				} else {
					vertex *v = new vertex;
					v->id = -(int)edgev.size()-1;
					v->v.position = pos;
					v->v.color = cur->v->v.color;//for now; could also average colors
					v->p = 0;
					edgev.push_back(v);

					link *add = new link;
					cur->next = add;
					add->next = next;
					add->sym = 0;
					add->v = cur->v;
					cur->v = v;
					add->v->p = add;
					cur->v->p = cur;//(link*)0xc001;//cur
					add->f = cur->f;
				}

				cur = next;
			} while(cur != start);
		}

		if(!checkwindingorder())
			assert(false && "check winding order failed");

		// Smooth the original vertices
		for(int i=0; i < (int)v.size(); ++i) {
			vertex *x = v[i];
			vec4 sum(0.f);
			int cnt=0;
			link *start = x->p;
			link *cur = start;
			do {
				assert(cur->sym->v != x);
				assert(cur->v == x);
				sum += cur->sym->v->v.position;
				assert(cur->f->id >= 0);
				sum += facev[cur->f->id]->v.position;
				++cnt;
				cur = cur->next->sym;
			} while(cur != start);

			if(cnt >= 3) {
				x->v.position *= float(cnt-2)/float(cnt);
				sum.w = 0;
				x->v.position += sum / float(cnt*cnt);
				x->v.position.w = 1;
			}
		}

		// Divide the faces
		{
			// Add new links
			for(int i=0; i < (int)f.size(); ++i) {
				link *start = f[i]->p;
				link *cur = start;
				do {
					link *next = cur->next->next;

					link *a = new link;
					link *b = new link;
					a->sym = b;
					b->sym = a;
					a->v = facev[i];//(vertex*)0xa;//facev[i]
					b->v = cur->v;//(vertex*)0xb;//cur->v
					facev[i]->p = a;
					b->next = cur->next;
					cur->next = a;
					a->next = 0;

					cur = next;
				} while(cur != start);
			}

			// Connect up the new links in a cycle
			for(int i=0; i < (int)f.size(); ++i) {
				link *start = f[i]->p;
				link *cur = start;
				do {
					link *b = cur->next->sym;
					link *a = b->next->next->next;
					a->next = b;
					cur = b->next->next;
				} while(cur != start);
			}

			// Add new faces
			int n=f.size();
			for(int i=0; i < n; ++i) {
				f[i]->p = facev[i]->p;
				link *start = f[i]->p;//start is an 'a' -- ie, a link that points to facev[i]
				link *cur = start->next->sym;
				// skip the first one; that is what f[i] will become
				while(cur != start) {
					face *add = new face;
					add->id = f.size();
					add->p = cur;
					f.push_back(add);
					cur = cur->next->sym;
				}
			}

			// Fix face pointers
			for(int i=0; i < (int)f.size(); ++i) {
				link *start = f[i]->p;
				link *cur = start;
				do {
					cur->f = f[i];
					cur = cur->next;
				} while(cur != start);
			}
		}

		// Add new vertices to mesh
		for(int i=0; i < (int)facev.size(); ++i) {
			facev[i]->id = v.size();
			v.push_back(facev[i]);
		}
		for(int i=0; i < (int)edgev.size(); ++i) {
			edgev[i]->id = v.size();
			v.push_back(edgev[i]);
		}

		if(!checkquads())
			assert(false && "failed check quads");
	}

	updatenormals();
}


void HalfEdge::updatenormals() {
	for(int i=0; i < (int)v.size(); ++i)
		v[i]->v.normal = vec4(0.f);

	for(int i=0; i < (int)f.size(); ++i) {
		link *start = f[i]->p;
		link *cur = start;

		const vec4 &a = cur->v->v.position;
		const vec4 &b = cur->next->v->v.position;
		vec4 normal;

		cur = cur->next->next;
		do {
			const vec4 &c = cur->v->v.position;
			normal = vec4(cross(vec3(c-a), vec3(b-a)), 0);// Jake's facelists are CW
			if(length(normal) >= EPS) {
				normal = normalize(normal);
				break;
			}
			cur = cur->next;
		} while(cur != start);
		assert(cur != start);

		cur = start;
		do {
			cur->v->v.normal += normal;
			cur = cur->next;
		} while(cur != start);
	}

	for(int i=0; i < (int)v.size(); ++i)
		v[i]->v.normal = normalize(v[i]->v.normal);
}


bool HalfEdge::checkwindingorder() const {
	// Verify that every polygon has the same winding order (assuming everything else is correct)
	for(int i=0; i < (int)f.size(); ++i) {
		link *start = f[i]->p;
		link *cur = start;
		do {
			if(cur->v == cur->sym->v) return false;
			cur = cur->next;
		} while(cur != start);
	}
	return true;
}

bool HalfEdge::checkvertexcircling() const {
	// Verify that circling around a vertex works correctly
	for(int i=0; i < (int)v.size(); ++i) {
		vertex *x = v[i];
		link *start = x->p;
		link *cur = start;
		do {
			if(cur->sym->v == x || cur->v != x) return false;
			cur = cur->next->sym;
		} while(cur != start);
	}
	return true;
}

bool HalfEdge::checkfacepointers() const {
	for(int i=0; i < (int)f.size(); ++i) {
		link *start = f[i]->p;
		link *cur = start;
		do {
			if((unsigned)cur->f == 0xcdcdcdcd || cur->f == 0) return false;
			cur=cur->next;
		} while(cur != start);
	}
	return true;
}

bool HalfEdge::checkquads() const {
	for(int i=0; i < (int)f.size(); ++i) {
		link *start = f[i]->p;
		link *cur = start;
		int cnt=0;
		do {
			++cnt;
			cur = cur->next;
		} while(cur != start);
		if(cnt != 4) return false;
	}
	return true;
}
