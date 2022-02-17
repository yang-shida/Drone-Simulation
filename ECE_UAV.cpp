/*
Author: Shida Yang
Class: ECE6122
Last Date Modified: 11/15/2021


this is a test



Description:
	This program defines the ECE_UAV object
*/

#include "ECE_UAV.h"

#define OBJ_PATH		"cono_hi.obj"
#define TEXTURE_PATH	"uav_texture.bmp"

extern void threadFunction(ECE_UAV* pUAV);

// static variables
std::vector<glm::vec3> ECE_UAV::vertices;
std::vector<glm::vec2> ECE_UAV::uvs;
std::vector< glm::vec3 > ECE_UAV::normals;
GLuint ECE_UAV::texture;

/*
Purpose: ECE_UAV constructor
Input: initial position of the UAV
Output: an ECE_UAV object
Return: N/A
*/
ECE_UAV::ECE_UAV(glm::vec3 initPosition)
	// initialize position, mass, velocity, acceleration
	:position(initPosition.x, initPosition.y, initPosition.z + UVA_SIZE),
	mass(1), velocity(0, 0, 0), acceleration(0, 0, 0)
{
	// update model based on position and correct orientation
	Model = 
		glm::translate(glm::mat4(1.0f), position) *
		glm::rotate(float(90.0 / 180.0 * 3.1415926), glm::vec3(1, 0, 0)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(UVA_SIZE, UVA_SIZE, UVA_SIZE));
}

/*
Purpose: start the movement
Input: N/A
Output: N/A
Return: N/A
*/
void ECE_UAV::start()
{
	// create a thread and detech it from the main thread
	simulator = thread(threadFunction, this);
	simulator.detach();
}

/*
Purpose: draw this ECE_UAV using openGL
Input: 
	const glm::mat4& Projection: projection matrix
	const glm::mat4& View: view matrix
	const GLuint& MatrixID: MVP matric ID in openGL shader
	const GLuint& TextureID: texture ID in openGL shader
	const GLuint& vertexbuffer: vertex buffer in openGL shader
	const GLuint& uvbuffer: : uv buffer in openGL shader
	const GLuint& normalbuffer: normal buffer in openGL shader
	const GLuint& ViewMatrixID: view matric ID in openGL shader
	const GLuint& ModelMatrixID: model matric ID in openGL shader
	const GLuint& useColorID: useColor uniform ID in openGL shader
	const GLuint& colorIntensityID: colorIntensity uniform ID in openGL shader
	float colorIntensity: color intensity of the ECE_UAV
Output: N/A
Return: N/A
*/
void ECE_UAV::draw(const glm::mat4& Projection, const glm::mat4& View, const GLuint& MatrixID, const GLuint& TextureID, const GLuint& vertexbuffer, const GLuint& uvbuffer, const GLuint& normalbuffer, const GLuint& ViewMatrixID, const GLuint& ModelMatrixID, const GLuint& useColorID, const GLuint& colorIntensityID, float colorIntensity)
{
	// update Model
	Model = 
		glm::translate(glm::mat4(1.0f), position) *
		glm::rotate(float(90.0 / 180.0 * 3.1415926), glm::vec3(1, 0, 0)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(UVA_SIZE, UVA_SIZE, UVA_SIZE));;

	// calculate MVP and send it to shader
	glm::mat4 MVP = Projection * View * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);

	glUniform1i(useColorID, false);
	glUniform1f(colorIntensityID, colorIntensity);

	// send vertex, uv and normal data
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. 
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the football field
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

/*
Purpose: get UAV position
Input: N/A
Output: N/A
Return: the current position of UAV
*/
const glm::vec3& ECE_UAV::getPosition()
{
	return position;
}

/*
Purpose: get UAV velocity
Input: N/A
Output: N/A
Return: the current velocity of UAV
*/
const glm::vec3& ECE_UAV::getVelocity()
{
	return velocity;
}

/*
Purpose: get UAV acceleration
Input: N/A
Output: N/A
Return: the current acceleration of UAV
*/
const glm::vec3& ECE_UAV::getAcceleration()
{
	return acceleration;
}

/*
Purpose: set UAV position
Input: new position of UAV
Output: N/A
Return: N/A
*/
void ECE_UAV::setPosition(glm::vec3 &newValue)
{
	position.x = newValue.x;
	position.y = newValue.y;
	position.z = newValue.z;
}

/*
Purpose: set UAV velocity
Input: new velocity of UAV
Output: N/A
Return: N/A
*/
void ECE_UAV::setVelocity(glm::vec3 &newValue)
{
	velocity.x = newValue.x;
	velocity.y = newValue.y;
	velocity.z = newValue.z;
}

/*
Purpose: set UAV acceleration
Input: new acceleration of UAV
Output: N/A
Return: N/A
*/
void ECE_UAV::setAcceleration(glm::vec3 &newValue)
{
	acceleration.x = newValue.x;
	acceleration.y = newValue.y;
	acceleration.z = newValue.z;
}

/*
Purpose: load UAV vertices, uvs, normals from its obj file and texture from the bmp file
Input: N/A
Output: filled vertices, uvs, normals vectors
Return: N/A
*/
void ECE_UAV::loadObjAndTexture()
{
	// load object
	if (!loadOBJ(OBJ_PATH, ECE_UAV::vertices, ECE_UAV::uvs, ECE_UAV::normals))
	{
		throw string("Fail to load UAV object!");
	}
	// load texture
	ECE_UAV::texture = loadBMP_custom(TEXTURE_PATH);
}