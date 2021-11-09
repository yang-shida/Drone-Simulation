#include "Football_Field.h"

#define BMP_WIDTH	256.0
#define BMP_LENGTH	512.0
#define FF_X_MIN	16.0
#define FF_Y_MIN	15.0
#define FF_X_MAX	482.0
#define FF_Y_MAX	241.0

#define LENGTH	(120.0/2)
#define WIDTH	(LENGTH/((FF_X_MAX-FF_X_MIN)/(FF_Y_MAX-FF_Y_MIN)))

#define TEXTURE_PATH	"ff_c.bmp"

Football_Field::Football_Field()
	:Model(glm::scale(glm::mat4(1.0f), glm::vec3(LENGTH, WIDTH, 1.0f)))
{
	vertices.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
	vertices.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	vertices.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	vertices.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
	vertices.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	vertices.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));

	uvs.push_back(glm::vec2(FF_X_MIN / BMP_LENGTH, 1 - FF_Y_MIN / BMP_WIDTH));
	uvs.push_back(glm::vec2(FF_X_MAX / BMP_LENGTH, 1 - FF_Y_MIN / BMP_WIDTH));
	uvs.push_back(glm::vec2(FF_X_MAX / BMP_LENGTH, 1 - FF_Y_MAX / BMP_WIDTH));
	uvs.push_back(glm::vec2(FF_X_MIN / BMP_LENGTH, 1 - FF_Y_MIN / BMP_WIDTH));
	uvs.push_back(glm::vec2(FF_X_MAX / BMP_LENGTH, 1 - FF_Y_MAX / BMP_WIDTH));
	uvs.push_back(glm::vec2(FF_X_MIN / BMP_LENGTH, 1 - FF_Y_MAX / BMP_WIDTH));

	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	texture = loadBMP_custom(TEXTURE_PATH);
}

void Football_Field::draw(const glm::mat4& Projection, const glm::mat4& View, const GLuint& MatrixID, const GLuint& TextureID, const GLuint& vertexbuffer, const GLuint& uvbuffer, const GLuint& normalbuffer, const GLuint &ViewMatrixID, const GLuint &ModelMatrixID)
{
	// calculate MVP and send it to shader
	glm::mat4 MVP = Projection * View * Model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);

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

}