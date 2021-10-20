#pragma once

#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <optional>
#include <unordered_map>
#include <vector>
#include "Conversions.h"

constexpr int CHUNK_SIZE = 64;
constexpr int CHUNK_DEPTH = 2;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SIZE;

constexpr int WATER_LEVEL = CHUNK_SIZE;

class ChunkMap;

struct VoxelInstance
{
    VoxelID kind = 0;
    uint8_t sunLight = 0;
    uint8_t torchLight = 0;
};

class Chunk
{
  public:
    Chunk(ChunkMap* map, const ChunkPosition& p)
        : m_pChunkMap(map)
        , m_position(p)
    {
    }

    void setVoxel(const VoxelPosition& voxelPosition, VoxelID voxelId);
    VoxelID getVoxel(const VoxelPosition& voxelPosition) const;

    void qSetVoxel(const VoxelPosition& voxelPosition, VoxelID voxelId);
    VoxelID qGetVoxel(const VoxelPosition& voxelPosition) const;

    void setSunlight(const VoxelPosition& voxelPosition, uint8_t light);
    uint8_t getSunlight(const VoxelPosition& voxelPosition) const;

    void setTorchlight(const VoxelPosition& voxelPosition, uint8_t light);
    uint8_t getTorchlight(const VoxelPosition& voxelPosition) const;

    ChunkPosition position() const
    {
        return m_position;
    };

    bool isFaceVisible(VoxelPosition pos, int axis, bool isBackFace) const;
    bool compareStep(VoxelPosition a, VoxelPosition b, int dir, bool isBackFace) const;

    bool hasTerrain = false;

  private:
    ChunkMap* m_pChunkMap;
    ChunkPosition m_position;

    std::array<VoxelInstance, CHUNK_VOLUME> m_voxels;
};
