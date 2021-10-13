#pragma once

#include <cstdint>
#include <glad/glad.h>
class TextureArray2D;

enum VoxelType : uint16_t
{
    AIR = 0,
    WATER = 1,
    GRASS = 2,
    DIRT = 3,
    STONE = 4,
    SAND = 5,

    TEST_TORCH = 6,

    NUM_VOXELS
};

struct Voxel
{
    Voxel() = default;
    Voxel(const char* name, GLuint textureTop, GLuint textureSide, GLuint textureBottom,
          bool isTransparent, bool isLight = false)
        : name{name}
        , textureTop{textureTop}
        , textureSide{textureSide}
        , textureBottom{textureBottom}
        , isTransparent(isTransparent)
        , isLight(isLight)
    {
    }
    const char* name;
    GLuint textureTop;
    GLuint textureSide;
    GLuint textureBottom;

    bool isTransparent;
    bool isLight;
};

void initVoxelSystem(TextureArray2D& textureArray);
const Voxel& getVoxelType(VoxelType type);

bool isVoxelSolid(uint16_t i);

GLuint getVoxelTexture(uint16_t id, int direction, bool isBackFace);