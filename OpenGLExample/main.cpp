// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#include "Random.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <math.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// specify that we want the OpenGL core profile before including GLFW headers
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "boid.h"


#define PI 3.14159265359

using namespace std;
using namespace glm;

//Forward definitions
bool CheckGLErrors(string location);
void QueryGLVersion();
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);


vec2 mousePos;
bool leftmousePressed = false;
bool rightmousePressed = false;
bool play = false;
Camera* activeCamera;


GLFWwindow* window = 0;

mat4 winRatio = mat4(1.f);

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}

// handles keyboard input events
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
			if(!play)
				play = true;
			else
				play = false;
		}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if( (action == GLFW_PRESS) || (action == GLFW_RELEASE) ){
		if(button == GLFW_MOUSE_BUTTON_LEFT)
			leftmousePressed = !leftmousePressed;
		else if(button == GLFW_MOUSE_BUTTON_RIGHT)
			rightmousePressed = !rightmousePressed;
	}
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);

	vec2 diff = newPos - mousePos;
	if(leftmousePressed){
		activeCamera->trackballRight(-diff.x);
		activeCamera->trackballUp(-diff.y);
	}
	else if(rightmousePressed){
		float zoomBase = (diff.y > 0) ? 1.f/2.f : 2.f;

		activeCamera->zoom(pow(zoomBase, abs(diff.y)));
	}

	mousePos = newPos;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	glViewport(0, 0, width, height);

	float minDim = float(std::min(width, height));

	winRatio[0][0] = minDim/float(width);
	winRatio[1][1] = minDim/float(height);
}

void printVec3(vec3 toPrint)
{
	cout << "X: " << toPrint.x << endl;
	cout << "Y: " << toPrint.y << endl;
	cout << "Z: " << toPrint.z << endl;
}

void readFile(vector<vec3>* filePoints, vector<vec3>* fileNormals, vector<unsigned int>* fileInds, char* filename)
{
	ifstream myFile;
	
	float x,y,z;
	vec3 input;
	unsigned int ind;
	char nextChar;

	myFile.open(filename);
	if(myFile.is_open())
	{
		myFile >> nextChar;
		cout << nextChar;
		while(!myFile.eof())
		{
			
			if(nextChar == 'V')
			{
				while(!myFile.eof() && nextChar == 'V')
				{
					myFile >> x >> y >> z;
					filePoints->push_back(vec3(x,y,z));
					myFile >> nextChar;
				}
			}
			else if (nextChar == 'N')
			{
				while(!myFile.eof() && nextChar == 'N')
				{
					myFile >> x >> y >> z;
					fileNormals->push_back(vec3(x,y,z));
					myFile >> nextChar;
				}
			}
			else if (nextChar == 'I')
			{
				while(!myFile.eof() && nextChar == 'I' )
				{
					myFile >> ind;
					fileInds->push_back(ind);
					myFile >> nextChar;
				}
			}
		}	
	}
	else
	{
		cout << "File not opened" << endl;
	}
	
	myFile.close();
	
}


//==========================================================================
// TUTORIAL STUFF


//vec2 and vec3 are part of the glm math library. 
//Include in your own project by putting the glm directory in your project, 
//and including glm/glm.hpp as I have at the top of the file.
//"using namespace glm;" will allow you to avoid writing everyting as glm::vec2

struct VertexBuffers{
	enum{ VERTICES=0, NORMALS, INDICES, COUNT};

	GLuint id[COUNT];
};
void deleteStuff(GLuint vao, VertexBuffers vbo, GLuint program);
//Describe the setup of the Vertex Array Object
bool initVAO(GLuint vao, const VertexBuffers& vbo)
{
	glBindVertexArray(vao);		//Set the active Vertex Array

	glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
	glBindBuffer( GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES] );		//Set the active Vertex Buffer
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::NORMALS]);
	glVertexAttribPointer(
		1,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.id[VertexBuffers::INDICES]);

	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}


