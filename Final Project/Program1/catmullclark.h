/**
 * Author: Zach J. Wheeler
 * 4/23/2014
 * Graphics Final Project
 * declarations for Catmull-Clark algorithm
 */


#pragma once


#include "Mesh.h"
#include "Vertex.h"

#include <map>
using std::map;
using std::pair;


/// Takes a mesh object and subdivides it
void catmullclark(Mesh &mesh, int iterations) {
	//Average the vertex normals
	typedef map<glm::vec4, pair<int, Vertex*> > MapType;
	MapType m;
	vector<int> to(mesh.getVertices().size());
	for(int i=0; i < (int)to.size(); ++i)
		to[i]=i;
	int cnt=0;
	for(int i=0; i < (int)mesh.getVertices().size(); ++i) {
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
	mesh.getVertices().clear();
	mesh.getVertices().resize(cnt);
	for(MapType::iterator it=m.begin(); it != m.end(); ++it) {
		mesh.getVertices()[it->second.first] = it->second.second;
	}
	for(int i=0; i < (int)mesh.getFaces().size(); ++i) {
		unsigned int *idx = mesh.getFaces()[i]->indices;
		for(int j=0; j < mesh.getFaces()[i]->numVertices; ++j)
			idx[j] = to[idx[j]];
	}


}