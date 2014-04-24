/**
 * Author: Zach J. Wheeler
 * 4/23/2014
 * Graphics Final Project
 * implement Catmull-Clark algorithm
 */


#include "catmullclark.h"
#include "Vertex.h"

#include <map>
using std::map;
using std::pair;

#include <algorithm>
using std::lexicographical_compare;

#include <vector>
using std::vector;

//because MSVC sucks and we need _finite() instead of the standard isfinite()
#include <float.h>

namespace {
	struct Comp {
		bool operator() (const glm::vec4 &a, const glm::vec4 &b) const {
			return lexicographical_compare((float*)&a, (float*)&a+4, (float*)&b, (float*)&b+4);
		}
	};

	void validateNormal(glm::vec4 normal) {
		assert(_finite(normal.x));
		assert(_finite(normal.y));
		assert(_finite(normal.z));
		assert(abs(normal.x) + abs(normal.y) + abs(normal.z) > 0);
		assert(normal.w == 0);
	}
}


void catmullclark(Mesh &mesh, int iterations) {
	//Average the vertex normals
	typedef map<glm::vec4, pair<int, Vertex*>, Comp> MapType;
	MapType m;
	vector<int> to(mesh.getVertices().size());
	for(int i=0; i < (int)to.size(); ++i)
		to[i]=i;
	int cnt=0;
	for(int i=0; i < (int)mesh.getVertices().size(); ++i) {
		assert(mesh.getVertices()[i]);
		Vertex &v = *mesh.getVertices()[i];
		validateNormal(v.normal);
		if(m.count(v.position)) {
			assert(m[v.position].second);
			m[v.position].second->normal += glm::normalize(v.normal);
			m[v.position].second->color += v.color;
		} else {
			m[v.position].second = new Vertex(v.position, v.color, glm::normalize(v.normal));
			m[v.position].first = cnt++;
		}
		to[i] = m[v.position].first;
	}
	vector<int> counts(to.size());
	for(int i=0; i < (int)counts.size(); ++i)
		++counts[to[i]];
	for(int i=0; i < (int)mesh.getVertices().size(); ++i)
		delete mesh.getVertices()[i];
	mesh.getVertices().clear();
	mesh.getVertices().resize(cnt, 0);
	for(MapType::iterator it=m.begin(); it != m.end(); ++it) {
		assert(0 <= it->second.first);
		assert(it->second.first < cnt);
		mesh.getVertices()[it->second.first] = it->second.second;
	}
	for(int i=0; i < cnt; ++i) {
		assert(mesh.getVertices()[i]);
		assert(counts[i]);
		mesh.getVertices()[i]->color /= counts[i];
		assert(glm::length(mesh.getVertices()[i]->normal) > 1e-12);
		mesh.getVertices()[i]->normal = glm::normalize(mesh.getVertices()[i]->normal);
	}
	for(int i=0; i < (int)mesh.getFaces().size(); ++i) {
		unsigned int *idx = mesh.getFaces()[i]->indices;
		for(int j=0; j < mesh.getFaces()[i]->numVertices; ++j)
			idx[j] = to[idx[j]];
	}


}