//Loads buffers with data
bool loadBuffer(const VertexBuffers& vbo, 
				const vector<vec3>& points, 
				const vector<vec3> normals, 
				const vector<unsigned int>& indices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*points.size(),		//Size of data in array (in bytes)
		&points[0],						//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
		);

	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::NORMALS]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*normals.size(),	//Size of data in array (in bytes)
		&normals[0],					//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.id[VertexBuffers::INDICES]);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*indices.size(),
		&indices[0],
		GL_STATIC_DRAW
		);

	return !CheckGLErrors("loadBuffer");	
}

//Compile and link shaders, storing the program ID in shader array
GLuint initShader(string vertexName, string fragmentName)
{	
	string vertexSource = LoadSource(vertexName);		//Put vertex file text into string
	string fragmentSource = LoadSource(fragmentName);		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	
	return LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array
}

//Initialization
void initGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
}

bool loadUniforms(GLuint program, mat4 perspective, mat4 modelview)
{
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelviewMatrix"),
						1,
						false,
						&modelview[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "perspectiveMatrix"),
						1,
						false,
						&perspective[0][0]);

	return !CheckGLErrors("loadUniforms");
}

//Draws buffers to screen
void render(GLuint vao, int startElement, int numElements, GLuint program, VertexBuffers vbo, vector<vec3> points, vector<vec3>normals, vector<unsigned int> indices)
{
	

	glBindVertexArray(vao);		//Use the LINES vertex array
	glUseProgram(program);
	loadBuffer(vbo, points, normals, indices);
	
	glDrawElements(
			GL_TRIANGLES,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
			numElements,		//How many indices
			GL_UNSIGNED_INT,	//Type
			(void*)0			//Offset
			);
	
	CheckGLErrors("render");
	glUseProgram(0);
	glBindVertexArray(0);

}


void generateSquare(vector<vec3>* vertices, vector<vec3>* normals, 
					vector<unsigned int>* indices, float width)
{
	vertices->push_back(vec3(-width*0.5f, -width*0.5f, 0.f));
	vertices->push_back(vec3(width*0.5f, -width*0.5f, 0.f));
	vertices->push_back(vec3(width*0.5f, width*0.5f, 0.f));
	vertices->push_back(vec3(-width*0.5f, width*0.5f, 0.f));

	normals->push_back(vec3(0.f, 0.f, 1.f));
	normals->push_back(vec3(0.f, 0.f, 1.f));
	normals->push_back(vec3(0.f, 0.f, 1.f));
	normals->push_back(vec3(0.f, 0.f, 1.f));

	//First triangle
	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);
	//Second triangle
	indices->push_back(2);
	indices->push_back(3);
	indices->push_back(0);
}

GLFWwindow* createGLFWWindow()
{
	// initialize the GLFW windowing system
    if (!glfwInit()) {
        cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
        return NULL;
    }
    glfwSetErrorCallback(ErrorCallback);

    // attempt to create a window with an OpenGL 4.1 core profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  //  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1024, 1024, "OpenGL Example", 0, 0);
    if (!window) {
        cout << "Program failed to create GLFW window, TERMINATING" << endl;
        glfwTerminate();
        return NULL;
    }

    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwMakeContextCurrent(window);

    return window;
}



