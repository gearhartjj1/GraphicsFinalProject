/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is scenegraph.cpp it contains the implementation for the scene graph class
*/

#include "SceneGraph.h"

//set these once when creating the scene in the widget and then NEVER agin
Cube* SceneGraph::cube;
Chair* SceneGraph::chair;
Table* SceneGraph::table;

SceneGraph::SceneGraph()
{
	width = 0;
	depth = 0;
	numNodes = 0;
	geo = 0;
	transX = 0;
	transY = 0;
	transZ = 0;
	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;
	rotY = 0;
	floorScale = 1.2;
	selected = false;
	selectedIndex = 0;
	nextIndex = 0;
}

SceneGraph::~SceneGraph()
{
	for(int i = 0; i < meshes.size(); i++)
	{
		if(meshes[i])
		{
			delete meshes[i];
			meshes[i] = 0;
		}
	}
}

SceneGraph::SceneGraph(Geometry* g, int w, int d, int numN, glm::vec3 trans, glm::vec3 scale, float rot)
{
	geo = g;
	width = w;
	depth = d;
	numNodes = numN;
	transX = trans.x;
	transY = trans.y;
	transZ = trans.z;
	scaleX = scale.x;
	scaleY = scale.y;
	scaleZ = scale.z;
	rotY = rot;
	children = new SceneGraph*[width*depth];

	for(int i = 0; i < width * depth; i++)
	{
		children[i] = 0;
	}
	floorScale = 1.2;
	selected = false;
	nextIndex = 0;
	selectedIndex = 0;
}

void SceneGraph::traverse(glm::mat4 m) const
{
	glm::mat4 tr = glm::translate(glm::mat4(1.0f),glm::vec3(transX,transY,transZ));
	glm::mat4 sc = glm::scale(glm::mat4(1.0f),glm::vec3(scaleX,scaleY,scaleZ));
	glm::mat4 ro = glm::rotate(glm::mat4(1.0f),rotY,glm::vec3(0.0f,1.0f,0.0f));

	m = m * tr * sc * ro;
	if(geo)
	{
		if(selected)
		{
			geo->draw(m,geo->getSelectedColor());
		}
		else
		{
			geo->draw(m,geo->getColor());
		}
	}

	//in this program there would only be one child with the design, but in theory there could be width*depth children I suppose
	for(int i = 0; i < width * depth; i++)
	{
		if(children[i])
		{
			//children[i]->setTransY(children[i]->getTransY()*scaleY);
			children[i]->traverse(m);
		}
	}
}

void SceneGraph::draw(glm::mat4 m)
{
	traverse(m);
	
	//add floor
	//floor transforms
	glm::mat4 sc = glm::scale(glm::mat4(1.0f),glm::vec3(width*floorScale,0.1,depth*floorScale));
	//have to subtract one from the translation because the scene is zero indexed
	glm::mat4 flrTr = glm::translate(glm::mat4(1.0f),glm::vec3((width)/2+2,-0.1,-(depth)/2-1));
	cube->draw(m * flrTr *sc, glm::vec3(1,1,0));

	//walls
	sc = glm::scale(glm::mat4(1.0f),glm::vec3(width*floorScale,width/2,0.2));
	glm::mat4 tr = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,(width*floorScale)/2));
	glm::mat4 transform = m * flrTr * tr * sc;
	cube->draw(transform,glm::vec3(1,1,0));

	tr = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-(depth*floorScale)/2));
	transform = m * flrTr * tr * sc;
	cube->draw(transform,glm::vec3(1,1,0));

	sc = glm::scale(glm::mat4(1.0f),glm::vec3(0.2,width/2,depth*floorScale));
	tr = glm::translate(glm::mat4(1.0f),glm::vec3(-((width*floorScale)/2),0,0));
	cube->draw(m * flrTr * tr * sc,glm::vec3(1,1,0));

	tr = glm::translate(glm::mat4(1.0f),glm::vec3((width*floorScale)/2,0,0));
	transform = m * flrTr * tr * sc;
	cube->draw(transform,glm::vec3(1,1,0));
}

void SceneGraph::addChild(SceneGraph* sg, int x, int z)
{
	/*if(sg->geo)
	{
		sg->setTransY(sg->geo->getHeight());
	}*/

	if(children[x * depth + z])
	{
		children[x * depth + z]->addChild(sg);
	}
	else
	{
		sg->setTransX(x + 0.5f);
		sg->setTransZ(z + 0.5f);
		children[x * depth + z] = sg;
	}
}

void SceneGraph::addChild(SceneGraph* sg)
{
	if(this->geo)
	{
		sg->setTransY(this->geo->getHeight());
	}

	if(children[0])
	{
		children[0]->addChild(sg);
	}
	else
	{
		children[0] = sg;
	}
}

void SceneGraph::fillGraph(string inputName)
{
	numNodes = 0;
	ifstream fin;
	fin.open(inputName);

	fin >> width;
	fin >> depth;
	fin >> numNodes;

	children = new SceneGraph*[width*depth];
	for(int i = 0; i < width * depth; i++)
	{
		children[i] = 0;
	}

	string furnType = "";
	int xIndex = 0, zIndex = 0;
	float rotation = 0, xScale = 0, yScale = 0, zScale = 0;
	fin >> furnType;

	while(!fin.fail())
	{
		Geometry* g;

		if(furnType == "box")
		{
			g = cube;
		}
		else if(furnType == "chair")
		{
			g = chair;
		}
		else if(furnType == "table")
		{
			g = table;
		}
		else if(furnType == "mesh")
		{
			string fileName = "";
			fin >> fileName;
			//if it is a new mesh type make a new mesh
			int location;
			bool found = false;
			for(location = 0; location < fileNames.size(); location++)
			{
				if(fileNames[location]==fileName)
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				Mesh* m = new Mesh(fileName);
				fileNames.push_back(fileName);
				meshes.push_back(m);
				g = meshes[meshes.size()-1];
			}
			else
			{
				g = meshes[location];
			}
			int numDivides = 0;
			fin >> numDivides;
			//do something with subdivision
		}

		fin >> xIndex;
		fin >> zIndex;
		fin >> rotation;
		fin >> xScale >> yScale >> zScale;

		//puttin in zero for the translations and letting the addChild funtion take care of that
		SceneGraph* sgp = new SceneGraph(g,width,depth,numNodes,glm::vec3(0,0,0),glm::vec3(xScale,yScale,zScale),rotation);

		//numbers adjusted because the values input are zero indexed
		addChild(sgp,xIndex+1,-zIndex-1);
		
		fin >> furnType;
	}

	fin.close();
}

SceneGraph* SceneGraph::getNextNode(SceneGraph* current)
{
	if(getNumNodes()<=0)
	{
		return 0;
	}

	//if there is already a node selected and it has children then traverse the children
	if(current && current->children[0])
	{
		current->setSelected(false);
		current->children[0]->setSelected(true);
		return current->children[0];
	}

	for(int i = nextIndex; i < width * depth; i++)
	{
		if(children[i])
		{
			if(current)
				current->setSelected(false);
			children[i]->setSelected(true);
			selectedIndex = i;
			nextIndex = i+1;
			return children[i];
		}
	}
	nextIndex = 0;
	return getNextNode(current);

}