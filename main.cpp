/*
Author: Shida Yang
Class: ECE6122
Last Date Modified: 11/15/2021
Description:
	This program simulates a UAV show using openGL
*/

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <random>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/objloader.hpp>

#include "Football_Field.h"
#include "ECE_UAV.h"
#include "Sphere.h"

#define NUM_OF_UAVS	15

uint32_t countx = 0;

void threadFunction(ECE_UAV* pUAV);
const glm::vec3 getMaxAccelerationAlongDirection(glm::vec3& direction, glm::vec3& resistingVector);

// number of UAVs that reach sphere
int onSphereCount(0);
mutex onSphereCountMtx;

// number of UAVs that finish the animation
int finishCount(0);
mutex finishCountMtx;

// could be any one of the UAV, used to prevent duplicated print from different threads
ECE_UAV* firstUAV;

// animation start time
double startTime = 0;

bool earlyStop = false;

// gravity vector
glm::vec3 gVector(0, 0, -GRAVITY_ACCEL);

/*
Purpose: simulates a UAV show using openGL
Input: N/A
Output: openGL animation
Return: N/A
*/
int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	// setup openGL window
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 800, "Shida's Window", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// set input key mode
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// vertex array
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	

	// projection and view of camera
	glm::mat4 Projection = glm::perspective(
		glm::radians(45.0f),
		1.0f, 
		0.1f, 
		300.0f
	);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, -150, 100), // Camera location
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 0, 1)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	GLuint useColorID = glGetUniformLocation(programID, "useColor");
	GLuint colorIntensityID = glGetUniformLocation(programID, "colorIntensity");
	float colorIntensity = 1;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// vertex buffer
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	// UV buffer
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);

	// normal buffer
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	
	// initialize a football field
	Football_Field ff;

	// initialize a sphere
	Sphere sphere(0, 0, 50, 10);

	// load UAV obj file and texture file
	try 
	{
		ECE_UAV::loadObjAndTexture();
		// create an array of UAVs at different location
		ECE_UAV UAVs[] = 
		{ 
			ECE_UAV(glm::vec3(-50, 27, 0)),
			ECE_UAV(glm::vec3(-25, 27, 0)),
			ECE_UAV(glm::vec3(0, 27, 0)),
			ECE_UAV(glm::vec3(25, 27, 0)),
			ECE_UAV(glm::vec3(50, 27, 0)),
			ECE_UAV(glm::vec3(-50, 0, 0)),
			ECE_UAV(glm::vec3(-25, 0, 0)),
			ECE_UAV(glm::vec3(0, 0, 0)),
			ECE_UAV(glm::vec3(25, 0, 0)),
			ECE_UAV(glm::vec3(50, 0, 0)),
			ECE_UAV(glm::vec3(-50, -27, 0)),
			ECE_UAV(glm::vec3(-25, -27, 0)),
			ECE_UAV(glm::vec3(0, -27, 0)),
			ECE_UAV(glm::vec3(25, -27, 0)),
			ECE_UAV(glm::vec3(50, -27, 0))
		};

		// point to one of the UAVs
		firstUAV = &UAVs[0];
	
		// initialize timing
		double lastTime = glfwGetTime();
		double currentTime = glfwGetTime();

		// print start message
		startTime = glfwGetTime();
		cout << fixed << setprecision(3) << "[" << right << setw(7) << glfwGetTime() - startTime << " sec] Simulation started" << endl;

		// start animation for all UAVs
		for (int i = 0; i < NUM_OF_UAVS; i++)
		{
			UAVs[i].start();
		}

		// initialize color change direction
		bool colorDecreasing = true;

		
		// every 30ms, update frame
		do
		{
			// wait 30ms
			while (currentTime - lastTime <= 0.03)
			{
				currentTime = glfwGetTime();
			}
			lastTime = currentTime;

			// animation still going (at least for some of the UAVs)
			if (finishCount < NUM_OF_UAVS)
			{
				
				// Clear the screen
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// Use our shader
				glUseProgram(programID);

				// draw all UAVs
				for (int i = 0; i < NUM_OF_UAVS; i++)
				{
					UAVs[i].draw(Projection, View, MatrixID, TextureID, vertexbuffer, uvbuffer, normalbuffer, ViewMatrixID, ModelMatrixID, useColorID, colorIntensityID, colorIntensity);
				}

				// draw football field
				ff.draw(Projection, View, MatrixID, TextureID, vertexbuffer, uvbuffer, normalbuffer, ViewMatrixID, ModelMatrixID, useColorID, colorIntensityID);

				// draw virtual sphere
				sphere.draw(Projection, View, MatrixID, TextureID, vertexbuffer, uvbuffer, normalbuffer, ViewMatrixID, ModelMatrixID, useColorID, colorIntensityID);

				// change UAV color intensity based between half and full
#define COLOR_INTENSITY_DELTA	(0.5*0.03/1.0)
				if (colorDecreasing)
				{
					colorIntensity -= COLOR_INTENSITY_DELTA;
					if (colorIntensity <= 0.5)
					{
						colorDecreasing = false;
					}
				}
				else
				{
					colorIntensity += COLOR_INTENSITY_DELTA;
					if (colorIntensity >= 1)
					{
						colorDecreasing = true;
					}
				}


				// check collision
				for (int i = 0; i < NUM_OF_UAVS - 1; i++)
				{
					// only check those pairs that has not been checked
					for (int j = i + 1; j < NUM_OF_UAVS; j++)
					{
						// if distance too close (less than 0.01m = 1cm), swap velocity
						if (glm::distance(UAVs[i].getPosition(), UAVs[j].getPosition()) <= (0.01 + 2 * UVA_SIZE))
						{
							// check if approaching
							if (UAVs[i].getPosition().x < UAVs[j].getPosition().x && UAVs[i].getVelocity().x > 0 && UAVs[j].getVelocity().x < 0 ||
								UAVs[i].getPosition().x > UAVs[j].getPosition().x && UAVs[i].getVelocity().x < 0 && UAVs[j].getVelocity().x > 0 ||
								UAVs[i].getPosition().y < UAVs[j].getPosition().y && UAVs[i].getVelocity().y > 0 && UAVs[j].getVelocity().y < 0 ||
								UAVs[i].getPosition().y > UAVs[j].getPosition().y && UAVs[i].getVelocity().y < 0 && UAVs[j].getVelocity().y > 0 ||
								UAVs[i].getPosition().z < UAVs[j].getPosition().z && UAVs[i].getVelocity().z > 0 && UAVs[j].getVelocity().z < 0 ||
								UAVs[i].getPosition().z > UAVs[j].getPosition().z && UAVs[i].getVelocity().z < 0 && UAVs[j].getVelocity().z > 0
								) 
							{
								glm::vec3 tempVelocityi = UAVs[i].getVelocity();
								glm::vec3 tempVelocityj = UAVs[j].getVelocity();
								UAVs[i].setVelocity(tempVelocityj);
								UAVs[j].setVelocity(tempVelocityi);
								//cout << "Collision! Location: (" << UAVs[i].getPosition().x << ", " << UAVs[i].getPosition().y << ", " << UAVs[i].getPosition().z << ")" << endl;
							}
							
						}
					}
				}
				// update frame
				glfwSwapBuffers(window);
			}
			
			// handle other events (like keyboard press)
			glfwPollEvents();

			if (!(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0))
				earlyStop = true;

		} while (!earlyStop);
	}
	catch (string msg)
	{
		// goes here is unable to load obj file for UAV
		cout << msg << endl;
	}

	while (finishCount < 15);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

