#pragma once

#include "Chunk.h"
#include "Conversions.h"
#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <optional>
#include <unordered_map>
#include <vector>

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

    uint8_t getLightLevel(const VoxelPosition& voxelPosition) const;

    Chunk& getChunk(const ChunkPosition& chunk);

    Chunk& addChunk(const ChunkPosition& chunk);

    void destroyWorld();

  private:
    std::unordered_map<ChunkPosition, Chunk, ChunkPositionHash> m_chunks;

    Chunk empty;
};