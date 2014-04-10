/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is the cpp file for MyGLWidget it contains the definitions for functions for the main widget used to control openGL
*/

#include "MyGLWidget.h"
#include "../glm/gtc/matrix_transform.hpp"

MyGLWidget::MyGLWidget(QWidget* parent) : QGLWidget(parent), camera(glm::vec4(0.0f, 0.0f, 10.0f,0.0f), glm::vec4(0.0f, 0.0f, 0.0f,0.0f), glm::vec4(0.0f, 1.0f, 0.0f,0.0f)) {
}

MyGLWidget::~MyGLWidget() {
	delete baseCube;
	delete chair;
	delete table;
	delete testMesh;
	delete testMesh2;
}

void MyGLWidget::initializeGL() {
	baseCube = new Cube();
	chair = new Chair(baseCube);
	table = new Table(baseCube);
	scene = new SceneGraph();

	testMesh = new Mesh("flashLight.txt");
	testMesh2 = new Mesh("surfrev1.dat");

	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vboc);
	glGenBuffers(1, &nbo);
	glGenBuffers(1, &ibo);

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgram = glCreateProgram();

	const char* vertexSource = textFileRead("lambert.vert");
	const char* fragmentSource = textFileRead("lambert.frag");

	//const char* vertexSource = textFileRead("super_simple.vert");
	//const char* fragmentSource = textFileRead("super_simple.frag");

	glShaderSource(vertexShader, 1, &vertexSource, 0);
	glShaderSource(fragmentShader, 1, &fragmentSource, 0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//also want to get the location of "vs_color"
	vLocation = glGetAttribLocation(shaderProgram, "vs_position");
	vColor = glGetAttribLocation(shaderProgram, "vs_color");
	vNormal = glGetAttribLocation(shaderProgram, "vs_normal");

	//Do something similar to this to get the location of "u_modelMatrix"
	u_projLocation = glGetUniformLocation(shaderProgram, "u_projMatrix");
	u_modelMatrix = glGetUniformLocation(shaderProgram, "u_modelMatrix");
	u_lightPos = glGetUniformLocation(shaderProgram, "u_lightPos");

	Geometry::setColorLoc(vColor);
	Geometry::setVertexLoc(vLocation);
	Geometry::setNormalLoc(vNormal);
	Geometry::setModelLoc(u_modelMatrix);
	Geometry::setCbo(vboc);
	Geometry::setVbo(vbo);
	Geometry::setNbo(nbo);
	Geometry::setIbo(ibo);

	baseCube->bufferData();
	//testMesh->bufferData();

	scene->setCube(baseCube);
	scene->setChair(chair);
	scene->setTable(table);

	scene->fillGraph("testScene2.txt");

	//unorthodox things going on with the y value appears to be opposite of what it should be
	lightLocation = glm::vec4((scene->getWidth()* scene->getFloorScale())/2,5,-(scene->getDepth()*(scene->getFloorScale())/2),1);

	glUseProgram(shaderProgram);
}

void MyGLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Start with identity matrix
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	//matrix used to position the camera to point at the center of the scene
	//glm::mat4 centerCamera = glm::translate(glm::mat4(1.0f),glm::vec3(-(scene->getWidth()*(scene->getFloorScale()))/2,0,(scene->getDepth()*(scene->getFloorScale())/2)));
	//modelMatrix = modelMatrix * centerCamera;

	modelMatrix = camera.getMatrix() * modelMatrix;

	//drawing box for light
	glm::mat4 lightBoxTranslate = glm::translate(glm::mat4(1.0f),glm::vec3(lightLocation.x,lightLocation.y,lightLocation.z));
	glm::mat4 lightBoxScale = glm::scale(glm::mat4(1.0f),glm::vec3(0.25,0.25,0.25));
	//baseCube->draw(modelMatrix * lightBoxTranslate * lightBoxScale, glm::vec3(1,1,1));

	glm::vec4 worldLightLocation = modelMatrix * lightLocation;
	glUniform4fv(u_lightPos,1,&worldLightLocation[0]);

	//scene->draw(modelMatrix);

	//drawMesh(testMesh,modelMatrix,glm::vec3(1,1,1));
	testMesh->draw(modelMatrix,glm::vec3(1,1,1));
	modelMatrix = modelMatrix * glm::translate(glm::mat4(1.0f),glm::vec3(2,0,0));
	testMesh2->draw(modelMatrix,glm::vec3(0.5,1,0.25));
	glFlush();
}