/*
Purpose: animation thread function
Input: pointer to the UAV for this animation
Output: UAV animation
Return: N/A
*/
void threadFunction(ECE_UAV* pUAV)
{
	// initializing timing
	double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	double timeDiff = currentTime - lastTime;

	// get a random number generator
	default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());
	uniform_real_distribution<float> distribution(0, 1.0);

	// four phases of the animation
	enum ANIMATION_PHASE {
		STATIONARY,
		FLYING_TO_SPHERE,
		FLYING_ON_SPHERE,
		FINISH
	};

	// start from stationary
	ANIMATION_PHASE curr_state = STATIONARY;

	// initialize timers for two phases
	double stationaryTimer = 0;
	double flyingOnSphereTimer = 0;

	// used to bound UAV to sphere surface
	float fs = 0;

	// update position, velocity, acceleration every 10ms
	while (!earlyStop)
	{
		// wait 10ms
		timeDiff = 0;
		while (timeDiff <= 0.01)
		{
			currentTime = glfwGetTime();
			timeDiff = currentTime - lastTime;
		}
		lastTime = currentTime;

		// check which state we are in
		switch (curr_state)
		{
		// stationary, just check timer
		case STATIONARY:
		{
			stationaryTimer += timeDiff;
			// timeout, print message, go to next state
			if (stationaryTimer >= 5)
			{
				// only print once
				if (pUAV == firstUAV)
				{
					cout << fixed << setprecision(3) << "[" << right << setw(7) << glfwGetTime() - startTime << " sec] Flying to sphere" << endl;
				}
				stationaryTimer = 0;
				curr_state = FLYING_TO_SPHERE;
			}
			break;
		}
		// from original position to sphere surface
		case FLYING_TO_SPHERE:
		{
			// update position (pos=pos+v*dt)
			glm::vec3 newPos(
				pUAV->getPosition().x + pUAV->getVelocity().x * timeDiff,
				pUAV->getPosition().y + pUAV->getVelocity().y * timeDiff,
				pUAV->getPosition().z + pUAV->getVelocity().z * timeDiff
			);
			pUAV->setPosition(newPos);

			// update velocity (v=v+a*dt)
			glm::vec3 newVelocity(
				pUAV->getVelocity().x + pUAV->getAcceleration().x * timeDiff,
				pUAV->getVelocity().y + pUAV->getAcceleration().y * timeDiff,
				pUAV->getVelocity().z + pUAV->getAcceleration().z * timeDiff
			);
			// check if velocity is within limit
			double velocityMag = sqrt(pow(newVelocity.x, 2) + pow(newVelocity.y, 2) + pow(newVelocity.z, 2));
			if (velocityMag >= MAX_VELOCITY_1)
			{
				newVelocity.x *= MAX_VELOCITY_1 / velocityMag;
				newVelocity.y *= MAX_VELOCITY_1 / velocityMag;
				newVelocity.z *= MAX_VELOCITY_1 / velocityMag;
			}
			pUAV->setVelocity(newVelocity);

			// update acceleration
			// use max acceleration that moves UAV from original pos to sphere surface, speed is controlled above
			glm::vec3 direction = glm::vec3(0, 0, 50) - pUAV->getPosition();
			glm::vec3 newAcceleration = getMaxAccelerationAlongDirection(direction, gVector);
			pUAV->setAcceleration(newAcceleration);

			// if reached the sphere surface
			if (sqrt(pow(pUAV->getPosition().x - 0, 2) + pow(pUAV->getPosition().y - 0, 2) + pow(pUAV->getPosition().z - 50, 2)) <= 10.0)
			{
				// safely increment onSphereCount
				lock_guard<mutex> lck(onSphereCountMtx);
				onSphereCount++;
				curr_state = FLYING_ON_SPHERE;

				// only output when the last one is on sphere
				if (onSphereCount == 15)
				{
					cout << fixed << setprecision(3) << "[" << right << setw(7) << glfwGetTime() - startTime << " sec] Last UAV reached sphere, going in circle" << endl;
				}

				// update acceleration to give UAVs some initial tangential speed in random direction
				glm::vec3 direction = glm::vec3(0, 0, 50) - pUAV->getPosition();
				/*
					ax+by+cz=0
					z=-(xa+yb)/c
				*/
				float x = (float)(distribution(generator) * 2) - 1;
				float y = (float)(distribution(generator) * 2) - 1;
				float z = -(x * direction.x + y * direction.y) / direction.z;
				direction.x = x;
				direction.y = y;
				direction.z = z;

				glm::vec3 newAcceleration = getMaxAccelerationAlongDirection(direction, gVector);
				pUAV->setAcceleration(newAcceleration);
			}
			break;
		}
		// fly on sphere surface
		case FLYING_ON_SPHERE:
		{
			// don't start timer until all UAVs are here
			if (onSphereCount >= NUM_OF_UAVS)
			{
				flyingOnSphereTimer += timeDiff;
				if (flyingOnSphereTimer >= 60)
				{
					flyingOnSphereTimer = 0;
					curr_state = FINISH;
				}
			}

			// update position (pos=pos+v*dt)
			glm::vec3 newPos(
				pUAV->getPosition().x + pUAV->getVelocity().x * timeDiff,
				pUAV->getPosition().y + pUAV->getVelocity().y * timeDiff,
				pUAV->getPosition().z + pUAV->getVelocity().z * timeDiff
			);
			pUAV->setPosition(newPos);

			// update velocity (v=v+a*dt)
			glm::vec3 newVelocity(
				pUAV->getVelocity().x + pUAV->getAcceleration().x * timeDiff,
				pUAV->getVelocity().y + pUAV->getAcceleration().y * timeDiff,
				pUAV->getVelocity().z + pUAV->getAcceleration().z * timeDiff
			);

			// check if velocity is within limit
			float velocityMag = sqrt(pow(newVelocity.x, 2) + pow(newVelocity.y, 2) + pow(newVelocity.z, 2));

			pUAV->setVelocity(newVelocity);

			// update acceleration

			// calculate centripetal acceleration based on pos and v
			glm::vec3 centerDirection = glm::normalize(glm::vec3(0, 0, 50) - pUAV->getPosition());
			glm::vec3 tangentDirection = glm::normalize(pUAV->getVelocity() - glm::dot(pUAV->getVelocity(), centerDirection) * centerDirection);

			glm::vec3 maxNewAcceleration = getMaxAccelerationAlongDirection(centerDirection, gVector);

			float maxNewAccelerationMag = glm::distance(maxNewAcceleration, glm::vec3(0,0,0));
			glm::vec3 tangentVelocity = glm::dot(pUAV->getVelocity(), tangentDirection) * glm::normalize(tangentDirection);
			float requiredAccelMag = glm::distance(tangentVelocity, glm::vec3(0, 0, 0)) * glm::distance(tangentVelocity, glm::vec3(0, 0, 0)) / glm::distance((glm::vec3(0, 0, 50) - pUAV->getPosition()), glm::vec3(0, 0, 0));

			// make sure we don't ask for more than what the UAV can provide
			glm::vec3 newAcceleration;
			if (requiredAccelMag <= maxNewAccelerationMag)
			{
				newAcceleration.x = maxNewAcceleration.x * requiredAccelMag / maxNewAccelerationMag;
				newAcceleration.y = maxNewAcceleration.y * requiredAccelMag / maxNewAccelerationMag;
				newAcceleration.z = maxNewAcceleration.z * requiredAccelMag / maxNewAccelerationMag;
			}
			else
			{
				newAcceleration = maxNewAcceleration;
			}

			// tangent accel
			float x = 1;
			// keep UAV speed at 6 m/s
			float targetVelocity = 6;
			// calculate tangential acceleration based on the current speed
			glm::vec3 requiredTangentAccel = x * (targetVelocity - velocityMag) * glm::normalize(tangentDirection);

			maxNewAcceleration = getMaxAccelerationAlongDirection(requiredTangentAccel, newAcceleration);
			maxNewAccelerationMag = glm::distance(maxNewAcceleration, glm::vec3(0, 0, 0));
			// make sure we don't ask for more than what the UAV can provide
			if (glm::distance(requiredTangentAccel, glm::vec3(0, 0, 0)) > maxNewAccelerationMag)
			{
				requiredTangentAccel = maxNewAcceleration;
			}
			// add tangential accel to total accel
			newAcceleration += requiredTangentAccel;

			// bound UAV to surface of sphere using PD controller
			float pfs = 2, ifs = 10;
			glm::vec3 temp = glm::vec3(0, 0, 50) - pUAV->getPosition();
			float newfs = pfs * (glm::distance(temp, glm::vec3(0, 0, 0)) - 10.0); // calculate P component
			float dfs = newfs - fs;	// calculate D component make it stable
			fs = newfs;
			// combine P D components
			glm::vec3 fsV = glm::normalize(glm::vec3(0, 0, 50) - pUAV->getPosition());
			fsV.x *= (fs + dfs * ifs);
			fsV.y *= (fs + dfs * ifs);
			fsV.z *= (fs + dfs * ifs);
			newAcceleration += fsV;

			maxNewAcceleration = getMaxAccelerationAlongDirection(newAcceleration, gVector);
			maxNewAccelerationMag = glm::distance(maxNewAcceleration, glm::vec3(0, 0, 0));
			// make sure we don't ask for more than what the UAV can provide
			if (glm::distance(newAcceleration, glm::vec3(0, 0, 0)) > maxNewAccelerationMag)
			{
				newAcceleration = maxNewAcceleration;
			}

			// finally, update our new acceleration
			pUAV->setAcceleration(newAcceleration);
			
			//cout << "Distance: " << glm::distance(pUAV->getPosition(), glm::vec3(0, 0, 50)) << " | Velocity: " << glm::distance(pUAV->getVelocity(), glm::vec3(0, 0, 0)) << " | Accel: " << glm::distance(pUAV->getAcceleration(), glm::vec3(0, 0, 0)) << endl;

			break;
		}
		case FINISH:
		{
			// safely increment finishCount
			lock_guard<mutex> lck(finishCountMtx);
			finishCount++;
			// only output once
			if (finishCount == 15)
			{
				cout << fixed << setprecision(3) << "[" << right << setw(7) << glfwGetTime() - startTime << " sec] Simulation finished" << endl;
			}
			return;
		}
		}
	}
	if (earlyStop)
	{
		lock_guard<mutex> lck(finishCountMtx);
		finishCount++;
	}
}