// ==========================================================================
// PROGRAM ENTRY POINT
//TODO Figure out how to draw triangles that are the same size based on a single point right now it draws triangles just werid looking ones
void drawBoids(vector<Boid> allBoids, vector<vec3>* boidsToDraw, vector<vec3>* boidNorms, vector<unsigned int>* boidInds)
{

	vec3 currBoid;
	vec3 prevBoid;
	int currIndNum = 0;
	//cout << "NUMBER OF BOIDS: "<< allBoids.size() << endl;
	for(int i = 0; i < allBoids.size(); i++)
	{
		
		currBoid = allBoids[i].getPos().p1;
		boidsToDraw->push_back(currBoid);
		currBoid = allBoids[i].getPos().p2;
		boidsToDraw->push_back(currBoid);
		currBoid = allBoids[i].getPos().p3;
		boidsToDraw->push_back(currBoid);
		
		
		boidNorms->push_back(vec3(1.0f, 0.0f, 0.0f));
		boidNorms->push_back(vec3(0.0f, 1.0f, 0.0f));
		boidNorms->push_back(vec3(0.0f, 0.0f, 1.0f));
		
		boidInds->push_back(currIndNum);
		currIndNum += 1;
		boidInds->push_back(currIndNum);
		currIndNum += 1;
		boidInds->push_back(currIndNum);
		currIndNum += 1;
	}
	

}
void initBoids(Boid firstBoid, vector<Boid>* allBoids)
{
	int numBoids = 5;
	Random random;
	allBoids->push_back(firstBoid);
	float randomNum;
	Boid prevBoid;

		
	for(int i = 0; i < numBoids-1; i++)
	{
		Boid newBoid;
		pos newPos;
		//newBoid.setPos(prevBoid.getPos());
	//	if(first)
		//	{
				newBoid.setPos(firstBoid.getPos());
	//			first = false;
	//		}
		float test = (float) i+1;
		
		newPos = newBoid.getPos();

	if(i % 3 == 0)
	{
		newPos.p1 += vec3(test, 0, test);
		newPos.p2 += vec3(test, 0, test);
		newPos.p3 += vec3(test, 0, test);
	}
	else if (i % 3 == 1)
	{
		newPos.p1 += vec3(0, test, test);
		newPos.p2 += vec3(0, test, test);
		newPos.p3 += vec3(0, test, test);
	}
	else
	{
		newPos.p1 += vec3(0, 0, test);
		newPos.p2 += vec3(0, 0, test);
		newPos.p3 += vec3(0, 0, test);
	}
		newBoid.setPos(newPos);

		allBoids->push_back(newBoid);

	}
}
/*Boids fly to center of neighbours*/
vec3 ruleOne(Boid boid, vector<Boid> allBoids, int currBoid)
{
	vec3 v1;
	vec3 center;
	//vector<Boid> newBoids = allBoids;
	for(int i = 0; i < allBoids.size(); i++)
	{
		if(i != currBoid)
			center += allBoids[i].getCenter();
	}
	center = center / (float)(allBoids.size()-1);
	v1 = (center - boid.getCenter()) / 10.0f;
	return v1;
	
}
/*Fly small distance from other boids*/
vec3 ruleTwo(Boid boid, vector<Boid> allBoids, int currBoid)
{
	vec3 center = vec3(0.0f,0.0f,0.0f);
	for(int i = 0; i < allBoids.size(); i++)
	{
		if(i != currBoid)
			{
				if(length(allBoids[i].getCenter() - boid.getCenter()) < 10.0f)
				{
					center -= (allBoids[i].getCenter() - boid.getCenter());
				}
				cout << length(allBoids[i].getCenter() - boid.getCenter()) << endl;
			}
	}
	return center;
	
}
/*Boids match velocity with neighbours*/
vec3 ruleThree(Boid boid, vector<Boid> allBoids, int currBoid)
{
 
	vec3 vel = vec3(0.0f,0.0f,0.0f);
	for(int i = 0; i < allBoids.size(); i++)
	{
		if(i != currBoid)
			{
				vel += allBoids[i].getVel();
			}
	}
	vel = vel / (float)(allBoids.size()-1);
	
	return ((vel - boid.getVel())/ 8.0f);
}
/* limit velocity of boids*/
vec3 velLim(vec3 vel)
{
	float velLimitUpper = 1.0f;
	if(length(vel) > velLimitUpper && length(vel) > 0)
		vel = (vel / length(vel)) * velLimitUpper;
	
	float velLimitLower = -1.0f;
	if(length(vel) < velLimitLower && length(vel) < 0)
		vel = (vel / length(vel)) * velLimitLower;
	
	
	
	return vel;
}
Boid boundBoids(Boid boid)
{

	vec3 p1 = boid.getPos().p1;
	vec3 p2 = boid.getPos().p2;
	vec3 p3 = boid.getPos().p3;
	float bound = 100.0f;
	float xMin = -bound, xMax = bound, yMin = -bound, yMax = bound, zMin = -bound, zMax = bound;
	
	if(p1.x < xMin)
		p1.x = xMax;
	else if(p1.x > xMax)
		p1.x = xMin;
		
	if(p1.y < yMin)
		p1.y = yMax;
	else if(p1.y > yMax)
		p1.y = yMin;
		
	if(p1.z < zMin)
		p1.z = zMax;
	else if(p1.z > zMax)
		p1.z = zMin;

	if(p2.x < xMin)
		p2.x = xMax;
	else if(p2.x > xMax)
		p2.x = xMin;
		
	if(p2.y < yMin)
		p2.y = yMax;
	else if(p2.y > yMax)
		p2.y = yMin;
		
	if(p2.z < zMin)
		p2.z = zMax;
	else if(p2.z > zMax)
		p2.z = zMin;
		
	if(p3.x < xMin)
		p3.x = xMax;
	else if(p3.x > xMax)
		p3.x = xMin;
		
	if(p3.y < yMin)
		p3.y = yMax;
	else if(p3.y > yMax)
		p3.y = yMin;
		
	if(p3.z < zMin)
		p3.z = zMax;
	else if(p3.z > zMax)
		p3.z = zMin;
	
	pos newPos;
	newPos.p1 = p1;
	newPos.p2 = p2;
	newPos.p3 = p3;
	
	boid.setPos(newPos);
	
	
	return boid;
}
Boid moveBoids(vector<Boid> allBoids, int currBoid, float dt)
{
	Boid currBoidToMove = allBoids[currBoid];
	vec3 v1 = ruleOne(allBoids[currBoid], allBoids, currBoid);
	vec3 v2 = ruleTwo(allBoids[currBoid], allBoids, currBoid);
	vec3 v3 = ruleThree(allBoids[currBoid], allBoids, currBoid);
	vec3 newVel = currBoidToMove.getVel() + v1 + v2 + v3;
	
	

	newVel = velLim(newVel*dt);
	currBoidToMove.setVel(newVel);
	
	pos newPos = currBoidToMove.getPos();
	newPos.p1 += newVel;
	newPos.p2 += newVel;
	newPos.p3 += newVel;
	
	currBoidToMove.setPos(newPos);
	
	currBoidToMove = boundBoids(currBoidToMove);

	return currBoidToMove;
}
int main(int argc, char *argv[])
{   
    window = createGLFWWindow();
    if(window == NULL)
    	return -1;

    //Initialize glad
    if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}

    // query and print out information about our OpenGL environment
    QueryGLVersion();

	initGL();

	//Initialize shader
	GLuint program = initShader("vertex.glsl", "fragment.glsl");

	GLuint vao;
	VertexBuffers vbo;

	//Generate object ids
	glGenVertexArrays(1, &vao);
	glGenBuffers(VertexBuffers::COUNT, vbo.id);

	initVAO(vao, vbo);

	//Geometry information
	vector<vec3> points, normals;
	vector<unsigned int> indices;

	generateSquare(&points, &normals, &indices, 1.f);
	
	/*Stuff from a file*/
	vector<vec3> filePoints;
	vector<vec3> fileNormals;
	vector<unsigned int> fileInds;
	char* filename = "boidPos.txt";
	readFile(&filePoints, &fileNormals, &fileInds, filename); //fileNormals and fileInds not used atm not sure how to use for now...
	
	Boid firstBoid;
	pos firstPos;
	firstPos.p1 = filePoints[0];
	firstPos.p2 = filePoints[1];
	firstPos.p3 = filePoints[2];
	firstBoid.setPos(firstPos);
	
	
	vector<Boid> boids;
	initBoids(firstBoid, &boids);
	
	
	vector<vec3> boidsToDraw, boidNorms;
	vector<unsigned int> boidInds;
	
	//boids.push_back(firstBoid);
	//boids.push_back(secondBoid);
	//boids.push_back(thirdBoid);
	//cout<< "BOIDS SIZE: " << boids.size() << endl;
	 //TODO setup each void for drawing

	Camera cam = Camera(vec3(0, 0, -1), vec3(0, 0, 20));
	activeCamera = &cam;
	//float fovy, float aspect, float zNear, float zFar
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 400.f);

	mat4 moveObj = mat4(1.0f);
	float dt = 0.05f;
	float time = 0.0f;
	float timestep = 1.0f / 1000.0f;
	float extratime;
    // run an event-triggered main loop
    vector<Boid> boidsHold;
    while (!glfwWindowShouldClose(window))
    {
		dt = 0.01f;
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//Clear color and depth buffers (Haven't covered yet)
		drawBoids(boids, &boidsToDraw, &boidNorms, &boidInds);
		if(play)
			{
				//time += dt;
				dt += extratime;
				while(dt >= timestep)
				{	
					for(int i = 0; i < boids.size(); i++)
						boidsHold.push_back(moveBoids(boids, i, dt));
					
					boids = boidsHold;
					boidsHold.clear();
					extratime = dt;
					dt -= timestep;
					
				}
			}
		
        //loadUniforms(program, winRatio*perspectiveMatrix*cam.getMatrix(), moveObj);
        //render(vao, 0, indices.size(), program, vbo, points, normals, indices); // call function to draw our scene
		
		//loadUniforms(program, winRatio*perspectiveMatrix*cam.getMatrix(), moveObj);
       // render(vao, 0, fileInds.size(), program, vbo, filePoints, fileNormals, fileInds); // call function to draw our scene
			
		loadUniforms(program, winRatio*perspectiveMatrix*cam.getMatrix(), moveObj);
		render(vao, 0, boidInds.size(), program, vbo, boidsToDraw, boidNorms, boidInds); // call function to draw our scene
		boidsToDraw.clear();
		boidNorms.clear();
		boidInds.clear();
       
        glfwSwapBuffers(window);// scene is rendered to the back buffer, so swap to front for display

 
        glfwPollEvents(); // sleep until next event before drawing again
	}

	// clean up allocated resources before exit
	
	deleteStuff(vao, vbo, program);
	
	return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void deleteStuff(GLuint vao, VertexBuffers vbo, GLuint program)
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(VertexBuffers::COUNT, vbo.id);
	glDeleteProgram(program);


	glfwDestroyWindow(window);
	glfwTerminate();
}
void QueryGLVersion()
{
    // query opengl version and renderer information
    string version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    string glslver  = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    cout << "OpenGL [ " << version << " ] "
         << "with GLSL [ " << glslver << " ] "
         << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors(string location)
{
    bool error = false;
    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
    {
        cout << "OpenGL ERROR:  ";
        switch (flag) {
        case GL_INVALID_ENUM:
            cout << location << ": " << "GL_INVALID_ENUM" << endl; break;
        case GL_INVALID_VALUE:
            cout << location << ": " << "GL_INVALID_VALUE" << endl; break;
        case GL_INVALID_OPERATION:
            cout << location << ": " << "GL_INVALID_OPERATION" << endl; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            cout << location << ": " << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
        case GL_OUT_OF_MEMORY:
            cout << location << ": " << "GL_OUT_OF_MEMORY" << endl; break;
        default:
            cout << "[unknown error code]" << endl;
        }
        error = true;
    }
    return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
    string source;

    ifstream input(filename.c_str());
    if (input) {
        copy(istreambuf_iterator<char>(input),
             istreambuf_iterator<char>(),
             back_inserter(source));
        input.close();
    }
    else {
        cout << "ERROR: Could not load shader source from file "
             << filename << endl;
    }

    return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
    // allocate shader object name
    GLuint shaderObject = glCreateShader(shaderType);

    // try compiling the source as a shader of the given type
    const GLchar *source_ptr = source.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);

    // retrieve compile status
    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
        cout << "ERROR compiling shader:" << endl << endl;
        cout << source << endl;
        cout << info << endl;
    }

    return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    // allocate program object name
    GLuint programObject = glCreateProgram();

    // attach provided shader objects to this program
    if (vertexShader)   glAttachShader(programObject, vertexShader);
    if (fragmentShader) glAttachShader(programObject, fragmentShader);

    // try linking the program with given attachments
    glLinkProgram(programObject);

    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking shader program:" << endl;
        cout << info << endl;
    }

    return programObject;
}


// ==========================================================================
