/*
Author: Shida Yang
Class: ECE6122
Last Date Modified: 11/15/2021
Description:
	This program defines the Sphere object
*/

#include "Sphere.h"

#define NUM_OF_SLICES	36.0
#define NUM_OF_LEVELS	20.0

/*
Purpose: Sphere constructor, manually calculate and fill in vertices, uvs, normals vectors
Input: N/A
Output: an Sphere object
Return: N/A
*/
Sphere::Sphere(float x, float y, float z, float radius)
	:position(x,y,z), radius(radius)
{

	// top-most level (only triangles)
	float topZ = 1;
	float bottomZ = 1 - 2 / NUM_OF_LEVELS;
	float topRadius = sqrt(1 - pow(topZ, 2));
	float bottomRadius = sqrt(1 - pow(bottomZ, 2));
	float angleInc = 2 * 3.1415926 / NUM_OF_SLICES;
	for (int i = 0; i < NUM_OF_SLICES; i++)
	{
		vertices.push_back(glm::vec3(0, 0, topZ));
		vertices.push_back(glm::vec3(bottomRadius * cos(angleInc * i), bottomRadius * sin(angleInc * i), bottomZ));
		vertices.push_back(glm::vec3(bottomRadius * cos(angleInc * (i + 1)), bottomRadius * sin(angleInc * (i + 1)), bottomZ));
	}

	// from top to middle
	for (int j = 1; j < NUM_OF_LEVELS / 2; j++)
	{
		topZ = 1 - j * 2 / NUM_OF_LEVELS;
		bottomZ = 1 - (j + 1) * 2 / NUM_OF_LEVELS;
		topRadius = sqrt(1 - pow(topZ, 2));
		bottomRadius = sqrt(1 - pow(bottomZ, 2));
		for (int i = 0; i < NUM_OF_SLICES; i++)
		{
			// left triangle
			vertices.push_back(glm::vec3(topRadius * cos(angleInc * i), topRadius * sin(angleInc * i), topZ));
			vertices.push_back(glm::vec3(topRadius * cos(angleInc * (i + 1)), topRadius * sin(angleInc * (i + 1)), topZ));
			vertices.push_back(glm::vec3(bottomRadius * cos(angleInc * i), bottomRadius * sin(angleInc * i), bottomZ));

			// right triangle
			vertices.push_back(glm::vec3(bottomRadius * cos(angleInc * i), bottomRadius * sin(angleInc * i), bottomZ));
			vertices.push_back(glm::vec3(bottomRadius * cos(angleInc * (i + 1)), bottomRadius * sin(angleInc * (i + 1)), bottomZ));
			vertices.push_back(glm::vec3(topRadius * cos(angleInc * (i + 1)), topRadius * sin(angleInc * (i + 1)), topZ));
		}
	}

	// from middle to bottom (mirror the top half)
	int halfSize = vertices.size();
	for (int i = halfSize - 1; i >= 0; i--)
	{
		vertices.push_back(vertices[i]);
		vertices.back().z = -vertices.back().z;
	}

	// generate normals
	for (int i = 0; i < vertices.size(); i++)
	{
		// normal is from center to vertex
		normals.push_back(glm::normalize(vertices[i]));
	}

}

/*
Purpose: draw the Sphere using openGL
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
Output: N/A
Return: N/A
*/
void Sphere::draw(const glm::mat4& Projection, const glm::mat4& View, const GLuint& MatrixID, const GLuint& TextureID, const GLuint& vertexbuffer, const GLuint& uvbuffer, const GLuint& normalbuffer, const GLuint& ViewMatrixID, const GLuint& ModelMatrixID, const GLuint& useColorID, const GLuint& colorIntensityID)
{
	// calculate Model
	glm::mat4 Model =
		glm::translate(glm::mat4(1.0f), position) *
		glm::scale(glm::mat4(1.0f), glm::vec3(radius, radius, radius));;

	// calculate MVP and send it to shader
	glm::mat4 MVP = Projection * View * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);

	glUniform1i(useColorID, true);
	glUniform1f(colorIntensityID, 1);

	// send vertex, uv and normal data
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

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

	// 3rd attribute buffer : normal
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the sphere
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
}