/*
Purpose: calculate maximum acceleration along a direction given a resisting force and the max acceleration constrain
Input: 
	glm::vec3& direction: direction of the net acceleration
	glm::vec3& resistingVector: resisting force
Output: N/A
Return: max acceleration along given direction
*/
const glm::vec3 getMaxAccelerationAlongDirection(glm::vec3& direction, glm::vec3& resistingVector)
{
	// calculated using line-sphere intersection algebra
	
	// radius of sphere
	float r = 20.0;

	glm::vec3 normalizedDir = glm::normalize(direction);

	// two points on line
	glm::vec3 ptA = resistingVector;
	glm::vec3 ptB = resistingVector + normalizedDir;

	// https://stackoverflow.com/questions/6533856/ray-sphere-intersection
	// solving line/sphere intersection
	float a = pow(ptB.x - ptA.x, 2) + pow(ptB.y - ptA.y, 2) + pow(ptB.z - ptA.z, 2);
	float b = 2 * ((ptB.x - ptA.x) * ptA.x + (ptB.y - ptA.y) * ptA.y + (ptB.z - ptA.z) * ptA.z);
	float c = pow(ptA.x, 2) + pow(ptA.y, 2) + pow(ptA.z, 2) - pow(r, 2);

	float discrimant = b * b - 4 * a * c;
	if (discrimant < 0)
	{
		return glm::vec3(0, 0, 0);
	}

	float d1 = (-b + sqrt(discrimant)) / (2 * a);
	float d2 = (-b - sqrt(discrimant)) / (2 * a);

	glm::vec3 res1(
		ptA.x + d1 * (ptB.x - ptA.x),
		ptA.y + d1 * (ptB.y - ptA.y),
		ptA.z + d1 * (ptB.z - ptA.z)
	);

	glm::vec3 res2(
		ptA.x + d2 * (ptB.x - ptA.x),
		ptA.y + d2 * (ptB.y - ptA.y),
		ptA.z + d2 * (ptB.z - ptA.z)
	);

	// remove resisting force from res
	res1 -= resistingVector;
	res2 -= resistingVector;

	// return the answer that follows the direction
	float epslon = 0.000001;
	// x is non-zero, use x
	if (abs(normalizedDir.x) >= epslon)
	{
		return
			normalizedDir.x > 0 ?
			(res1.x > 0 ? res1 : res2) :
			(res1.x < 0 ? res1 : res2);
	}
	else
	{
		// y is non-zero, use y
		if (abs(normalizedDir.y) >= epslon)
		{
			return
				normalizedDir.y > 0 ?
				(res1.y > 0 ? res1 : res2) :
				(res1.y < 0 ? res1 : res2);
		}
		else
		{
			// z is non-zero, use z
			if (abs(normalizedDir.z) >= epslon)
			{
				return
					normalizedDir.z > 0 ?
					(res1.z > 0 ? res1 : res2) :
					(res1.z < 0 ? res1 : res2);
			}
			// everything is 0, no accel
			else
			{
				return glm::vec3(0, 0, 0);
			}
		}
	}

}