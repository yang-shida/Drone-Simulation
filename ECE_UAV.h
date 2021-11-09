#pragma once

#include <vector>
#include <thread>
#include <string>

using namespace std;

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>

#include <common/texture.hpp>
#include <common/objloader.hpp>

class ECE_UAV
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector< glm::vec3 > normals;
	GLuint texture;
	glm::mat4 Model;

	float mass;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	thread simulator;

public:
	ECE_UAV(glm::vec3 initPosition);
	void start();
	void draw(const glm::mat4& Projection, const glm::mat4& View, const GLuint& MatrixID, const GLuint& TextureID, const GLuint& vertexbuffer, const GLuint& uvbuffer, const GLuint& normalbuffer, const GLuint& ViewMatrixID, const GLuint& ModelMatrixID);
};

