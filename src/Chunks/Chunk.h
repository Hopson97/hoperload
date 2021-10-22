#pragma once

#include "Conversions.h"
#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr int CHUNK_SIZE = 64;
constexpr int CHUNK_DEPTH = 2;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SIZE;

constexpr int WATER_LEVEL = CHUNK_SIZE;

class ChunkMap;

struct VoxelPositionHash
{
    std::size_t operator()(const VoxelPosition& position) const
    {
        return (position.x * 88339) ^ (position.y * 91967) ^ (position.z * 91967);
    }
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

    void setBlockLight(const VoxelPosition& voxelPosition, uint8_t light);
    uint8_t getBlockLight(const VoxelPosition& voxelPosition) const;

    void floodLights();
    void resetLights();

    ChunkPosition position() const
    {
        return m_position;
    };

    bool isFaceVisible(VoxelPosition pos, int axis, bool isBackFace) const;
    bool compareStep(VoxelPosition a, VoxelPosition b, int dir, bool isBackFace) const;

    bool hasTerrain = false;

  private:
    void floodSunlight(VoxelPosition position, int lightLevel);
    void floodBlockLight(VoxelPosition position, int lightLevel);

    ChunkMap* m_pChunkMap;
    ChunkPosition m_position;

    std::array<VoxelID, CHUNK_VOLUME> m_voxels{0};
    std::array<uint8_t, CHUNK_VOLUME> m_blockLight{0};
    std::array<uint8_t, CHUNK_VOLUME> m_sunLight{0};
    std::unordered_set<VoxelPosition, VoxelPositionHash> m_lightPositions;
};
