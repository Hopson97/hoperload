#include "ChunkMap.h"

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

void ChunkMap::destroyWorld()
{
    m_chunks.clear();
}