void MyGLWidget::generateGasket() {
	glm::vec2* points = new glm::vec2[500000];
	glm::vec3* colors = new glm::vec3[500000];

	glm::vec2 vertices[3] = {glm::vec2(-1.0, -1.0), glm::vec2(0.0, 1.0), glm::vec2(1.0, -1.0)};
	points[0] = glm::vec2(0.25, 0.50);
	colors[0] = glm::vec3(1.0,1.0,1.0);

	for(int i = 1; i < 500000; i++) {
		int k = rand() % 3;

		points[i] = glm::vec2((points[i-1].x + vertices[k].x) / 2.0, (points[i-1].y + vertices[k].y) / 2.0);
		colors[i] = glm::vec3(1.0,0.0,0.0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 500000 * sizeof(glm::vec2), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vLocation);
	glVertexAttribPointer(vLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboc);
	glBufferData(GL_ARRAY_BUFFER, 500000 * sizeof(glm::vec3), colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vColor);//new
	glVertexAttribPointer(vColor,3,GL_FLOAT,GL_FALSE,0,0);//new

	delete [] points;
	delete [] colors;
}

void MyGLWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
	
	//Here's how to make matrices for transformations, check the documentation of GLM for rotation, scaling, and translation
	glm::mat4 projection = glm::perspective(90.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 30.0f);
	//camera = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//Can multiply matrices together, careful about ordering!
	
	projection = projection * camera.getMatrix();

	//Do something similar for u_modelMatrix before rendering things
	glUniformMatrix4fv(u_projLocation, 1, GL_FALSE, &projection[0][0]);
}

//need to add code to get the indices form the mesh and draw by buffering to and using the index buffer
void MyGLWidget::drawMesh(Mesh* m, glm::mat4 modelMatrix, glm::vec3 color)
{
	int numPoints = m->getNumVertices();
	int indicesPerFace = m->getVerticesPerFace();
	int numFaces = m->getNumFaces();
	int numIndices = numFaces * indicesPerFace;

	glm::vec4* points = new glm::vec4[numPoints];
	glm::vec4* normals = new glm::vec4[numPoints];
	glm::vec3* colors = new glm::vec3[numPoints];
	//unsigned int* indices = new unsigned int[numIndices];

	unsigned int* indices = new unsigned int[numIndices];

	for(int i = 0; i < numPoints; i++)
	{
		points[i] = (m->getVertices())[i]->position;
		normals[i] = (m->getVertices())[i]->normal;
		colors[i] = color;
	}

	/*for(int i = 0; i < numPoints; i++)
	{
		indices[i] = i;
	}*/
	for(int i = 0; i < numFaces; i++)
	{
		for(int j = 0; j < indicesPerFace; j++)
		{
			indices[i*indicesPerFace+j] = (m->getFaces())[i]->indices[j];
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(glm::vec4), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vLocation);
	glVertexAttribPointer(vLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(glm::vec4),normals,GL_STATIC_DRAW);
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal,4,GL_FLOAT,GL_FALSE,0,0);

	glBindBuffer(GL_ARRAY_BUFFER, vboc);
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(glm::vec3),colors,GL_STATIC_DRAW);
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor,4,GL_FLOAT,GL_FALSE,0,0);

	glUniformMatrix4fv(u_modelMatrix, 1, GL_FALSE, &modelMatrix[0][0]);

	//set up the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//will need to do the other drawing that uses the index buffer
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_QUADS, 0, numPoints);

	delete points;
	delete normals;
	delete colors;
	delete indices;
}

//from swiftless.com
char* MyGLWidget::textFileRead(const char* fileName) {
    char* text;
    
    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");
        
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);
            
            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

//slots
void MyGLWidget::chooseScene(void)
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("Choose Scene"),"/home",tr("text Files (*.txt)"));
	delete scene;
	scene = new SceneGraph();
	scene->fillGraph(fileName.toStdString());
	this->update();
}

void MyGLWidget::rotateUp()
{
	double rotateValue = 5;
	camera.rotateVerticle(rotateValue);
	this->update();
}

void MyGLWidget::rotateDown()
{
	double rotateValue = -5;
	camera.rotateVerticle(rotateValue);
	this->update();
}

void MyGLWidget::rotateLeft()
{
	double rotateValue = 5;
	camera.rotateHorizontal(rotateValue);
	this->update();
}

void MyGLWidget::rotateRight()
{
	double rotateValue = -5;
	camera.rotateHorizontal(rotateValue);
	this->update();
}

void MyGLWidget::zoomIn()
{
	double zoomValue = 1;
	camera.zoom(zoomValue);
	this->update();
}

void MyGLWidget::zoomOut()
{
	double zoomValue = -1;
	camera.zoom(zoomValue);
	this->update();
}

void MyGLWidget::lightPlusX()
{
	lightLocation.x +=1;
	this->update();
}

void MyGLWidget::lightMinusX()
{
	lightLocation.x -=1;
	this->update();
}

void MyGLWidget::lightPlusY()
{
	lightLocation.y +=1;
	this->update();
}

void MyGLWidget::lightMinusY()
{
	lightLocation.y -=1;
	this->update();
}

void MyGLWidget::lightPlusZ()
{
	lightLocation.z +=1;
	this->update();
}

void MyGLWidget::lightMinusZ()
{
	lightLocation.z -=1;
	this->update();
}