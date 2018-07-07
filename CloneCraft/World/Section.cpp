#include "Section.h"

#include "OctavePerlin.h"
#include "Converter.h"
#include "ChunkMap.h"
#include "Debug.h"

#include <iostream>

template<typename T, int S>
using arr = std::array<T, S>;

struct Face {
	vec3 pos;
	vec2 tex;
};

Section::Section(ChunkMap* const chunkMap, Chunk* const chunk, ivec3 position)
		: p_chunkMap{ chunkMap }, p_chunk{ chunk }, m_position{ position }, 
		m_blocks{ ivec3{ Const::SECTION_SIDE, Const::SECTION_HEIGHT, Const::SECTION_SIDE } } {
}

Section::~Section() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Section::loadBlocks() {
	for (int x = 0; x < Const::SECTION_SIDE; ++x)
		for (int z = 0; z < Const::SECTION_SIDE; ++z)
			for (int y = 0; y < Const::SECTION_HEIGHT; ++y) {
				ivec3 pos{ x, y, z };
				m_blocks.at(pos) = p_chunk->getChunkGenerator().getBlock(pos + Converter::sectionToGlobal(m_position));
			}
}

void Section::loadFaces() {

	std::vector<Face> faces;
	for (int x = 0; x < Const::SECTION_SIDE; ++x)
		for (int y = 0; y < Const::SECTION_HEIGHT; ++y)
			for (int z = 0; z < Const::SECTION_SIDE; ++z) {
				ivec3 pos{ x, y, z };
				if (m_blocks.at(pos) == 0)
					continue;

				ivec3 globalPos{ Converter::sectionToGlobal(m_position) + pos };

				for (int dir = 0; dir < Dir3D::SIZE; ++dir) {
					if (getNearBlock(pos + Dir3D::find(static_cast<Dir3D::Dir>(dir))) == 0) {
						for (int vtx = 0; vtx < 4; ++vtx) {
							faces.push_back({ dirToFace[dir][vtx] + vec3(globalPos), textureCoords[vtx] });
						}
					}
				}
			}

	std::vector<GLuint> indices;
	for (int faceIndex = 0; faceIndex < (int)faces.size() / 4; ++faceIndex) // Each face (4 vertices)
		for (GLuint rectIndex : rectIndices) // Add the indices with an offset of 4 * faceIndex
			indices.push_back(4 * faceIndex + rectIndex);

	indicesNb = indices.size();
	if (indicesNb != 0) {
		// The VBO stores the vertices
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Face) * faces.size(), faces.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// The EBO stores the indices
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		Debug::glCheckError();
	}
}

void Section::loadVAOs() {
	if (indicesNb != 0) {
		// Create and bind the VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		// Bind the buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// Attributes of the VAO
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		// Unbind all
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		// Unbind the EBO after unbinding the VAO else the EBO will be removed from the VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Section::unloadVAOs() {
	if (indicesNb != 0) {
		glDeleteVertexArrays(1, &VAO);
	}
}

void Section::render(Shader &shader, Texture2D &texture) const {
	if (indicesNb != 0) {
		// Activate shader and texture to draw the object
		shader.use();
		glActiveTexture(GL_TEXTURE0);
		texture.bind();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indicesNb, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

GLuint Section::getBlock(ivec3 pos) const {
	return m_blocks.at(pos);
}

bool Section::isInSection(ivec3 pos) {
	return 0 <= pos.x && pos.x < Const::SECTION_SIDE && 0 <= pos.y && pos.y < Const::SECTION_HEIGHT && 
			0 <= pos.z && pos.z < Const::SECTION_SIDE;
}

GLuint Section::getNearBlock(ivec3 pos) {
	if (isInSection(pos)) {
		return m_blocks.at(pos);
	} else {
		ivec3 globalPos{ pos + Converter::sectionToGlobal(m_position) };
		return p_chunkMap->getBlock(globalPos);
	}
}

const arr<GLuint, 6> Section::rectIndices{
	0, 1, 2,
	2, 3, 0
};

const arr<vec2, 4> Section::textureCoords{{
	{ 0, 0 },
	{ 0, 1 },
	{ 1, 1 },
	{ 1, 0 }
} };

const arr<arr<vec3, 4>, Dir3D::SIZE> Section::dirToFace = []() {

	const arr<vec3, 4> faceX0{ {
		{ 0, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 1, 1 },
		{ 0, 0, 1 }
	} };

	const arr<vec3, 4> faceX1{ {
		{ 1, 0, 1 },
		{ 1, 1, 1 },
		{ 1, 1, 0 },
		{ 1, 0, 0 }
	} };

	const arr<vec3, 4> faceY0{ {
		{ 0, 0, 1 },
		{ 1, 0, 1 },
		{ 1, 0, 0 },
		{ 0, 0, 0 }
	} };

	const arr<vec3, 4> faceY1{ {
		{ 1, 1, 1 },
		{ 0, 1, 1 },
		{ 0, 1, 0 },
		{ 1, 1, 0 }
	} };

	const arr<vec3, 4> faceZ0{{
		{ 1, 0, 0 },
		{ 1, 1, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 0 }
	}};

	const arr<vec3, 4> faceZ1{ {
		{ 0, 0, 1 },
		{ 0, 1, 1 },
		{ 1, 1, 1 },
		{ 1, 0, 1 }
	} };

	return arr<arr<vec3, 4>, Dir3D::SIZE>{ faceX1, faceY1, faceZ1, faceX0, faceY0, faceZ0 };
}();