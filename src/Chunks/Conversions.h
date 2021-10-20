#pragma once

#include <glm/common.hpp>
#include <glm/glm.hpp>

using VoxelPosition = glm::ivec3;
using ChunkPosition = glm::ivec2;
using VoxelID = uint8_t;

ChunkPosition worldToChunkPosition(const glm::vec3& vec);
VoxelPosition worldToLocalVoxelPosition(const glm::vec3& vec);
VoxelPosition worldToGlobalVoxelPosition(const glm::vec3& vec);

VoxelPosition globalVoxelToLocalVoxelPosition(const VoxelPosition& position);

VoxelPosition localToGlobalVoxelPosition(const VoxelPosition& voxelPosition,
                                         const ChunkPosition& localChunkPosition);