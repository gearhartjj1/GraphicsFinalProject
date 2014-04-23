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

namespace {
	struct Comp {
		bool operator() (const glm::vec4 &a, const glm::vec4 &b) const {
			return lexicographical_compare((float*)&a, (float*)&a+4, (float*)&b, (float*)&b+4);
		}
	};
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
		if(m.count(v.position)) {
			m[v.position].second->normal += glm::normalize(v.normal);
			m[v.position].second->color += glm::normalize(v.color);
			to[i] = m[v.position].first;
		} else {
			m[v.position].second = new Vertex(v.position, glm::normalize(v.color), glm::normalize(v.normal));
			m[v.position].first = cnt++;
		}
	}
	for(int i=0; i < (int)mesh.getVertices().size(); ++i)
		delete mesh.getVertices()[i];
	mesh.getVertices().clear();
	mesh.getVertices().resize(cnt);
	for(MapType::iterator it=m.begin(); it != m.end(); ++it) {
		mesh.getVertices()[it->second.first] = it->second.second;
	}
	for(int i=0; i < cnt; ++i) {
		mesh.getVertices()[i]->color = glm::normalize(mesh.getVertices()[i]->color);
		mesh.getVertices()[i]->normal = glm::normalize(mesh.getVertices()[i]->normal);
	}
	for(int i=0; i < (int)mesh.getFaces().size(); ++i) {
		unsigned int *idx = mesh.getFaces()[i]->indices;
		for(int j=0; j < mesh.getFaces()[i]->numVertices; ++j)
			idx[j] = to[idx[j]];
	}


}