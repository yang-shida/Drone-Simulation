/*
Author: Shida Yang
Class: ECE6122
Last Date Modified: 11/15/2021
Description:
	Header for the Football_Field object
*/

#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <common/texture.hpp>

class Football_Field
{
private:
	// vertices, uvs, and normals used to draw it
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	// the texture
	GLuint texture;
	// its model matrix
	glm::mat4 Model;

public:
	Football_Field();
	void draw(const glm::mat4& Projection, const glm::mat4& View, const GLuint& MatrixID, const GLuint& TextureID, const GLuint& vertexbuffer, const GLuint& uvbuffer, const GLuint& normalbuffer, const GLuint& ViewMatrixID, const GLuint& ModelMatrixID, const GLuint& useColorID, const GLuint& colorIntensityID);
};

