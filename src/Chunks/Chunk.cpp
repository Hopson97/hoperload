#include "Chunk.h"
#include "Conversions.h"
#include "Voxels.h"

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
    m_voxels[localVoxelToLocalIndex(voxelPosition)].kind = voxelId;
}

VoxelID Chunk::qGetVoxel(const VoxelPosition& voxelPosition) const
{
    assert(!voxelPositionOutOfChunkBounds(voxelPosition));
    return m_voxels[localVoxelToLocalIndex(voxelPosition)].kind;
}

void Chunk::setSunlight(const VoxelPosition& voxelPosition, uint8_t light)
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return;
    }
    m_voxels[localVoxelToLocalIndex(voxelPosition)].sunLight = light;
}

uint8_t Chunk::getSunlight(const VoxelPosition& voxelPosition) const
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return 15;
    }
    return m_voxels[localVoxelToLocalIndex(voxelPosition)].sunLight;
}

void Chunk::setTorchlight(const VoxelPosition& voxelPosition, uint8_t light)
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return;
    }
    m_voxels[localVoxelToLocalIndex(voxelPosition)].torchLight = light;
}

uint8_t Chunk::getTorchlight(const VoxelPosition& voxelPosition) const
{
    if (voxelPositionOutOfChunkBounds(voxelPosition))
    {
        return 15;
    }
    return m_voxels[localVoxelToLocalIndex(voxelPosition)].torchLight;
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

///
///
///     CHUNK MAP
///
///
///
Chunk& ChunkMap::setVoxel(const VoxelPosition& voxelPosition, VoxelID voxelId)
{
    auto chunkPosition = worldToChunkPosition(voxelPosition);
    auto itr = m_chunks.find(chunkPosition);
    auto local = globalVoxelToLocalVoxelPosition(voxelPosition);
    if (itr != m_chunks.cend())
    {
        itr->second.qSetVoxel(local, voxelId);
        return itr->second;
    }
    else
    {
        Chunk& c = addChunk(chunkPosition);
        c.qSetVoxel(local, voxelId);
        return c;
    }
}

VoxelID ChunkMap::getVoxel(const VoxelPosition& voxelPosition) const
{
    auto chunkPosition = worldToChunkPosition(voxelPosition);
    auto itr = m_chunks.find(chunkPosition);
    if (itr == m_chunks.cend())
    {
        return 0;
    }
    return itr->second.qGetVoxel(globalVoxelToLocalVoxelPosition(voxelPosition));
}

void ChunkMap::setSunlight(const VoxelPosition& voxelPosition, uint8_t light)
{
    auto chunkPosition = worldToChunkPosition(voxelPosition);
    auto itr = m_chunks.find(chunkPosition);
    auto local = globalVoxelToLocalVoxelPosition(voxelPosition);
    if (itr != m_chunks.cend())
    {
        itr->second.setSunlight(local, light);
    }
}

uint8_t ChunkMap::getSunlight(const VoxelPosition& voxelPosition) const
{
    auto chunkPosition = worldToChunkPosition(voxelPosition);
    auto itr = m_chunks.find(chunkPosition);
    if (itr == m_chunks.cend())
    {
        return 15;
    }
    return itr->second.getSunlight(globalVoxelToLocalVoxelPosition(voxelPosition));
}

const Chunk& ChunkMap::getChunk(const ChunkPosition& chunk) const
{
    auto itr = m_chunks.find(chunk);
    if (itr == m_chunks.cend())
    {
        return empty;
    }
    return itr->second;
}

Chunk& ChunkMap::addChunk(const ChunkPosition& chunk)
{
    auto itr = m_chunks.find(chunk);
    if (itr == m_chunks.cend())
    {
        return m_chunks
            .emplace(std::piecewise_construct, std::forward_as_tuple(chunk),
                     std::forward_as_tuple(this, chunk))
            .first->second;
    }
    return itr->second;
}

void ChunkMap::ensureNeighbours(const ChunkPosition& chunkPosition)
{
    const auto& cp = chunkPosition;
    addChunk(cp);
    addChunk({cp.x, cp.y + 1});
    addChunk({cp.x, cp.y - 1});
    addChunk({cp.x - 1, cp.y});
    addChunk({cp.x + 1, cp.y});
}

bool ChunkMap::hasNeighbours(const ChunkPosition& chunkPosition) const
{
    const auto& cp = chunkPosition;

    return getChunk({cp.x, cp.y + 1}).hasTerrain &&
           getChunk({cp.x, cp.y - 1}).hasTerrain &&
           getChunk({cp.x - 1, cp.y}).hasTerrain && getChunk({cp.x + 1, cp.y}).hasTerrain;
}

void ChunkMap::destroyWorld() { m_chunks.clear(); }
