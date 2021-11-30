#pragma once

#include <vector>
#include <thread>
#include <string>
#include <mutex>

using namespace std;

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>

#include <common/texture.hpp>
#include <common/objloader.hpp>

#define UVA_SIZE		0.5

#define MAX_VELOCITY_1	2.0
#define MIN_VELOCITY_2	2.0
#define MAX_VELOCITY_2	10.0
#define MAX_ACCEL		20.0
/*
Author: Shida Yang
Class: ECE6122
Last Date Modified: 11/15/2021
Description:
	Header for the ECE_UAV object
*/

#define GRAVITY_ACCEL	10.0

class ECE_UAV
{
private:
	// static variables (common to all UAV objects so we don't need to do repeated file load, which is slow)
	// vertices, uvs, and normals used to draw it
	static std::vector<glm::vec3> vertices;
	static std::vector<glm::vec2> uvs;
	static std::vector< glm::vec3 > normals;
	// the texture
	static GLuint texture;

	// model object of the UAV
	glm::mat4 Model;

	// its mass
	float mass;

	// position, velocity, acceleration of the UAV
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;		// final NET acceleration, need to take max force into account when generating this acceleration

	// simulator for the animation thread
	thread simulator;

public:
	ECE_UAV(glm::vec3 initPosition);
	void start();
	void draw(const glm::mat4& Projection, const glm::mat4& View, const GLuint& MatrixID, const GLuint& TextureID, const GLuint& vertexbuffer, const GLuint& uvbuffer, const GLuint& normalbuffer, const GLuint& ViewMatrixID, const GLuint& ModelMatrixID, const GLuint& useColorID, const GLuint& colorIntensityID, float colorIntensity);

	const glm::vec3& getPosition();
	const glm::vec3& getVelocity();
	const glm::vec3& getAcceleration();

	void setPosition(glm::vec3 &newValue);
	void setVelocity(glm::vec3 &newValue);
	void setAcceleration(glm::vec3 &newValue);

	static void loadObjAndTexture();


};

