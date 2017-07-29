#include "Chunk.h"

Chunk::Chunk(ChunkMap* const chunkMap, ivec2 position)
	: p_chunkMap{ chunkMap }, m_position { position }
{
	for (int i = 0; i < SECTION_HEIGHT; ++i)
		m_sections[i] = std::make_unique<Section>(p_chunkMap, ivec3{ m_position.x, i, m_position.y });
}

Chunk::~Chunk()
{
}

void Chunk::loadBlocks()
{
	for (int i = 0; i < SECTION_HEIGHT; ++i)
		m_sections[i]->loadBlocks();
	loadedBlocks = true;
}

void Chunk::loadFaces()
{
	for (int i = 0; i < SECTION_HEIGHT; ++i)
		m_sections[i]->loadFaces();
	loadedFaces = true;
}

void Chunk::loadVAOs()
{
	for (int i = 0; i < SECTION_HEIGHT; ++i)
		m_sections[i]->loadVAOs();
}

bool Chunk::hasLoadedBlocks() const
{
	return loadedBlocks;
}

bool Chunk::hasLoadedFaces() const
{
	return loadedFaces;
}

void Chunk::render(Shader & shader, Texture2D & texture) const
{
	for (int i = 0; i < SECTION_HEIGHT; ++i)
		m_sections[i]->render(shader, texture);
}

Section& Chunk::getSection(int height)
{
	return *m_sections[height].get();
}
