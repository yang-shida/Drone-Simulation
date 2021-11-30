/*
Author: Shida Yang
Class: ECE6122
Last Date Modified: 11/15/2021
Description:
	Header for the Sphere object
*/

#pragma once

#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>

class Sphere
{
private:
	// center of sphere
	glm::vec3 position;
	// its radius
	float radius;
	// vertices and normals used to draw it
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

public:
	Sphere(float x, float y, float z, float radius);
	void draw(const glm::mat4& Projection, const glm::mat4& View, const GLuint& MatrixID, const GLuint& TextureID, const GLuint& vertexbuffer, const GLuint& uvbuffer, const GLuint& normalbuffer, const GLuint& ViewMatrixID, const GLuint& ModelMatrixID, const GLuint& useColorID, const GLuint& colorIntensityID);
};

