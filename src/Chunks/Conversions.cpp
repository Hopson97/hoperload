#include "Conversions.h"

#include "ChunkMap.h"

ChunkPosition worldToChunkPosition(const glm::vec3& vec)
{
    int x = static_cast<int>(vec.x);
    int y = static_cast<int>(vec.y);
    return {x < 0 ? ((x - CHUNK_SIZE) / CHUNK_SIZE) : (x / CHUNK_SIZE),
            y < 0 ? ((y - CHUNK_SIZE) / CHUNK_SIZE) : (y / CHUNK_SIZE)};
}

VoxelPosition worldToLocalVoxelPosition(const glm::vec3& vec)
{
    int x = static_cast<int>(vec.x);
    int y = static_cast<int>(vec.y);
    int z = static_cast<int>(vec.z);
    return {x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_DEPTH};
}

VoxelPosition worldToGlobalVoxelPosition(const glm::vec3& vec)
{
    auto localChunk = worldToChunkPosition(vec);
    auto localVoxel = worldToLocalVoxelPosition(vec);
    return {localVoxel.x + localChunk.x * CHUNK_SIZE,
            localVoxel.y + localChunk.y * CHUNK_SIZE, 1};
}

VoxelPosition globalVoxelToLocalVoxelPosition(const VoxelPosition& position)
{
    // Deals with negative coordinates too
    return {(CHUNK_SIZE + (position.x % CHUNK_SIZE)) % CHUNK_SIZE,
            (CHUNK_SIZE + (position.y % CHUNK_SIZE)) % CHUNK_SIZE,
            (CHUNK_SIZE + (position.z % CHUNK_SIZE)) % CHUNK_SIZE};
}

VoxelPosition localToGlobalVoxelPosition(const VoxelPosition& voxelPosition,
                                         const ChunkPosition& localChunkPosition)
{
    return {localChunkPosition.x * CHUNK_SIZE + voxelPosition.x,
            localChunkPosition.y * CHUNK_SIZE + voxelPosition.y, voxelPosition.z};
}