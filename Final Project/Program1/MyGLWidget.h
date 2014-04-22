/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is the header file for the MyGLWidget it has the declaration for the main widget that controls most things
*/

#pragma once
#define GLEW_STATIC
#include "glew.h"
#include "../glm/glm.hpp"
#include <QGLWidget>
//library for open file dialog
#include <qfiledialog.h>
#include <stdlib.h>
#include <fstream>
using std::ofstream;
#include <sstream>
using std::stringstream;
#include "SceneGraph.h"
#include "Cube.h"
#include "Chair.h"
#include "Table.h"
#include "camera.h"
#include "Mesh.h"

class MyGLWidget : public QGLWidget
{
	Q_OBJECT
public:
	MyGLWidget(QWidget*);
	~MyGLWidget(void);

	void initializeGL(void);
	void paintGL(void);
	void resizeGL(int, int);

	void generateGasket();

	void drawMesh(Mesh* m, glm::mat4 modelMatrix, glm::vec3 color);
	//void drawCube();
	//void drawChair();
	//void drawTable();
	//void drawGraph();

	//helper function to read shader source and put it in a char array
	//thanks to Swiftless.com
	char* textFileRead(const char*);

private:
	void setTextBoxes();
	Cube* baseCube;
	Chair* chair;
	Table* table;

	SceneGraph* scene;
	SceneGraph* editNode;
	Camera camera;

	Mesh* testMesh;
	Mesh* testMesh2;

	//location of the lightsource, perhaps put in different class later
	glm::vec4 lightLocation;

	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;

	unsigned int vbo;
	unsigned int vboc;
	unsigned int nbo;
	unsigned int ibo;
	unsigned int vLocation;
	unsigned int vColor;
	unsigned int vNormal;

	unsigned int u_projLocation;
	unsigned int u_modelMatrix;
	unsigned int u_lightPos;

public slots:
	//slots for taking new values for edit node
	void setNodeTransX(QString x);
	void setNodeTransY(QString y);
	void setNodeTransZ(QString z);
	void setNodeScaleX(QString sX);
	void setNodeScaleY(QString sY);
	void setNodeScaleZ(QString sZ);
	void setNodeRotation(QString r);

	void selectNode();
	void selectPrevNode();
	void unSelectNode();
	void chooseScene(void);
	void rotateUp();
	void rotateDown();
	void rotateLeft();
	void rotateRight();
	void zoomIn();
	void zoomOut();
	void lightPlusX();
	void lightMinusX();
	void lightPlusY();
	void lightMinusY();
	void lightPlusZ();
	void lightMinusZ();

signals:
	void sendTransX(QString t);
	void sendTransY(QString t);
	void sendTransZ(QString t);
	void sendScaleX(QString s);
	void sendScaleY(QString s);
	void sendScaleZ(QString s);
	void sendRotation(QString r);
};