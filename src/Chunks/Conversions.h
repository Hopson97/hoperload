#pragma once

#include "Chunk.h"

ChunkPosition worldToChunkPosition(const glm::vec3& vec);
VoxelPosition worldToLocalVoxelPosition(const glm::vec3& vec);
VoxelPosition worldToGlobalVoxelPosition(const glm::vec3& vec);

VoxelPosition globalVoxelToLocalVoxelPosition(const VoxelPosition& position);

VoxelPosition localToGlobalVoxelPosition(const VoxelPosition& voxelPosition,
                                         const ChunkPosition& localChunkPosition);