/*
Author: Jacob Gearhart
Course: COMP 361, Computer Graphics
Date: February 10, 2014
Description: this is the cpp file for MyGLWidget it contains the definitions for functions for the main widget used to control openGL
*/

#include "MyGLWidget.h"
#include "../glm/gtc/matrix_transform.hpp"
#include "catmullclark.h"
#include "EasyBMP.h"

MyGLWidget::MyGLWidget(QWidget* parent) : QGLWidget(parent), camera(glm::vec4(0.0f,0.0f,0.0f,0.0f), glm::vec4(0.0f, 0.0f, 10.0f,0.0f), glm::vec4(0.0f, 1.0f, 0.0f,0.0f)) {
	editNode = 0;
}

MyGLWidget::~MyGLWidget() {
	delete baseCube;
	delete chair;
	delete table;
	delete sphere;
	//delete testMesh;
	//delete testMesh2;
}

void MyGLWidget::initializeGL() {
	baseCube = new Cube();
	chair = new Chair(baseCube);
	table = new Table(baseCube);
	sphere = new Sphere;
	scene = new SceneGraph();

	//testMesh = new Mesh("extrusion1.dat");
	//testMesh2 = new Mesh("surfrev1.dat");

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
	//testMesh->bufferData(glm::vec3(1,1,1));;

	scene->setCube(baseCube);
	scene->setChair(chair);
	scene->setTable(table);
	scene->setSphere(sphere);

	scene->fillGraph("sample1.txt");

	lightLocation = glm::vec4((scene->getWidth()* scene->getFloorScale())/2,5,-(scene->getDepth()*(scene->getFloorScale())/2),1);

	glUseProgram(shaderProgram);
}

void MyGLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Start with identity matrix
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	//matrix used to position the camera to point at the center of the scene
	glm::mat4 centerCamera = glm::translate(glm::mat4(1.0f),glm::vec3(-(scene->getWidth()*(scene->getFloorScale()))/2,0,(scene->getDepth()*(scene->getFloorScale())/2)));
	modelMatrix = modelMatrix * centerCamera;
	//modelMatrix = modelMatrix * glm::scale(glm::mat4(1.0f),glm::vec3(4,4,4));
	//camera.setRef(glm::vec4((scene->getWidth()*(scene->getFloorScale()))/2,0,-(scene->getDepth()*(scene->getFloorScale())/2),0));

	modelMatrix = camera.getMatrix() * modelMatrix;

	//drawing box for light
	glm::mat4 lightBoxTranslate = glm::translate(glm::mat4(1.0f),glm::vec3(lightLocation.x,lightLocation.y,lightLocation.z));
	glm::mat4 lightBoxScale = glm::scale(glm::mat4(1.0f),glm::vec3(0.25,0.25,0.25));
	baseCube->draw(modelMatrix * lightBoxTranslate * lightBoxScale, glm::vec3(1,1,1));

	worldLightLocation = modelMatrix * lightLocation;
	glUniform4fv(u_lightPos,1,&worldLightLocation[0]);

	scene->draw(modelMatrix);

	//drawMesh(testMesh,modelMatrix,glm::vec3(1,1,1));
	//testMesh->draw(modelMatrix,glm::vec3(1,1,1));
	//modelMatrix = modelMatrix * glm::translate(glm::mat4(1.0f),glm::vec3(2,0,0));
	//testMesh2->draw(modelMatrix,glm::vec3(0.5,1,0.25));
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
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	perspectiveAngle = 90.0f;
	glm::mat4 projection = glm::perspective(perspectiveAngle, aspectRatio, 0.1f, 30.0f);
	//camera = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//Can multiply matrices together, careful about ordering!
	
	//projection = projection * camera.getMatrix();

	//Do something similar for u_modelMatrix before rendering things
	glUniformMatrix4fv(u_projLocation, 1, GL_FALSE, &projection[0][0]);
	//rayTrace("test.bmp",800,600);
}

//loop through generating all rays and then for each ray raytrace the scene and get the color and make a bmp from it
void MyGLWidget::rayTrace(string imageName, int width, int height)
{
	//set up bit map stuff
	BMP output;
	output.SetSize(width, height);
	output.SetBitDepth(24);
	
	//verify that these are correct
	/*glm::vec3 C = glm::normalize(glm::vec3(camera.getRef()-camera.getPos()));
	glm::vec3 M = glm::vec3(camera.getPos()) + C*0.1f;
	glm::vec3 up(camera.getUpV());
	up = glm::cross(up, C);
	up = glm::normalize(glm::cross(C, up));
	glm::vec3 V = .1f*tan(perspectiveAngle/2)*up;
	//V = glm::normalize(V);
	//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),90.0f,glm::vec3(C));
	//glm::vec4 rotV = rotation * glm::vec4(V,1);
	glm::vec3 H = glm::length(V) * glm::normalize(glm::cross(C, up));
	H*=aspectRatio;
	glm::vec3 E(camera.getPos());
	//H = glm::normalize(H);*/

	glm::vec3 V = .1f*tan(perspectiveAngle*.5f)*glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 H = float(width)/float(height) * glm::length(V) * glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 M = .1f*glm::vec3(0.f, 0.f, -1.f);
	glm::vec3 E(0.f);

	//glm::vec3 E(0.f), M(0.f,0.f,1.f), H(1.f*float(width)/float(height),0.f,0.f), V(0.f,1.f,0.f);

	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			//calculate direction of the ray
			glm::vec3 P = M + (2*x/float(width-1)-1)*H - (2*y/float(height-1)-1)*V;
			glm::vec3 D = glm::normalize(P-E);
			glm::vec3 color = glm::vec3(0,0,0);
			if(scene->rayTrace(P,D,color,worldLightLocation))
			{
				int stuff = 0;
			}
			//put the resulting color in the bmp
			int r = glm::clamp(int(abs(color.r)*256), 0, 255);
			int g = glm::clamp(int(abs(color.g)*256), 0, 255);
			int b = glm::clamp(int(abs(color.b)*256), 0, 255);

			output(x,y)->Red = r;
			output(x,y)->Green = g;
			output(x,y)->Blue = b;
		}
	}

	output.WriteToFile(imageName.c_str());
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

