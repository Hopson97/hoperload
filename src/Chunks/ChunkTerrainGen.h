#pragma once

#include "Chunk.h"

struct TerrainGenOptions
{
    int octaves = 8;
    float amplitude = 230;
    float smoothness = 500;
    float roughness = 0.58;
    float offset = 0;
    int seed;
};

void createChunkTerrain(Chunk& chunk, int chunkX, int chunkY, int worldWidth,
                        int worldHeight, const TerrainGenOptions& terrainGenOptions);