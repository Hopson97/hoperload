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