void MyGLWidget::setTextBoxes()
{
	stringstream s;
	if(!editNode)
		return;

	s << editNode->getTransX();
	emit sendTransX(QString::fromStdString(s.str()));
	s.str("");

	s << editNode->getTransY();
	emit sendTransY(QString::fromStdString(s.str()));
	s.str("");

	s << editNode->getTransZ();
	emit sendTransZ(QString::fromStdString(s.str()));
	s.str("");

	s << editNode->getScaleX();
	emit sendScaleX(QString::fromStdString(s.str()));
	s.str("");

	s << editNode->getScaleY();
	emit sendScaleY(QString::fromStdString(s.str()));
	s.str("");

	s << editNode->getScaleZ();
	emit sendScaleZ(QString::fromStdString(s.str()));
	s.str("");

	s << editNode->getRotY();
	emit sendRotation(QString::fromStdString(s.str()));
	s.str("");

	this->update();
}

void MyGLWidget::selectPrevNode()
{
	editNode = scene->getPreviousNode(editNode);
	setTextBoxes();
}

void MyGLWidget::selectNode()
{
	editNode = scene->getNextNode(editNode);
	setTextBoxes();
}

void MyGLWidget::unSelectNode()
{
	if(!editNode)
		return;
	editNode->setSelected(false);
	//editNode = 0;
	
	emit sendTransX("");
	emit sendTransY("");
	emit sendTransZ("");
	emit sendScaleX("");
	emit sendScaleY("");
	emit sendScaleZ("");
	emit sendRotation("");

	this->update();
}

void MyGLWidget::setNodeTransX(QString x)
{
	if(!editNode)
	{
		return;
	}

	double transX = editNode->getTransX();
	transX = atof(x.toStdString().c_str());
	editNode->setTransX(transX);

	stringstream s;
	s << editNode->getTransX();
	emit sendTransX(QString::fromStdString(s.str()));
	s.str("");
	this->update();
}

void MyGLWidget::setNodeTransY(QString y)
{
	if(!editNode)
	{
		return;
	}

	double transY = editNode->getTransY();
	transY = atof(y.toStdString().c_str());
	editNode->setTransY(transY);

	stringstream s;
	s << editNode->getTransY();
	emit sendTransY(QString::fromStdString(s.str()));
	s.str("");
	this->update();
}

void MyGLWidget::setNodeTransZ(QString z)
{
	if(!editNode)
	{
		return;
	}

	double transZ = editNode->getTransZ();
	transZ = atof(z.toStdString().c_str());
	editNode->setTransZ(transZ);

	stringstream s;
	s << editNode->getTransZ();
	emit sendTransZ(QString::fromStdString(s.str()));
	s.str("");
	this->update();
}

void MyGLWidget::setNodeScaleX(QString sX)
{
	if(!editNode)
	{
		return;
	}

	double scaleX = editNode->getScaleX();
	scaleX = atof(sX.toStdString().c_str());
	editNode->setScaleX(scaleX);

	stringstream s;
	s << editNode->getScaleX();
	emit sendScaleX(QString::fromStdString(s.str()));
	s.str("");
	this->update();
}

void MyGLWidget::setNodeScaleY(QString sY)
{
	if(!editNode)
	{
		return;
	}

	double scaleY = editNode->getScaleY();
	scaleY = atof(sY.toStdString().c_str());
	editNode->setScaleY(scaleY);

	stringstream s;
	s << editNode->getScaleY();
	emit sendScaleY(QString::fromStdString(s.str()));
	s.str("");
	this->update();
}

void MyGLWidget::setNodeScaleZ(QString sZ)
{
	if(!editNode)
	{
		return;
	}

	double scaleZ = editNode->getScaleZ();
	scaleZ = atof(sZ.toStdString().c_str());
	editNode->setScaleZ(scaleZ);

	stringstream s;
	s << editNode->getScaleZ();
	emit sendScaleZ(QString::fromStdString(s.str()));
	s.str("");
	this->update();
}

void MyGLWidget::setNodeRotation(QString r)
{
	if(!editNode)
	{
		return;
	}

	double rot = editNode->getRotY();
	rot = atof(r.toStdString().c_str());
	editNode->setRotY(rot);

	stringstream s;
	s << editNode->getRotY();
	emit sendRotation(QString::fromStdString(s.str()));
	s.str("");
	this->update();
}

void MyGLWidget::subDivide()
{
	//can check whether is meshb
	if(editNode && editNode->getGeometry() && editNode->getGeometry()->getIsMesh()) {
		Mesh *mesh = dynamic_cast<Mesh*>(editNode->getGeometry());
		if(mesh) {
			Mesh *newmesh = new Mesh(*mesh);
			catmullclark(*newmesh, 1);
			editNode->setGeometry(newmesh);//this could cause a leak, but I don't care.
			update();
		}
	}
}

void MyGLWidget::runRayTrace()
{
	rayTrace("test.bmp",800,600);
}