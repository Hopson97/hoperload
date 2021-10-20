#pragma once

#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <optional>
#include <unordered_map>
#include <vector>
#include "Conversions.h"
#include "Chunk.h"

// http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
struct ChunkPositionHash
{
    std::size_t operator()(const ChunkPosition& position) const
    {
        return (position.x * 88339) ^ (position.y * 91967);
    }
};

class ChunkMap
{
  public:
    ChunkMap()
        : empty(this, {0, 0})
    {
        empty.hasTerrain = true;
    }

    Chunk& setVoxel(const VoxelPosition& voxelPosition, VoxelID voxelId);
    VoxelID getVoxel(const VoxelPosition& voxelPosition) const;

    void setSunlight(const VoxelPosition& voxelPosition, uint8_t light);
    uint8_t getSunlight(const VoxelPosition& voxelPosition) const;

    const Chunk& getChunk(const ChunkPosition& chunk) const;

    Chunk& addChunk(const ChunkPosition& chunk);
    void ensureNeighbours(const ChunkPosition& chunkPosition);

    bool hasNeighbours(const ChunkPosition& chunkPosition) const;

    void destroyWorld();

  private:
    std::unordered_map<ChunkPosition, Chunk, ChunkPositionHash> m_chunks;

    Chunk empty;
};