#include "Chunk.h"
#include "Conversions.h"
#include "Voxels.h"

#include "ChunkMap.h"

namespace
{
    int localVoxelToLocalIndex(const VoxelPosition& position)
    {
        return position.y * (CHUNK_AREA) + position.z * CHUNK_SIZE + position.x;
    }

    bool voxelPositionOutOfChunkBounds(const VoxelPosition& voxelPosition)
    {
        return voxelPosition.x < 0 || voxelPosition.x >= CHUNK_SIZE ||
               voxelPosition.y < 0 || voxelPosition.y >= CHUNK_SIZE ||
               voxelPosition.z < 0 || voxelPosition.z >= CHUNK_SIZE;
    }
} // namespace

void Chunk::setVoxel(const VoxelPosition& voxelPosition, VoxelID voxelId)
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        m_pChunkMap->setVoxel(localToGlobalVoxelPosition(voxelPosition, m_position),
                              voxelId);
    }
    qSetVoxel(voxelPosition, voxelId);
}

VoxelID Chunk::getVoxel(const VoxelPosition& voxelPosition) const
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return m_pChunkMap->getVoxel(
            localToGlobalVoxelPosition(voxelPosition, m_position));
    }
    return qGetVoxel(voxelPosition);
}

void Chunk::qSetVoxel(const VoxelPosition& voxelPosition, VoxelID voxelId)
{
    assert(!voxelPositionOutOfChunkBounds(voxelPosition));
    m_voxels[localVoxelToLocalIndex(voxelPosition)] = voxelId;

    auto itr = m_lightPositions.find(voxelPosition);
    if (itr != m_lightPositions.end())
    {
        m_lightPositions.erase(itr);
        resetLights();
    }

    if (getVoxelType(static_cast<VoxelType>(voxelId)).isLight)
    {
        m_lightPositions.emplace(voxelPosition);
    }
}

VoxelID Chunk::qGetVoxel(const VoxelPosition& voxelPosition) const
{
    assert(!voxelPositionOutOfChunkBounds(voxelPosition));
    return m_voxels[localVoxelToLocalIndex(voxelPosition)];
}

void Chunk::setSunlight(const VoxelPosition& voxelPosition, uint8_t light)
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return;
    }
    m_sunLight[localVoxelToLocalIndex(voxelPosition)] = light;
}

uint8_t Chunk::getSunlight(const VoxelPosition& voxelPosition) const
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return 15;
    }
    return m_sunLight[localVoxelToLocalIndex(voxelPosition)];
}

void Chunk::setBlockLight(const VoxelPosition& voxelPosition, uint8_t light)
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return;
    }
    m_blockLight[localVoxelToLocalIndex(voxelPosition)] = light;
}

uint8_t Chunk::getBlockLight(const VoxelPosition& voxelPosition) const
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return 15;
    }
    return m_blockLight[localVoxelToLocalIndex(voxelPosition)];
}

bool Chunk::isFaceVisible(VoxelPosition pos, int axis, bool isBackFace) const
{
    // Convert the block position to the adjacent voxel pos that is currently
    // being looked at
    pos[axis] += isBackFace ? -1 : 1;

    return !isVoxelSolid(getVoxel(pos));
}

bool Chunk::compareStep(VoxelPosition a, VoxelPosition b, int dir, bool isBackFace) const
{
    auto voxelA = getVoxel(a);
    auto voxelB = getVoxel(b);
    return voxelA == voxelB && isVoxelSolid(voxelB) && isFaceVisible(b, dir, isBackFace);
}

void Chunk::floodLights()
{
    // Sun
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            auto voxel = qGetVoxel({x, y, 0});
            if (voxel == AIR)
            {
                floodSunlight({x, y, 1}, 15);
            }
        }
    }

    for (auto& light : m_lightPositions)
    {
        floodBlockLight(light, 15);
    }
}

void Chunk::floodSunlight(VoxelPosition position, int lightLevel)
{
    setSunlight(position, lightLevel);
    setSunlight(position + glm::ivec3{0, 0, -1}, std::max(lightLevel - 2, 1));

    auto v = getVoxel(position);
    if (v == AIR)
    {

        lightLevel -= 1;
    }
    else if (v == WATER)
    {
        lightLevel -= 2;
    }
    else
    {
        lightLevel -= 3;
    }

    if (lightLevel <= 0)
    {
        return;
    }

    auto tryFlood = [&](const glm::ivec3 offset) {
        auto newFloodPosition = position + offset;

        if (getSunlight(newFloodPosition) <= lightLevel)
        {
            floodSunlight(newFloodPosition, lightLevel);
        }
    };

    tryFlood({1, 0, 0});
    tryFlood({-1, 0, 0});
    tryFlood({0, 1, 0});
    tryFlood({0, -1, 0});
}

void Chunk::floodBlockLight(VoxelPosition position, int lightLevel)
{
    setBlockLight(position, lightLevel);
    setBlockLight(position + glm::ivec3{0, 0, -1}, std::max(lightLevel - 2, 1));

    auto v = getVoxel(position);
    if (v == AIR)
    {

        lightLevel -= 1;
    }
    else if (v == WATER)
    {
        lightLevel -= 2;
    }
    else
    {
        lightLevel -= 3;
    }

    if (lightLevel <= 0)
    {
        return;
    }

    auto tryFlood = [&](const glm::ivec3 offset) {
        auto newFloodPosition = position + offset;

        if (getBlockLight(newFloodPosition) <= lightLevel)
        {
            floodBlockLight(newFloodPosition, lightLevel);
        }
    };

    tryFlood({1, 0, 0});
    tryFlood({-1, 0, 0});
    tryFlood({0, 1, 0});
    tryFlood({0, -1, 0});
}

void Chunk::resetLights()
{
    m_sunLight.fill(0);
    m_blockLight.fill(0);
}